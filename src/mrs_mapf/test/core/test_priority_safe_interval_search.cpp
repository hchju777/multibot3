// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_mapf/core/self_checks.hpp"
#include "mrs_mapf/core/visit_order.hpp"
#include "mrs_mapf/plugins/priority_safe_interval_search.hpp"

using namespace mrs_mapf::core;
using mrs_mapf::plugins::PrioritySafeIntervalSearch;

/// @file test_priority_safe_interval_search.cpp
/// @brief End-to-end test of the ONE implemented plugin (`255`§255-2-2 「셋째」
/// family — feasible-only, no completeness claim).
///
/// 🔴 Known limitation this file documents WITH A TEST rather than hiding it
/// (`20b_impl_mapf.md`): a full-corridor opposing swap that forces one robot to
/// WAIT at an intermediate vertex can produce a raw path pair this plugin
/// reports as `solve().ok()`, whose combined `visit_order` is nonetheless
/// CYCLIC once assembled — because the contract's entry-order-only
/// `visit_order` (진입 순) cannot distinguish "arrived and immediately left"
/// from "arrived and waited," and the retreat->entry mapping assumes departure
/// order tracks entry order. This is exactly the family's declared lack of a
/// completeness/soundness guarantee (`255`§255-2-2 「셋째」 row: "없다"), NOT a
/// discrete_plan schema violation — the plugin's raw output alone does not
/// promise a self-check-clean combined plan; `service::MapfPlanningService`
/// is the layer that runs self-checks and escalates/falls back when this
/// happens (see `test_planning_service.cpp`'s R23->R29 case, which uses this
/// exact scenario).

namespace
{

PlanningRequest opposing_traffic_request()
{
    PlanningRequest req;
    req.instance_id = "test-opposing";
    req.roadmap = mrs_mapf::test::make_line_roadmap();
    req.robot_specs = mrs_mapf::test::make_specs(2, 1.0);
    AssignmentEntry a0;
    a0.robot = "r0";
    a0.start = "A";
    a0.goal_locations = {"D"};
    AssignmentEntry a1;
    a1.robot = "r1";
    a1.start = "D";
    a1.goal_locations = {"A"};
    req.assignments = {a0, a1};
    req.affected_robots = {"r0", "r1"};
    return req;
}

}  // namespace

int main()
{
    // --- raw feasibility: an opposing-traffic pair on a single-lane corridor —
    //     the plugin finds a continuous-time-safe path for EACH robot (no
    //     vertex/edge overlap in the times it itself computed); whether the
    //     COMBINED discrete plan self-checks is a separate question (see the
    //     file header and test_planning_service.cpp) ---
    {
        PrioritySafeIntervalSearch strategy;
        auto req = opposing_traffic_request();
        auto out = strategy.solve(req, req.affected_robots);
        CHECK(out.ok());
        if (out.ok())
        {
            const auto& plans_by_robot = out.value();
            CHECK(plans_by_robot.count("r0") == 1);
            CHECK(plans_by_robot.count("r1") == 1);
            CHECK(plans_by_robot.at("r0").plan.steps.front().location == "A");
            CHECK(plans_by_robot.at("r0").plan.steps.back().location == "D");
            CHECK(plans_by_robot.at("r1").plan.steps.front().location == "D");
            CHECK(plans_by_robot.at("r1").plan.steps.back().location == "A");
            CHECK(plans_by_robot.at("r0").plan.terminal == Terminal::kGoalReached);
            CHECK(plans_by_robot.at("r1").plan.terminal == Terminal::kGoalReached);
        }
    }

    // --- a scenario the FULL pipeline (plugin + visit_order + self-check)
    //     DOES accept and genuinely EXERCISES ⓑ (edge_count > 0, not vacuously
    //     untested): r0 travels A->B->C (2 segments, so Type-1 contributes an
    //     edge on its own); r1 travels C->D but departs C immediately at t=0,
    //     long before r0 arrives at C at t=10 — a real shared-vertex handoff
    //     with no conflict, same-direction-of-time (no wait-induced order
    //     mismatch, unlike the opposing-swap case this file's header documents) ---
    {
        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.instance_id = "test-disjoint";
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

        auto out = strategy.solve(req, req.affected_robots);
        CHECK(out.ok());
        if (out.ok())
        {
            const auto& plans_by_robot = out.value();
            std::vector<RobotPlan> plans = {plans_by_robot.at("r0").plan,
                                            plans_by_robot.at("r1").plan};
            std::map<std::string, double> speed_of = {{"r0", 1.0}, {"r1", 1.0}};
            std::map<std::string, std::vector<double>> schedules = {
                {"r0", plans_by_robot.at("r0").arrival_s},
                {"r1", plans_by_robot.at("r1").arrival_s}};
            DiscretePlanDraft draft;
            draft.instance_id = "test-disjoint";
            draft.plan_revision = 1;
            draft.plans = plans;
            draft.visit_order = build_visit_order(plans, speed_of, req.roadmap, {}, schedules);

            SelfCheckReport report;
            SelfCheckOutcome outcome = run_self_checks(draft, report);
            CHECK(outcome == SelfCheckOutcome::kOk);
        }
    }

    // --- an idle robot (no goals) => length-1 plan, goal_reached, no search needed ---
    {
        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "B";
        a0.goal_locations = {};  // idle.
        req.assignments = {a0};
        req.affected_robots = {"r0"};
        auto out = strategy.solve(req, req.affected_robots);
        CHECK(out.ok());
        if (out.ok())
        {
            CHECK(out.value().at("r0").plan.steps.size() == 1);
            CHECK(out.value().at("r0").plan.steps.front().location == "B");
            CHECK(out.value().at("r0").plan.terminal == Terminal::kGoalReached);
        }
    }

    // --- unreachable goal (disconnected node) => solve fails (feasible-only family) ---
    {
        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.roadmap.node_ids.push_back("ISLAND");
        req.roadmap.out_arcs["ISLAND"] = {};  // no edges in/out — unreachable.
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"ISLAND"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};
        auto out = strategy.solve(req, req.affected_robots);
        CHECK(!out.ok());
        if (!out.ok())
        {
            CHECK(out.error().robot == "r0");
        }
    }

    // --- blocked edge is honored (R04): the only route is excluded => infeasible ---
    {
        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"D"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};
        req.blocked = {{"B", "C"}, {"C", "B"}};  // sever the only path, both directions.
        auto out = strategy.solve(req, req.affected_robots);
        CHECK(!out.ok());
    }

    // --- 48차 회귀(`369_p2` 실측): an idle (no-goal) robot parked FOREVER on the
    // only chokepoint of another robot's route must make that route honestly
    // INFEASIBLE (a real `PlanFailure`), never a numerically-finite "success"
    // through a vertex nobody will ever vacate. Robot ids are chosen so pure
    // alphabetical solve order would plan the mover BEFORE the idle robot
    // ("a" < "z") — this is exactly the ordering `369_p2`'s launch evidence
    // caught (`visit_order(J0302) 의 r5#0 -> r2#5`, idle robot alphabetically
    // AFTER the mover it silently let through). Before the 48차 fix (idle-first
    // solve order + `kParkedHorizon`-aware `earliest_safe_departure`), this
    // scenario returned `ok()` with a path straight through "z"'s permanent
    // park — a plan no self-check could ever accept.
    {
        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();  // A-B-C-D, only route.
        req.robot_specs = {{"a", 1.0}, {"z", 1.0}};         // ids must match the assignments below.
        AssignmentEntry mover;
        mover.robot = "a";
        mover.start = "A";
        mover.goal_locations = {"D"};  // the ONLY route passes through "B".
        AssignmentEntry idle;
        idle.robot = "z";
        idle.start = "B";
        idle.goal_locations = {};  // idle — parked at B forever.
        req.assignments = {mover, idle};
        req.affected_robots = {"a", "z"};

        auto out = strategy.solve(req, req.affected_robots);
        CHECK(!out.ok());
        if (!out.ok())
        {
            CHECK(out.error().robot == "a");
        }
    }

    // --- 48차 회귀 companion: the SAME idle-forever robot must NOT block a
    // mover whose route does not actually need that vertex — the fix must not
    // over-block. Diamond roadmap A->{B,C}->D (two independent routes); "z"
    // parks forever on B, "a" must be free to reach D via C instead. ---
    {
        Roadmap diamond;
        diamond.node_ids = {"A", "B", "C", "D"};
        for (const auto& n : diamond.node_ids)
        {
            diamond.out_arcs[n] = {};
        }
        diamond.endpoints = {"A", "D"};
        auto link = [&](const std::string& u, const std::string& v, double len)
        {
            diamond.out_arcs[u].push_back({u, v, len, 2.0, 1, ""});
            diamond.out_arcs[v].push_back({v, u, len, 2.0, 1, ""});
        };
        link("A", "B", 5.0);
        link("B", "D", 5.0);
        link("A", "C", 5.0);
        link("C", "D", 5.0);

        PrioritySafeIntervalSearch strategy;
        PlanningRequest req;
        req.roadmap = diamond;
        req.robot_specs = {{"a", 1.0}, {"z", 1.0}};  // ids must match the assignments below.
        AssignmentEntry mover;
        mover.robot = "a";
        mover.start = "A";
        mover.goal_locations = {"D"};
        AssignmentEntry idle;
        idle.robot = "z";
        idle.start = "B";
        idle.goal_locations = {};
        req.assignments = {mover, idle};
        req.affected_robots = {"a", "z"};

        auto out = strategy.solve(req, req.affected_robots);
        CHECK(out.ok());
        if (out.ok())
        {
            const auto& plans_by_robot = out.value();
            CHECK(plans_by_robot.at("a").plan.terminal == Terminal::kGoalReached);
            CHECK(plans_by_robot.at("a").plan.steps.back().location == "D");
            // Must have routed via C, not through z's permanent park at B.
            bool visited_b = false;
            for (const auto& s : plans_by_robot.at("a").plan.steps)
            {
                visited_b = visited_b || (s.location == "B");
            }
            CHECK(!visited_b);
        }
    }

    return mapf_test::summary();
}
