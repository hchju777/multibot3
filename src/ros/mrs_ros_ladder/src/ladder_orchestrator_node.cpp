/**
 * @file ladder_orchestrator_node.cpp
 * @brief @ref ladder_orchestrator_node.hpp 의 구현 — [0a] 로그 전용 ladder_orchestrator.
 *
 * 규율(CLAUDE.md 규율 2): 모든 콜백을 try/catch 로 감싼다. 사다리는 상향 경로의 **종점**이므로
 * 여기가 죽으면 [0a] 의 상향 왕복 실측이 통째로 사라진다 — 잡아서 로깅하고 계속 산다.
 */

#include <algorithm>
#include <cinttypes>
#include <exception>
#include <limits>
#include <string>

#include "mrs_metrics/metrics_collector.hpp"
#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_msg_convert/msg_convert.hpp"
#include "mrs_ros_ladder/ladder_orchestrator_node.hpp"

namespace mrs
{

namespace
{

/** @brief 로그 스로틀 간격 [ms]. */
constexpr int THROTTLE_MS = 2000;

} // namespace

LadderOrchestratorNode::LadderOrchestratorNode() : rclcpp::Node("ladder_orchestrator")
{
  robot_count_ = this->declare_parameter<int>("robot_count", 2);
  publish_rung_event_ = this->declare_parameter<bool>("publish_rung_event", true);
  summary_log_interval_ = this->declare_parameter<int>("summary_log_interval", 20);

  const std::int64_t version_param =
    this->declare_parameter<std::int64_t>("expected_roadmap_version", 0);
  // 기본값 1 = map_registry 의 `uniform_view_id` 기본값 (근거는 local_traj_node.cpp 동일 지점).
  const std::int64_t view_id_param = this->declare_parameter<std::int64_t>("expected_view_id", 1);
  expected_scope_.view_kind = mrs::ViewKind::UNIFORM; // 계약 §0.1 정본표가 고정한 값이다
  expected_scope_.roadmap_version =
    static_cast<std::uint64_t>(std::max<std::int64_t>(0, version_param));
  expected_scope_.view_id = static_cast<std::uint32_t>(std::max<std::int64_t>(0, view_id_param));
  scope_known_ = mrs::is_usable_scope(expected_scope_);

  // nav2-reference §2-A1: MutuallyExclusive 단일 그룹. Reentrant·MultiThreadedExecutor 미사용.
  io_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  rclcpp::SubscriptionOptions sub_options;
  sub_options.callback_group = io_group_;

  rclcpp::QoS version_qos(1);
  version_qos.reliable();
  version_qos.transient_local();
  roadmap_version_sub_ = this->create_subscription<std_msgs::msg::UInt64>(
    "/map_registry/roadmap_version", version_qos,
    std::bind(&LadderOrchestratorNode::on_roadmap_version, this, std::placeholders::_1),
    sub_options);

  // architecture §1.4: 상향은 전역 단일 토픽이 아니라 **per-robot** 토픽이다(D-10 로봇별 손실
  // 주입 지점 + 링크 장애의 QoS 귀속). 서버는 로봇 수 파라미터로 N 개 구독을 만든다.
  rclcpp::QoS escalation_qos(20);
  escalation_qos.reliable();
  escalation_qos.durability_volatile();
  for (int i = 0; i < robot_count_; ++i)
  {
    const std::size_t robot_index = static_cast<std::size_t>(i);
    escalation_report_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::EscalationReport>(
        "/robot_" + std::to_string(i) + "/escalation_report", escalation_qos,
        [this, robot_index](const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
        {
          this->on_escalation_report(robot_index, msg);
        },
        sub_options));
  }

  rclcpp::QoS rung_qos(50);
  rung_qos.reliable();
  rung_qos.durability_volatile();
  rung_event_sub_ = this->create_subscription<mrs_interfaces::msg::RungEvent>(
    "/ladder/rung_event", rung_qos,
    std::bind(&LadderOrchestratorNode::on_rung_event, this, std::placeholders::_1), sub_options);
  rung_event_pub_ =
    this->create_publisher<mrs_interfaces::msg::RungEvent>("/ladder/rung_event", rung_qos);

  RCLCPP_INFO(
    this->get_logger(),
    "ladder_orchestrator [0a] 기동 (로그 전용, 라우팅 없음) — robot_count=%d, "
    "rung_event 발행=%s, 기대 스코프 %s",
    robot_count_, publish_rung_event_ ? "on" : "off",
    scope_known_ ? "확정" : "미확정(latched 대기)");
}

void LadderOrchestratorNode::on_roadmap_version(const std_msgs::msg::UInt64::SharedPtr msg)
{
  try
  {
    // std_msgs/UInt64 는 계약 스키마가 아니라 무효화 키 스칼라다(계약 §3 정본표).
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
    RCLCPP_INFO(
      this->get_logger(), "기대 스코프 갱신 — roadmap_version=%" PRIu64 ", view_id=%u",
      expected_scope_.roadmap_version, expected_scope_.view_id);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_roadmap_version 실패 (현상 유지): %s", e.what());
  }
}

void LadderOrchestratorNode::on_escalation_report(
  std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
{
  try
  {
    if (!scope_known_)
    {
      warn_scope_unknown();
      return;
    }

    double now_s = 0.0;
    if (!now_seconds(now_s))
    {
      return;
    }

    mrs::EscalationReason reason = mrs::EscalationReason::BLOCKED;
    const mrs::convert::ConvertResult result =
      mrs::convert::reason_from_msg(*msg, expected_scope_, reason);
    if (!result.ok)
    {
      // 폐기 + 로그 + 카운터. ⛔ 미지의 사유를 기존 값으로 뭉개지 않는다(계약 §0.2).
      count_discard("escalation_report", result.reason);
      return;
    }

    record_escalation(robot_index, *msg, reason, now_s);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_escalation_report 실패 (계측만 손실): %s", e.what());
  }
}

void LadderOrchestratorNode::warn_scope_unknown()
{
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, THROTTLE_MS,
    "기대 스코프 미확정 — escalation_report 를 해석하지 않습니다 "
    "(/map_registry/roadmap_version 대기 또는 param expected_roadmap_version 설정).");
}

void LadderOrchestratorNode::record_escalation(
  std::size_t robot_index, const mrs_interfaces::msg::EscalationReport & msg,
  mrs::EscalationReason reason, double now_s)
{
  if (msg.robot_id != static_cast<std::uint16_t>(robot_index))
  {
    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, THROTTLE_MS,
      "[0a] 오배선 의심 — /robot_%zu/escalation_report 에 robot_id=%u 가 실려 왔습니다.",
      robot_index, static_cast<unsigned>(msg.robot_id));
  }

  ++escalation_received_;
  ++reason_counts_[static_cast<std::uint8_t>(reason)];
  log_escalation_latency(msg.header.stamp, msg.robot_id, msg.event_id, reason, now_s);

  if (publish_rung_event_)
  {
    publish_rung_event(msg.robot_id, msg.event_id, now_s);
  }
  if (
    summary_log_interval_ > 0 &&
    (escalation_received_ % static_cast<std::uint64_t>(summary_log_interval_)) == 0U)
  {
    log_summary();
  }
}

void LadderOrchestratorNode::log_escalation_latency(
  const builtin_interfaces::msg::Time & stamp, std::uint16_t robot_id, std::uint64_t event_id,
  mrs::EscalationReason reason, double now_s)
{
  double stamp_s = 0.0;
  const mrs::convert::ConvertResult result = mrs::convert::time_to_seconds(stamp, stamp_s);
  if (!result.ok)
  {
    count_discard("escalation_report.header.stamp", result.reason);
    return;
  }

  const double latency_s = now_s - stamp_s;
  escalation_latency_max_s_ = std::max(escalation_latency_max_s_, latency_s);

  // 사유는 **값으로** 남긴다 — 사유별 카운터가 뭉개지면 D-08 라우팅 오발동을 사후에 구별할 수
  // 없다(계약 L-04 SYNC_LOST 주의). severity 는 계약 L-04 매핑표의 코드 정본을 그대로 쓴다.
  const mrs::EscalationSeverity severity = mrs::severity_of(reason);
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] escalation_received robot=%u event_id=%" PRIu64
    " reason=%u severity=%s recv_s=%.6f latency_s=%.6f",
    static_cast<unsigned>(robot_id), event_id, static_cast<unsigned>(reason),
    (severity == mrs::EscalationSeverity::HARD) ? "HARD" : "SOFT", now_s, latency_s);
}

void LadderOrchestratorNode::publish_rung_event(
  std::uint16_t robot_id, std::uint64_t event_id, double now_s)
{
  mrs::RungEventRecord record;
  record.event_id = event_id;
  // 보고 출처가 L4 = R0 이고, L4 가 흡수하지 못해 상향한 것이므로 전이는 ESCALATE 다.
  record.rung = mrs::Rung::R0_LOCAL_ABSORB;
  record.transition = static_cast<std::uint8_t>(mrs::RungTransition::ESCALATE);
  // ⚠ `trigger_kind` 는 **심각도가 아니라 출처**다(contract_types.hpp 경고 — 병합 금지).
  // `RungEvent.msg` 가 TRIGGER_HARD 를 "EscalationReport 계열"로 정의하므로 그대로 따른다.
  // WINDOW_EXPIRING(SEVERITY_SOFT)도 출처는 EscalationReport 이므로 HARD 로 나간다 —
  // 이 해석이 계약 의도와 맞는지는 contract-keeper 판정 대상으로 리포트에 OPEN 으로 올린다.
  record.trigger_kind = static_cast<std::uint8_t>(mrs::RungTriggerKind::HARD);
  record.affected_robots.push_back(static_cast<mrs::RobotId>(robot_id));
  // ── 미산출 필드 (지어내지 않는다 — 파일 머리말의 감수한 결손) ──
  record.edge_set_fingerprint = 0U;
  record.phi_hat_before_s = 0.0; // 계약이 유한값을 요구해 NaN 을 쓸 수 없다. judge 부재로 미산출
  record.phi_hat_after_s = std::numeric_limits<double>::quiet_NaN(); // 계약이 정한 "미산출"
  record.attempt_count = 0U;
  record.duration_s = 0.0; // ⛔ 여기에 전달 지연을 싣지 않는다 — L-13 의 의미(가로대 처리 시간)와
                           //    다른 양을 넣으면 M5 왕복 계측이 조용히 오염된다. 지연은 로그로만.

  mrs_interfaces::msg::RungEvent message;
  const mrs::convert::ConvertResult result = mrs::convert::to_msg(record, now_s, message);
  if (!result.ok)
  {
    count_discard("rung_event.publish", result.reason);
    return;
  }
  rung_event_pub_->publish(message);
  ++rung_event_published_;
}

void LadderOrchestratorNode::on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg)
{
  try
  {
    ++rung_event_received_;
    double now_s = 0.0;
    double stamp_s = 0.0;
    if (!now_seconds(now_s))
    {
      return;
    }
    const mrs::convert::ConvertResult result =
      mrs::convert::time_to_seconds(msg->header.stamp, stamp_s);
    if (!result.ok)
    {
      count_discard("rung_event.header.stamp", result.reason);
      return;
    }
    // [0a] 에서는 이 노드가 유일한 발행자이므로 이 수치는 **자기 발행분의 DDS 왕복**이다.
    // [3] 에서 서버 노드들이 발행자가 되면 같은 계측이 노드 간 지연을 재게 된다.
    RCLCPP_INFO(
      this->get_logger(),
      "[0a-metric] rung_event_received event_id=%" PRIu64 " rung=%u transition=%u latency_s=%.6f",
      msg->event_id, static_cast<unsigned>(msg->rung), static_cast<unsigned>(msg->transition),
      now_s - stamp_s);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_rung_event 실패 (계측만 손실): %s", e.what());
  }
}

bool LadderOrchestratorNode::now_seconds(double & out_seconds)
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

void LadderOrchestratorNode::count_discard(
  const char * topic_label, mrs::convert::ConvertStatus status)
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

void LadderOrchestratorNode::log_summary()
{
  RCLCPP_INFO(
    this->get_logger(),
    "[0a-metric] ladder_summary escalations=%" PRIu64 " rung_published=%" PRIu64
    " rung_received=%" PRIu64 " escalation_latency_max_s=%.6f",
    escalation_received_, rung_event_published_, rung_event_received_, escalation_latency_max_s_);

  for (const auto & entry : reason_counts_)
  {
    RCLCPP_INFO(
      this->get_logger(), "[0a-metric] escalation_reason reason=%u count=%" PRIu64,
      static_cast<unsigned>(entry.first), entry.second);
  }

  for (std::size_t i = 0; i < discard_by_status_.size(); ++i)
  {
    if (discard_by_status_[i] == 0U)
    {
      continue;
    }
    RCLCPP_INFO(
      this->get_logger(), "[0a-metric] discard reason=%s count=%" PRIu64,
      mrs::convert::to_string(static_cast<mrs::convert::ConvertStatus>(i)), discard_by_status_[i]);
  }
}

} // namespace mrs
