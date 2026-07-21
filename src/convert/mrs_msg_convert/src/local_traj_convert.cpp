#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs_interfaces::msg::LocalPlanShare make_local_plan_share(
  mrs::RobotId robot_id, std::uint32_t tick_seq,
  const std::vector<double> & control_points, double shift_ratio_u)
{
  (void)robot_id;
  (void)tick_seq;
  (void)control_points;
  (void)shift_ratio_u;
  throw std::logic_error("not implemented: mrs::convert::make_local_plan_share — Phase 5 (ros-builder) 대상");
}

mrs::NeighborTrajectory from_msg(const mrs_interfaces::msg::LocalPlanShare & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::from_msg(LocalPlanShare) — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
