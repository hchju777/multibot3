/**
 * @file task_and_reorder_convert.cpp
 * @brief 재할당·부분재계획·재조정 서비스 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 스텁은 기본 생성 @ref mrs::convert::ConvertResult
 * (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않는다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult trigger_from_request(
  const mrs_interfaces::srv::ReassignRequest::Request & req, mrs::ReassignTrigger & out)
{
  (void)req;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult fill_response(
  mrs::ReassignResult result, mrs::RobotId new_robot_id, double earliest_retry_s,
  mrs_interfaces::srv::ReassignRequest::Response & resp)
{
  (void)result;
  (void)new_robot_id;
  (void)earliest_retry_s;
  (void)resp;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_request(
  const mrs_interfaces::srv::PartialReplan::Request & req, const mrs::ViewScope & expected,
  std::vector<mrs::CommitState> & out_commit_states,
  std::vector<mrs::FrozenOrder> & out_frozen_orders)
{
  (void)req;
  (void)expected;
  (void)out_commit_states;
  (void)out_frozen_orders;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_request(
  const mrs_interfaces::srv::ReorderPassing::Request & req, mrs::ReorderRequest & out)
{
  (void)req;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult fill_response(
  mrs::ReorderAck ack, std::uint64_t job_id,
  mrs_interfaces::srv::ReorderPassing::Response & resp)
{
  (void)ack;
  (void)job_id;
  (void)resp;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
