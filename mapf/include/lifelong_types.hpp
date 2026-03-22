#pragma once

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
};

struct PlanningRequest {
    std::vector<std::string> robot_ids;
    std::vector<int> starts;
    std::vector<int> goals;
    PlanMode mode = PlanMode::REPLAN;
    PlannerParams params;
};

struct PlanResult {
    Solution planning_solution;
    bool solved = false;
    bool valid = false;
    int cost = 0;
    int makespan = 0;
    std::string validation_error;
};

struct EpisodeResult {
    int index = 0;
    std::vector<std::string> activated_event_ids;
    PlanningRequest request;
    PlanResult result;
    int executed_until_timestep = 0;
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
};
