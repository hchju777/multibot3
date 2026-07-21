#include <stdexcept>

#include "mrs_reorder_milp/milp_job_tracker.hpp"

namespace mrs
{

MilpResult MilpJobTracker::solve_switching(const MilpModel & model)
{
  (void)model;
  throw std::logic_error(
    "not implemented: MilpJobTracker::solve_switching — Phase 5 (coordination-builder) 대상");
}

void MilpJobTracker::cancel_all()
{
  throw std::logic_error(
    "not implemented: MilpJobTracker::cancel_all — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
