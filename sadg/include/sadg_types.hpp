#pragma once

#include "lifelong_types.hpp"
#include "planning_types.hpp"

#include <cstdint>
#include <deque>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace sadg {

using RobotIndex = int;
using ActionIndex = int;
using ResourceIndex = int;
using Tick = int;

using RobotId = std::string;
using ActionId = std::string;
using ResourceId = std::string;
using EdgeId = std::string;

enum class ActionExecState {
    PENDING,
    READY,
    BLOCKED,
    DISPATCHED,
    DONE,
    CANCELLED
};

enum class RobotExecState {
    READY,
    WAITING_DEPENDENCY,
    EXECUTING,
    HELD,
    IDLE_AT_GOAL,
    SAFE_STOP
};

enum class ResourceState {
    FREE,
    RESERVED,
    OCCUPIED,
    BLOCKED
};

enum class SadgEdgeType {
    INTRA_ROBOT_ORDER,
    VERTEX_RELEASE,
    EDGE_ORDER,
    HOLD_BARRIER,
    SWITCHABLE_ORDER
};

enum class ResourceKind {
    VERTEX,
    DIRECTED_EDGE,
    GATEWAY,
    VIRTUAL_LOCK,
    INTERSECTION,
    STATION_ZONE,
    SAFETY_ZONE
};

enum class ExecutionFeedbackType {
    POSITION_UPDATE,
    ACTION_DONE,
    HOLD_ACK,
    RESUME_ACK,
    FAULT,
    LOCALIZATION_LOST
};

enum class ExecutionCommandType {
    MOVE_TO_VERTEX,
    HOLD_POSITION,
    RESUME,
    SAFE_STOP
};

enum class SadgPlanHealth {
    HEALTHY,
    TIMING_DEVIATION,
    STALE,
    REPLAN_REQUIRED_STALE,
    INVALID
};

struct SadgTimingProfile {
    int duration_ticks = 1;
    int slack_ticks = 0;
};

struct ActionTimingWindow {
    Tick planned_start_tick = 0;
    Tick planned_finish_tick = 0;
    Tick latest_safe_finish_tick = 0;
    std::optional<int64_t> actual_dispatch_time_ms;
    std::optional<int64_t> actual_finish_time_ms;
};

struct SadgTimingPolicy {
    int dt_ms = 1000;
    int completion_slack_ms = 1000;
    int dependency_wait_timeout_ms = 3000;
    int stale_observation_window_ms = 2000;
};

struct SadgReplanTriggerPolicy {
    int dependency_block_escalation_ms = 3000;
    bool replan_on_goal_change = true;
    bool replan_on_resource_unavailable = true;
};

struct SadgSchedulingPolicy {
    bool dispatch_wait_actions = true;
};

struct SadgActionNode {
    ActionId action_id;
    RobotIndex robot_index = -1;
    ActionIndex action_index = -1;

    int from_vertex = -1;
    int to_vertex = -1;
    bool is_wait = false;

    Tick start_tick = 0;
    Tick finish_tick = 0;
    SadgTimingProfile timing;
    ActionTimingWindow window;

    std::vector<ResourceIndex> resources;
};

struct SadgDependencyEdge {
    EdgeId edge_id;
    ActionIndex from_action = -1;
    ActionIndex to_action = -1;
    SadgEdgeType type = SadgEdgeType::INTRA_ROBOT_ORDER;
    bool switchable = false;
    bool active = true;
};

struct SadgResource {
    ResourceId resource_id;
    ResourceIndex resource_index = -1;
    ResourceKind kind = ResourceKind::VERTEX;
    ResourceState state = ResourceState::FREE;

    std::optional<RobotIndex> owner_robot_index;
    std::optional<ActionIndex> owner_action_index;

    std::vector<int> backing_vertices;
    std::optional<std::pair<int, int>> directed_edge;
};

struct RobotSadgState {
    RobotId robot_id;
    RobotIndex robot_index = -1;

    int current_vertex = -1;
    std::optional<int> current_goal;

    ActionIndex committed_action_index = -1;
    std::optional<ActionIndex> current_action_index;

    RobotExecState state = RobotExecState::IDLE_AT_GOAL;

    bool externally_held = false;
    bool faulted = false;
    bool localization_lost = false;
};

struct SadgPlanInput {
    FleetSnapshot snapshot;
    Solution solution;
    std::vector<GoalAssignment> assignments;
    int base_timestep = 0;
};

struct ExecutionFeedback {
    RobotId robot_id;
    int64_t timestamp_ms = 0;
    uint64_t sequence_no = 0;
    ExecutionFeedbackType type = ExecutionFeedbackType::POSITION_UPDATE;

    std::optional<ActionId> action_id;
    std::optional<int> actual_vertex;
    std::optional<double> confidence;
    std::string detail;
};

struct ExecutionCommand {
    RobotId robot_id;
    ExecutionCommandType type = ExecutionCommandType::MOVE_TO_VERTEX;
    std::optional<int> target_vertex;
    std::optional<ActionId> action_id;
    std::string reason;
};

struct SadgReplanIntent {
    FleetSnapshot snapshot;
    std::vector<GoalAssignment> assignments;
    std::vector<int> affected_robot_indices;
    FrozenPlanSet frozen;
    std::string reason;
};

struct RuntimeReplanResult {
    PlanningStatus status = PlanningStatus::INVALID_REQUEST;
    Solution solution;
    PlanMetrics metrics;
    bool used_global_replan = false;
    bool safe_stop_required = false;
    std::string reason;
};

struct SadgHealthReport {
    SadgPlanHealth health = SadgPlanHealth::HEALTHY;
    std::vector<RobotIndex> seed_robot_indices;
    std::vector<ActionIndex> implicated_actions;
    std::vector<ResourceIndex> implicated_resources;
    std::string reason;
};

struct SadgRuntimeState {
    std::vector<ActionExecState> action_states;
    std::vector<RobotExecState> robot_states;
    std::vector<ResourceState> resource_states;

    std::vector<int> active_predecessor_count;
    std::vector<bool> edge_active;

    std::vector<std::optional<int64_t>> action_dispatch_time_ms;
    std::vector<std::optional<int64_t>> action_finish_time_ms;

    std::vector<std::optional<ActionIndex>> robot_current_action;
    std::vector<ActionIndex> robot_committed_prefix_end;
};

struct SadgScheduleDecision {
    std::vector<ActionIndex> ready_actions;
    std::vector<ActionIndex> dispatched_actions;
    std::vector<RobotIndex> blocked_robots;
    std::string reason;
};

struct SadgSchedulerInitResult {
    SadgRuntimeState state;
    std::vector<RobotSadgState> robots;
    std::string warning;
};

struct SadgFeedbackApplyResult {
    std::vector<ActionIndex> updated_actions;
    std::vector<RobotIndex> updated_robots;
    std::vector<ResourceIndex> updated_resources;
    std::string reason;
};

struct SadgReadyResult {
    std::vector<ActionIndex> newly_ready_actions;
    std::vector<ActionIndex> blocked_actions;
};

struct SadgDispatchResult {
    std::vector<ExecutionCommand> commands;
    std::vector<ActionIndex> dispatched_actions;
};

inline std::string_view to_string(ActionExecState state)
{
    switch (state) {
    case ActionExecState::PENDING:
        return "PENDING";
    case ActionExecState::READY:
        return "READY";
    case ActionExecState::BLOCKED:
        return "BLOCKED";
    case ActionExecState::DISPATCHED:
        return "DISPATCHED";
    case ActionExecState::DONE:
        return "DONE";
    case ActionExecState::CANCELLED:
        return "CANCELLED";
    }
    return "UNKNOWN_ACTION_STATE";
}

inline std::string_view to_string(RobotExecState state)
{
    switch (state) {
    case RobotExecState::READY:
        return "READY";
    case RobotExecState::WAITING_DEPENDENCY:
        return "WAITING_DEPENDENCY";
    case RobotExecState::EXECUTING:
        return "EXECUTING";
    case RobotExecState::HELD:
        return "HELD";
    case RobotExecState::IDLE_AT_GOAL:
        return "IDLE_AT_GOAL";
    case RobotExecState::SAFE_STOP:
        return "SAFE_STOP";
    }
    return "UNKNOWN_ROBOT_STATE";
}

inline std::string_view to_string(ResourceState state)
{
    switch (state) {
    case ResourceState::FREE:
        return "FREE";
    case ResourceState::RESERVED:
        return "RESERVED";
    case ResourceState::OCCUPIED:
        return "OCCUPIED";
    case ResourceState::BLOCKED:
        return "BLOCKED";
    }
    return "UNKNOWN_RESOURCE_STATE";
}

inline std::string_view to_string(SadgEdgeType type)
{
    switch (type) {
    case SadgEdgeType::INTRA_ROBOT_ORDER:
        return "INTRA_ROBOT_ORDER";
    case SadgEdgeType::VERTEX_RELEASE:
        return "VERTEX_RELEASE";
    case SadgEdgeType::EDGE_ORDER:
        return "EDGE_ORDER";
    case SadgEdgeType::HOLD_BARRIER:
        return "HOLD_BARRIER";
    case SadgEdgeType::SWITCHABLE_ORDER:
        return "SWITCHABLE_ORDER";
    }
    return "UNKNOWN_EDGE_TYPE";
}

inline std::string_view to_string(ResourceKind kind)
{
    switch (kind) {
    case ResourceKind::VERTEX:
        return "VERTEX";
    case ResourceKind::DIRECTED_EDGE:
        return "DIRECTED_EDGE";
    case ResourceKind::GATEWAY:
        return "GATEWAY";
    case ResourceKind::VIRTUAL_LOCK:
        return "VIRTUAL_LOCK";
    case ResourceKind::INTERSECTION:
        return "INTERSECTION";
    case ResourceKind::STATION_ZONE:
        return "STATION_ZONE";
    case ResourceKind::SAFETY_ZONE:
        return "SAFETY_ZONE";
    }
    return "UNKNOWN_RESOURCE_KIND";
}

inline std::string_view to_string(ExecutionFeedbackType type)
{
    switch (type) {
    case ExecutionFeedbackType::POSITION_UPDATE:
        return "POSITION_UPDATE";
    case ExecutionFeedbackType::ACTION_DONE:
        return "ACTION_DONE";
    case ExecutionFeedbackType::HOLD_ACK:
        return "HOLD_ACK";
    case ExecutionFeedbackType::RESUME_ACK:
        return "RESUME_ACK";
    case ExecutionFeedbackType::FAULT:
        return "FAULT";
    case ExecutionFeedbackType::LOCALIZATION_LOST:
        return "LOCALIZATION_LOST";
    }
    return "UNKNOWN_FEEDBACK_TYPE";
}

inline std::string_view to_string(ExecutionCommandType type)
{
    switch (type) {
    case ExecutionCommandType::MOVE_TO_VERTEX:
        return "MOVE_TO_VERTEX";
    case ExecutionCommandType::HOLD_POSITION:
        return "HOLD_POSITION";
    case ExecutionCommandType::RESUME:
        return "RESUME";
    case ExecutionCommandType::SAFE_STOP:
        return "SAFE_STOP";
    }
    return "UNKNOWN_COMMAND_TYPE";
}

inline std::string_view to_string(SadgPlanHealth health)
{
    switch (health) {
    case SadgPlanHealth::HEALTHY:
        return "HEALTHY";
    case SadgPlanHealth::TIMING_DEVIATION:
        return "TIMING_DEVIATION";
    case SadgPlanHealth::STALE:
        return "STALE";
    case SadgPlanHealth::REPLAN_REQUIRED_STALE:
        return "REPLAN_REQUIRED_STALE";
    case SadgPlanHealth::INVALID:
        return "INVALID";
    }
    return "UNKNOWN_PLAN_HEALTH";
}

inline std::ostream& operator<<(std::ostream& os, ActionExecState state)
{
    return os << to_string(state);
}

inline std::ostream& operator<<(std::ostream& os, RobotExecState state)
{
    return os << to_string(state);
}

inline std::ostream& operator<<(std::ostream& os, ResourceState state)
{
    return os << to_string(state);
}

inline std::ostream& operator<<(std::ostream& os, SadgEdgeType type)
{
    return os << to_string(type);
}

inline std::ostream& operator<<(std::ostream& os, ResourceKind kind)
{
    return os << to_string(kind);
}

inline std::ostream& operator<<(std::ostream& os, ExecutionFeedbackType type)
{
    return os << to_string(type);
}

inline std::ostream& operator<<(std::ostream& os, ExecutionCommandType type)
{
    return os << to_string(type);
}

inline std::ostream& operator<<(std::ostream& os, SadgPlanHealth health)
{
    return os << to_string(health);
}

}  // namespace sadg
