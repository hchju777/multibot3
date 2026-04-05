#pragma once

#include "lifelong_types.hpp"

struct PlanningRequest {
    std::vector<std::string> robot_ids;
    std::vector<int> starts;
    std::vector<int> goals;
    int base_timestep = 0;
    PlanMode mode = PlanMode::REPLAN;
    PlannerParams params;
    std::optional<FleetSnapshot> snapshot;
    std::vector<int> affected_robot_indices;
    FrozenPlanSet frozen;
};
