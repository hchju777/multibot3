#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs_interfaces::msg::JudgeVerdict make_judge_verdict(
  mrs::EventId event_id, const mrs::JudgeDecision & decision,
  double completion_excess_s, double threshold_s)
{
  (void)event_id;
  (void)decision;
  (void)completion_excess_s;
  (void)threshold_s;
  throw std::logic_error("not implemented: mrs::convert::make_judge_verdict — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
