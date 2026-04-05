#include "sadg_validator.hpp"

#include <algorithm>

namespace sadg {
namespace {

int severity_rank(SadgPlanHealth health)
{
    switch (health) {
    case SadgPlanHealth::HEALTHY:
        return 0;
    case SadgPlanHealth::TIMING_DEVIATION:
        return 1;
    case SadgPlanHealth::STALE:
        return 2;
    case SadgPlanHealth::REPLAN_REQUIRED_STALE:
        return 3;
    case SadgPlanHealth::INVALID:
        return 4;
    }
    return -1;
}

void escalate_report(SadgHealthReport& report,
                     SadgPlanHealth health,
                     std::string reason,
                     std::optional<RobotIndex> robot_idx = std::nullopt,
                     std::optional<ActionIndex> action_idx = std::nullopt,
                     std::optional<ResourceIndex> resource_idx = std::nullopt)
{
    if (severity_rank(health) < severity_rank(report.health)) {
        return;
    }

    if (severity_rank(health) > severity_rank(report.health)) {
        report.health = health;
        report.reason = std::move(reason);
        report.seed_robot_indices.clear();
        report.implicated_actions.clear();
        report.implicated_resources.clear();
    } else if (report.reason.empty()) {
        report.reason = std::move(reason);
    }

    if (robot_idx.has_value() &&
        std::find(report.seed_robot_indices.begin(),
                  report.seed_robot_indices.end(),
                  *robot_idx) == report.seed_robot_indices.end()) {
        report.seed_robot_indices.push_back(*robot_idx);
    }
    if (action_idx.has_value() &&
        std::find(report.implicated_actions.begin(),
                  report.implicated_actions.end(),
                  *action_idx) == report.implicated_actions.end()) {
        report.implicated_actions.push_back(*action_idx);
    }
    if (resource_idx.has_value() &&
        std::find(report.implicated_resources.begin(),
                  report.implicated_resources.end(),
                  *resource_idx) == report.implicated_resources.end()) {
        report.implicated_resources.push_back(*resource_idx);
    }
}

bool position_compatible(const SadgGraph& graph,
                         const SadgRuntimeState& state,
                         const RobotSadgState& robot)
{
    if (robot.current_action_index.has_value()) {
        const auto& action = graph.action(*robot.current_action_index);
        return robot.current_vertex == action.from_vertex ||
               robot.current_vertex == action.to_vertex;
    }

    if (state.robot_committed_prefix_end.at(static_cast<std::size_t>(robot.robot_index)) >= 0) {
        const auto action_idx =
            state.robot_committed_prefix_end.at(static_cast<std::size_t>(robot.robot_index));
        return robot.current_vertex == graph.action(action_idx).to_vertex;
    }

    const auto& actions = graph.robot_actions(robot.robot_index);
    if (actions.empty()) {
        return true;
    }
    return robot.current_vertex == graph.action(actions.front()).from_vertex;
}

}  // namespace

SadgHealthReport SadgValidator::evaluate(const SadgGraph& graph,
                                         const SadgRuntimeState& state,
                                         const std::vector<RobotSadgState>& robots,
                                         int64_t now_ms,
                                         const SadgTimingPolicy& timing_policy,
                                         const SadgReplanTriggerPolicy& trigger_policy) const
{
    SadgHealthReport report;

    for (const auto& robot : robots) {
        if (robot.localization_lost || robot.faulted) {
            escalate_report(report,
                            SadgPlanHealth::INVALID,
                            "robot feedback is no longer trustworthy",
                            robot.robot_index);
        }

        if (!position_compatible(graph, state, robot)) {
            escalate_report(report,
                            SadgPlanHealth::INVALID,
                            "robot position is incompatible with committed prefix",
                            robot.robot_index,
                            robot.current_action_index);
        }
    }

    for (ActionIndex action_idx = 0; action_idx < static_cast<ActionIndex>(graph.action_count());
         ++action_idx) {
        const auto action_state = state.action_states.at(static_cast<std::size_t>(action_idx));
        if ((action_state == ActionExecState::DISPATCHED ||
             action_state == ActionExecState::DONE) &&
            state.active_predecessor_count.at(static_cast<std::size_t>(action_idx)) > 0) {
            escalate_report(report,
                            SadgPlanHealth::INVALID,
                            "action progressed before predecessor dependencies were released",
                            graph.action(action_idx).robot_index,
                            action_idx);
        }
    }

    for (ResourceIndex resource_idx = 0;
         resource_idx < static_cast<ResourceIndex>(graph.resource_count());
         ++resource_idx) {
        int active_users = 0;
        std::optional<RobotIndex> seed_robot;
        for (ActionIndex action_idx : graph.resource_actions(resource_idx)) {
            const auto action_state = state.action_states.at(static_cast<std::size_t>(action_idx));
            if (action_state == ActionExecState::DISPATCHED) {
                ++active_users;
                seed_robot = graph.action(action_idx).robot_index;
            }
        }
        if (active_users > 1) {
            escalate_report(report,
                            SadgPlanHealth::INVALID,
                            "resource is occupied by multiple dispatched actions",
                            seed_robot,
                            std::nullopt,
                            resource_idx);
        }
    }

    for (ActionIndex action_idx = 0; action_idx < static_cast<ActionIndex>(graph.action_count());
         ++action_idx) {
        if (state.action_states.at(static_cast<std::size_t>(action_idx)) !=
            ActionExecState::DISPATCHED) {
            continue;
        }

        const auto deadline_ms =
            static_cast<int64_t>(graph.action(action_idx).window.planned_finish_tick) *
            timing_policy.dt_ms;
        const auto latest_safe_finish_ms =
            static_cast<int64_t>(graph.action(action_idx).window.latest_safe_finish_tick) *
            timing_policy.dt_ms;

        if (now_ms > latest_safe_finish_ms + trigger_policy.dependency_block_escalation_ms) {
            escalate_report(report,
                            SadgPlanHealth::REPLAN_REQUIRED_STALE,
                            "dispatched action exceeded latest safe finish for too long",
                            graph.action(action_idx).robot_index,
                            action_idx);
            continue;
        }

        if (now_ms > latest_safe_finish_ms + timing_policy.stale_observation_window_ms) {
            escalate_report(report,
                            SadgPlanHealth::STALE,
                            "dispatched action exceeded latest safe finish",
                            graph.action(action_idx).robot_index,
                            action_idx);
            continue;
        }

        if (now_ms > deadline_ms) {
            escalate_report(report,
                            SadgPlanHealth::TIMING_DEVIATION,
                            "dispatched action is running later than nominal finish",
                            graph.action(action_idx).robot_index,
                            action_idx);
        }
    }

    if (report.health == SadgPlanHealth::HEALTHY) {
        report.reason = "plan remains healthy";
    }
    return report;
}

}  // namespace sadg
