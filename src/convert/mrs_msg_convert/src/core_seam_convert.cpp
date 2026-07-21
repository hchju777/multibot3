/**
 * @file core_seam_convert.cpp
 * @brief L-01/L-04 계열 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 이 파일의 함수는 전부 스텁이다. 예외를 던지지 않으며(계약 §0.1 V3, R-15 (c)), 기본 생성
 * @ref mrs::convert::ConvertResult 를 반환한다 — `ok = false` 이므로 호출자는 안전 폴백으로 간다.
 * 사유를 **실제 실패 사유처럼 보이는 값으로 채우지 않는다**: 미구현을 `FIELD_RANGE_VIOLATION`
 * 같은 값으로 보고하면 폐기 카운터가 존재하지 않는 결함을 가리키게 된다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult to_msg(
  const mrs::ExecutionWindow & window, double stamp_s,
  mrs_interfaces::msg::ExecutionWindow & out)
{
  (void)window;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::ExecutionWindow & msg, const mrs::ViewScope & expected,
  mrs::ExecutionWindow & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_escalation_report(
  mrs::RobotId robot_id, mrs::EventId event_id, mrs::EscalationReason reason,
  std::uint32_t window_seq, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::EscalationReport & out)
{
  (void)robot_id;
  (void)event_id;
  (void)reason;
  (void)window_seq;
  (void)scope;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult reason_from_msg(
  const mrs_interfaces::msg::EscalationReport & msg, const mrs::ViewScope & expected,
  mrs::EscalationReason & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const std::vector<mrs::RobotPath> & paths, bool is_partial, mrs::EventId event_id,
  std::uint32_t plan_epoch, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::PlannedPaths & out)
{
  (void)paths;
  (void)is_partial;
  (void)event_id;
  (void)plan_epoch;
  (void)scope;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::PlannedPaths & msg, const mrs::ViewScope & expected,
  std::vector<mrs::RobotPath> & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_request(
  const mrs_interfaces::srv::PlanPaths::Request & req, const mrs::ViewScope & expected,
  std::vector<mrs::PlanRequestEntry> & out)
{
  (void)req;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_task_assignment(
  std::uint64_t task_id, mrs::RobotId robot_id, mrs::PhysicalNodeId pickup_node,
  mrs::PhysicalNodeId delivery_node, std::uint32_t assignment_epoch, double waiting_time_s,
  std::uint32_t reassign_count, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::TaskAssignment & out)
{
  (void)task_id;
  (void)robot_id;
  (void)pickup_node;
  (void)delivery_node;
  (void)assignment_epoch;
  (void)waiting_time_s;
  (void)reassign_count;
  (void)scope;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(const mrs::CommitState & state, mrs_interfaces::msg::CommitState & out)
{
  (void)state;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::CommitState & msg, const mrs::ViewScope & expected,
  mrs::CommitState & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const std::vector<mrs::FrozenOrder> & orders,
  std::vector<mrs_interfaces::msg::FrozenOrder> & out)
{
  (void)orders;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_commit_status(
  mrs::RobotId robot_id, std::uint32_t window_seq, std::uint32_t committed_through_segment_index,
  const std::vector<mrs::Pose2D> & commit_hull_vertices, mrs::AdoptionStatus qp_status,
  double stamp_s, mrs_interfaces::msg::CommitStatus & out)
{
  (void)robot_id;
  (void)window_seq;
  (void)committed_through_segment_index;
  (void)commit_hull_vertices;
  (void)qp_status;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
