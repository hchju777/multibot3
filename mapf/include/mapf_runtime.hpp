#pragma once

#include "lifelong_types.hpp"

class MapfPlanner;

class ScenarioRuntime {
public:
    explicit ScenarioRuntime(MapfPlanner& planner);

    ScenarioRunResult run(const LifelongScenario& scenario);

private:
    MapfPlanner& planner_;
};

using MapfRuntime = ScenarioRuntime;
