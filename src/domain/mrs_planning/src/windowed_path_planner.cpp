#include <stdexcept>

#include "mrs_planning/windowed_path_planner.hpp"

namespace mrs
{

PathSolverOutput WindowedPathPlanner::plan(const PathSolverInput & input)
{
  (void)input;
  throw std::logic_error(
    "not implemented: WindowedPathPlanner::plan — Phase 5 (coordination-builder) 대상");
}

PathSolverOutput WindowedPathPlanner::plan_partial_replan(
  const PathSolverInput & input, const CommitState & commit_boundary,
  const std::vector<FrozenOrder> & frozen_orders)
{
  (void)input;
  (void)commit_boundary;
  (void)frozen_orders;
  throw std::logic_error("not implemented: WindowedPathPlanner::plan_partial_replan — Phase 5 "
                         "(coordination-builder) 대상");
}

} // namespace mrs
