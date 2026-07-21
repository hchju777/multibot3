/**
 * @file sim_convert.cpp
 * @brief sim 경계 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 스텁은 기본 생성 @ref mrs::convert::ConvertResult
 * (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않는다.
 *
 * @note `backend_name` 화이트리스트는 이 파일에 두지 않는다(R-15 (c3)) — 허용 문자열은
 *       `mrs_sim_abstraction` 이 상수로 노출하고 여기서는 참조만 한다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult to_msg(
  const mrs::RobotObservation & observation, double stamp_s,
  mrs_interfaces::msg::RobotState & out)
{
  (void)observation;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::RobotState & msg, const mrs::ViewScope & expected,
  mrs::RobotObservation & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const mrs::SimCapabilities & caps, mrs_interfaces::msg::SimCapabilities & out)
{
  (void)caps;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const mrs::SimMetricSample & sample, double stamp_s,
  mrs_interfaces::msg::SimMetricSample & out)
{
  (void)sample;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_request(
  const mrs_interfaces::srv::SimInject::Request & req, const mrs::ViewScope & expected,
  mrs::FaultInjection & out)
{
  (void)req;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
