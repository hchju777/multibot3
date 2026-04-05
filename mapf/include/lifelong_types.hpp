#pragma once

#include "planning_types.hpp"
#include "params.hpp"
#include "solution.hpp"
#include "stop_condition.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

enum class ScenarioEventType {
    ASSIGN_GOAL,
};

enum class ScenarioTriggerType {
    AT_START,
    AFTER_INITIAL,
    ON_ROBOT_IDLE,
};

enum class ReplanPolicy {
    REPLAN_ON_ASSIGNMENT,
};

struct RobotSpec {
    std::string id;
    int start = -1;
    std::optional<int> home;
};

struct ScenarioTrigger {
    ScenarioTriggerType type = ScenarioTriggerType::AT_START;
    std::optional<std::string> robot_id;
};

struct ScenarioEvent {
    std::string id;
    ScenarioEventType type = ScenarioEventType::ASSIGN_GOAL;
    std::string robot_id;
    int goal = -1;
    ScenarioTrigger when;
    std::vector<std::string> after_event_ids;
};

struct LifelongScenario {
    int version = 1;
    std::string path;
    std::string map_path;
    std::string params_path;
    std::vector<RobotSpec> robots;
    ReplanPolicy replan_policy = ReplanPolicy::REPLAN_ON_ASSIGNMENT;
    std::vector<ScenarioEvent> events;
};

struct RobotRuntimeState {
    std::string id;
    int current_vertex = -1;  // planning graph vertex id
    std::optional<int> home_vertex;
    std::optional<int> active_goal;
    std::optional<std::string> active_event_id;
    bool held = false;
};

struct FleetSnapshot {
    int current_time = 0;
    std::vector<RobotRuntimeState> robots;
};

struct InitialPlanRequest {
    FleetSnapshot snapshot;
    std::vector<GoalAssignment> assignments;
    std::optional<PlannerParams> params_override;
};
using InitialPlanningRequest = InitialPlanRequest;

struct AffectedReplanRequest {
    FleetSnapshot snapshot;
    std::vector<GoalAssignment> assignments;
    std::vector<int> affected_robot_indices;
    FrozenPlanSet frozen;
    std::optional<PlannerParams> params_override;
};
using AffectedReplanningRequest = AffectedReplanRequest;

struct PlanningRequestSummary {
    std::vector<std::string> robot_ids;
    std::vector<int> affected_robot_indices;
    int base_timestep = 0;
    PlanMode mode = PlanMode::REPLAN;
    int frozen_plan_count = 0;
    int hold_count = 0;
};
using EpisodeRequestSummary = PlanningRequestSummary;

struct PlanResult {
    Solution planning_solution;
    PlanningStatus status = PlanningStatus::INVALID_REQUEST;
    bool solved = false;
    bool valid = false;
    bool timed_out = false;
    int cost = 0;
    int makespan = 0;
    PlanMetrics metrics;
    std::string validation_error;
    std::string reason;
};

struct EpisodeResult {
    int index = 0;
    std::vector<std::string> activated_event_ids;
    PlanningRequestSummary request;
    PlanResult result;
    int executed_until_timestep = 0;
    bool used_global_replan = false;
};

struct ScenarioFrame {
    int t = 0;
    int episode_index = 0;
    std::vector<int> positions;
    std::vector<std::optional<int>> active_goals;
    std::vector<std::string> activated_event_ids;
};

struct ScenarioRunResult {
    std::vector<EpisodeResult> episodes;
    std::vector<ScenarioFrame> frames;
    std::vector<RobotRuntimeState> final_robots;
    std::size_t consumed_events = 0;
    bool safe_stop_required = false;
    std::string stop_reason;
};
