#include "sadg_builder.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace sadg {
namespace {

SadgTimingProfile make_timing_profile(bool is_wait, int delta_ticks)
{
    SadgTimingProfile timing;
    timing.duration_ticks = std::max(1, delta_ticks);
    timing.slack_ticks = is_wait ? 0 : 1;
    return timing;
}

std::string make_action_id(int robot_index, int action_index)
{
    std::ostringstream oss;
    oss << "r" << robot_index << ":a" << action_index;
    return oss.str();
}

std::string make_edge_id(const char* prefix, int from_idx, int to_idx)
{
    std::ostringstream oss;
    oss << prefix << ":" << from_idx << "->" << to_idx;
    return oss.str();
}

}  // namespace

SadgBuildResult SadgBuilder::build(const SadgBuildRequest& request) const
{
    SadgBuildResult result;
    const auto& snapshot = request.plan_input.snapshot;
    const auto& solution = request.plan_input.solution;

    if (snapshot.robots.size() != solution.size()) {
        throw std::runtime_error("snapshot.robot count must match solution size");
    }

    for (int robot_idx = 0; robot_idx < static_cast<int>(solution.size()); ++robot_idx) {
        const auto& plan = solution[static_cast<std::size_t>(robot_idx)];
        for (int step = 0; step + 1 < static_cast<int>(plan.size()); ++step) {
            const auto& [from_vertex, from_tick] = plan[static_cast<std::size_t>(step)];
            const auto& [to_vertex, to_tick] = plan[static_cast<std::size_t>(step + 1)];

            SadgActionNode action;
            action.action_id = make_action_id(robot_idx, step);
            action.robot_index = robot_idx;
            action.action_index = step;
            action.from_vertex = from_vertex;
            action.to_vertex = to_vertex;
            action.is_wait = from_vertex == to_vertex;
            action.start_tick = from_tick;
            action.finish_tick = to_tick;
            action.timing = make_timing_profile(action.is_wait, to_tick - from_tick);
            action.window.planned_start_tick = from_tick;
            action.window.planned_finish_tick = to_tick;
            action.window.latest_safe_finish_tick = to_tick + action.timing.slack_ticks;

            const ActionIndex action_idx = result.graph.add_action(std::move(action));
            result.graph.connect_action_resource(action_idx,
                                                 result.graph.ensure_vertex_resource(to_vertex));
            if (from_vertex != to_vertex) {
                result.graph.connect_action_resource(
                    action_idx,
                    result.graph.ensure_directed_edge_resource(from_vertex, to_vertex));
            }
        }
    }

    for (int robot_idx = 0; robot_idx < static_cast<int>(solution.size()); ++robot_idx) {
        const auto& actions = result.graph.robot_actions(robot_idx);
        for (std::size_t i = 0; i + 1 < actions.size(); ++i) {
            SadgDependencyEdge edge;
            edge.edge_id = make_edge_id("intra", actions[i], actions[i + 1]);
            edge.from_action = actions[i];
            edge.to_action = actions[i + 1];
            edge.type = SadgEdgeType::INTRA_ROBOT_ORDER;
            result.graph.add_edge(std::move(edge));
        }
    }

    // Preserve the observed inter-robot vertex and reverse-edge order from the MAPF solution.
    for (ActionIndex lhs = 0; lhs < static_cast<ActionIndex>(result.graph.action_count()); ++lhs) {
        for (ActionIndex rhs = lhs + 1;
             rhs < static_cast<ActionIndex>(result.graph.action_count());
             ++rhs) {
            const auto& a = result.graph.action(lhs);
            const auto& b = result.graph.action(rhs);
            if (a.robot_index == b.robot_index) continue;

            if (a.to_vertex == b.from_vertex && a.finish_tick <= b.start_tick) {
                SadgDependencyEdge edge;
                edge.edge_id = make_edge_id("release", lhs, rhs);
                edge.from_action = lhs;
                edge.to_action = rhs;
                edge.type = SadgEdgeType::VERTEX_RELEASE;
                result.graph.add_edge(std::move(edge));
            } else if (b.to_vertex == a.from_vertex && b.finish_tick <= a.start_tick) {
                SadgDependencyEdge edge;
                edge.edge_id = make_edge_id("release", rhs, lhs);
                edge.from_action = rhs;
                edge.to_action = lhs;
                edge.type = SadgEdgeType::VERTEX_RELEASE;
                result.graph.add_edge(std::move(edge));
            }

            const bool reverse_moves =
                !a.is_wait && !b.is_wait &&
                a.from_vertex == b.to_vertex && a.to_vertex == b.from_vertex;
            if (reverse_moves) {
                const bool a_before_b =
                    (a.start_tick < b.start_tick) ||
                    (a.start_tick == b.start_tick && lhs < rhs);
                SadgDependencyEdge edge;
                edge.edge_id = make_edge_id("order", a_before_b ? lhs : rhs,
                                            a_before_b ? rhs : lhs);
                edge.from_action = a_before_b ? lhs : rhs;
                edge.to_action = a_before_b ? rhs : lhs;
                edge.type = SadgEdgeType::EDGE_ORDER;
                edge.switchable = true;
                result.graph.add_edge(std::move(edge));
            }
        }
    }

    return result;
}

}  // namespace sadg
