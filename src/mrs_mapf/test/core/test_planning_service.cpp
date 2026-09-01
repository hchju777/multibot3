// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_mapf/plugins/priority_safe_interval_search.hpp"
#include "mrs_mapf/service/mapf_planning_service.hpp"

using namespace mrs_mapf::core;
using namespace mrs_mapf::service;
using mrs_mapf::plugins::PrioritySafeIntervalSearch;

/// @file test_planning_service.cpp
/// @brief End-to-end `run_once()`: the wrapping procedure (§255-1: no search
/// logic of its own) — normal publish, D-3 (`PX10`) explicit failure and
/// success (frozen start resolution), partiality (out-of-scope byte copy,
/// G-3), R06 report-only reachability, and the self-check-triggered
/// R18(floor)->R23(escalate)->R29(safety-stop fallback) chain — including the
/// case where even a raw-feasible pair of paths fails the COMBINED self-check
/// (`test_priority_safe_interval_search.cpp`'s documented family limitation)
/// and the service degrades safely instead of publishing a cyclic plan.

int main()
{
    // --- normal success: r0 travels A->B->C (2 segments), r1 departs its
    //     shared start C for D immediately at t=0 — a real, non-vacuous,
    //     conflict-free shared-vertex handoff (see
    //     test_priority_safe_interval_search.cpp's matching fixture) ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.instance_id = "svc-normal";
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

        RunResult r = svc.run_once(req, 1, "svc-normal");
        CHECK(r.outcome == RunOutcome::kPublished);
        CHECK(!r.escalated_to_global);
        CHECK(!r.used_safety_stop_fallback);
        CHECK(r.draft.plan_revision == 1);
        CHECK(r.draft.plans.size() == 2);
        CHECK(r.self_check.outcome == SelfCheckOutcome::kOk);
    }

    // --- D-3(a): baseline present but no retained plan => explicit failure, no publish ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"B"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};
        req.baseline = {{"r0", 1}};
        req.request_plan_revision = 5;
        // no previous_plan set — the required retained plan is absent.
        RunResult r = svc.run_once(req, 6, "svc-test");
        CHECK(r.outcome == RunOutcome::kNoBaselinePlan);
        CHECK(svc.no_baseline_plan_failures() == 1);
    }

    // --- D-3: baseline + matching previous_plan resolves the frozen start ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";  // assignment's OWN start — must NOT be used once baseline resolves.
        a0.goal_locations = {"D"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};

        DiscretePlanDraft prev;
        prev.instance_id = "svc-test";
        prev.plan_revision = 5;
        RobotPlan prev_r0;
        prev_r0.robot = "r0";
        prev_r0.terminal = Terminal::kPlanTruncated;
        prev_r0.steps = {{0, "A"},
                         {1, "B"},
                         {2, "C"}};  // r0 already committed 2 segments (k=2 -> at C).
        prev.plans = {prev_r0};
        req.previous_plan = prev;
        req.request_plan_revision = 5;
        req.baseline = {{"r0", 2}};  // frozen start = prev.plans[r0].steps[2].location = "C".

        RunResult r = svc.run_once(req, 6, "svc-test");
        CHECK(r.outcome == RunOutcome::kPublished);
        bool found_r0 = false;
        for (const auto& p : r.draft.plans)
        {
            if (p.robot == "r0")
            {
                found_r0 = true;
                CHECK(p.steps.front().location ==
                      "C");  // frozen start honored, NOT assignment.start="A".
            }
        }
        CHECK(found_r0);
    }

    // --- partiality (G-3): an out-of-scope robot's plan is byte-copied verbatim ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.robot_specs = mrs_mapf::test::make_specs(2, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"B"};
        AssignmentEntry a1;
        a1.robot = "r1";
        a1.start = "D";
        a1.goal_locations = {"A"};  // irrelevant — r1 is out of scope this call.
        req.assignments = {a0, a1};
        req.affected_robots = {"r0"};  // r1 is OUT of scope this call.

        DiscretePlanDraft prev;
        prev.plan_revision = 0;
        RobotPlan prev_r1;
        prev_r1.robot = "r1";
        prev_r1.terminal = Terminal::kPlanTruncated;
        prev_r1.steps = {{0, "D"},
                         {1, "C"}};  // r1's retained plan — deliberately NOT its assignment goal.
        prev.plans = {prev_r1};
        req.previous_plan = prev;

        RunResult r = svc.run_once(req, 1, "svc-test");
        CHECK(r.outcome == RunOutcome::kPublished);
        bool found_r1 = false;
        for (const auto& p : r.draft.plans)
        {
            if (p.robot == "r1")
            {
                found_r1 = true;
                CHECK(p.steps.size() == 2);
                CHECK(p.steps[0].location == "D");
                CHECK(p.steps[1].location == "C");  // byte-identical to the retained plan.
                CHECK(p.terminal == Terminal::kPlanTruncated);
            }
        }
        CHECK(found_r1);
    }

    // --- R18(floor)->R23(escalate)->R29(fallback), driven by an UNREACHABLE
    //     GOAL: neither the floor nor the (identical, single-robot) global
    //     scope can route to a disconnected node ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.roadmap.node_ids.push_back("ISLAND");
        req.roadmap.out_arcs["ISLAND"] = {};
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"ISLAND"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};

        RunResult r = svc.run_once(req, 1, "svc-test");
        CHECK(r.escalated_to_global);
        CHECK(r.used_safety_stop_fallback);
        CHECK(r.outcome ==
              RunOutcome::kPublished);  // single robot, length-1 => untested, still published.
        CHECK(r.self_check.outcome == SelfCheckOutcome::kUntested);
        CHECK(r.draft.plans.size() == 1);
        CHECK(r.draft.plans[0].steps.size() == 1);
        CHECK(r.draft.plans[0].steps[0].location == "A");
        CHECK(r.draft.plans[0].terminal == Terminal::kNoProgress);
    }

    // --- R18(floor)->R23(escalate)->R29(fallback), driven by a SELF-CHECK
    //     failure: `test_priority_safe_interval_search.cpp`'s documented
    //     limitation (a full-corridor opposing swap requiring a wait can yield
    //     a raw-feasible-but-cyclic combined plan). Escalating to "global"
    //     replans the SAME two robots deterministically (floor already was the
    //     full roster) — the identical cyclic result recurs, so the service
    //     correctly falls all the way through to the safety-stop fallback
    //     rather than ever publishing the cyclic plan. ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.instance_id = "svc-swap";
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

        RunResult r = svc.run_once(req, 1, "svc-swap");
        CHECK(r.escalated_to_global);
        CHECK(r.used_safety_stop_fallback);
        CHECK(r.outcome == RunOutcome::kPublished);
        CHECK(r.draft.plans.size() == 2);
        for (const auto& p : r.draft.plans)
        {
            CHECK(p.steps.size() == 1);  // parked, not the (cyclic) raw search result.
            CHECK(p.terminal == Terminal::kNoProgress);
        }
    }

    // --- R06 report-only reachability: does not change the outcome ---
    {
        PrioritySafeIntervalSearch strategy;
        MapfPlanningService svc(strategy);
        PlanningRequest req;
        req.roadmap = mrs_mapf::test::make_line_roadmap();
        req.roadmap.endpoints.push_back("UNREACHABLE_EP");
        req.roadmap.node_ids.push_back("UNREACHABLE_EP");
        req.roadmap.out_arcs["UNREACHABLE_EP"] = {};
        req.robot_specs = mrs_mapf::test::make_specs(1, 1.0);
        AssignmentEntry a0;
        a0.robot = "r0";
        a0.start = "A";
        a0.goal_locations = {"B"};
        req.assignments = {a0};
        req.affected_robots = {"r0"};

        RunResult r = svc.run_once(req, 1, "svc-test");
        CHECK(r.outcome == RunOutcome::kPublished);  // unaffected.
        bool warned = false;
        for (const auto& w : r.reachability_warnings)
        {
            if (w.find("UNREACHABLE_EP") != std::string::npos)
            {
                warned = true;
            }
        }
        CHECK(warned);
    }

    return mapf_test::summary();
}
