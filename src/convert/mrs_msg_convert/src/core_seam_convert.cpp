#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs_interfaces::msg::ExecutionWindow to_msg(const mrs::ExecutionWindow & window)
{
  (void)window;
  throw std::logic_error("not implemented: mrs::convert::to_msg(ExecutionWindow) — Phase 5 (ros-builder) 대상");
}

mrs::ExecutionWindow from_msg(const mrs_interfaces::msg::ExecutionWindow & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::from_msg(ExecutionWindow) — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::EscalationReport make_escalation_report(
  mrs::RobotId robot_id, mrs::EventId event_id, mrs::EscalationReason reason,
  std::uint32_t window_seq)
{
  (void)robot_id;
  (void)event_id;
  (void)reason;
  (void)window_seq;
  throw std::logic_error("not implemented: mrs::convert::make_escalation_report — Phase 5 (ros-builder) 대상");
}

mrs::EscalationReason reason_from_msg(const mrs_interfaces::msg::EscalationReport & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::reason_from_msg — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::PlannedPaths to_msg(const std::vector<mrs::RobotPath> & paths, bool is_partial)
{
  (void)paths;
  (void)is_partial;
  throw std::logic_error("not implemented: mrs::convert::to_msg(RobotPath[]) — Phase 5 (ros-builder) 대상");
}

std::vector<mrs::RobotPath> from_msg(const mrs_interfaces::msg::PlannedPaths & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::from_msg(PlannedPaths) — Phase 5 (ros-builder) 대상");
}

std::vector<mrs::PlanRequestEntry> from_request(const mrs_interfaces::srv::PlanPaths::Request & req)
{
  (void)req;
  throw std::logic_error("not implemented: mrs::convert::from_request(PlanPaths::Request) — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::TaskAssignment make_task_assignment(
  std::uint64_t task_id, mrs::RobotId robot_id, mrs::NodeId pickup_node,
  mrs::NodeId delivery_node, std::uint32_t assignment_epoch)
{
  (void)task_id;
  (void)robot_id;
  (void)pickup_node;
  (void)delivery_node;
  (void)assignment_epoch;
  throw std::logic_error("not implemented: mrs::convert::make_task_assignment — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::CommitState to_msg(const mrs::CommitState & state)
{
  (void)state;
  throw std::logic_error("not implemented: mrs::convert::to_msg(CommitState) — Phase 5 (ros-builder) 대상");
}

mrs::CommitState from_msg(const mrs_interfaces::msg::CommitState & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::from_msg(CommitState) — Phase 5 (ros-builder) 대상");
}

std::vector<mrs_interfaces::msg::FrozenOrder> to_msg(const std::vector<mrs::FrozenOrder> & orders)
{
  (void)orders;
  throw std::logic_error("not implemented: mrs::convert::to_msg(FrozenOrder[]) — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::CommitStatus make_commit_status(
  mrs::RobotId robot_id, std::uint32_t window_seq, std::uint32_t committed_through_segment_index,
  const std::vector<mrs::Pose2D> & commit_hull_vertices, std::uint8_t qp_status)
{
  (void)robot_id;
  (void)window_seq;
  (void)committed_through_segment_index;
  (void)commit_hull_vertices;
  (void)qp_status;
  throw std::logic_error("not implemented: mrs::convert::make_commit_status — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
