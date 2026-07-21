#include <stdexcept>

#include "mrs_depgraph/dependency_graph.hpp"

namespace mrs
{

bool DependencyGraph::build_from_paths(
  const std::vector<RobotPath> & paths, std::uint64_t roadmap_version, std::uint32_t view_id)
{
  (void)paths;
  (void)roadmap_version;
  (void)view_id;
  throw std::logic_error(
    "not implemented: DependencyGraph::build_from_paths — Phase 5 (coordination-builder) 대상");
}

bool DependencyGraph::release_next_window(RobotId robot_id, ExecutionWindow & out_window)
{
  (void)robot_id;
  (void)out_window;
  throw std::logic_error(
    "not implemented: DependencyGraph::release_next_window — Phase 5 (coordination-builder) 대상");
}

void DependencyGraph::on_progress_event(RobotId robot_id, NodeId cleared_node, double time_s)
{
  (void)robot_id;
  (void)cleared_node;
  (void)time_s;
  throw std::logic_error(
    "not implemented: DependencyGraph::on_progress_event — Phase 5 (coordination-builder) 대상");
}

bool DependencyGraph::splice_partial_plan(
  RobotId robot_id, const std::vector<RobotPath> & new_partial_paths,
  const std::vector<FrozenOrder> & frozen_orders, const CommitState & commit_boundary)
{
  (void)robot_id;
  (void)new_partial_paths;
  (void)frozen_orders;
  (void)commit_boundary;
  throw std::logic_error(
    "not implemented: DependencyGraph::splice_partial_plan — Phase 5 (coordination-builder) 대상");
}

void DependencyGraph::invalidate_stale_windows(double now_s)
{
  (void)now_s;
  throw std::logic_error("not implemented: DependencyGraph::invalidate_stale_windows — Phase 5 "
                         "(coordination-builder) 대상");
}

} // namespace mrs
