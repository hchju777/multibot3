#include "sadg_scheduler.hpp"

#include <algorithm>
#include <stdexcept>

namespace sadg {

SadgScheduler::SadgScheduler(SadgSchedulingPolicy policy)
    : policy_(std::move(policy))
{}

void SadgScheduler::sync_robot_states(const SadgGraph& graph,
                                      const SadgRuntimeState& state,
                                      std::vector<RobotSadgState>& robots) const
{
    for (RobotIndex robot_idx = 0; robot_idx < static_cast<RobotIndex>(robots.size());
         ++robot_idx) {
        auto& robot = robots.at(static_cast<std::size_t>(robot_idx));

        if (robot.state == RobotExecState::SAFE_STOP) {
            continue;
        }
        if (robot.faulted || robot.localization_lost || robot.externally_held ||
            robot.state == RobotExecState::HELD) {
            robot.state = RobotExecState::HELD;
            continue;
        }

        if (state.robot_current_action.at(static_cast<std::size_t>(robot_idx)).has_value()) {
            robot.state = RobotExecState::EXECUTING;
            continue;
        }

        bool has_ready = false;
        bool has_pending_work = false;
        for (ActionIndex action_idx : graph.robot_actions(robot_idx)) {
            const auto action_state =
                state.action_states.at(static_cast<std::size_t>(action_idx));
            if (action_state == ActionExecState::READY) {
                has_ready = true;
                break;
            }
            if (action_state != ActionExecState::DONE &&
                action_state != ActionExecState::CANCELLED) {
                has_pending_work = true;
            }
        }

        if (has_ready) {
            robot.state = RobotExecState::READY;
        } else if (has_pending_work) {
            robot.state = RobotExecState::WAITING_DEPENDENCY;
        } else {
            robot.state = RobotExecState::IDLE_AT_GOAL;
        }
    }
}

SadgSchedulerInitResult SadgScheduler::initialize(const SadgGraph& graph,
                                                  const FleetSnapshot& snapshot) const
{
    SadgSchedulerInitResult result;
    const auto robot_count = snapshot.robots.size();
    result.robots.reserve(robot_count);

    result.state.action_states.assign(graph.action_count(), ActionExecState::PENDING);
    result.state.robot_states.assign(robot_count, RobotExecState::IDLE_AT_GOAL);
    result.state.resource_states.assign(graph.resource_count(), ResourceState::FREE);
    result.state.active_predecessor_count.assign(graph.action_count(), 0);
    result.state.edge_active.assign(graph.edge_count(), true);
    result.state.action_dispatch_time_ms.assign(graph.action_count(), std::nullopt);
    result.state.action_finish_time_ms.assign(graph.action_count(), std::nullopt);
    result.state.robot_current_action.assign(robot_count, std::nullopt);
    result.state.robot_committed_prefix_end.assign(robot_count, -1);

    for (std::size_t robot_idx = 0; robot_idx < robot_count; ++robot_idx) {
        RobotSadgState robot;
        robot.robot_id = snapshot.robots[robot_idx].id;
        robot.robot_index = static_cast<RobotIndex>(robot_idx);
        robot.current_vertex = snapshot.robots[robot_idx].current_vertex;
        robot.current_goal = snapshot.robots[robot_idx].active_goal;
        robot.state = graph.robot_actions(static_cast<RobotIndex>(robot_idx)).empty()
                          ? RobotExecState::IDLE_AT_GOAL
                          : RobotExecState::WAITING_DEPENDENCY;
        result.robots.push_back(std::move(robot));
    }

    for (ActionIndex action_idx = 0; action_idx < static_cast<ActionIndex>(graph.action_count());
         ++action_idx) {
        int active_predecessors = 0;
        for (int edge_idx : graph.in_edges(action_idx)) {
            if (result.state.edge_active[static_cast<std::size_t>(edge_idx)]) {
                ++active_predecessors;
            }
        }
        result.state.active_predecessor_count[static_cast<std::size_t>(action_idx)] =
            active_predecessors;
    }

    for (ActionIndex action_idx = 0; action_idx < static_cast<ActionIndex>(graph.action_count());
         ++action_idx) {
        if (result.state.active_predecessor_count[static_cast<std::size_t>(action_idx)] == 0) {
            result.state.action_states[static_cast<std::size_t>(action_idx)] =
                ActionExecState::READY;
        }
    }

    sync_robot_states(graph, result.state, result.robots);
    for (RobotIndex robot_idx = 0; robot_idx < static_cast<RobotIndex>(result.robots.size());
         ++robot_idx) {
        result.state.robot_states[static_cast<std::size_t>(robot_idx)] =
            result.robots[static_cast<std::size_t>(robot_idx)].state;
    }

    return result;
}

bool SadgScheduler::resources_available(const SadgGraph& graph,
                                        const SadgRuntimeState& state,
                                        const SadgActionNode& action) const
{
    for (ResourceIndex resource_idx : action.resources) {
        const auto resource_state =
            state.resource_states.at(static_cast<std::size_t>(resource_idx));
        if (resource_state != ResourceState::FREE) {
            const auto& resource = graph.resource(resource_idx);
            if (resource.owner_action_index != action.action_index ||
                resource.owner_robot_index != action.robot_index) {
                return false;
            }
        }
    }
    return true;
}

SadgFeedbackApplyResult SadgScheduler::apply_feedback(const SadgGraph& graph,
                                                      SadgRuntimeState& state,
                                                      std::vector<RobotSadgState>& robots,
                                                      const ExecutionFeedback& feedback) const
{
    SadgFeedbackApplyResult result;
    auto robot_it = std::find_if(robots.begin(), robots.end(), [&](const RobotSadgState& robot) {
        return robot.robot_id == feedback.robot_id;
    });
    if (robot_it == robots.end()) {
        throw std::runtime_error("feedback references unknown robot_id: " + feedback.robot_id);
    }

    const RobotIndex robot_idx = robot_it->robot_index;
    if (feedback.actual_vertex.has_value()) {
        robot_it->current_vertex = *feedback.actual_vertex;
    }

    switch (feedback.type) {
    case ExecutionFeedbackType::POSITION_UPDATE:
        result.updated_robots.push_back(robot_idx);
        break;

    case ExecutionFeedbackType::ACTION_DONE: {
        std::optional<ActionIndex> action_idx;
        if (feedback.action_id.has_value() && graph.has_action(*feedback.action_id)) {
            action_idx = graph.find_action_index(*feedback.action_id);
        } else {
            action_idx = state.robot_current_action.at(static_cast<std::size_t>(robot_idx));
        }
        if (!action_idx.has_value()) break;

        state.action_states[static_cast<std::size_t>(*action_idx)] = ActionExecState::DONE;
        state.action_finish_time_ms[static_cast<std::size_t>(*action_idx)] =
            feedback.timestamp_ms;
        state.robot_current_action[static_cast<std::size_t>(robot_idx)] = std::nullopt;
        state.robot_committed_prefix_end[static_cast<std::size_t>(robot_idx)] = *action_idx;
        result.updated_actions.push_back(*action_idx);

        for (ResourceIndex resource_idx : graph.action(*action_idx).resources) {
            state.resource_states[static_cast<std::size_t>(resource_idx)] = ResourceState::FREE;
            result.updated_resources.push_back(resource_idx);
        }

        for (int edge_idx : graph.out_edges(*action_idx)) {
            if (!state.edge_active[static_cast<std::size_t>(edge_idx)]) continue;
            const ActionIndex successor = graph.edge(edge_idx).to_action;
            auto& predecessor_count =
                state.active_predecessor_count[static_cast<std::size_t>(successor)];
            predecessor_count = std::max(0, predecessor_count - 1);
        }

        if (const auto next = graph.next_robot_action(robot_idx, *action_idx);
            next.has_value()) {
            const auto next_state =
                state.action_states[static_cast<std::size_t>(*next)];
            robot_it->state = next_state == ActionExecState::READY
                                  ? RobotExecState::READY
                                  : RobotExecState::WAITING_DEPENDENCY;
        } else {
            robot_it->state = RobotExecState::IDLE_AT_GOAL;
        }

        state.robot_states[static_cast<std::size_t>(robot_idx)] = robot_it->state;
        result.updated_robots.push_back(robot_idx);
        break;
    }

    case ExecutionFeedbackType::HOLD_ACK:
        robot_it->state = RobotExecState::HELD;
        robot_it->externally_held = true;
        state.robot_states[static_cast<std::size_t>(robot_idx)] = robot_it->state;
        result.updated_robots.push_back(robot_idx);
        break;

    case ExecutionFeedbackType::RESUME_ACK:
        robot_it->externally_held = false;
        if (state.robot_current_action[static_cast<std::size_t>(robot_idx)].has_value()) {
            robot_it->state = RobotExecState::EXECUTING;
        } else {
            robot_it->state = RobotExecState::WAITING_DEPENDENCY;
        }
        state.robot_states[static_cast<std::size_t>(robot_idx)] = robot_it->state;
        result.updated_robots.push_back(robot_idx);
        break;

    case ExecutionFeedbackType::FAULT:
    case ExecutionFeedbackType::LOCALIZATION_LOST:
        robot_it->faulted = true;
        if (feedback.type == ExecutionFeedbackType::LOCALIZATION_LOST) {
            robot_it->localization_lost = true;
        }
        robot_it->state = RobotExecState::HELD;
        state.robot_states[static_cast<std::size_t>(robot_idx)] = robot_it->state;
        result.updated_robots.push_back(robot_idx);
        break;
    }

    return result;
}

SadgReadyResult SadgScheduler::recompute_ready(const SadgGraph& graph,
                                               SadgRuntimeState& state,
                                               std::vector<RobotSadgState>& robots) const
{
    SadgReadyResult result;
    for (ActionIndex action_idx = 0; action_idx < static_cast<ActionIndex>(graph.action_count());
         ++action_idx) {
        auto& action_state = state.action_states[static_cast<std::size_t>(action_idx)];
        if (action_state == ActionExecState::DONE || action_state == ActionExecState::CANCELLED ||
            action_state == ActionExecState::DISPATCHED) {
            continue;
        }

        const auto& action = graph.action(action_idx);
        const auto& robot = robots.at(static_cast<std::size_t>(action.robot_index));
        if (robot.state == RobotExecState::HELD || robot.state == RobotExecState::SAFE_STOP) {
            action_state = ActionExecState::BLOCKED;
            result.blocked_actions.push_back(action_idx);
            continue;
        }

        const bool deps_ready =
            state.active_predecessor_count[static_cast<std::size_t>(action_idx)] == 0;
        const bool resources_ready = resources_available(graph, state, action);
        if (deps_ready && resources_ready) {
            if (action_state != ActionExecState::READY) {
                action_state = ActionExecState::READY;
                result.newly_ready_actions.push_back(action_idx);
            }
        } else if (resources_ready) {
            action_state = ActionExecState::PENDING;
        } else {
            action_state = ActionExecState::BLOCKED;
            result.blocked_actions.push_back(action_idx);
        }
    }

    sync_robot_states(graph, state, robots);
    for (RobotIndex robot_idx = 0; robot_idx < static_cast<RobotIndex>(robots.size());
         ++robot_idx) {
        state.robot_states[static_cast<std::size_t>(robot_idx)] =
            robots[static_cast<std::size_t>(robot_idx)].state;
    }
    return result;
}

SadgDispatchResult SadgScheduler::dispatch_ready_actions(const SadgGraph& graph,
                                                         SadgRuntimeState& state,
                                                         std::vector<RobotSadgState>& robots,
                                                         int64_t now_ms) const
{
    SadgDispatchResult result;
    for (RobotIndex robot_idx = 0; robot_idx < static_cast<RobotIndex>(robots.size());
         ++robot_idx) {
        if (state.robot_current_action[static_cast<std::size_t>(robot_idx)].has_value()) continue;
        if (robots[static_cast<std::size_t>(robot_idx)].state == RobotExecState::HELD ||
            robots[static_cast<std::size_t>(robot_idx)].state == RobotExecState::SAFE_STOP) {
            continue;
        }

        for (ActionIndex action_idx : graph.robot_actions(robot_idx)) {
            if (state.action_states[static_cast<std::size_t>(action_idx)] !=
                ActionExecState::READY) {
                continue;
            }

            const auto& action = graph.action(action_idx);
            if (action.is_wait && !policy_.dispatch_wait_actions) {
                continue;
            }
            if (!resources_available(graph, state, action)) {
                continue;
            }

            state.action_states[static_cast<std::size_t>(action_idx)] =
                ActionExecState::DISPATCHED;
            state.action_dispatch_time_ms[static_cast<std::size_t>(action_idx)] = now_ms;
            state.robot_current_action[static_cast<std::size_t>(robot_idx)] = action_idx;
            robots[static_cast<std::size_t>(robot_idx)].current_action_index = action_idx;
            robots[static_cast<std::size_t>(robot_idx)].state = RobotExecState::EXECUTING;
            state.robot_states[static_cast<std::size_t>(robot_idx)] =
                RobotExecState::EXECUTING;

            for (ResourceIndex resource_idx : action.resources) {
                state.resource_states[static_cast<std::size_t>(resource_idx)] =
                    action.is_wait ? ResourceState::RESERVED : ResourceState::OCCUPIED;
            }

            ExecutionCommand command;
            command.robot_id = robots[static_cast<std::size_t>(robot_idx)].robot_id;
            command.action_id = action.action_id;
            command.target_vertex = action.to_vertex;
            command.type = action.is_wait ? ExecutionCommandType::HOLD_POSITION
                                          : ExecutionCommandType::MOVE_TO_VERTEX;
            command.reason = "dispatch_ready_action";

            result.commands.push_back(std::move(command));
            result.dispatched_actions.push_back(action_idx);
            break;
        }
    }

    return result;
}

void SadgScheduler::hold_robot(const SadgGraph&,
                               SadgRuntimeState& state,
                               std::vector<RobotSadgState>& robots,
                               RobotIndex robot_idx,
                               const std::string&) const
{
    robots.at(static_cast<std::size_t>(robot_idx)).state = RobotExecState::HELD;
    robots.at(static_cast<std::size_t>(robot_idx)).externally_held = true;
    state.robot_states[static_cast<std::size_t>(robot_idx)] = RobotExecState::HELD;
    if (const auto current =
            state.robot_current_action.at(static_cast<std::size_t>(robot_idx));
        current.has_value()) {
        state.action_states[static_cast<std::size_t>(*current)] = ActionExecState::BLOCKED;
    }
}

void SadgScheduler::enter_safe_stop(const SadgGraph& graph,
                                    SadgRuntimeState& state,
                                    std::vector<RobotSadgState>& robots,
                                    const std::string&) const
{
    for (auto& robot : robots) {
        robot.state = RobotExecState::SAFE_STOP;
    }
    std::fill(state.robot_states.begin(), state.robot_states.end(), RobotExecState::SAFE_STOP);
    for (std::size_t i = 0; i < state.action_states.size(); ++i) {
        if (state.action_states[i] != ActionExecState::DONE) {
            state.action_states[i] = ActionExecState::CANCELLED;
        }
    }
    for (ResourceIndex resource_idx = 0;
         resource_idx < static_cast<ResourceIndex>(graph.resource_count());
         ++resource_idx) {
        state.resource_states[static_cast<std::size_t>(resource_idx)] = ResourceState::BLOCKED;
    }
}

}  // namespace sadg
