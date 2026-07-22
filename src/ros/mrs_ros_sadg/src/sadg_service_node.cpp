/**
 * @file sadg_service_node.cpp
 * @brief @ref mrs::SadgServiceNode 본문 — L3 SADG 노드 (로드맵 [2] Split B: 실 depgraph 배선).
 *
 * ## 이 파일이 지키는 규율
 *  1. **콜백 안에 알고리즘 없음** — 릴리스 판정(의존성 충족)·세그먼트 선택은 `mrs_depgraph` 가,
 *     도메인↔메시지 변환은 `mrs_msg_convert` 가 단독으로 한다. 이 노드는 배관(구독·타이머·발행·
 *     봉투 스탬프·진행 이탈 판정)만 든다.
 *  2. **모든 콜백은 try/catch** — 노드가 죽지 않는다(CLAUDE.md 규율 2).
 *  3. **판정은 convert, 보고는 노드**(R-15 (b)) — 사유별 카운터 + 스로틀 로그 + 폐기.
 *  4. **시각 변환은 `mrs_msg_convert` 헬퍼 경유**(R-18) — 이 파일에 `1e9` 나눗셈이 없다.
 *  5. **수치 창작 금지** — 미정 값은 ROS2 param 으로 노출하고 성격을 주석에 남긴다.
 *  6. 콜백은 짧게 유지한다 — 경계 검사(`tools/check_boundaries.sh` 항목 3)의 길이 상한은
 *     "얇은 I/O 어댑터"를 기계로 지키게 하는 장치다.
 */

#include "mrs_ros_sadg/sadg_service_node.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
#include <unordered_map>
#include <utility>

#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs
{
namespace
{

/** @brief MapRegistry 균일 뷰 조회 서비스의 계약 이름 (계약 §3 정본표). */
constexpr const char * UNIFORM_VIEW_SERVICE = "/map_registry/get_uniform_view";
/** @brief `/planned_paths` 토픽 이름 (계약 §3 정본표). */
constexpr const char * PLANNED_PATHS_TOPIC = "/planned_paths";
/** @brief 스로틀 로그 주기 [ms]. */
constexpr int LOG_THROTTLE_MS = 5000;

/**
 * @brief 에스컬레이션 사유를 로그용 문자열로 바꾼다.
 *
 * 사유를 숫자로만 남기면 사후 감사에서 `SYNC_LOST` 와 `QP_INFEASIBLE` 의 구별이 로그 판독자에게
 * 사라진다 — 계약 §0.2 의 "뭉갬 금지"가 로그에도 적용되는 이유다.
 *
 * @param[in] reason 에스컬레이션 사유. 자료형 `mrs::EscalationReason`.
 * @return `const char *` — 널 종료 정적 문자열. 알 수 없는 값이면 `"UNKNOWN"`.
 */
[[nodiscard]] const char * escalation_reason_name(EscalationReason reason) noexcept
{
  switch (reason)
  {
    case EscalationReason::BLOCKED:
      return "BLOCKED";
    case EscalationReason::WINDOW_EXPIRING:
      return "WINDOW_EXPIRING";
    case EscalationReason::QP_INFEASIBLE:
      return "QP_INFEASIBLE";
    case EscalationReason::LOCALIZATION_LOST:
      return "LOCALIZATION_LOST";
    case EscalationReason::SYNC_LOST:
      return "SYNC_LOST";
  }
  return "UNKNOWN";
}

/**
 * @brief 두 계획이 **릴리스 관점에서 동일**한지 판정한다 (build-once 판정의 핵심).
 *
 * ⚠ `arrival_time_s` 는 **비교하지 않는다**. `pp_service` 의 canned 솔버는 재발행마다 계획 시작
 * 시각을 현재 시각으로 다시 잡아 방문 도착시각을 전부 갱신하므로(내용은 같은 왕복), 시각까지
 * 비교하면 "동일한 계획"이 매번 달라 보여 build-once 가 무력화된다. depgraph 의 릴리스/진행
 * 프론티어는 **로봇 집합 + 각 로봇의 방문 노드열**로만 결정되므로 그 둘만 비교한다.
 *
 * 로봇 순서에 무관하다(로봇 id 로 대조). 노드 id 는 UNIFORM 강타입 동등성으로 비교한다.
 *
 * @param[in] a 이전 계획. 자료형 `std::vector<mrs::RobotPath>`.
 * @param[in] b 새 계획. 자료형 `std::vector<mrs::RobotPath>`.
 * @return `bool` — 로봇 집합과 각 로봇의 방문 노드열이 완전히 같으면 true.
 */
[[nodiscard]] bool plans_equivalent(
  const std::vector<RobotPath> & a, const std::vector<RobotPath> & b)
{
  if (a.size() != b.size())
  {
    return false;
  }

  std::unordered_map<std::uint16_t, const std::vector<TimedNodeVisit> *> a_by_id;
  a_by_id.reserve(a.size());
  for (const RobotPath & path : a)
  {
    a_by_id.emplace(path.robot_id, &path.visits);
  }
  if (a_by_id.size() != a.size())
  {
    return false; // 이전 계획에 로봇 id 중복 — 안전하게 "다름"으로 본다(재구축)
  }

  for (const RobotPath & path : b)
  {
    const auto it = a_by_id.find(path.robot_id);
    if (it == a_by_id.end())
    {
      return false; // 로봇 집합 불일치
    }
    const std::vector<TimedNodeVisit> & prev_visits = *it->second;
    if (prev_visits.size() != path.visits.size())
    {
      return false;
    }
    for (std::size_t k = 0; k < path.visits.size(); ++k)
    {
      if (prev_visits[k].node_id != path.visits[k].node_id)
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace

SadgServiceNode::SadgServiceNode()
    : rclcpp::Node("sadg_service"), btpg_tracker_(nullptr), milp_tracker_(nullptr), judge_(nullptr)
{
  // ── 파라미터 ─────────────────────────────────────────────────────────────
  // 값의 성격 3구분: (문헌치) 이론·계약이 준 값 / (배관) 실행 편의 상수 /
  // ([0a] 실측 대상) 지금 정할 근거가 없어 실측 후 확정할 값.
  robot_count_ = this->declare_parameter<int>("robot_count", 2); // [0a] = 2대 (architecture §7)
  this->declare_parameter<std::string>("reorder_transport", "inproc"); // 계약 Q-2: inproc|service
  window_release_period_s_ = this->declare_parameter<double>(
    "window_release_period_s", 0.5); // 문헌치 — 계약 Q-11 세그먼트 Δt = 0.5 s.
                                     // ⚠ [0a] 한정: 진행 이벤트원이 없어 타이머로 굴린다(헤더 주석)
  segments_per_window_ = this->declare_parameter<int>(
    "segments_per_window", 1); // [2] 이후 **inert** — 창당 세그먼트 수는 depgraph 가 의존성
                               // 프론티어(막힘 지점)까지로 결정한다. 파라미터는 하위호환 위해 유지
                               // (더미 [0a] 릴리스에서만 실효했다)
  window_validity_horizon_s_ = this->declare_parameter<double>("window_validity_horizon_s", 2.0);
  // ⚠ **[0a] 실측 대상**: 계약 불변식 W5(A8)는 `window_valid_until − now ≥ u_max/a_max + Δt_h` 를
  // 요구하지만 u_max·a_max 는 로봇 동역학 상수가 확정돼야 나오고 그 확정은 미결이다
  // (계약 U-01 / architecture Q-11·Q-16). 이 값은 **W5 를 만족한다는 주장이 아니라 배관 실측용
  // placeholder** 이며, [0a] 더미는 W5 를 검사하지 않는다. 이 값을 근거로 W5 를 확정하지 말 것.
  map_query_retry_period_s_ = this->declare_parameter<double>(
    "map_query_retry_period_s", 1.0); // 배관 — MapRegistry 기동 대기 재시도 주기
  map_query_timeout_s_ = this->declare_parameter<double>(
    "map_query_timeout_s", 2.0); // 배관 — 응답 없는 요청 회수 시한(nav2 §A2 pending 누수 방지)

  if (robot_count_ <= 0)
  {
    RCLCPP_ERROR(
      this->get_logger(), "robot_count=%d 은 유효하지 않다 — 1 로 보정한다", robot_count_);
    robot_count_ = 1;
  }
  if (segments_per_window_ <= 0)
  {
    segments_per_window_ = 1;
  }

  // 계약 §0 시각 규약: 절대 시각은 전부 시뮬 시계 기준이며 벽시계와 혼용하지 않는다.
  if (!this->get_parameter("use_sim_time").as_bool())
  {
    RCLCPP_WARN(
      this->get_logger(),
      "use_sim_time=false — 계약 §0 은 시뮬 시계를 요구한다. bringup 이 use_sim_time:=true 를 "
      "설정하지 않으면 window_valid_until 이 L4 시계와 다른 축에 놓인다");
  }

  if (this->get_parameter("reorder_transport").as_string() == "service")
  {
    // 계약 §3: `/sadg_service/reorder_passing` 은 **기본 미노출**이며 이 param 이 service 일 때만
    // 노출된다. [0a] 에는 R1(BTPG)도 R2(MILP)도 없어 접수/거부를 판정할 근거가 없다 —
    // 서비스를 띄우고 아무 ack 나 돌려주면 호출자에게 거짓을 말하게 되므로 **띄우지 않는다**.
    RCLCPP_WARN(
      this->get_logger(),
      "reorder_transport=service 이지만 [0a] 더미는 R1/R2 가 없어 /sadg_service/reorder_passing 을 "
      "노출하지 않는다 — 재조정 배선은 로드맵 [3] 대상이다");
  }

  // ── 진행 추적 초기화 — 로봇별 마지막 유효 점유 노드 (이탈 판정 기준). 기동 시 "미상"(센티넬).
  //    릴리스 상태(프론티어·window_seq)는 depgraph 가 소유하므로 여기서 들지 않는다.
  last_occupied_node_.assign(static_cast<std::size_t>(robot_count_), UNIFORM_NODE_ID_NONE);

  // ── 콜백그룹 (계약 Q-3 — 릴리스 경로를 무거운 경로에서 격리) ────────────
  release_cb_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  // 두 번째 인자 false = 노드 기본 실행자에 자동 등록하지 않는다. 전용 실행자가 이 그룹을 돈다.
  ingest_cb_group_ =
    this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive, false);

  rclcpp::SubscriptionOptions ingest_options;
  ingest_options.callback_group = ingest_cb_group_;

  // ── 구독·발행 (QoS 는 계약 §3 정본표) ───────────────────────────────────
  rclcpp::QoS planned_paths_qos(5);
  planned_paths_qos.reliable();
  planned_paths_qos.durability_volatile();
  planned_paths_sub_ = this->create_subscription<mrs_interfaces::msg::PlannedPaths>(
    PLANNED_PATHS_TOPIC, planned_paths_qos,
    std::bind(&SadgServiceNode::on_planned_paths, this, std::placeholders::_1), ingest_options);

  rclcpp::QoS window_qos(5);
  window_qos.reliable();
  window_qos.durability_volatile(); // transient_local 금지 — 늦게 뜬 노드의 좀비 창 재생 차단(§1.5)

  rclcpp::QoS escalation_qos(20);
  escalation_qos.reliable();
  escalation_qos.durability_volatile();

  // robot_state QoS 는 계약 §3 정본표 = best_effort / volatile / depth 1 (sim_bridge 발행과 정합).
  rclcpp::QoS robot_state_qos(1);
  robot_state_qos.best_effort();
  robot_state_qos.durability_volatile();

  escalation_report_subs_.reserve(static_cast<std::size_t>(robot_count_));
  robot_state_subs_.reserve(static_cast<std::size_t>(robot_count_));
  execution_window_pubs_.reserve(static_cast<std::size_t>(robot_count_));
  for (int index = 0; index < robot_count_; ++index)
  {
    const std::string ns = "/robot_" + std::to_string(index);
    const std::size_t robot_index = static_cast<std::size_t>(index);

    execution_window_pubs_.push_back(this->create_publisher<mrs_interfaces::msg::ExecutionWindow>(
      ns + "/execution_window", window_qos));

    robot_state_subs_.push_back(this->create_subscription<mrs_interfaces::msg::RobotState>(
      ns + "/robot_state", robot_state_qos,
      [this, robot_index](const mrs_interfaces::msg::RobotState::SharedPtr msg)
      {
        this->on_robot_state(robot_index, msg);
      },
      ingest_options));

    escalation_report_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::EscalationReport>(
        ns + "/escalation_report", escalation_qos,
        [this, robot_index](const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
        {
          this->on_escalation_report(robot_index, msg);
        },
        ingest_options));
  }

  // 계측 토픽 2종: [0a] 는 발행하지 않는다(R1/R2/judge 가 없어 가로대 사건이 발생하지 않는다).
  // 배선만 남기는 이유는 architecture §1.2 가 이 엣지를 선언했고 [3] 이 그대로 쓰기 때문이다.
  rclcpp::QoS metrics_qos(50);
  metrics_qos.reliable();
  metrics_qos.durability_volatile();
  judge_verdict_pub_ =
    this->create_publisher<mrs_interfaces::msg::JudgeVerdict>("/ladder/judge_verdict", metrics_qos);
  rung_event_pub_ =
    this->create_publisher<mrs_interfaces::msg::RungEvent>("/ladder/rung_event", metrics_qos);

  // ── MapRegistry 조회 (뷰 스코프의 유일 출처) ────────────────────────────
  uniform_view_client_ = this->create_client<mrs_interfaces::srv::GetUniformView>(
    UNIFORM_VIEW_SERVICE, rclcpp::ServicesQoS(), ingest_cb_group_);

  const double retry_period =
    (std::isfinite(map_query_retry_period_s_) && map_query_retry_period_s_ > 0.0)
      ? map_query_retry_period_s_
      : 1.0;
  map_query_timer_ = this->create_timer(
    std::chrono::duration<double>(retry_period),
    std::bind(&SadgServiceNode::on_map_query_timer, this), ingest_cb_group_);

  const double release_period =
    (std::isfinite(window_release_period_s_) && window_release_period_s_ > 0.0)
      ? window_release_period_s_
      : 0.5;
  release_timer_ = this->create_timer(
    std::chrono::duration<double>(release_period),
    std::bind(&SadgServiceNode::on_release_timer, this), release_cb_group_);

  // ⚠ 마지막에 띄운다 — 이 줄 이후로 ingest 그룹의 콜백이 다른 스레드에서 돌기 시작한다.
  ingest_thread_ = std::make_unique<mrs::ros_sadg::CallbackGroupThread>(
    ingest_cb_group_, this->get_node_base_interface(), this->get_logger());

  RCLCPP_INFO(
    this->get_logger(),
    "sadg_service started ([2] depgraph 배선) — robot_count=%d release_period_s=%.3f "
    "validity_horizon_s=%.3f (창당 세그먼트 수는 depgraph 의존성 프론티어가 결정)",
    robot_count_, release_period, window_validity_horizon_s_);
}

double SadgServiceNode::now_seconds() const
{
  return this->get_clock()->now().seconds();
}

bool SadgServiceNode::current_scope(ViewScope & out) const
{
  const std::lock_guard<std::mutex> lock(state_mutex_);
  if (!scope_ready_)
  {
    return false;
  }

  out = view_scope_;
  return true;
}

void SadgServiceNode::report_convert_failure(
  const char * context, const mrs::convert::ConvertResult & result)
{
  if (result.ok)
  {
    return;
  }

  // 두 콜백그룹(릴리스·수신)이 서로 다른 스레드에서 이 함수를 부르므로 카운터에 경계를 준다.
  std::uint64_t reason_count = 0;
  std::uint64_t total_count = 0;
  {
    const std::lock_guard<std::mutex> lock(diag_mutex_);
    reason_count = convert_diag_.record(result.reason);
    total_count = convert_diag_.total();
  }

  if (mrs::ros_sadg::is_unimplemented_convert(result))
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "%s: mrs_msg_convert 함수가 아직 미구현 스텁이다 — 메시지를 해석/발행할 수 없다 "
      "(누적 %lu건). [0a] 배관 차단 항목이며 convert 본문 구현 후 해소된다",
      context, static_cast<unsigned long>(reason_count));
    return;
  }

  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "%s: 변환 실패 reason=%s (해당 사유 누적 %lu건, 전체 %lu건) — 폐기하고 현상 유지", context,
    mrs::convert::to_string(result.reason), static_cast<unsigned long>(reason_count),
    static_cast<unsigned long>(total_count));
}

bool SadgServiceNode::reclaim_stale_map_query()
{
  if (!map_query_pending_)
  {
    return true;
  }

  const double waited_s = now_seconds() - map_query_sent_at_s_;
  if (waited_s < map_query_timeout_s_)
  {
    return false; // 아직 기다릴 시간이다. 중복 요청을 보내지 않는다.
  }

  // 응답 없는 요청을 방치하면 장시간 실험에서 pending 이 누적된다(nav2-reference §A2).
  (void)uniform_view_client_->remove_pending_request(map_query_request_id_);
  map_query_pending_ = false;
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "GetUniformView 응답이 %.1f s 안에 오지 않아 요청을 회수했다 — 재시도한다", waited_s);
  return true;
}

void SadgServiceNode::send_uniform_view_request()
{
  auto request = std::make_shared<mrs_interfaces::srv::GetUniformView::Request>();
  request->roadmap_version = 0U; // 0 = 최신 (GetUniformView.srv 규약)

  auto future_and_id = uniform_view_client_->async_send_request(
    request, std::bind(&SadgServiceNode::on_uniform_view_response, this, std::placeholders::_1));
  map_query_request_id_ = future_and_id.request_id;
  map_query_sent_at_s_ = now_seconds();
  map_query_pending_ = true;
}

void SadgServiceNode::on_map_query_timer()
{
  try
  {
    ViewScope scope;
    if (current_scope(scope))
    {
      if (map_query_timer_)
      {
        map_query_timer_->cancel();
      }
      return;
    }

    if (!reclaim_stale_map_query())
    {
      return;
    }

    if (!uniform_view_client_->service_is_ready())
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "%s 서비스 대기 중 — 뷰 스코프 미확보 상태에서는 창을 릴리스하지 않는다",
        UNIFORM_VIEW_SERVICE);
      return;
    }

    send_uniform_view_request();
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_map_query_timer 실패 (안전 폴백: 다음 주기 재시도): %s", e.what());
  }
}

bool SadgServiceNode::adopt_uniform_view(
  const mrs_interfaces::srv::GetUniformView::Response & response)
{
  ViewScope scope;
  scope.view_kind = ViewKind::UNIFORM; // 계약 §0.1 정본표: 이 응답의 노드는 균일 뷰다
  scope.roadmap_version = response.roadmap_version;
  scope.view_id = response.view_id;

  if (!is_usable_scope(scope))
  {
    // roadmap_version = 0 은 계약이 런타임에 금지한다. 이 값으로 창을 내면 L4 가 100% 폐기한다.
    RCLCPP_ERROR(
      this->get_logger(),
      "GetUniformView 가 사용 불가 스코프를 응답했다 (roadmap_version=%lu view_id=%u) — 폐기",
      static_cast<unsigned long>(scope.roadmap_version), scope.view_id);
    return false;
  }

  {
    const std::lock_guard<std::mutex> lock(state_mutex_);
    view_scope_ = scope;
    scope_ready_ = true;
  }

  RCLCPP_INFO(
    this->get_logger(), "뷰 스코프 확보: roadmap_version=%lu view_id=%u unit_length_m=%.3f",
    static_cast<unsigned long>(scope.roadmap_version), scope.view_id, response.unit_length_m);
  return true;
}

void SadgServiceNode::on_uniform_view_response(
  rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture future)
{
  try
  {
    map_query_pending_ = false;

    const auto response = future.get();
    if (response == nullptr)
    {
      RCLCPP_WARN(this->get_logger(), "GetUniformView 응답이 비었다 — 재시도한다");
      return;
    }

    if (response->result != mrs_interfaces::srv::GetUniformView::Response::RESULT_OK)
    {
      RCLCPP_WARN(
        this->get_logger(), "GetUniformView result=%u (OK 아님) — 재시도한다",
        static_cast<unsigned>(response->result));
      return;
    }

    if (adopt_uniform_view(*response) && map_query_timer_)
    {
      map_query_timer_->cancel();
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_uniform_view_response 실패 (안전 폴백: 릴리스 없음): %s", e.what());
  }
}

void SadgServiceNode::log_receive_latency(
  const char * label, const builtin_interfaces::msg::Time & stamp, double & out_latency_s)
{
  out_latency_s = 0.0;

  // 시각 변환은 반드시 convert 헬퍼를 거친다(R-18 구속) — 음수·NaN·범위 가드가 그 헬퍼에만 있다.
  double published_at_s = 0.0;
  const mrs::convert::ConvertResult result = mrs::convert::time_to_seconds(stamp, published_at_s);
  if (!result.ok)
  {
    // 지연을 못 재는 것과 메시지를 못 쓰는 것은 다르다 — 계측만 포기하고 처리는 계속한다.
    report_convert_failure(label, result);
    return;
  }

  out_latency_s = now_seconds() - published_at_s;
}

void SadgServiceNode::apply_planned_paths(
  const std::vector<RobotPath> & paths, std::uint32_t plan_epoch)
{
  const std::lock_guard<std::mutex> lock(state_mutex_);

  // plan_epoch 는 창 스탬프용 — 내용이 같아 재구축을 건너뛰어도 최신 세대를 창에 실어야 한다.
  current_plan_epoch_ = plan_epoch;

  // build-once: 내용(로봇 집합 + 방문 노드열)이 마지막 구축과 같으면 재구축하지 않는다.
  // 재구축하면 depgraph 의 릴리스/클리어 프론티어가 리셋돼 로봇이 영원히 처음부터 릴리스된다.
  if (graph_built_ && plans_equivalent(last_built_paths_, paths))
  {
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "[2] 계획 내용 동일 — depgraph 재구축 건너뜀(진행 보존), plan_epoch=%u 만 갱신", plan_epoch);
    return;
  }

  // build_from_paths 는 window_seq 를 재빌드 간 보존한다(W1). 스코프는 depgraph 에 그대로 넘긴다.
  if (!graph_.build_from_paths(paths, view_scope_.roadmap_version, view_scope_.view_id))
  {
    RCLCPP_WARN(
      this->get_logger(),
      "[2] build_from_paths 실패 (paths=%zu roadmap_version=%lu) — 구계획 유지(안전 폴백)",
      paths.size(), static_cast<unsigned long>(view_scope_.roadmap_version));
    return;
  }

  last_built_paths_ = paths;
  graph_built_ = true;
  // 새 내용으로 그래프가 프론티어를 리셋했으므로 진행 추적도 초기화한다 — 재구축 직후 첫 관측이
  // 구계획 기준 이탈을 새 그래프에 잘못 흘리지 않게 한다.
  std::fill(last_occupied_node_.begin(), last_occupied_node_.end(), UNIFORM_NODE_ID_NONE);

  RCLCPP_INFO(
    this->get_logger(), "[2] depgraph 재구축: plan_epoch=%u robots=%zu", plan_epoch, paths.size());
}

// `plan_epoch` 은 봉투 필드라 변환 결과에 실리지 않는다(계약 §0.3) — 메시지에서 직접 읽는다.
// 노드 id 가 아니므로 규칙 V3 위반이 아니다.
void SadgServiceNode::on_planned_paths(const mrs_interfaces::msg::PlannedPaths::SharedPtr msg)
{
  try
  {
    if (msg == nullptr)
    {
      return;
    }

    ViewScope expected;
    if (!current_scope(expected))
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "/planned_paths 수신했으나 뷰 스코프 미확보 — 대조할 기준이 없어 폐기한다(안전 폴백)");
      return;
    }

    double latency_s = 0.0;
    log_receive_latency("time_to_seconds(PlannedPaths.header.stamp)", msg->header.stamp, latency_s);

    std::vector<RobotPath> paths;
    const mrs::convert::ConvertResult result = mrs::convert::from_msg(*msg, expected, paths);
    if (!result.ok)
    {
      report_convert_failure("from_msg(PlannedPaths)", result);
      return; // 폐기 + 현상 유지. 절반만 해석한 계획으로 대기열을 갱신하지 않는다.
    }

    RCLCPP_INFO(
      this->get_logger(), "[2] /planned_paths 수신 plan_epoch=%u paths=%zu 지연=%.6f s",
      msg->plan_epoch, msg->paths.size(), latency_s);
    apply_planned_paths(paths, msg->plan_epoch);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_planned_paths 실패 (안전 폴백: 구계획 유지): %s", e.what());
  }
}

bool SadgServiceNode::release_robot_window_locked(RobotId robot_id, double now_s)
{
  // 의존성 판정·세그먼트 선택·window_seq 부여는 depgraph 소유다. 노드는 봉투 2건만 스탬프한다.
  ExecutionWindow window;
  if (!graph_.release_next_window(robot_id, window))
  {
    return false; // 릴리스할 창 없음 — 막힘(선행 미충족) 또는 소진. 다음 틱 재시도(정상).
  }

  window.plan_epoch = current_plan_epoch_; // 봉투 (depgraph 는 0 으로 둔다)
  window.window_valid_until_s =
    now_s + window_validity_horizon_s_; // 봉투 (W5 입력게이트, [0b] 실측)

  const std::size_t robot_index = static_cast<std::size_t>(robot_id);
  if (robot_index >= execution_window_pubs_.size())
  {
    return false; // robot_id 가 발행자 범위 밖 — 방어적(정상 경로에선 발생하지 않음)
  }

  mrs_interfaces::msg::ExecutionWindow msg;
  const mrs::convert::ConvertResult result = mrs::convert::to_msg(window, now_s, msg);
  if (!result.ok)
  {
    report_convert_failure("to_msg(ExecutionWindow)", result);
    return false; // 발행하지 않는다. depgraph 프론티어는 이미 전진했으므로 이 창은 유실된다.
  }

  execution_window_pubs_[robot_index]->publish(msg);
  ++released_window_count_;
  log_released_window(window, now_s);
  return true;
}

void SadgServiceNode::log_released_window(const ExecutionWindow & window, double now_s)
{
  RCLCPP_INFO(
    this->get_logger(),
    "[2] 창 릴리스 robot=%u window_seq=%u plan_epoch=%u segments=%zu preds=%zu "
    "valid_until_s=%.6f stamp_s=%.6f",
    static_cast<unsigned>(window.robot_id), window.window_seq, window.plan_epoch,
    window.segments.size(), window.predecessor_constraints.size(), window.window_valid_until_s,
    now_s);
  if (!window.predecessor_constraints.empty())
  {
    const PredecessorConstraint & pred = window.predecessor_constraints.front();
    RCLCPP_INFO(
      this->get_logger(),
      "[2]   → robot=%u 는 노드 %u 앞에서 대기: 선행 robot=%u 가 그 노드를 클리어해야 진입",
      static_cast<unsigned>(window.robot_id), pred.node_id.value(),
      static_cast<unsigned>(pred.predecessor_robot_id));
  }
}

void SadgServiceNode::on_release_timer()
{
  try
  {
    const double now_s = now_seconds();

    const std::lock_guard<std::mutex> lock(state_mutex_);
    if (!scope_ready_ || !graph_built_)
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "스코프/계획 미확보(scope=%d built=%d) — 릴리스하지 않는다(안전 폴백: 로봇 정지 유지)",
        static_cast<int>(scope_ready_), static_cast<int>(graph_built_));
      return;
    }

    // 타이머는 릴리스 트리거일 뿐 — 막힌 로봇은 false 를 돌려주고 다음 틱에 재시도된다.
    for (int index = 0; index < robot_count_; ++index)
    {
      (void)release_robot_window_locked(static_cast<RobotId>(index), now_s);
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_release_timer 실패 (안전 폴백: 릴리스 생략): %s", e.what());
  }
}

void SadgServiceNode::advance_progress_locked(
  std::size_t robot_index, RobotId robot_id, UniformNodeId occupied_node, double time_s)
{
  if (robot_index >= last_occupied_node_.size())
  {
    return; // 방어적 — 범위 밖 로봇 인덱스
  }

  const UniformNodeId prev = last_occupied_node_[robot_index];
  // 이탈 판정: 직전에 유효 노드 X 를 점유하고 있었고, 이번 관측이 X 가 아니면(센티넬이든 다른
  // 노드든) 로봇이 X 를 **클리어(이탈)**한 것이다. "도착"이 아니라 "이탈"이 진행의 정의다.
  if (!prev.is_none() && occupied_node != prev)
  {
    graph_.on_progress_event(robot_id, prev, time_s);
    RCLCPP_INFO(
      this->get_logger(), "[2] 진행: robot=%u 노드 %u 이탈(clear) at sim_t=%.6f",
      static_cast<unsigned>(robot_id), prev.value(), time_s);
  }
  last_occupied_node_[robot_index] = occupied_node;
}

void SadgServiceNode::on_robot_state(
  std::size_t robot_index, const mrs_interfaces::msg::RobotState::SharedPtr msg)
{
  try
  {
    if (msg == nullptr)
    {
      return;
    }

    const std::lock_guard<std::mutex> lock(state_mutex_);
    if (!scope_ready_ || !graph_built_)
    {
      return; // 스코프/그래프 미확보 — 진행을 반영할 대상이 없다(조용히 무시, 안전 폴백)
    }

    RobotObservation observation;
    const mrs::convert::ConvertResult result =
      mrs::convert::from_msg(*msg, view_scope_, observation);
    if (!result.ok)
    {
      report_convert_failure("from_msg(RobotState)", result); // 스코프 불일치 포함 → skip
      return;
    }

    // 시각은 RobotState.header.stamp(시뮬 시각). 변환 실패 시 진행을 이번엔 반영하지 않는다.
    double sim_time_s = 0.0;
    const mrs::convert::ConvertResult time_result =
      mrs::convert::time_to_seconds(msg->header.stamp, sim_time_s);
    if (!time_result.ok)
    {
      report_convert_failure("time_to_seconds(RobotState.header.stamp)", time_result);
      return;
    }

    advance_progress_locked(
      robot_index, static_cast<RobotId>(robot_index), observation.occupied_node, sim_time_s);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_robot_state 실패 (안전 폴백: 진행 미반영): %s", e.what());
  }
}

void SadgServiceNode::log_escalation_report(
  std::size_t robot_index, EscalationReason reason,
  const mrs_interfaces::msg::EscalationReport & msg, double latency_s)
{
  RCLCPP_INFO(
    this->get_logger(),
    "[2] 에스컬레이션 수신 robot=%zu reason=%s severity=%s event_id=%lu window_seq=%u "
    "지연=%.6f s (누적 %lu건, 라우팅 없음 — ladder 소관)",
    robot_index, escalation_reason_name(reason),
    severity_of(reason) == EscalationSeverity::HARD ? "HARD" : "SOFT",
    static_cast<unsigned long>(msg.event_id), msg.window_seq, latency_s,
    static_cast<unsigned long>(escalation_report_count_));
}

// 로그만 남긴다 — 라우팅은 `ladder_orchestrator` 소관이다(D-08). 특히 SYNC_LOST 는 HARD 이지만
// R3/R4 로 보내면 안 되는 값이며(R-06), 그 판단은 이 노드의 것이 아니다.
void SadgServiceNode::on_escalation_report(
  std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
{
  try
  {
    if (msg == nullptr)
    {
      return;
    }

    ViewScope expected;
    if (!current_scope(expected))
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "robot_%zu 에스컬레이션 수신했으나 뷰 스코프 미확보 — 폐기", robot_index);
      return;
    }

    EscalationReason reason{};
    const mrs::convert::ConvertResult result =
      mrs::convert::reason_from_msg(*msg, expected, reason);
    if (!result.ok)
    {
      report_convert_failure("reason_from_msg(EscalationReport)", result);
      return;
    }

    ++escalation_report_count_;
    double latency_s = 0.0;
    log_receive_latency(
      "time_to_seconds(EscalationReport.header.stamp)", msg->header.stamp, latency_s);
    log_escalation_report(robot_index, reason, *msg, latency_s);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_escalation_report 실패 (안전 폴백: 무시): %s", e.what());
  }
}

} // namespace mrs
