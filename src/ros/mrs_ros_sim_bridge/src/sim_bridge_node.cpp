/**
 * @file sim_bridge_node.cpp
 * @brief @ref mrs::SimBridgeNode 구현 — sim 추상화 통로 + `/clock` 유일 발행자.
 *
 * 이 파일에 **알고리즘은 없다**. 운동학 적분·충돌 판정·지표 산출은 전부 `ISimBackend`
 * 구현체(도메인)가 한다. 여기 있는 것은 파라미터 조회, 배선, 변환 호출, 실패 보고뿐이다.
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
#include <string>
#include <utility>

#include "mrs_interfaces/msg/roadmap_node.hpp"
#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_msg_convert/msg_convert.hpp"
#include "mrs_ros_sim_bridge/sim_bridge_node.hpp"

namespace mrs
{

namespace
{

/** @brief 인프로세스 결정론 백엔드 (`FakeSimBackend`). [0a] 의 유일한 실행 가능 백엔드. */
constexpr const char * BACKEND_FAKE = "fake";
/** @brief 별도 프로세스 pysim 백엔드. C++ 어댑터 미구현 — 기동 거부 대상([0a] 유예). */
constexpr const char * BACKEND_PYSIM = "pysim";
/** @brief Isaac Sim 백엔드. 진화 단계 대상 — 기동 거부 대상. */
constexpr const char * BACKEND_ISAAC = "isaac";

/** @brief 로그 스로틀 간격 [ms]. 20 Hz × N 로봇 스트림이 로그를 덮지 않게 한다. */
constexpr int LOG_THROTTLE_MS = 2000;

/** @brief MapRegistry 균일 뷰 조회 서비스의 계약 이름 (계약 §3 정본표). */
constexpr const char * UNIFORM_VIEW_SERVICE = "/map_registry/get_uniform_view";

} // namespace

SimBridgeNode::SimBridgeNode() : rclcpp::Node("sim_bridge")
{
  declare_and_read_parameters();

  // 백엔드 접근을 한 줄로 세운다(nav2-reference §2 A1 채택 — Reentrant 금지).
  // 스텝·센스·구동이 서로 끼어들면 백엔드의 결정론이 메시지 도착 순서에 종속된다.
  backend_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  rclcpp::SubscriptionOptions sub_options;
  sub_options.callback_group = backend_group_;

  // 계약 §3 정본표: /clock = reliable / volatile / depth 1.
  rclcpp::QoS clock_qos(1);
  clock_qos.reliable();
  clock_qos.durability_volatile();
  clock_pub_ = this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", clock_qos);

  // 계약 §3 정본표: robot_state = best_effort / volatile / depth 1.
  rclcpp::QoS state_qos(1);
  state_qos.best_effort();
  state_qos.durability_volatile();

  // 계약 §3 정본표: cmd_vel = best_effort / volatile / depth 1.
  rclcpp::QoS cmd_qos(1);
  cmd_qos.best_effort();
  cmd_qos.durability_volatile();

  // 계약 §3 정본표: /sim/metrics_tap = reliable / volatile / depth 50.
  rclcpp::QoS metrics_qos(50);
  metrics_qos.reliable();
  metrics_qos.durability_volatile();
  metrics_pub_ =
    this->create_publisher<mrs_interfaces::msg::SimMetricSample>("/sim/metrics_tap", metrics_qos);

  pending_actuations_.assign(
    static_cast<std::size_t>(std::max(robot_count_, 0)), PendingActuation{});

  for (int i = 0; i < robot_count_; ++i)
  {
    const std::string ns = "/robot_" + std::to_string(i);
    robot_state_pubs_.push_back(
      this->create_publisher<mrs_interfaces::msg::RobotState>(ns + "/robot_state", state_qos));

    const std::size_t robot_index = static_cast<std::size_t>(i);
    cmd_vel_subs_.push_back(this->create_subscription<geometry_msgs::msg::Twist>(
      ns + "/cmd_vel", cmd_qos,
      [this, robot_index](const geometry_msgs::msg::Twist::SharedPtr msg)
      {
        this->on_cmd_vel(robot_index, msg);
      },
      sub_options));
  }

  step_srv_ = this->create_service<mrs_interfaces::srv::SimStep>(
    "/sim/step",
    std::bind(&SimBridgeNode::on_step, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), backend_group_);
  inject_srv_ = this->create_service<mrs_interfaces::srv::SimInject>(
    "/sim/inject",
    std::bind(&SimBridgeNode::on_inject, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), backend_group_);
  capabilities_srv_ = this->create_service<mrs_interfaces::srv::SimQueryCapabilities>(
    "/sim/query_capabilities",
    std::bind(
      &SimBridgeNode::on_query_capabilities, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), backend_group_);

  // 동기 기동 검사 — pysim/isaac kill-gate 와 초기 배치 배열 길이·관측 스코프는 여기서 즉시
  // 막는다(조용한 fake 대체 없음). 실제 백엔드 구성은 부착 표(균일 뷰)를 얻은 뒤로 미룬다.
  if (!is_backend_selection_supported() || !validate_spawn_parameters())
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "기동 거부 — 백엔드 선택·초기 배치 검사 실패 (sim_backend='%s'). 조용히 fake 로 대체하지 "
      "않습니다(§5.3 kill-gate 정직성).",
      sim_backend_.c_str());
    return; // startup_accepted_ 는 false 로 남아 main 이 기동을 거부한다.
  }
  startup_accepted_ = true;

  // ⚠ 백엔드 구성을 MapRegistry 균일 뷰 도착까지 미룬다: FakeSimBackend 는 부착 표를
  //   configure() 시점에만 받고(런타임 세터 없음), 그 표는 균일 뷰가 유일 출처다(계약 §0.1).
  //   구성 전까지는 clock/step 타이머를 만들지 않으므로 /clock 발행도 보류된다 — 균일 뷰 조회는
  //   /clock 에 의존하지 않아 데드락이 없다(map_registry 는 사전 구축된 뷰를 즉시 응답한다).
  configure_backend_async();
}

SimBridgeNode::~SimBridgeNode()
{
  try
  {
    log_discard_summary();
  }
  catch (const std::exception & e)
  {
    // 소멸자에서 예외가 밖으로 나가면 std::terminate 다. 여기서 끝낸다.
    RCLCPP_ERROR(this->get_logger(), "폐기 카운터 요약 실패: %s", e.what());
  }
  catch (...)
  {
    RCLCPP_ERROR(this->get_logger(), "폐기 카운터 요약 실패 (미지의 예외)");
  }
}

void SimBridgeNode::declare_and_read_parameters()
{
  declare_backend_parameters();
  declare_timing_parameters();
  declare_scenario_parameters();
  build_view_scopes();
}

void SimBridgeNode::declare_backend_parameters()
{
  robot_count_ = static_cast<int>(this->declare_parameter<int>("robot_count", 2));

  // 백엔드 선택은 config 한 줄이다(architecture §5.1, R-01). 미지의 값이면 폴백 없이 기동 실패
  // (nav2-reference §2 F1 채택 — "문자열 선택 + 미지의 키면 폴백 없이 기동 실패").
  sim_backend_ = this->declare_parameter<std::string>("sim_backend", BACKEND_FAKE);

  // 결정론 재현 키. 같은 시드 + 같은 지령열이면 같은 궤적이 나온다.
  sim_seed_ = this->declare_parameter<std::int64_t>("sim_seed", 0);

  // false 로 두면 세계는 `/sim/step` 서비스로만 전진한다(bench_runner 의 lockstep 구동 경로).
  auto_step_ = this->declare_parameter<bool>("auto_step", true);
}

void SimBridgeNode::declare_timing_parameters()
{
  // [0a] 실측 대상: 시뮬 스텝 dt. 0.05 는 문헌·확정 근거가 없는 **placeholder** 다.
  //   R-A2 상 Δt_h 는 이 값의 정수배여야 하며, 그 검사는 mrs_bringup 이 기동 시 수행한다.
  sim_step_s_ = this->declare_parameter<double>("sim_step_s", 0.05);

  // [0a] 실측 대상: 자동 스텝의 벽시계 주기. 기본값을 sim_step_s 와 같게 두면 공칭 배속이
  //   1.0 이 된다 — "실시간 페이싱"이라는 정의에서 나온 값이지 측정치가 아니다.
  //   실제 배속은 realtime_factor 지표로 **측정**한다.
  step_wall_period_s_ = this->declare_parameter<double>("step_wall_period_s", 0.05);

  // [0a] 실측 대상: /clock 재발행 주기. 스텝보다 촘촘해야 늦게 뜬 노드가 즉시 시각을 얻는다.
  clock_publish_wall_period_s_ =
    this->declare_parameter<double>("clock_publish_wall_period_s", 0.01);

  // [0a] 실측 대상: cmd_vel 워치독 상한. 계약 §3 의 deadline(2·주기)과 정합해야 하나 그 '주기'가
  //   아직 미확정(Q-16)이라 값의 근거가 없다 — placeholder 다.
  cmd_vel_timeout_s_ = this->declare_parameter<double>("cmd_vel_timeout_s", 0.5);

  realtime_report_period_s_ = this->declare_parameter<double>("realtime_report_period_s", 1.0);
}

void SimBridgeNode::declare_scenario_parameters()
{
  // 로봇 반지름·부착 반경·구동 지연은 시나리오 구성값이다(측정 대상 아님).
  robot_radius_m_ = this->declare_parameter<double>("robot_radius_m", 0.3);
  node_attach_radius_m_ = this->declare_parameter<double>("node_attach_radius_m", 0.25);
  // architecture §5.2 누수 조기 발견용 스윕축. 0 = 즉시 반영.
  actuate_to_state_latency_s_ = this->declare_parameter<double>("actuate_to_state_latency_s", 0.0);

  initial_x_m_ = this->declare_parameter<std::vector<double>>("initial_x_m", std::vector<double>{});
  initial_y_m_ = this->declare_parameter<std::vector<double>>("initial_y_m", std::vector<double>{});
  initial_theta_rad_ =
    this->declare_parameter<std::vector<double>>("initial_theta_rad", std::vector<double>{});

  // 정식 경로는 MapRegistry 의 GetUniformView 응답이 발급한 (roadmap_version, view_id) 다.
  //   sim_bridge 는 그 서비스를 호출하지 않으므로(백엔드가 뷰를 모른다 — 계약 L-16 은 그럴 때
  //   "부착 자체를 하지 말 것"을 규정한다) 발급값을 파라미터로 받는다. **두 기본값은
  //   map_registry 의 `roadmap_version`·`uniform_view_id` 기본값과 같아야 한다** — 어긋나면
  //   RobotState 를 받는 l4 가 VIEW_SCOPE_MISMATCH 로 전량 폐기하고 증상은 침묵이다.
  //   0 을 쓰지 않는 이유: roadmap_version 0 은 계약이 런타임 금지, view_id 0 은 물리 뷰 예약값.
  view_roadmap_version_ = this->declare_parameter<std::int64_t>("view_roadmap_version", 1);
  view_uniform_view_id_ = this->declare_parameter<std::int64_t>("view_uniform_view_id", 1);

  // 균일 뷰 조회 배관값. 재시도·시한은 map_registry 기동 대기 흡수용이고, give-up 은 뷰를
  // 끝내 못 얻을 때 계약 L-16(부착 없이 진행)으로 수렴시켜 세계가 영원히 멈추는 것을 막는다.
  map_query_retry_period_s_ = this->declare_parameter<double>("map_query_retry_period_s", 1.0);
  map_query_timeout_s_ = this->declare_parameter<double>("map_query_timeout_s", 2.0);
  map_query_give_up_s_ = this->declare_parameter<double>("map_query_give_up_s", 8.0);
}

void SimBridgeNode::build_view_scopes()
{
  const std::uint64_t roadmap_version =
    static_cast<std::uint64_t>(std::max<std::int64_t>(view_roadmap_version_, 0));

  observation_scope_.view_kind = ViewKind::UNIFORM;
  observation_scope_.roadmap_version = roadmap_version;
  observation_scope_.view_id =
    static_cast<std::uint32_t>(std::max<std::int64_t>(view_uniform_view_id_, 0));

  // SimInject.target_edge_id 는 **물리 뷰** 엣지다(계약 §0.1 정본표). 물리 뷰의 view_id 는 항상 0.
  physical_scope_.view_kind = ViewKind::PHYSICAL;
  physical_scope_.roadmap_version = roadmap_version;
  physical_scope_.view_id = VIEW_ID_PHYSICAL;
}

bool SimBridgeNode::is_backend_selection_supported() const
{
  if (robot_count_ <= 0)
  {
    RCLCPP_FATAL(this->get_logger(), "robot_count(%d) 는 1 이상이어야 합니다.", robot_count_);
    return false;
  }

  if (sim_backend_ == BACKEND_PYSIM || sim_backend_ == BACKEND_ISAAC)
  {
    // 조용한 대체 금지: capabilities().backend_name 이 실제와 달라지면 sim-runner 의
    // OPEN(pending-isaac) 자동 태깅이 근거를 잃는다(architecture §5.3, R-01 조건 3).
    RCLCPP_FATAL(
      this->get_logger(),
      "sim_backend='%s' 는 아직 부착되지 않았습니다 (C++ 프로세스 어댑터 미구현). "
      "[0a] 는 sim_backend:=fake (인프로세스 FakeSimBackend) 로 통과합니다 — "
      "fake 로 조용히 대체하지 않습니다.",
      sim_backend_.c_str());
    return false;
  }

  if (sim_backend_ != BACKEND_FAKE)
  {
    RCLCPP_FATAL(
      this->get_logger(), "sim_backend='%s' 는 알 수 없는 값입니다 (fake|pysim|isaac).",
      sim_backend_.c_str());
    return false;
  }
  return true;
}

void SimBridgeNode::configure_backend_async()
{
  uniform_view_client_ = this->create_client<mrs_interfaces::srv::GetUniformView>(
    UNIFORM_VIEW_SERVICE, rclcpp::ServicesQoS(), backend_group_);

  map_query_started_wall_s_ = steady_clock_.now().seconds();

  // 재시도 타이머는 **벽시계**로 돈다 — /clock 이 아직 흐르지 않으므로 노드 시계 기반 타이머는
  // 영원히 발화하지 않는다(이 노드의 모든 타이머가 벽시계인 것과 같은 이유, nav2-reference §2 C2).
  const double period =
    (std::isfinite(map_query_retry_period_s_) && map_query_retry_period_s_ > 0.0)
      ? map_query_retry_period_s_
      : 1.0;
  map_query_timer_ = this->create_wall_timer(
    std::chrono::duration<double>(period), std::bind(&SimBridgeNode::on_map_query_timer, this),
    backend_group_);
}

void SimBridgeNode::on_map_query_timer()
{
  try
  {
    if (backend_ready_)
    {
      if (map_query_timer_ != nullptr)
      {
        map_query_timer_->cancel();
      }
      return;
    }

    if (map_query_gave_up())
    {
      return;
    }
    if (!reclaim_stale_map_query())
    {
      return;
    }
    if (!uniform_view_client_->service_is_ready())
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "%s 서비스 대기 중 — 부착 표 확보 전까지 /clock 발행을 보류합니다.", UNIFORM_VIEW_SERVICE);
      return;
    }
    send_uniform_view_request();
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_map_query_timer 예외 (안전 폴백: 다음 주기 재시도): %s", e.what());
  }
}

bool SimBridgeNode::map_query_gave_up()
{
  const double waited_s = steady_clock_.now().seconds() - map_query_started_wall_s_;
  if (waited_s <= map_query_give_up_s_)
  {
    return false;
  }

  RCLCPP_WARN(
    this->get_logger(),
    "MapRegistry 균일 뷰를 %.1f s 안에 얻지 못했습니다 — 계약 L-16 대로 **부착 없이** 기동합니다"
    "(로봇은 occupied_node 센티넬을 보고합니다).",
    waited_s);
  finalize_backend({});
  return true;
}

bool SimBridgeNode::reclaim_stale_map_query()
{
  if (!map_query_pending_)
  {
    return true;
  }

  const double waited_s = steady_clock_.now().seconds() - map_query_sent_at_wall_s_;
  if (waited_s < map_query_timeout_s_)
  {
    return false; // 아직 기다릴 시간이다 — 중복 요청을 보내지 않는다.
  }

  (void)uniform_view_client_->remove_pending_request(map_query_request_id_);
  map_query_pending_ = false;
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
    "GetUniformView 응답이 %.1f s 안에 오지 않아 요청을 회수했습니다 — 재시도합니다.", waited_s);
  return true;
}

void SimBridgeNode::send_uniform_view_request()
{
  auto request = std::make_shared<mrs_interfaces::srv::GetUniformView::Request>();
  request->roadmap_version =
    static_cast<std::uint64_t>(std::max<std::int64_t>(view_roadmap_version_, 0));

  auto future_and_id = uniform_view_client_->async_send_request(
    request, std::bind(&SimBridgeNode::on_uniform_view_response, this, std::placeholders::_1));
  map_query_request_id_ = future_and_id.request_id;
  map_query_sent_at_wall_s_ = steady_clock_.now().seconds();
  map_query_pending_ = true;
}

void SimBridgeNode::on_uniform_view_response(
  rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture future)
{
  using Resp = mrs_interfaces::srv::GetUniformView::Response;
  try
  {
    map_query_pending_ = false;
    if (backend_ready_)
    {
      return; // 이미 확정됐다(시한초과 폴백 등). 늦게 온 응답은 버린다.
    }

    const auto response = future.get();
    if (response == nullptr || response->result != Resp::RESULT_OK)
    {
      RCLCPP_WARN(
        this->get_logger(), "GetUniformView 실패/미OK (result=%u) — 재시도합니다.",
        response != nullptr ? static_cast<unsigned>(response->result) : 0U);
      return;
    }

    // RESULT_OK 는 권위 있는 뷰다 — 여기서 **최종 결정**을 내린다(부착 or L-16 무부착).
    std::vector<FakeNodeAnchor> anchors;
    if (!build_anchors_from_view(*response, anchors))
    {
      RCLCPP_WARN(
        this->get_logger(),
        "균일 뷰→부착 표 변환 실패 (스코프 불일치·노드 id 센티넬 등) — 계약 L-16 대로 부착 "
        "없이 기동합니다.");
      finalize_backend({});
      return;
    }
    finalize_backend(anchors);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_uniform_view_response 예외 (부착 없이 기동): %s", e.what());
    finalize_backend({});
  }
}

bool SimBridgeNode::build_anchors_from_view(
  const mrs_interfaces::srv::GetUniformView::Response & response,
  std::vector<FakeNodeAnchor> & anchors) const
{
  // 스코프 정합: 응답 (roadmap_version, view_id) 가 관측 스코프와 다르면 부착하지 않는다 —
  // 다른 스코프의 노드 id 를 관측 스코프로 실으면 오태깅이 되어 수신자가 조용히 폐기한다.
  if (
    response.roadmap_version != observation_scope_.roadmap_version ||
    response.view_id != observation_scope_.view_id)
  {
    return false;
  }

  anchors.clear();
  anchors.reserve(response.nodes.size());
  for (const mrs_interfaces::msg::RoadmapNode & node : response.nodes)
  {
    // 규칙 V3: uint32 → 강타입 랩은 convert 단독 소유. 균일 뷰 노드는 센티넬을 허용하지 않는다.
    UniformNodeId node_id;
    const convert::ConvertResult result =
      convert::node_id_from_msg(node.node_id, convert::NoneNodePolicy::REJECT, node_id);
    if (!result.ok)
    {
      return false;
    }
    FakeNodeAnchor anchor;
    anchor.node_id = node_id;
    anchor.x_m = node.x_m;
    anchor.y_m = node.y_m;
    anchors.push_back(anchor);
  }
  return !anchors.empty();
}

void SimBridgeNode::finalize_backend(const std::vector<FakeNodeAnchor> & anchors)
{
  if (backend_ready_)
  {
    return; // 멱등 — 백엔드는 한 번만 확정한다.
  }

  if (!try_build_backend(anchors))
  {
    if (!anchors.empty())
    {
      RCLCPP_WARN(
        this->get_logger(),
        "부착 표로 백엔드 구성 실패 — 계약 L-16 대로 부착 없이 재시도합니다(정지보다 부착 "
        "없이 도는 편이 낫다).");
      if (!try_build_backend({}))
      {
        return; // FATAL 은 try_build_backend 가 남겼다. backend_ready_=false → 세계 정지.
      }
    }
    else
    {
      return; // 부착 없이도 실패 — 기동 거부 상태로 남는다(FATAL 기록됨).
    }
  }

  backend_ready_ = true;
  if (map_query_timer_ != nullptr)
  {
    map_query_timer_->cancel();
  }
  start_stepping();
}

bool SimBridgeNode::try_build_backend(const std::vector<FakeNodeAnchor> & anchors)
{
  FakeSimConfig config;
  if (!build_fake_config(config, anchors))
  {
    return false;
  }

  // ⚠ SimBackendRegistry 를 쓰지 않는다: 현재 구현이 `std::logic_error` 를 던지는 스텁이고
  //   `mrs_sim_abstraction` 은 이번 작업에서 **수정 금지** 대상이다. 선택 로직을 Composition
  //   Root 인접(이 노드)에 두는 것은 architecture §2.4 검사 5 와 정합한다.
  auto fake_backend = std::make_shared<FakeSimBackend>();
  if (!fake_backend->configure(config))
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "FakeSimBackend::configure 실패 — 구성(스텝 dt·반지름·로봇 id 중복·뷰 스코프·부착 id "
      "중복)을 확인하십시오.");
    return false;
  }
  if (!fake_backend->reset(static_cast<std::uint64_t>(std::max<std::int64_t>(sim_seed_, 0))))
  {
    RCLCPP_FATAL(this->get_logger(), "FakeSimBackend::reset 실패 — 시드 고정에 실패했습니다.");
    return false;
  }

  backend_ = std::move(fake_backend);
  sim_time_s_ = 0.0;
  step_count_ = 0;
  attached_anchor_count_ = config.node_anchors.size();
  return true;
}

void SimBridgeNode::start_stepping()
{
  last_realtime_report_sim_s_ = sim_time_s_;
  last_realtime_report_wall_s_ = steady_clock_.now().seconds();

  // ⚠ nav2-reference §2 C2 (필수 채택): /clock 발행 타이머는 **벽시계**다.
  //   use_sim_time 으로 돌리면 자기가 발행하지 않은 시계를 기다리며 부팅 데드락에 빠진다.
  clock_timer_ = this->create_wall_timer(
    std::chrono::duration<double>(clock_publish_wall_period_s_),
    std::bind(&SimBridgeNode::on_clock_timer, this), backend_group_);

  if (auto_step_)
  {
    // 자동 스텝 타이머도 **벽시계**여야 한다 — 시뮬 시각을 전진시키는 주체가 그 시각을
    // 기다리면 세계가 영원히 멈춘다(같은 부팅 데드락의 다른 얼굴).
    step_timer_ = this->create_wall_timer(
      std::chrono::duration<double>(step_wall_period_s_),
      std::bind(&SimBridgeNode::on_step_timer, this), backend_group_);
  }

  publish_clock();

  const SimCapabilities capabilities = backend_->capabilities();
  RCLCPP_INFO(
    this->get_logger(),
    "sim_bridge 기동 — backend=%s, robot_count=%d, sim_step_s=%.6f, auto_step=%s, "
    "deterministic=%s, fidelity=%u, node_anchors=%zu",
    capabilities.backend_name.c_str(), robot_count_, sim_step_s_, auto_step_ ? "true" : "false",
    capabilities.deterministic_with_seed ? "true" : "false",
    static_cast<unsigned>(capabilities.physics_fidelity), attached_anchor_count_);
}

bool SimBridgeNode::validate_spawn_parameters() const
{
  const std::size_t count = static_cast<std::size_t>(robot_count_);

  if (
    initial_x_m_.size() != count || initial_y_m_.size() != count ||
    initial_theta_rad_.size() != count)
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "초기 배치 배열 길이 불일치 — robot_count=%zu 인데 x=%zu, y=%zu, theta=%zu 입니다. "
      "초기 자세를 추측해서 채우지 않습니다.",
      count, initial_x_m_.size(), initial_y_m_.size(), initial_theta_rad_.size());
    return false;
  }

  if (!is_usable_scope(observation_scope_))
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "관측 뷰 스코프가 사용 불가입니다 (roadmap_version=%lu, view_id=%u). "
      "roadmap_version 0 은 계약이 런타임에 금지한 값입니다.",
      static_cast<unsigned long>(observation_scope_.roadmap_version), observation_scope_.view_id);
    return false;
  }
  return true;
}

bool SimBridgeNode::build_fake_config(
  FakeSimConfig & config, const std::vector<FakeNodeAnchor> & anchors) const
{
  if (!validate_spawn_parameters())
  {
    return false;
  }

  const std::size_t count = static_cast<std::size_t>(robot_count_);
  config.step_dt_s = sim_step_s_;
  config.robot_radius_m = robot_radius_m_;
  config.node_attach_radius_m = node_attach_radius_m_;
  config.actuate_to_state_latency_s = actuate_to_state_latency_s_;
  config.view_scope = observation_scope_;

  // 부착 표는 MapRegistry 균일 뷰(@ref build_anchors_from_view)에서 온다. 비어 있으면 계약 L-16
  // 대로 부착하지 않고 정상 경로(센티넬 + edge_progress)로 보고한다 — 추측해서 붙이지 않는다.
  config.node_anchors = anchors;

  config.robots.clear();
  config.robots.reserve(count);
  for (std::size_t i = 0; i < count; ++i)
  {
    FakeRobotSpawn spawn;
    spawn.robot_id = static_cast<RobotId>(i);
    spawn.initial_pose.x_m = initial_x_m_[i];
    spawn.initial_pose.y_m = initial_y_m_[i];
    spawn.initial_pose.theta_rad = initial_theta_rad_[i];
    config.robots.push_back(spawn);
  }
  return true;
}

void SimBridgeNode::on_clock_timer()
{
  try
  {
    publish_clock();
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_clock_timer 예외 (무시): %s",
      e.what());
  }
}

void SimBridgeNode::on_step_timer()
{
  try
  {
    if (!advance_sim(1U))
    {
      RCLCPP_ERROR_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "자동 스텝 실패 — 시뮬을 전진시키지 않고 현상유지합니다.");
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_step_timer 예외 (현상유지): %s",
      e.what());
  }
}

void SimBridgeNode::on_step(
  const std::shared_ptr<mrs_interfaces::srv::SimStep::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimStep::Response> response)
{
  using ResponseT = mrs_interfaces::srv::SimStep::Response;
  try
  {
    if (request == nullptr || response == nullptr)
    {
      return;
    }

    if (!backend_ready_)
    {
      response->result = ResponseT::RESULT_BACKEND_ERROR;
      response->step_count = step_count_;
      response->message = "backend not ready";
      return;
    }

    fill_step_response(advance_sim(request->steps), *response);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_step 예외 (안전 응답): %s", e.what());
    if (response != nullptr)
    {
      response->result = ResponseT::RESULT_BACKEND_ERROR;
      response->message = "exception in sim_bridge";
    }
  }
}

void SimBridgeNode::fill_step_response(
  bool advanced, mrs_interfaces::srv::SimStep::Response & response)
{
  using ResponseT = mrs_interfaces::srv::SimStep::Response;

  response.result = advanced ? ResponseT::RESULT_OK : ResponseT::RESULT_BACKEND_ERROR;
  response.step_count = step_count_;

  // R-18 구속: 시각 변환은 반드시 seconds_to_time 경유. 1e9 직접 나눗셈 금지 —
  // 음수·NaN·범위 가드가 그 헬퍼 한 곳에 있는 것이 존재 이유다.
  const convert::ConvertResult time_result =
    convert::seconds_to_time(sim_time_s_, response.sim_time);
  if (!time_result.ok)
  {
    record_convert_failure("sim_step_response_time", time_result.reason);
    response.result = ResponseT::RESULT_BACKEND_ERROR;
    response.message =
      std::string("sim_time conversion guard: ") + convert::to_string(time_result.reason);
    return;
  }
  response.message = advanced ? "" : "backend step failed";
}

std::uint8_t SimBridgeNode::inject_result_for(convert::ConvertStatus reason) const
{
  using ResponseT = mrs_interfaces::srv::SimInject::Response;
  return (reason == convert::ConvertStatus::ENUM_OUT_OF_RANGE) ? ResponseT::RESULT_UNSUPPORTED
                                                               : ResponseT::RESULT_BAD_TARGET;
}

void SimBridgeNode::on_inject(
  const std::shared_ptr<mrs_interfaces::srv::SimInject::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimInject::Response> response)
{
  using ResponseT = mrs_interfaces::srv::SimInject::Response;
  try
  {
    if (request == nullptr || response == nullptr)
    {
      return;
    }
    handle_inject(*request, *response);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_inject 예외 (안전 응답): %s",
      e.what());
    if (response != nullptr)
    {
      response->result = ResponseT::RESULT_BAD_TARGET;
      response->injection_id = 0U;
      response->message = "exception in sim_bridge";
    }
  }
}

void SimBridgeNode::handle_inject(
  const mrs_interfaces::srv::SimInject::Request & request,
  mrs_interfaces::srv::SimInject::Response & response)
{
  using ResponseT = mrs_interfaces::srv::SimInject::Response;

  if (!backend_ready_)
  {
    response.result = ResponseT::RESULT_UNSUPPORTED;
    response.injection_id = 0U;
    response.message = "backend not ready";
    return;
  }

  FaultInjection fault;
  const convert::ConvertResult convert_result =
    convert::from_request(request, physical_scope_, fault);
  if (!convert_result.ok)
  {
    record_convert_failure("sim_inject_request", convert_result.reason);
    response.result = inject_result_for(convert_result.reason);
    response.injection_id = 0U;
    response.message = convert::to_string(convert_result.reason);
    return;
  }

  std::uint64_t injection_id = 0U;
  if (!backend_->inject(fault, injection_id))
  {
    response.result = ResponseT::RESULT_UNSUPPORTED;
    response.injection_id = 0U;
    response.message = "backend does not support this fault kind";
    return;
  }

  response.result = ResponseT::RESULT_ACCEPTED;
  response.injection_id = injection_id;
  response.message = "";
}

void SimBridgeNode::on_query_capabilities(
  const std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Response> response)
{
  (void)request;
  try
  {
    if (response == nullptr)
    {
      return;
    }
    if (!backend_ready_)
    {
      // 백엔드가 없으면 능력을 **지어내지 않는다**. 기본 생성값(빈 이름)이 나가면 수신자의
      // 변환이 FIELD_RANGE_VIOLATION 으로 폐기하므로 오해가 남지 않는다.
      RCLCPP_ERROR_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "capabilities 질의를 받았으나 백엔드가 없습니다 — 값을 지어내지 않습니다.");
      return;
    }

    const SimCapabilities capabilities = backend_->capabilities();
    const convert::ConvertResult convert_result =
      convert::to_msg(capabilities, response->capabilities);
    if (!convert_result.ok)
    {
      record_convert_failure("sim_capabilities", convert_result.reason);
      response->capabilities = mrs_interfaces::msg::SimCapabilities{};
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_query_capabilities 예외: %s",
      e.what());
  }
}

void SimBridgeNode::on_cmd_vel(
  std::size_t robot_index, const geometry_msgs::msg::Twist::SharedPtr msg)
{
  try
  {
    if (msg == nullptr || robot_index >= pending_actuations_.size())
    {
      return;
    }

    const double v_mps = msg->linear.x;
    const double omega_rps = msg->angular.z;

    // ⚠ 계층 규율의 유예 지점: `Twist` → `ActuationCommand` **수신** 변환 함수가
    //   `mrs_msg_convert` 에 없다(발행 방향 `make_cmd_vel` 만 존재). 이번 작업은 convert 패키지
    //   수정이 금지돼 있어 유한성 가드만 여기서 수행한다. 계약 요청 사항으로 상신했다
    //   (요청: `from_msg(const geometry_msgs::msg::Twist &, mrs::ActuationCommand &)` 신설).
    //   비유한 지령은 **폐기**하고 타임스탬프를 갱신하지 않는다 — 워치독이 정지로 흡수한다.
    if (!std::isfinite(v_mps) || !std::isfinite(omega_rps))
    {
      ++rejected_cmd_vel_count_;
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "robot_%zu cmd_vel 비유한 지령 폐기 (누적 %llu) — 워치독이 정지로 폴백합니다.", robot_index,
        static_cast<unsigned long long>(rejected_cmd_vel_count_));
      return;
    }

    PendingActuation & pending = pending_actuations_[robot_index];
    pending.v_mps = v_mps;
    pending.omega_rps = omega_rps;
    pending.received_sim_time_s = sim_time_s_;
    pending.ever_received = true;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "on_cmd_vel 예외 (지령 무시): %s",
      e.what());
  }
}

bool SimBridgeNode::advance_sim(std::uint32_t steps)
{
  if (!backend_ready_ || backend_ == nullptr)
  {
    return false;
  }

  const double wall_before_s = steady_clock_.now().seconds();

  apply_actuation();

  double advanced_sim_time_s = sim_time_s_;
  if (!backend_->step(steps, advanced_sim_time_s))
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
      "ISimBackend::step 실패 — 시뮬 시각을 전진시키지 않습니다(현상유지).");
    return false;
  }

  sim_time_s_ = advanced_sim_time_s;
  step_count_ += (steps == 0U) ? 1U : static_cast<std::uint64_t>(steps);

  // 시각을 먼저 내보낸다 — 수신자가 새 상태를 옛 시각으로 해석하지 않게.
  publish_clock();
  publish_robot_states();
  publish_backend_metrics();

  const double wall_after_s = steady_clock_.now().seconds();
  publish_bridge_metrics(wall_after_s - wall_before_s);
  return true;
}

void SimBridgeNode::apply_actuation()
{
  std::vector<ActuationCommand> commands;
  commands.reserve(pending_actuations_.size());

  for (std::size_t i = 0; i < pending_actuations_.size(); ++i)
  {
    const PendingActuation & pending = pending_actuations_[i];
    ActuationCommand command;
    command.robot_id = static_cast<RobotId>(i);

    const double age_s = sim_time_s_ - pending.received_sim_time_s;
    const bool stale = !pending.ever_received || age_s > cmd_vel_timeout_s_;
    if (stale)
    {
      // 계약 §1.5: cmd_vel 유실 시 안전 폴백(감속·정지)은 sim_bridge 워치독의 책임이다.
      command.v_mps = 0.0;
      command.omega_rps = 0.0;
      ++watchdog_stop_count_;
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "robot_%zu cmd_vel 결손(age=%.3f s > %.3f s) — 정지 지령으로 폴백 (누적 %llu)", i, age_s,
        cmd_vel_timeout_s_, static_cast<unsigned long long>(watchdog_stop_count_));
    }
    else
    {
      command.v_mps = pending.v_mps;
      command.omega_rps = pending.omega_rps;
    }
    commands.push_back(command);
  }

  if (!commands.empty() && !backend_->actuate(commands))
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
      "ISimBackend::actuate 실패 — 이번 스텝의 지령이 반영되지 않았습니다.");
  }
}

void SimBridgeNode::publish_clock()
{
  if (clock_pub_ == nullptr)
  {
    return;
  }

  rosgraph_msgs::msg::Clock clock_msg;
  // R-18 구속: 음수·NaN·범위 가드가 이 헬퍼에 있다. 직접 나눗셈 금지.
  const convert::ConvertResult time_result = convert::seconds_to_time(sim_time_s_, clock_msg.clock);
  if (!time_result.ok)
  {
    record_convert_failure("clock", time_result.reason);
    return;
  }
  clock_pub_->publish(clock_msg);
}

void SimBridgeNode::publish_robot_states()
{
  std::vector<RobotObservation> observations;
  if (!backend_->sense(observations))
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
      "ISimBackend::sense 실패 — 이번 스텝의 robot_state 를 발행하지 않습니다(마지막 관측 유지).");
    return;
  }

  for (const RobotObservation & observation : observations)
  {
    const std::size_t index = static_cast<std::size_t>(observation.robot_id);
    if (index >= robot_state_pubs_.size())
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
        "백엔드가 구성 밖 로봇 id=%u 를 관측했습니다 — 발행하지 않습니다.",
        static_cast<unsigned>(observation.robot_id));
      continue;
    }

    mrs_interfaces::msg::RobotState state_msg;
    const convert::ConvertResult convert_result =
      convert::to_msg(observation, sim_time_s_, state_msg);
    if (!convert_result.ok)
    {
      record_convert_failure("robot_state", convert_result.reason);
      continue;
    }
    robot_state_pubs_[index]->publish(state_msg);
  }
}

void SimBridgeNode::publish_backend_metrics()
{
  std::vector<SimMetricSample> samples;
  if (!backend_->poll_metrics(samples))
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), steady_clock_, LOG_THROTTLE_MS, "ISimBackend::poll_metrics 실패");
    return;
  }

  for (const SimMetricSample & sample : samples)
  {
    // pending_isaac 은 **그대로** 전파한다. 여기서 떨어뜨리면 kill-gate 정직성이 무너진다.
    publish_metric_sample(sample, "metrics_tap_backend");
  }
}

void SimBridgeNode::publish_bridge_metrics(double step_wall_duration_s)
{
  const SimCapabilities capabilities = backend_->capabilities();
  // 물리 충실도가 FULL_PHYSICS 미만이면 이 백엔드에서 얻은 값은 연구 검증으로 승격 금지다.
  const bool pending_isaac = capabilities.physics_fidelity != PhysicsFidelity::FULL_PHYSICS;

  SimMetricSample step_cost;
  step_cost.key = "step_wall_duration_s";
  step_cost.value = step_wall_duration_s;
  step_cost.robot_id = ROBOT_ID_NONE;
  step_cost.pending_isaac = pending_isaac;
  publish_metric_sample(step_cost, "metrics_tap_bridge");

  const double sim_elapsed_s = sim_time_s_ - last_realtime_report_sim_s_;
  if (sim_elapsed_s < realtime_report_period_s_)
  {
    return;
  }

  const double wall_now_s = steady_clock_.now().seconds();
  const double wall_elapsed_s = wall_now_s - last_realtime_report_wall_s_;
  last_realtime_report_sim_s_ = sim_time_s_;
  last_realtime_report_wall_s_ = wall_now_s;

  if (wall_elapsed_s <= 0.0)
  {
    // 0 으로 나누느니 이번 창은 보고하지 않는다 — 배속을 지어내지 않는다.
    return;
  }

  SimMetricSample realtime_factor;
  realtime_factor.key = "realtime_factor";
  realtime_factor.value = sim_elapsed_s / wall_elapsed_s;
  realtime_factor.robot_id = ROBOT_ID_NONE;
  realtime_factor.pending_isaac = pending_isaac;
  publish_metric_sample(realtime_factor, "metrics_tap_bridge");
}

void SimBridgeNode::publish_metric_sample(const SimMetricSample & sample, const char * site)
{
  if (metrics_pub_ == nullptr)
  {
    return;
  }

  mrs_interfaces::msg::SimMetricSample sample_msg;
  const convert::ConvertResult convert_result = convert::to_msg(sample, sim_time_s_, sample_msg);
  if (!convert_result.ok)
  {
    record_convert_failure(site, convert_result.reason);
    return;
  }
  metrics_pub_->publish(sample_msg);
}

void SimBridgeNode::record_convert_failure(const char * site, convert::ConvertStatus reason)
{
  const std::string key = (site != nullptr) ? std::string(site) : std::string("unknown");
  DiscardCounters & counters = discard_counts_[key];
  const std::size_t index = static_cast<std::size_t>(reason);
  if (index < counters.size())
  {
    ++counters[index];
  }

  // 사유별로 **구분해서** 기록한다. 뭉개면 사후 감사에서 원인을 잃는다(계약 §0.2).
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), steady_clock_, LOG_THROTTLE_MS,
    "변환 실패 — site=%s reason=%s (해당 사유 누적 %llu)", key.c_str(), convert::to_string(reason),
    static_cast<unsigned long long>((index < counters.size()) ? counters[index] : 0U));
}

void SimBridgeNode::log_discard_summary() const
{
  RCLCPP_INFO(
    this->get_logger(),
    "sim_bridge 종료 요약 — steps=%llu, sim_time=%.3f s, watchdog_stop=%llu, "
    "rejected_cmd_vel=%llu",
    static_cast<unsigned long long>(step_count_), sim_time_s_,
    static_cast<unsigned long long>(watchdog_stop_count_),
    static_cast<unsigned long long>(rejected_cmd_vel_count_));

  for (const auto & entry : discard_counts_)
  {
    for (std::size_t i = 0; i < entry.second.size(); ++i)
    {
      if (entry.second[i] == 0U)
      {
        continue;
      }
      RCLCPP_INFO(
        this->get_logger(), "  폐기 site=%s reason=%s count=%llu", entry.first.c_str(),
        convert::to_string(static_cast<convert::ConvertStatus>(i)),
        static_cast<unsigned long long>(entry.second[i]));
    }
  }
}

} // namespace mrs
