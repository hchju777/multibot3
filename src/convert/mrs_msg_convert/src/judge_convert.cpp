/**
 * @file judge_convert.cpp
 * @brief L-11(JudgeVerdict) 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 스텁은 기본 생성 @ref mrs::convert::ConvertResult
 * (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않는다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult make_judge_verdict(
  mrs::EventId event_id, const mrs::JudgeDecision & decision, double completion_excess_s,
  double threshold_s, double stamp_s, mrs_interfaces::msg::JudgeVerdict & out)
{
  (void)event_id;
  (void)decision;
  (void)completion_excess_s;
  (void)threshold_s;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
