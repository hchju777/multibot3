#include <stdexcept>

#include "mrs_local_traj/neighbor_freshness_tracker.hpp"

namespace mrs
{

void NeighborFreshnessTracker::on_neighbor_plan(
  RobotId robot_id, std::uint32_t tick_seq, double shift_ratio_u)
{
  (void)robot_id;
  (void)tick_seq;
  (void)shift_ratio_u;
  throw std::logic_error(
    "not implemented: NeighborFreshnessTracker::on_neighbor_plan — Phase 5 (coordination-builder) 대상");
}

bool NeighborFreshnessTracker::is_fresh(RobotId robot_id, std::uint32_t required_tick_seq) const
{
  (void)robot_id;
  (void)required_tick_seq;
  throw std::logic_error(
    "not implemented: NeighborFreshnessTracker::is_fresh — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
