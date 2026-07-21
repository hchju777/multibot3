#include <stdexcept>

#include "mrs_ladder/ladder_state_machine.hpp"

namespace mrs
{

void LadderStateMachine::on_escalation(RobotId robot_id, EscalationReason reason, EventId event_id)
{
  (void)robot_id;
  (void)reason;
  (void)event_id;
  throw std::logic_error(
    "not implemented: LadderStateMachine::on_escalation — Phase 5 (coordination-builder) 대상");
}

void LadderStateMachine::on_judge_verdict(EventId event_id, bool promote_r3)
{
  (void)event_id;
  (void)promote_r3;
  throw std::logic_error(
    "not implemented: LadderStateMachine::on_judge_verdict — Phase 5 (coordination-builder) 대상");
}

Rung LadderStateMachine::current_rung(RobotId robot_id) const
{
  (void)robot_id;
  throw std::logic_error(
    "not implemented: LadderStateMachine::current_rung — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
