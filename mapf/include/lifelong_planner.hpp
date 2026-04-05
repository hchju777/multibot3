#pragma once

#include "goal_distance_cache.hpp"
#include "lifelong_types.hpp"
#include "prepared_map.hpp"

struct PlanningRequest;

class MapfPlanner {
public:
    MapfPlanner(PreparedMap map, Params params);

    const PreparedMap& prepared_map() const { return map_; }
    const Params& params() const { return params_; }

    PlanResult plan_initial(const InitialPlanRequest& req);
    PlanResult replan_affected(const AffectedReplanRequest& req);
    ScenarioRunResult run(const LifelongScenario& scenario);

private:
    PlanResult plan(const PlanningRequest& req);

    PreparedMap map_;
    Params params_;
    GoalDistanceCache goal_cache_;
};

using LifelongPlanner = MapfPlanner;
