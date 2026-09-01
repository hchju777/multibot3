// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <string>

#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_mapf/adapter/discrete_plan_adapter.hpp"
#include "mrs_mapf/core/visit_order.hpp"
#include "mrs_mapf/io/json_io.hpp"
#include "mrs_mapf/plugins/priority_safe_interval_search.hpp"
#include "mrs_mapf/service/mapf_planning_service.hpp"

using namespace mrs_mapf::core;
using namespace mrs_mapf::service;
using namespace mrs_mapf::adapter;
using mrs_mapf::plugins::PrioritySafeIntervalSearch;

/// @file test_boundary_emit.cpp
/// @brief Boundary-artifact checker test: run the full pipeline (plugin ->
/// service -> adapter -> io) on an opposing-traffic instance and write
/// `mrs.discrete_plan` 4.0.0 (+ the matching `mrs.roadmap`/`mrs.assignment` it
/// was computed against) next to the test binary, so
/// `check_mapf_solution.py`/`check_boundary_schema.py`/`check_schema_purity.py`
/// can be run against a real production of this module's own code (not a
/// hand-typed fixture).

namespace
{

std::string roadmap_json()
{
    // Mirrors test/fixtures.hpp's make_line_roadmap() (A-B-C-D, 5.0 m each edge).
    return R"({
  "schema": "mrs.roadmap",
  "schema_version": "3.0.0",
  "instance_id": "boundary-emit",
  "nodes": [
    {"id": "A", "x": 0.0, "y": 0.0, "clearance_m": 1.0},
    {"id": "B", "x": 5.0, "y": 0.0, "clearance_m": 1.0},
    {"id": "C", "x": 10.0, "y": 0.0, "clearance_m": 1.0},
    {"id": "D", "x": 15.0, "y": 0.0, "clearance_m": 1.0}
  ],
  "edges": [
    {"from": "A", "to": "B", "traversal": "bidirectional", "length_m": 5.0, "width_m": 2.0, "capacity_robots": 1, "corridor": ""},
    {"from": "B", "to": "C", "traversal": "bidirectional", "length_m": 5.0, "width_m": 2.0, "capacity_robots": 1, "corridor": ""},
    {"from": "C", "to": "D", "traversal": "bidirectional", "length_m": 5.0, "width_m": 2.0, "capacity_robots": 1, "corridor": ""}
  ],
  "endpoints": ["A", "C", "D"],
  "corridors": [],
  "provenance": {
    "min_separation_m": 0.6, "wall_inflation_m": 0.1,
    "capacity_rule_id": "rule_a", "generator_version": "test-fixture"
  }
})";
}

std::string assignment_json()
{
    return R"({
  "schema": "mrs.assignment",
  "schema_version": "2.0.0",
  "instance_id": "boundary-emit",
  "revision": 0,
  "assignments": [
    {"robot": "r0", "start": "A", "goals": [{"goal_id": "g0", "task": "t0", "location": "C"}]},
    {"robot": "r1", "start": "C", "goals": [{"goal_id": "g1", "task": "t1", "location": "D"}]}
  ],
  "unassigned_tasks": []
})";
}

}  // namespace

int main(int argc, char** argv)
{
    const std::string dir = argc > 1 ? std::string(argv[1]) : std::string(".");

    PlanningRequest req;
    req.instance_id = "boundary-emit";
    req.roadmap = mrs_mapf::test::make_line_roadmap();
    req.robot_specs = mrs_mapf::test::make_specs(2, 1.0);
    AssignmentEntry a0;
    a0.robot = "r0";
    a0.start = "A";
    a0.goal_locations = {"C"};
    AssignmentEntry a1;
    a1.robot = "r1";
    a1.start = "C";
    a1.goal_locations = {"D"};
    req.assignments = {a0, a1};
    req.affected_robots = {"r0", "r1"};

    PrioritySafeIntervalSearch strategy;
    MapfPlanningService svc(strategy);
    RunResult r = svc.run_once(req, 1, "boundary-emit");
    CHECK(r.outcome == RunOutcome::kPublished);
    CHECK(r.self_check.outcome == SelfCheckOutcome::kOk);

    BoundaryDiscretePlan doc;
    CHECK(ok(DiscretePlanAdapter::to_boundary(r.draft, doc)));
    const std::string plan_json = mrs_mapf::io::to_json(doc);
    CHECK(plan_json.find("\"schema\": \"mrs.discrete_plan\"") != std::string::npos);
    CHECK(plan_json.find("\"schema_version\": \"4.0.0\"") != std::string::npos);
    // Schema purity — no algorithm/solver keys anywhere in the emitted text.
    CHECK(plan_json.find("timestep") == std::string::npos);
    CHECK(plan_json.find("k_robustness") == std::string::npos);
    CHECK(plan_json.find("suboptimality") == std::string::npos);
    CHECK(plan_json.find("solver") == std::string::npos);
    CHECK(plan_json.find("makespan") == std::string::npos);
    CHECK(plan_json.find("\"cost\"") == std::string::npos);

    // Written with filename stems matching the schema files' own names
    // (`discrete_plan.schema.json` etc.) so check_boundary_schema.py's
    // stem-based artifact<->schema matching resolves them.
    std::ofstream(dir + "/discrete_plan.json") << plan_json;
    std::ofstream(dir + "/roadmap.json") << roadmap_json();
    std::ofstream(dir + "/assignment.json") << assignment_json();

    return mapf_test::summary();
}
