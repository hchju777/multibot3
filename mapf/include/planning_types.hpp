#pragma once

#include "solution.hpp"

#include <optional>
#include <string>
#include <vector>

enum class PlanningStatus {
    SOLVED,
    TIMEOUT_WITH_SOLUTION,
    TIMEOUT_NO_SOLUTION,
    UNSAT,
    INVALID_MAP,
    INVALID_REQUEST,
    SAFE_STOP_REQUIRED,
};

struct PlanMetrics {
    int soc = 0;
    int makespan = 0;
};

struct GoalAssignment {
    std::string id;
    std::string robot_id;
    int goal = -1;
};

struct FrozenRobotPlan {
    std::string robot_id;
    AgentPlan plan;
    bool reserve_goal_forever = true;
};

struct HoldReservation {
    std::string robot_id;
    int vertex = -1;
    int from_timestep = 0;
    std::optional<int> reverse_edge_to;
};

struct FrozenPlanSet {
    std::vector<FrozenRobotPlan> plan_suffixes;
    std::vector<HoldReservation> holds;

    bool empty() const
    {
        return plan_suffixes.empty() && holds.empty();
    }
};
