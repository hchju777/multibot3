#include <stdexcept>

#include "mrs_local_traj/shift_operator.hpp"

namespace mrs
{

std::vector<double> ShiftOperator::compute_initial_trajectory(
  const std::vector<double> & previous_control_points, double shift_ratio_u) const
{
  (void)previous_control_points;
  (void)shift_ratio_u;
  throw std::logic_error(
    "not implemented: ShiftOperator::compute_initial_trajectory — Phase 5 (coordination-builder)"
    " 대상 (theory T1-R3 de Casteljau 분할, T4-B1)");
}

} // namespace mrs
