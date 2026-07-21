/**
 * @file local_traj_node.cpp
 * @brief @ref local_traj_node.hpp 의 구현 — [0a] tracer bullet 판 l4_traj_node.
 *
 * 규율(CLAUDE.md 규율 2 / ros2-conventions): **모든 콜백은 try/catch 로 감싼다.** 잡아서 로깅하고
 * 안전 폴백(정지 지령 또는 현상 유지)한다. 예외로 노드를 죽이지 않는다 — 로봇 1 대의 예외가
 * [0a] 실측 런 전체를 날리면 그 런은 재현이 아니라 재실행 대상이 된다.
 */

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <utility>

#include <rclcpp/serialization.hpp>

#include "mrs/startup_checks.hpp"
#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_msg_convert/msg_convert.hpp"
#include "mrs_ros_l4/local_traj_node.hpp"
#include "mrs_ros_l4/tracer_follower.hpp"

namespace mrs
{

namespace
{

/** @brief 로그 스로틀 간격 [ms] — 10 Hz × N 로그가 콘솔을 덮는 것을 막는다. */
constexpr int THROTTLE_MS = 2000;

} // namespace

LocalTrajNode::LocalTrajNode() : rclcpp::Node("l4_traj_node")
{
  robot_id_ = this->declare_parameter<int>("robot_id", 0);
  neighbor_robot_ids_ = this->declare_parameter<std::vector<std::int64_t>>(
    "neighbor_robot_ids", std::vector<std::int64_t>{});

  // 기대 뷰 스코프. 지도 버전 0 은 "미지정"이며 `/map_registry/roadmap_version`(latched)에서
  // 학습한다. 좌표표를 param 으로 싣지 않는 이유는 tracer_follower.hpp 머리말 참조(C4).
  const std::int64_t version_param =
    this->declare_parameter<std::int64_t>("expected_roadmap_version", 0);
  // 기본값 1 = map_registry 의 `uniform_view_id` 기본값. 0 을 쓰지 않는 이유: 0 은 물리 뷰의
  // 예약값이라 균일 뷰가 0 이면 두 뷰의 (roadmap_version, view_id) 쌍이 같아져 스코프 대조가
  // 물리↔균일 혼동을 못 잡는다. 두 노드의 기본값을 맞춰 두어야 bringup YAML 없이
  // `ros2 run` 으로 띄웠을 때도 스코프가 어긋나지 않는다(어긋나면 전량 폐기 + 침묵 실패).
  const std::int64_t view_id_param = this->declare_parameter<std::int64_t>("expected_view_id", 1);
  expected_scope_.view_kind = mrs::ViewKind::UNIFORM; // 계약 §0.1 정본표가 고정한 값이다
  expected_scope_.roadmap_version =
    static_cast<std::uint64_t>(std::max<std::int64_t>(0, version_param));
  expected_scope_.view_id = static_cast<std::uint32_t>(std::max<std::int64_t>(0, view_id_param));
  scope_known_ = mrs::is_usable_scope(expected_scope_);

  follow_speed_mps_ = this->declare_parameter<double>("follow_speed_mps", 0.2);
  segment_timeout_s_ = this->declare_parameter<double>("segment_timeout_s", 1.0);
  escalation_remaining_segments_ = static_cast<std::size_t>(std::max<std::int64_t>(
    0, this->declare_parameter<std::int64_t>("escalation_remaining_segments", 1)));
  escalation_lead_time_s_ = this->declare_parameter<double>("escalation_lead_time_s", 0.5);
  share_num_segments_ = this->declare_parameter<int>("share_num_segments", 4);
  share_bernstein_degree_ = this->declare_parameter<int>("share_bernstein_degree", 5);
  share_segment_duration_s_ = this->declare_parameter<double>("share_segment_duration_s", 0.5);
  measure_wire_size_ = this->declare_parameter<bool>("measure_wire_size", true);
  startup_check_delay_ms_ = this->declare_parameter<int>("startup_check_delay_ms", 1000);
  summary_log_interval_ticks_ = this->declare_parameter<int>("summary_log_interval_ticks", 100);

  const std::string ns = "/robot_" + std::to_string(robot_id_);

  // nav2-reference §2-A1: MutuallyExclusive 단일 그룹. Reentrant·MultiThreadedExecutor 미사용.
  io_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  rclcpp::SubscriptionOptions sub_options;
  sub_options.callback_group = io_group_;

  rclcpp::QoS version_qos(1);
  version_qos.reliable();
  version_qos.transient_local(); // 늦게 뜬 L4 도 지도 버전을 받아야 한다 (계약 §3 정본표)
  roadmap_version_sub_ = this->create_subscription<std_msgs::msg::UInt64>(
    "/map_registry/roadmap_version", version_qos,
    std::bind(&LocalTrajNode::on_roadmap_version, this, std::placeholders::_1), sub_options);

  rclcpp::QoS window_qos(5);
  window_qos.reliable();
  window_qos.durability_volatile(); // transient_local 금지 — 좀비 창 재생 차단 (L-01)
  execution_window_sub_ = this->create_subscription<mrs_interfaces::msg::ExecutionWindow>(
    ns + "/execution_window", window_qos,
    std::bind(&LocalTrajNode::on_execution_window, this, std::placeholders::_1), sub_options);

  rclcpp::QoS tick_qos(1);
  tick_qos.reliable();
  tick_qos.durability_volatile();
  tick_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  tick_qos.liveliness(rclcpp::LivelinessPolicy::Automatic);
  tick_qos.liveliness_lease_duration(rclcpp::Duration::from_seconds(0.3));
  plan_tick_sub_ = this->create_subscription<mrs_interfaces::msg::PlanTick>(
    "/plan_tick", tick_qos, std::bind(&LocalTrajNode::on_plan_tick, this, std::placeholders::_1),
    sub_options);

  rclcpp::QoS state_qos(1);
  state_qos.best_effort();
  state_qos.durability_volatile();
  robot_state_sub_ = this->create_subscription<mrs_interfaces::msg::RobotState>(
    ns + "/robot_state", state_qos,
    std::bind(&LocalTrajNode::on_robot_state, this, std::placeholders::_1), sub_options);

  // architecture §1.8-B: 구독 층에서 이웃을 고르지 않는다. 자기 자신만 제외하고 **전 구독**.
  rclcpp::QoS neighbor_qos(1);
  neighbor_qos.best_effort();
  neighbor_qos.durability_volatile();
  neighbor_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  for (const std::int64_t raw_id : neighbor_robot_ids_)
  {
    if (raw_id == static_cast<std::int64_t>(robot_id_))
    {
      continue;
    }
    const std::uint16_t neighbor_id = static_cast<std::uint16_t>(raw_id);
    neighbor_stats_[neighbor_id] = NeighborShareStats{};
    neighbor_plan_subs_.push_back(this->create_subscription<mrs_interfaces::msg::LocalPlanShare>(
      "/robot_" + std::to_string(raw_id) + "/local_plan_share", neighbor_qos,
      [this, neighbor_id](const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
      {
        this->on_neighbor_plan_share(neighbor_id, msg);
      },
      sub_options));
  }

  rclcpp::QoS cmd_qos(1);
  cmd_qos.best_effort();
  cmd_qos.durability_volatile();
  cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(ns + "/cmd_vel", cmd_qos);

  rclcpp::QoS escalation_qos(20);
  escalation_qos.reliable();
  escalation_qos.durability_volatile();
  escalation_report_pub_ = this->create_publisher<mrs_interfaces::msg::EscalationReport>(
    ns + "/escalation_report", escalation_qos);

  rclcpp::QoS share_qos(1);
  share_qos.best_effort();
  share_qos.durability_volatile();
  share_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  local_plan_share_pub_ = this->create_publisher<mrs_interfaces::msg::LocalPlanShare>(
    ns + "/local_plan_share", share_qos);

  // **벽시계** 타이머다. use_sim_time 인데 /clock 이 아직 없으면 시뮬 시계 타이머는 영원히
  // 발화하지 않아 기동 게이트가 통째로 죽는다(nav2-reference §2-C2 의 부팅 데드락 회피).
  startup_check_timer_ = this->create_wall_timer(
    std::chrono::milliseconds(std::max(1, startup_check_delay_ms_)),
    std::bind(&LocalTrajNode::check_startup_gates, this), io_group_);

  RCLCPP_INFO(
    this->get_logger(),
    "l4_traj_node [0a] 기동 — robot_id=%d, 이웃 구독 %zu 개, 기대 스코프 %s "
    "(roadmap_version=%" PRIu64 ", view_id=%u)",
    robot_id_, neighbor_plan_subs_.size(), scope_known_ ? "확정" : "미확정(latched 대기)",
    expected_scope_.roadmap_version, expected_scope_.view_id);
}

void LocalTrajNode::check_startup_gates()
{
  try
  {
    startup_check_timer_->cancel(); // 1 회성

    const std::size_t publisher_count = this->count_publishers("/plan_tick");
    if (!mrs::is_single_publisher_ok(publisher_count))
    {
      RCLCPP_FATAL(
        this->get_logger(),
        "R-05 위반: /plan_tick 발행자 %zu 개 감지(2 개 이상) — 기동을 거부합니다. "
        "(계약 L-15: 두 개의 틱 소스는 (A1) 지수 합의를 조용히 깨뜨려 F5 결과를 무효화합니다.)",
        publisher_count);
      rclcpp::shutdown();
      return;
    }
    RCLCPP_INFO(this->get_logger(), "R-05 검사 통과 — /plan_tick 발행자 %zu 개", publisher_count);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "check_startup_gates 실패 (기동 유지): %s", e.what());
  }
}

void LocalTrajNode::on_roadmap_version(const std_msgs::msg::UInt64::SharedPtr msg)
{
  try
  {
    // std_msgs/UInt64 는 계약 스키마가 아니라 무효화 키 스칼라이므로 convert 대상이 아니다
    // (계약 §3 정본표 — MapRegistry latched 토픽). 뷰 종류·view_id 는 여기서 오지 않는다.
    if (msg->data == mrs::ROADMAP_VERSION_UNSPECIFIED)
    {
      RCLCPP_WARN(this->get_logger(), "roadmap_version=0(미지정) 수신 — 무시합니다.");
      return;
    }
    if (scope_known_ && expected_scope_.roadmap_version == msg->data)
    {
      return;
    }
    expected_scope_.roadmap_version = msg->data;
    scope_known_ = mrs::is_usable_scope(expected_scope_);
    // 지도가 바뀌면 이전 뷰의 노드 id 로 만들어진 창은 더 이상 해석할 수 없다 — 폐기한다.
    has_window_ = false;
    RCLCPP_INFO(
      this->get_logger(), "기대 스코프 갱신 — roadmap_version=%" PRIu64 ", view_id=%u (창 폐기)",
      expected_scope_.roadmap_version, expected_scope_.view_id);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_roadmap_version 실패 (현상 유지): %s", e.what());
  }
}

void LocalTrajNode::on_execution_window(const mrs_interfaces::msg::ExecutionWindow::SharedPtr msg)
{
  try
  {
    if (!scope_known_)
    {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), log_throttle_clock_, THROTTLE_MS,
        "기대 스코프 미확정 — execution_window 를 수용하지 않습니다 "
        "(/map_registry/roadmap_version 대기 또는 param expected_roadmap_version 설정).");
      return;
    }

    double now_s = 0.0;
    if (!now_seconds(now_s))
    {
      return;
    }

    mrs::ExecutionWindow converted;
    const mrs::convert::ConvertResult result =
      mrs::convert::from_msg(*msg, expected_scope_, converted);
    if (!result.ok)
    {
      // 안전 폴백 = **그 창을 쓰지 않는다**. 현재 수용 중인 창의 실행은 계속한다.
      count_discard("execution_window", result.reason);
      return;
    }

    log_window_latency(msg->header.stamp, converted.window_seq, now_s);
    apply_window(converted, now_s);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_execution_window 실패 (창 미수용): %s", e.what());
  }
}

void LocalTrajNode::log_window_latency(
  const builtin_interfaces::msg::Time & stamp, std::uint32_t window_seq, double now_s)
{
  double stamp_s = 0.0;
  const mrs::convert::ConvertResult result = mrs::convert::time_to_seconds(stamp, stamp_s);
  if (!result.ok)
  {
    count_discard("execution_window.header.stamp", result.reason);
    return;
  }
  RCLCPP_INFO(
    this->get_logger(), "[0a-metric] window_latency robot=%d window_seq=%u latency_s=%.6f",
    robot_id_, window_seq, now_s - stamp_s);
}

void LocalTrajNode::apply_window(const mrs::ExecutionWindow & window, double now_s)
{
  // W1 (수신 측): 로봇별 단조증가. 역행 seq 는 순서 역전이므로 폐기한다(T2 P5).
  if (has_window_ && window.window_seq < window_.window_seq)
  {
    count_discard(
      "execution_window.seq_regression", mrs::convert::ConvertStatus::FIELD_RANGE_VIOLATION);
    return;
  }

  if (window.revision_kind == mrs::RevisionKind::TRUNCATE)
  {
    apply_window_truncate(window);
    return;
  }

  window_ = window;
  has_window_ = true;
  segment_index_ = 0;
  segment_entered_s_ = now_s;
  escalated_window_seq_ = -1;

  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] window_accepted robot=%d window_seq=%u plan_epoch=%u segments=%zu "
    "predecessors=%zu valid_until_s=%.6f",
    robot_id_, window_.window_seq, window_.plan_epoch, window_.segments.size(),
    window_.predecessor_constraints.size(), window_.window_valid_until_s);
}

void LocalTrajNode::apply_window_truncate(const mrs::ExecutionWindow & window)
{
  if (!has_window_ || window.window_seq != window_.window_seq)
  {
    // 대상 창을 보유하지 않은 TRUNCATE 는 해석할 수 없다 — 폐기(추측으로 적용하지 않는다).
    count_discard(
      "execution_window.truncate_orphan", mrs::convert::ConvertStatus::FIELD_RANGE_VIOLATION);
    return;
  }

  const std::int32_t k = window.valid_through_segment_index;
  const std::size_t keep_count = (k < 0) ? 0U : (static_cast<std::size_t>(k) + 1U);
  if (k < 0 || keep_count < segment_index_)
  {
    // 이미 소비한 접두를 자르려 한다 — **거부**하고 실행을 계속한다(계약 L-01 실패 거동).
    ++truncate_reject_count_;
    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, THROTTLE_MS,
      "[0a] TRUNCATE 거부 — window_seq=%u k=%d 이 소비 접두(%zu)를 자릅니다. 실행을 계속합니다. "
      "(누적 %" PRIu64 "; /commit_status 미발행이라 로그·카운터로만 보고 — [2] 전 해소 대상)",
      window.window_seq, k, segment_index_, truncate_reject_count_);
    return;
  }

  window_.segments.resize(std::min(keep_count, window_.segments.size()));
  RCLCPP_INFO(
    this->get_logger(), "[0a-metric] window_truncated robot=%d window_seq=%u kept_segments=%zu",
    robot_id_, window_.window_seq, window_.segments.size());
}

void LocalTrajNode::on_plan_tick(const mrs_interfaces::msg::PlanTick::SharedPtr msg)
{
  try
  {
    double now_s = 0.0;
    if (!now_seconds(now_s))
    {
      return;
    }

    record_tick_health(*msg, now_s);
    advance_segment_if_due(now_s);
    publish_cmd_vel(now_s);
    publish_plan_share(msg->tick_seq, msg->replan_period_s, now_s);
    publish_escalation_if_due(now_s);

    last_tick_seq_ = msg->tick_seq;
    has_tick_ = true;

    if (
      summary_log_interval_ticks_ > 0 &&
      (msg->tick_seq % static_cast<std::uint32_t>(summary_log_interval_ticks_)) == 0U)
    {
      log_discard_summary();
    }
  }
  catch (const std::exception & e)
  {
    // 안전 폴백: 이 틱의 지령을 내지 않는다(마지막 지령이 유지된다). 틱을 지어내지 않는다.
    RCLCPP_ERROR(this->get_logger(), "on_plan_tick 실패 (이 틱 건너뜀): %s", e.what());
  }
}

void LocalTrajNode::record_tick_health(const mrs_interfaces::msg::PlanTick & msg, double now_s)
{
  double tick_time_s = 0.0;
  const mrs::convert::ConvertResult result =
    mrs::convert::time_to_seconds(msg.tick_time, tick_time_s);
  if (!result.ok)
  {
    count_discard("plan_tick.tick_time", result.reason);
  }
  else
  {
    const double jitter_s = now_s - tick_time_s;
    tick_jitter_max_s_ = std::max(tick_jitter_max_s_, std::fabs(jitter_s));
    RCLCPP_INFO(
      this->get_logger(),
      "[0a-metric] tick_arrival robot=%d tick_seq=%u jitter_s=%.6f replan_period_s=%.6f", robot_id_,
      msg.tick_seq, jitter_s, msg.replan_period_s);
  }

  // R-A1: 유실은 **결번으로 드러난다**. ⛔ 결번을 자체 시계로 메우지 않는다(틱 외삽 금지, R-05).
  if (has_tick_ && msg.tick_seq > last_tick_seq_ + 1U)
  {
    const std::uint64_t missing = static_cast<std::uint64_t>(msg.tick_seq - last_tick_seq_ - 1U);
    tick_gap_total_ += missing;
    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, THROTTLE_MS,
      "[0a-metric] tick_gap robot=%d missing=%" PRIu64 " total=%" PRIu64, robot_id_, missing,
      tick_gap_total_);
  }
}

void LocalTrajNode::advance_segment_if_due(double now_s)
{
  if (!has_window_ || segment_index_ >= window_.segments.size())
  {
    return;
  }

  const mrs::UniformNodeId target_node = window_.segments[segment_index_].node_to;
  const mrs::tracer::SegmentAdvance decision = mrs::tracer::evaluate_segment_advance(
    has_observation_, observation_.occupied_node, target_node, now_s - segment_entered_s_,
    segment_timeout_s_);
  if (decision == mrs::tracer::SegmentAdvance::NOT_DUE)
  {
    return;
  }

  const bool by_timeout = (decision == mrs::tracer::SegmentAdvance::TIMEOUT);
  if (by_timeout)
  {
    ++segment_advance_timeout_;
  }
  else
  {
    ++segment_advance_reached_;
  }

  ++segment_index_;
  segment_entered_s_ = now_s;
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] segment_advance robot=%d window_seq=%u next_index=%zu reason=%s", robot_id_,
    window_.window_seq, segment_index_, by_timeout ? "TIMEOUT" : "NODE_REACHED");
}

void LocalTrajNode::publish_cmd_vel(double now_s)
{
  const bool live = has_window_ && mrs::tracer::is_window_live(window_, segment_index_, now_s);
  const mrs::tracer::FollowCommand command =
    mrs::tracer::make_follow_command(live, follow_speed_mps_);

  // [0a] 는 QP 가 없으므로 `LocalPlanOutput` 의 지령 2 필드만 채운다. 나머지 필드(채택 계획·
  // 커밋 볼록포·폴백 사유)는 [2] 에서 `ILocalPlanner` 가 채운다 — 여기서 지어내지 않는다.
  mrs::LocalPlanOutput output;
  output.cmd_v_mps = command.v_mps;
  output.cmd_omega_rps = command.omega_rps;

  geometry_msgs::msg::Twist twist;
  const mrs::convert::ConvertResult result = mrs::convert::make_cmd_vel(output, twist);
  if (!result.ok)
  {
    count_discard("cmd_vel", result.reason);
    twist = geometry_msgs::msg::Twist(); // 안전 폴백 — 전 성분 0 = 정지 지령
  }
  cmd_vel_pub_->publish(twist);
}

std::vector<double> LocalTrajNode::build_share_control_points(std::size_t point_count) const
{
  // 더미 페이로드 — **규격 크기는 그대로**다(architecture §7-8). 값은 마지막 관측 위치의
  // 반복이며 "제자리 계획"을 뜻한다. 관측 전에는 0 이다(위치를 지어내지 않는다).
  std::vector<double> control_points(point_count, 0.0);
  if (!has_observation_)
  {
    return control_points;
  }
  for (std::size_t i = 0; i + 1U < point_count; i += 2U)
  {
    control_points[i] = observation_.body_pose.x_m;
    control_points[i + 1U] = observation_.body_pose.y_m;
  }
  return control_points;
}

void LocalTrajNode::log_share_wire_size(
  const mrs_interfaces::msg::LocalPlanShare & message, std::size_t point_count)
{
  if (!measure_wire_size_ || share_wire_size_logged_)
  {
    return;
  }
  rclcpp::Serialization<mrs_interfaces::msg::LocalPlanShare> serializer;
  rclcpp::SerializedMessage serialized;
  serializer.serialize_message(&message, &serialized);
  share_wire_size_logged_ = true;
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] local_plan_share_wire_size robot=%d control_points=%zu serialized_bytes=%zu "
    "(§1.8-C 산술 추정치 ~450 B 의 실측 대조값 — Q-18)",
    robot_id_, point_count, serialized.size());
}

void LocalTrajNode::publish_plan_share(std::uint32_t tick_seq, double replan_period_s, double now_s)
{
  const std::uint8_t segments = static_cast<std::uint8_t>(std::max(1, share_num_segments_));
  const std::uint8_t degree = static_cast<std::uint8_t>(std::max(1, share_bernstein_degree_));
  const std::size_t point_count =
    2U * static_cast<std::size_t>(segments) * (static_cast<std::size_t>(degree) + 1U);
  const std::vector<double> control_points = build_share_control_points(point_count);

  // u = Δt_h/Δt. Δt_h 는 **틱이 실어 온 값**을 쓴다(지어내지 않는다). 범위 검사는 convert 가 한다.
  const double shift_ratio_u =
    (share_segment_duration_s_ > 0.0) ? (replan_period_s / share_segment_duration_s_) : 0.0;

  mrs_interfaces::msg::LocalPlanShare message;
  const mrs::convert::ConvertResult result = mrs::convert::make_local_plan_share(
    static_cast<mrs::RobotId>(robot_id_), tick_seq, control_points, segments, degree,
    share_segment_duration_s_, shift_ratio_u, now_s, message);
  if (!result.ok)
  {
    // ⚠ 계약 L-24 는 "매 틱 발행"을 요구하지만, 규격을 어긴 메시지를 내보내는 것은 더 나쁘다.
    // [0a] 에는 이 값을 소비하는 QP 가 없으므로 폐기가 안전한 선택이다 — 사유를 남긴다.
    count_discard("local_plan_share.publish", result.reason);
    return;
  }
  local_plan_share_pub_->publish(message);
  log_share_wire_size(message, point_count);
}

void LocalTrajNode::publish_escalation_if_due(double now_s)
{
  if (!has_window_ || !scope_known_)
  {
    return;
  }
  if (escalated_window_seq_ == static_cast<std::int64_t>(window_.window_seq))
  {
    return; // 창 1 개당 1 회 — 상향 경로를 포화시키면 왕복 지연이 큐잉 지연으로 바뀐다
  }
  if (!mrs::tracer::is_window_exhaustion_imminent(
        window_, segment_index_, now_s, escalation_remaining_segments_, escalation_lead_time_s_))
  {
    return;
  }

  // E1: `event_id != 0`. [0a] 에는 상류 교란 주입기가 없어 보고자가 상관 키를 발급한다 —
  // 로봇별로 분리(상위 32 비트 = robot_id)해 여러 로봇의 키가 겹치지 않게 한다.
  ++event_id_counter_;
  const mrs::EventId event_id = (static_cast<std::uint64_t>(robot_id_) << 32U) | event_id_counter_;

  mrs_interfaces::msg::EscalationReport message;
  const mrs::convert::ConvertResult result = mrs::convert::make_escalation_report(
    static_cast<mrs::RobotId>(robot_id_), event_id, mrs::EscalationReason::WINDOW_EXPIRING,
    window_.window_seq, expected_scope_, now_s, message);
  if (!result.ok)
  {
    count_discard("escalation_report", result.reason);
    return;
  }

  escalation_report_pub_->publish(message);
  escalated_window_seq_ = static_cast<std::int64_t>(window_.window_seq);
  ++escalation_publish_count_;
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] escalation_published robot=%d window_seq=%u event_id=%" PRIu64 " stamp_s=%.6f",
    robot_id_, window_.window_seq, event_id, now_s);
}

void LocalTrajNode::on_robot_state(const mrs_interfaces::msg::RobotState::SharedPtr msg)
{
  try
  {
    if (!scope_known_)
    {
      return; // 스코프 미확정 상태에서 균일 뷰 노드를 랩하면 다른 id 공간의 값이 된다
    }
    mrs::RobotObservation converted;
    const mrs::convert::ConvertResult result =
      mrs::convert::from_msg(*msg, expected_scope_, converted);
    if (!result.ok)
    {
      // 안전 폴백: **마지막 관측을 유지**한다(계약 L-11 수신 규약).
      count_discard("robot_state", result.reason);
      return;
    }
    observation_ = std::move(converted);
    has_observation_ = true;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_robot_state 실패 (마지막 관측 유지): %s", e.what());
  }
}

void LocalTrajNode::on_neighbor_plan_share(
  std::uint16_t neighbor_robot_id, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
{
  try
  {
    NeighborShareStats & stats = neighbor_stats_[neighbor_robot_id];

    mrs::NeighborTrajectory converted;
    const mrs::convert::ConvertResult result = mrs::convert::from_msg(*msg, converted);
    if (!result.ok)
    {
      ++stats.discarded;
      count_discard("local_plan_share.receive", result.reason);
      return;
    }
    ++stats.received;

    double now_s = 0.0;
    double stamp_s = 0.0;
    if (now_seconds(now_s) && mrs::convert::time_to_seconds(msg->header.stamp, stamp_s).ok)
    {
      stats.max_latency_s = std::max(stats.max_latency_s, now_s - stamp_s);
      RCLCPP_INFO(
        this->get_logger(), "[0a-metric] share_latency robot=%d from=%u tick_seq=%u latency_s=%.6f",
        robot_id_, static_cast<unsigned>(neighbor_robot_id), converted.tick_seq, now_s - stamp_s);
    }

    // ⛔ `tick_seq` 로 폐기 판정을 하지 않는다(감소는 유실이 아니라 재동기 — convert 헤더 경고).
    // 여기서는 **결번 계수만** 한다. 신선도 판정은 [2] 의 도메인 몫이다.
    if (stats.has_last_tick && converted.tick_seq > stats.last_tick_seq + 1U)
    {
      stats.gap_total += static_cast<std::uint64_t>(converted.tick_seq - stats.last_tick_seq - 1U);
    }
    stats.last_tick_seq = converted.tick_seq;
    stats.has_last_tick = true;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_neighbor_plan_share 실패 (계측만 손실): %s", e.what());
  }
}

bool LocalTrajNode::now_seconds(double & out_seconds)
{
  // R-18 구속: `1e9` 직접 산술 금지. 음수·NaN·범위 가드는 이 헬퍼 한 곳에만 있다.
  const builtin_interfaces::msg::Time stamp = this->get_clock()->now();
  double seconds = 0.0;
  const mrs::convert::ConvertResult result = mrs::convert::time_to_seconds(stamp, seconds);
  if (!result.ok)
  {
    count_discard("clock", result.reason);
    return false;
  }
  out_seconds = seconds;
  return true;
}

void LocalTrajNode::count_discard(const char * topic_label, mrs::convert::ConvertStatus status)
{
  const std::size_t index = static_cast<std::size_t>(status);
  if (index < discard_by_status_.size())
  {
    ++discard_by_status_[index];
  }
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, THROTTLE_MS,
    "[0a] 폐기 — 경로=%s 사유=%s (해당 사유 누적 %" PRIu64 ")", topic_label,
    mrs::convert::to_string(status),
    (index < discard_by_status_.size()) ? discard_by_status_[index] : std::uint64_t{0});
}

void LocalTrajNode::log_discard_summary()
{
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] summary robot=%d tick_gaps=%" PRIu64 " tick_jitter_max_s=%.6f "
    "seg_reached=%" PRIu64 " seg_timeout=%" PRIu64 " escalations=%" PRIu64
    " truncate_rejects=%" PRIu64,
    robot_id_, tick_gap_total_, tick_jitter_max_s_, segment_advance_reached_,
    segment_advance_timeout_, escalation_publish_count_, truncate_reject_count_);

  for (std::size_t i = 0; i < discard_by_status_.size(); ++i)
  {
    if (discard_by_status_[i] == 0U)
    {
      continue;
    }
    RCLCPP_INFO(
      this->get_logger(), "[0a-metric] discard robot=%d reason=%s count=%" PRIu64, robot_id_,
      mrs::convert::to_string(static_cast<mrs::convert::ConvertStatus>(i)), discard_by_status_[i]);
  }

  for (const auto & entry : neighbor_stats_)
  {
    RCLCPP_INFO(
      this->get_logger(),
      "[0a-metric] neighbor robot=%d from=%u received=%" PRIu64 " discarded=%" PRIu64
      " gaps=%" PRIu64 " max_latency_s=%.6f",
      robot_id_, static_cast<unsigned>(entry.first), entry.second.received, entry.second.discarded,
      entry.second.gap_total, entry.second.max_latency_s);
  }
}

} // namespace mrs
