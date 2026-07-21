/**
 * @file pp_service_node.cpp
 * @brief @ref mrs::PpServiceNode 본문 — [0a] tracer bullet 더미 L2 노드.
 *
 * ## 이 파일이 지키는 규율
 *  1. **콜백 안에 알고리즘 없음** — 경로는 `IPathSolver`(더미), 변환은 `mrs_msg_convert` 가 만든다.
 *  2. **모든 콜백은 try/catch** — 노드가 죽지 않는다(CLAUDE.md 규율 2). 실패는 로그 + 안전 폴백.
 *  3. **판정은 convert, 보고는 노드**(R-15 (b)) — 변환 실패는 사유별 카운터 + 스로틀 로그 + 폐기.
 *  4. **시각 변환은 `mrs_msg_convert` 헬퍼 경유**(R-18) — 이 파일에 `1e9` 나눗셈이 없다.
 *  5. **수치 창작 금지** — 미정 값은 ROS2 param 으로 노출하고 근거를 주석에 남긴다.
 *  6. 콜백은 짧게 유지한다 — 경계 검사(`tools/check_boundaries.sh` 항목 3)의 길이 상한은
 *     "얇은 I/O 어댑터"를 기계로 지키게 하는 장치다.
 */

#include "mrs_ros_pp/pp_service_node.hpp"

#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
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
/** @brief `/task_assignment` 토픽 이름 (계약 §3 정본표). */
constexpr const char * TASK_ASSIGNMENT_TOPIC = "/task_assignment";
/** @brief 스로틀 로그 주기 [ms]. 반복 로그가 콘솔을 덮지 않게 한다. */
constexpr int LOG_THROTTLE_MS = 5000;

/**
 * @brief 솔버 종료 상태를 로그용 문자열로 바꾼다.
 * @param[in] status 솔버 종료 상태. 자료형 `mrs::SolverStatus`.
 * @return `const char *` — 널 종료 정적 문자열. 알 수 없는 값이면 `"UNKNOWN"`.
 */
[[nodiscard]] const char * solver_status_name(SolverStatus status) noexcept
{
  switch (status)
  {
    case SolverStatus::SUCCESS:
      return "SUCCESS";
    case SolverStatus::INFEASIBLE:
      return "INFEASIBLE";
    case SolverStatus::BUDGET_EXCEEDED:
      return "BUDGET_EXCEEDED";
    case SolverStatus::STALE_VERSION:
      return "STALE_VERSION";
    case SolverStatus::INTERNAL_ERROR:
      return "INTERNAL_ERROR";
  }
  return "UNKNOWN";
}

} // namespace

PpServiceNode::PpServiceNode()
: rclcpp::Node("pp_service"),
  canned_solver_(nullptr),
  planner_(nullptr)
{
  // ── 파라미터 ─────────────────────────────────────────────────────────────
  // 값의 성격 3구분: (문헌치) 이론·계약이 준 값 / (배관) 실행 편의 상수 /
  // ([0a] 실측 대상) 지금 정할 근거가 없어 실측 후 확정할 값. 지어낸 값은 없다.
  robot_count_ = this->declare_parameter<int>("robot_count", 2); // [0a] = 2대 (architecture §7)
  segment_duration_s_ = this->declare_parameter<double>(
    "segment_duration_s", 0.5); // 문헌치 — 계약 Q-11 세그먼트 Δt = 0.5 s
  canned_lap_count_ = this->declare_parameter<int>(
    "canned_lap_count", 4); // 배관 — 세그먼트 2×4 = 8개. 창 릴리스를 여러 번 관측하기 위한 길이
  plan_budget_s_ = this->declare_parameter<double>(
    "plan_budget_s", 1.0); // 문헌치 — 확정서 realtime_requirement ≤ 1 s/스텝. 더미는 소비하지 않는다
  map_query_retry_period_s_ = this->declare_parameter<double>(
    "map_query_retry_period_s", 1.0); // 배관 — MapRegistry 기동 대기 재시도 주기
  map_query_timeout_s_ = this->declare_parameter<double>(
    "map_query_timeout_s", 2.0); // 배관 — 응답 없는 요청의 회수 시한(nav2 §A2 pending 누수 방지)
  auto_plan_period_s_ = this->declare_parameter<double>(
    "auto_plan_period_s", 0.0); // [0a] 실측 대상 — 0 = 기동 시 1회. >0 이면 지연 표본을 반복 채집

  if (robot_count_ <= 0)
  {
    RCLCPP_ERROR(
      this->get_logger(), "robot_count=%d 은 유효하지 않다 — 1 로 보정한다", robot_count_);
    robot_count_ = 1;
  }
  if (!std::isfinite(segment_duration_s_) || segment_duration_s_ <= 0.0)
  {
    RCLCPP_ERROR(this->get_logger(), "segment_duration_s 가 유효하지 않다 — 계획을 발행하지 못한다");
  }
  if (canned_lap_count_ < 0)
  {
    canned_lap_count_ = 0;
  }

  // 계약 §0 시각 규약: 모든 절대 시각은 시뮬 시계(`use_sim_time: true`) 기준이며 벽시계와
  // 혼용하지 않는다. 위반은 창 만료 판정(불변식 W5)을 엉뚱한 시점으로 옮기므로 크게 경고한다.
  if (!this->get_parameter("use_sim_time").as_bool())
  {
    RCLCPP_WARN(
      this->get_logger(),
      "use_sim_time=false — 계약 §0 은 시뮬 시계를 요구한다. bringup 이 use_sim_time:=true 를 "
      "설정하지 않으면 이 노드의 stamp 가 L3/L4 의 시계와 다른 축에 놓인다");
  }

  // ── seam a: 더미 솔버 주입 (Composition Root 는 [1] 이후 mrs_bringup 이 맡는다) ──────
  canned_solver_ = std::make_shared<mrs::ros_pp::CannedSolver>(
    segment_duration_s_, static_cast<std::uint32_t>(canned_lap_count_));
  planner_.set_solver(canned_solver_);

  // ── 발행·구독·서비스 (QoS 는 계약 §3 정본표) ────────────────────────────
  rclcpp::QoS planned_paths_qos(5);
  planned_paths_qos.reliable();
  planned_paths_qos.durability_volatile();
  planned_paths_pub_ = this->create_publisher<mrs_interfaces::msg::PlannedPaths>(
    PLANNED_PATHS_TOPIC, planned_paths_qos);

  rclcpp::QoS task_assignment_qos(5);
  task_assignment_qos.reliable();
  task_assignment_qos.durability_volatile();
  task_assignment_sub_ = this->create_subscription<mrs_interfaces::msg::TaskAssignment>(
    TASK_ASSIGNMENT_TOPIC, task_assignment_qos,
    std::bind(&PpServiceNode::on_task_assignment, this, std::placeholders::_1));

  plan_paths_srv_ = this->create_service<mrs_interfaces::srv::PlanPaths>(
    "/pp_service/plan_paths",
    std::bind(&PpServiceNode::on_plan_paths, this, std::placeholders::_1, std::placeholders::_2));
  partial_replan_srv_ = this->create_service<mrs_interfaces::srv::PartialReplan>(
    "/pp_service/partial_replan",
    std::bind(
      &PpServiceNode::on_partial_replan, this, std::placeholders::_1, std::placeholders::_2));

  // ── MapRegistry 조회 (뷰 스코프의 유일 출처) ────────────────────────────
  uniform_view_client_ =
    this->create_client<mrs_interfaces::srv::GetUniformView>(UNIFORM_VIEW_SERVICE);

  const double retry_period =
    (std::isfinite(map_query_retry_period_s_) && map_query_retry_period_s_ > 0.0)
      ? map_query_retry_period_s_
      : 1.0;
  map_query_timer_ = this->create_timer(
    std::chrono::duration<double>(retry_period),
    std::bind(&PpServiceNode::on_map_query_timer, this));

  if (std::isfinite(auto_plan_period_s_) && auto_plan_period_s_ > 0.0)
  {
    auto_plan_timer_ = this->create_timer(
      std::chrono::duration<double>(auto_plan_period_s_),
      [this]()
      {
        // 주기 재발행 — [0a] 의 "발행→수신 지연" 표본을 여러 번 얻기 위한 경로다.
        (void)this->publish_canned_plan("auto_plan_timer");
      });
  }

  RCLCPP_INFO(
    this->get_logger(),
    "pp_service started ([0a] dummy) — solver=%s robot_count=%d segment_duration_s=%.3f "
    "lap_count=%d auto_plan_period_s=%.3f",
    planner_.solver() != nullptr ? planner_.solver()->name().c_str() : "none", robot_count_,
    segment_duration_s_, canned_lap_count_, auto_plan_period_s_);
}

double PpServiceNode::now_seconds() const
{
  // `use_sim_time: true` 이면 노드 시계가 `/clock` 을 따른다. 벽시계를 쓰지 않는 이유가 그것이다.
  return this->get_clock()->now().seconds();
}

void PpServiceNode::report_convert_failure(
  const char * context, const mrs::convert::ConvertResult & result)
{
  if (result.ok)
  {
    return;
  }

  const std::uint64_t reason_count = convert_diag_.record(result.reason);

  if (mrs::ros_pp::is_unimplemented_convert(result))
  {
    // 배관이 막힌 것과 메시지가 잘못된 것을 구별해 남긴다 — 뭉개면 [0a] 실측이 원인을 잃는다.
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "%s: mrs_msg_convert 함수가 아직 미구현 스텁이다 — 메시지를 발행/해석할 수 없다 "
      "(누적 %lu건). [0a] 배관 차단 항목이며 convert 본문 구현 후 해소된다",
      context, static_cast<unsigned long>(reason_count));
    return;
  }

  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "%s: 변환 실패 reason=%s (해당 사유 누적 %lu건, 전체 %lu건) — 폐기하고 현상 유지", context,
    mrs::convert::to_string(result.reason), static_cast<unsigned long>(reason_count),
    static_cast<unsigned long>(convert_diag_.total()));
}

bool PpServiceNode::reclaim_stale_map_query()
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

void PpServiceNode::send_uniform_view_request()
{
  auto request = std::make_shared<mrs_interfaces::srv::GetUniformView::Request>();
  request->roadmap_version = 0U; // 0 = 최신 (GetUniformView.srv 규약)

  auto future_and_id = uniform_view_client_->async_send_request(
    request, std::bind(&PpServiceNode::on_uniform_view_response, this, std::placeholders::_1));
  map_query_request_id_ = future_and_id.request_id;
  map_query_sent_at_s_ = now_seconds();
  map_query_pending_ = true;
}

void PpServiceNode::on_map_query_timer()
{
  try
  {
    if (scope_ready_)
    {
      // 스코프를 확보했으면 더 물어볼 것이 없다. (지도 개정 무효화는 [1] 의
      // `/map_registry/roadmap_version` latched 구독으로 붙는다 — 계약 Q-6.)
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
        "%s 서비스 대기 중 — 뷰 스코프 미확보 상태에서는 계획을 발행하지 않는다",
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

bool PpServiceNode::adopt_uniform_view(
  const mrs_interfaces::srv::GetUniformView::Response & response)
{
  ViewScope scope;
  scope.view_kind = ViewKind::UNIFORM; // 계약 §0.1 정본표: 이 응답의 노드는 균일 뷰다
  scope.roadmap_version = response.roadmap_version;
  scope.view_id = response.view_id;

  if (!is_usable_scope(scope))
  {
    // roadmap_version = 0 은 계약이 런타임에 금지한다. 이 값으로 발행하면 수신자가 100% 폐기한다.
    RCLCPP_ERROR(
      this->get_logger(),
      "GetUniformView 가 사용 불가 스코프를 응답했다 (roadmap_version=%lu view_id=%u) — 폐기",
      static_cast<unsigned long>(scope.roadmap_version), scope.view_id);
    return false;
  }

  if (!build_canned_routes(response, scope))
  {
    RCLCPP_ERROR(
      this->get_logger(),
      "균일 뷰에서 고정 라우트를 만들지 못했다 (nodes=%zu edges=%zu) — 계획을 발행하지 않는다",
      response.nodes.size(), response.edges.size());
    return false;
  }

  view_scope_ = scope;
  scope_ready_ = true;
  RCLCPP_INFO(
    this->get_logger(), "뷰 스코프 확보: roadmap_version=%lu view_id=%u unit_length_m=%.3f routes=%zu",
    static_cast<unsigned long>(scope.roadmap_version), scope.view_id, response.unit_length_m,
    canned_routes_.size());
  return true;
}

void PpServiceNode::on_uniform_view_response(
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

    if (!adopt_uniform_view(*response))
    {
      return; // 사유는 adopt_uniform_view 가 이미 남겼다. 다음 주기에 재시도한다.
    }

    if (map_query_timer_)
    {
      map_query_timer_->cancel();
    }

    (void)publish_canned_plan("uniform_view_ready");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_uniform_view_response 실패 (안전 폴백: 미발행): %s", e.what());
  }
}

bool PpServiceNode::wrap_edge_nodes(
  const mrs_interfaces::msg::RoadmapEdge & edge, mrs::ros_pp::CannedRoute & route)
{
  // `uint32` → 강타입 랩은 `mrs_msg_convert` 단독 소유다(규칙 V3). 강타입 생성자를 직접 부르지 않는다.
  const mrs::convert::ConvertResult node_a_result = mrs::convert::node_id_from_msg(
    edge.node_a, mrs::convert::NoneNodePolicy::REJECT, route.node_a);
  if (!node_a_result.ok)
  {
    report_convert_failure("node_id_from_msg(edge.node_a)", node_a_result);
    return false;
  }

  const mrs::convert::ConvertResult node_b_result = mrs::convert::node_id_from_msg(
    edge.node_b, mrs::convert::NoneNodePolicy::REJECT, route.node_b);
  if (!node_b_result.ok)
  {
    report_convert_failure("node_id_from_msg(edge.node_b)", node_b_result);
    return false;
  }

  return true;
}

// 로봇 i 는 엣지 (i mod E) 위를 왕복한다. **엣지에서만** 라우트를 만드는 이유는 계약 불변식 W2
// (세그먼트 양 끝이 균일 뷰에서 인접)이다 — 임의 노드 쌍을 쓰면 [0a] 배관이 계약 위반 창을 낸다.
bool PpServiceNode::build_canned_routes(
  const mrs_interfaces::srv::GetUniformView::Response & response, const ViewScope & scope)
{
  if (response.edges.empty())
  {
    return false;
  }

  std::vector<mrs::ros_pp::CannedRoute> routes;
  routes.reserve(static_cast<std::size_t>(robot_count_));

  for (int index = 0; index < robot_count_; ++index)
  {
    const std::size_t edge_index = static_cast<std::size_t>(index) % response.edges.size();

    mrs::ros_pp::CannedRoute route;
    route.robot_id = static_cast<RobotId>(index);
    if (!wrap_edge_nodes(response.edges[edge_index], route))
    {
      return false;
    }

    routes.push_back(route);
  }

  canned_routes_ = routes;
  canned_solver_->set_routes(std::move(routes), scope);
  return canned_solver_->route_count() > 0U;
}

void PpServiceNode::fill_plan_input(PathSolverInput & input) const
{
  input.uniform_view = nullptr; // 더미는 그래프를 읽지 않는다 — 스코프 대조만 한다
  input.view_scope = view_scope_;
  input.budget_s = plan_budget_s_;
  input.allow_warm_start = false;
  input.entries.reserve(canned_routes_.size());

  for (const mrs::ros_pp::CannedRoute & route : canned_routes_)
  {
    PlanRequestEntry entry;
    entry.robot_id = route.robot_id;
    entry.start_node = route.node_a;
    entry.goal_node = route.node_b;
    entry.start_theta_rad = 0.0; // [0a] 더미는 자세를 모른다 — 회전 비용은 [1] L2 의 일이다
    input.entries.push_back(entry);
  }
}

bool PpServiceNode::publish_canned_plan(const char * trigger)
{
  try
  {
    if (!scope_ready_ || canned_routes_.empty())
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "%s: 뷰 스코프 미확보 — 계획을 발행하지 않는다(안전 폴백)", trigger);
      return false;
    }

    const double now_s = now_seconds();
    canned_solver_->set_plan_start_time_s(now_s);

    PathSolverInput input;
    fill_plan_input(input);

    // 운용 로직(`WindowedPathPlanner::plan`)은 Phase 5 대상이라 아직 예외를 던진다. [0a] 는
    // 배관만 재므로 seam a 구현체를 직접 호출한다 — "경로는 솔버가 만든다"는 규율은 그대로다.
    const PathSolverOutput output = canned_solver_->solve(input);
    if (output.status != SolverStatus::SUCCESS)
    {
      RCLCPP_ERROR_THROTTLE(
        this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
        "%s: 더미 솔버 실패 status=%s msg='%s' — 미발행(구계획 유지)", trigger,
        solver_status_name(output.status), output.message.c_str());
      return false;
    }

    return publish_plan_message(output.paths, now_s, trigger);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "publish_canned_plan 실패 (안전 폴백: 미발행): %s", e.what());
    return false;
  }
}

bool PpServiceNode::publish_plan_message(
  const std::vector<RobotPath> & paths, double now_s, const char * trigger)
{
  const std::uint32_t next_epoch = plan_epoch_ + 1U; // 단조증가 (계약 L-09)

  mrs_interfaces::msg::PlannedPaths msg;
  const mrs::convert::ConvertResult result = mrs::convert::to_msg(
    paths, /*is_partial=*/false, mrs::EVENT_ID_UNASSIGNED /* 정기 계획은 0 이 정당 */, next_epoch,
    view_scope_, now_s, msg);
  if (!result.ok)
  {
    report_convert_failure("to_msg(PlannedPaths)", result);
    return false;
  }

  planned_paths_pub_->publish(msg);
  plan_epoch_ = next_epoch;
  ++published_plan_count_;
  RCLCPP_INFO(
    this->get_logger(),
    "[0a] /planned_paths 발행 trigger=%s plan_epoch=%u paths=%zu stamp_s=%.6f (누적 %lu건)", trigger,
    plan_epoch_, msg.paths.size(), now_s, static_cast<unsigned long>(published_plan_count_));
  return true;
}

void PpServiceNode::on_task_assignment(const mrs_interfaces::msg::TaskAssignment::SharedPtr msg)
{
  try
  {
    if (msg == nullptr)
    {
      return;
    }

    // ⚠ `pickup_node`·`delivery_node` 를 **읽지 않는다**. 그 값은 물리 뷰 노드 id 이고,
    // `TaskAssignment` → 도메인 변환 함수가 `mrs_msg_convert` 에 없다. 노드가 직접 해석하면
    // 계약 §0.1 V3(변환·대조는 convert 단독 소유) 위반이다. 봉투 필드만 로그에 남긴다.
    RCLCPP_INFO(
      this->get_logger(),
      "[0a] /task_assignment 수신 task_id=%lu robot_id=%u epoch=%u — 내용은 해석하지 않고 "
      "고정 계획 재발행 트리거로만 쓴다",
      static_cast<unsigned long>(msg->task_id), static_cast<unsigned>(msg->robot_id),
      msg->assignment_epoch);

    (void)publish_canned_plan("task_assignment");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_task_assignment 실패 (안전 폴백: 무시): %s", e.what());
  }
}

void PpServiceNode::reject_plan_paths(
  mrs_interfaces::srv::PlanPaths::Response & response, const mrs::convert::ConvertResult & result)
{
  // ⚠ 계약 결손: `PlanPaths.srv` 에는 **형식 위반 전용 응답 코드가 없다**(§0.2 표는
  // `REJECTED_BAD_REQUEST` 를 요구하나 이 srv 에 그 값이 없다). 계약이 형식 위반을
  // `RESULT_INFEASIBLE` 로 뭉개는 것을 명시 금지하므로(솔버 실패와 구별 불가 → F2 계측 오염),
  // 회복 경로가 가장 무해한 `STALE_VERSION`(= 지도 재조회 후 재시도)으로 응답한다.
  // 사유 자체는 폐기 카운터와 `message` 에 남는다. **contract-keeper 판정 대상이다.**
  response.result = mrs_interfaces::srv::PlanPaths::Response::RESULT_STALE_VERSION;
  response.message = std::string("[0a] request rejected: convert reason=") +
                     mrs::convert::to_string(result.reason) +
                     " (PlanPaths.srv 에 BAD_REQUEST 코드 없음 — 계약 결손)";
}

// 요청이 계약에 맞아도 [0a] 는 응답의 `paths` 를 채울 수 없다 — 도메인 `RobotPath` →
// `RobotPath.msg` 변환이 `mrs_msg_convert` 에 없다(`PlannedPaths` 통째 변환만 존재). 노드가 손으로
// 필드를 옮기는 것은 규칙 V3 위반이므로 **구계획 유지**로 수렴시킨다(CLAUDE.md 규율 2).
void PpServiceNode::on_plan_paths(
  const std::shared_ptr<mrs_interfaces::srv::PlanPaths::Request> request,
  std::shared_ptr<mrs_interfaces::srv::PlanPaths::Response> response)
{
  try
  {
    if (request == nullptr || response == nullptr)
    {
      return;
    }

    response->solver_name =
      canned_solver_ != nullptr ? canned_solver_->name() : std::string("none");
    response->plan_epoch = plan_epoch_;
    response->planning_time_s = 0.0;

    std::vector<PlanRequestEntry> entries;
    const mrs::convert::ConvertResult result =
      mrs::convert::from_request(*request, view_scope_, entries);
    if (!result.ok)
    {
      report_convert_failure("from_request(PlanPaths)", result);
      reject_plan_paths(*response, result);
      return;
    }

    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "[0a] PlanPaths 요청 %zu건 접수 — 더미는 응답 경로를 만들지 않는다(구계획 유지)",
      entries.size());
    response->result = mrs_interfaces::srv::PlanPaths::Response::RESULT_BUDGET_EXCEEDED;
    response->message = "[0a] dummy: no response paths — hold plan (see /planned_paths)";
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_plan_paths 실패 (안전 폴백: 구계획 유지): %s", e.what());
    if (response != nullptr)
    {
      response->result = mrs_interfaces::srv::PlanPaths::Response::RESULT_BUDGET_EXCEEDED;
      response->message = "internal error — hold previous plan";
    }
  }
}

void PpServiceNode::reject_partial_replan(
  mrs_interfaces::srv::PartialReplan::Response & response,
  const mrs::convert::ConvertResult & result)
{
  if (result.reason == mrs::convert::ConvertStatus::VIEW_SCOPE_MISMATCH)
  {
    // 계약 §0.2: 인스턴스 스코프 불일치는 STALE_VERSION 으로 **구별해** 응답한다.
    response.result = mrs_interfaces::srv::PartialReplan::Response::RESULT_STALE_VERSION;
    response.message = "view scope mismatch — re-query MapRegistry and retry";
    return;
  }

  if (mrs::ros_pp::is_unimplemented_convert(result))
  {
    // 판정을 할 수 없는 상태다. 요청을 "형식 위반"으로 몰면 호출자에게 거짓을 말하는 것이므로
    // 구계획 유지로 수렴하는 응답을 준다.
    response.result = mrs_interfaces::srv::PartialReplan::Response::RESULT_BUDGET_EXCEEDED;
    response.message = "[0a] dummy: request convert not implemented — hold previous plan";
    return;
  }

  response.result = mrs_interfaces::srv::PartialReplan::Response::RESULT_REJECTED_BAD_REQUEST;
  response.message =
    std::string("bad request: convert reason=") + mrs::convert::to_string(result.reason);
}

// [0a] 는 접합(splice)을 구현하지 않는다. `INFEASIBLE` 로 응답하면 사다리가 R4(재할당)로 상향하므로
// (계약 L-06 의미론) 시스템 거동이 바뀐다 — 더미의 미구현을 알고리즘의 판정으로 둔갑시키지 않기
// 위해 **구계획 유지**로 응답한다.
void PpServiceNode::on_partial_replan(
  const std::shared_ptr<mrs_interfaces::srv::PartialReplan::Request> request,
  std::shared_ptr<mrs_interfaces::srv::PartialReplan::Response> response)
{
  try
  {
    if (request == nullptr || response == nullptr)
    {
      return;
    }

    response->plan_epoch = plan_epoch_;
    response->planning_time_s = 0.0;

    std::vector<CommitState> commit_states;
    std::vector<FrozenOrder> frozen_orders;
    const mrs::convert::ConvertResult result =
      mrs::convert::from_request(*request, view_scope_, commit_states, frozen_orders);
    if (!result.ok)
    {
      report_convert_failure("from_request(PartialReplan)", result);
      reject_partial_replan(*response, result);
      return;
    }

    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "[0a] PartialReplan 접수 (affected=%zu commit=%zu frozen=%zu) — R3 미구현, 구계획 유지",
      request->affected_robots.size(), commit_states.size(), frozen_orders.size());
    response->result = mrs_interfaces::srv::PartialReplan::Response::RESULT_BUDGET_EXCEEDED;
    response->message = "[0a] dummy pp_service: R3 splice not implemented — hold previous plan";
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(
      this->get_logger(), "on_partial_replan 실패 (안전 폴백: 구계획 유지): %s", e.what());
    if (response != nullptr)
    {
      response->result = mrs_interfaces::srv::PartialReplan::Response::RESULT_BUDGET_EXCEEDED;
      response->message = "internal error — hold previous plan";
    }
  }
}

} // namespace mrs
