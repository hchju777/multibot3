// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_mapf/core/self_checks.hpp"

using namespace mrs_mapf::core;

/// @file test_self_checks.cpp
/// @brief 🔴 함정 ①(발주서) — proves ⓐ(start-vertex uniqueness) and
/// ⓑ(A5 segment-graph acyclicity) are DISTINCT predicates: a plan exists where
/// ⓐ fires and ⓑ alone would not have caught it (a naive fallback parking two
/// robots on the same sentinel vertex — zero segments, so the segment graph has
/// zero edges and is vacuously "untested", not "failed"), and a plan exists
/// where ⓑ fires and ⓐ does not (a two-robot edge-swap whose starts are
/// distinct vertices).

namespace
{

DiscretePlanDraft sentinel_pileup_plan()
{
    // Two DIFFERENT robots both parked (length-1, R29-style fallback) at the
    // SAME vertex — the "센티널 몰림" failure mode.
    DiscretePlanDraft d;
    RobotPlan p0;
    p0.robot = "r0";
    p0.terminal = Terminal::kNoProgress;
    p0.steps.push_back({0, "SENTINEL"});
    RobotPlan p1;
    p1.robot = "r1";
    p1.terminal = Terminal::kNoProgress;
    p1.steps.push_back({0, "SENTINEL"});
    d.plans = {p0, p1};
    // Contract requires a visit_order entry for any location with >= 2 items —
    // constructed here purely so the fixture is self-consistent; irrelevant to
    // ⓑ, since zero-segment robots contribute zero segment-graph nodes/edges.
    VisitOrderEntry ve;
    ve.location = "SENTINEL";
    ve.sequence = {{"r0", 0}, {"r1", 0}};
    d.visit_order = {ve};
    return d;
}

DiscretePlanDraft edge_swap_cycle_plan()
{
    // r0: A -> B.  r1: B -> A.  Distinct starts (A != B) — ⓐ does NOT fire.
    // visit_order claims "r0 was at A before r1 arrived at A" (A: r0#0 before
    // r1#1) AND "r1 was at B before r0 arrived at B" (B: r1#0 before r0#1) —
    // the classic head-on swap, which the 퇴거->진입 mapping turns into the
    // 2-cycle r0#0 -> r1#0 -> r0#0.
    DiscretePlanDraft d;
    RobotPlan p0;
    p0.robot = "r0";
    p0.terminal = Terminal::kGoalReached;
    p0.steps = {{0, "A"}, {1, "B"}};
    RobotPlan p1;
    p1.robot = "r1";
    p1.terminal = Terminal::kGoalReached;
    p1.steps = {{0, "B"}, {1, "A"}};
    d.plans = {p0, p1};

    VisitOrderEntry at_a;
    at_a.location = "A";
    at_a.sequence = {{"r0", 0}, {"r1", 1}};
    VisitOrderEntry at_b;
    at_b.location = "B";
    at_b.sequence = {{"r1", 0}, {"r0", 1}};
    d.visit_order = {at_a, at_b};
    return d;
}

}  // namespace

int main()
{
    // --- ⓐ fires; ⓑ ALONE (on the same draft) is untested, not "failed" ---
    {
        DiscretePlanDraft d = sentinel_pileup_plan();
        SelfCheckReport report;
        SelfCheckOutcome outcome = run_self_checks(d, report);
        CHECK(outcome == SelfCheckOutcome::kStartVertexCollision);
        CHECK(report.collisions.size() == 1);
        CHECK(report.collisions[0] == "SENTINEL");

        // Demonstrate the trap directly: ⓑ run in isolation on the SAME two
        // plans (length-1 each, so zero segments) — WITHOUT the visit_order
        // bookkeeping ⓐ's fixture needed for contract validity — sees zero
        // segment-graph nodes and zero edges: vacuously "untested", not
        // "failed". This is exactly why implementing ⓑ alone misses the
        // sentinel pile-up: there is no segment for a naive parked-robot
        // collision to appear as a cycle IN.
        DiscretePlanDraft bare = d;
        bare.visit_order.clear();
        SelfCheckReport b_only;
        bool b_ok = check_segment_graph_acyclic(bare, b_only);
        CHECK(!b_ok);
        CHECK(b_only.outcome == SelfCheckOutcome::kUntested);
        CHECK(b_only.cycle.empty());  // not a cycle finding — a non-finding.
        CHECK(b_only.malformed_edges.empty());
    }

    // --- ⓑ fires (2-cycle); ⓐ does not (distinct start vertices) ---
    {
        DiscretePlanDraft d = edge_swap_cycle_plan();
        CHECK(check_start_vertex_uniqueness(d).empty());  // ⓐ alone: no collision.

        SelfCheckReport report;
        SelfCheckOutcome outcome = run_self_checks(d, report);
        CHECK(outcome == SelfCheckOutcome::kCycleDetected);
        CHECK(report.cycle.size() >= 2);
    }

    // --- A feasible, acyclic, collision-free plan passes both ---
    {
        DiscretePlanDraft d;
        RobotPlan p0;
        p0.robot = "r0";
        p0.terminal = Terminal::kGoalReached;
        p0.steps = {{0, "A"}, {1, "B"}, {2, "C"}};
        d.plans = {p0};
        SelfCheckReport report;
        SelfCheckOutcome outcome = run_self_checks(d, report);
        CHECK(outcome == SelfCheckOutcome::kOk);
    }

    return mapf_test::summary();
}
