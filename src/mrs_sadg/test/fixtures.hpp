// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_TEST_FIXTURES_HPP
#define MRS_SADG_TEST_FIXTURES_HPP

#include "mrs_sadg/core/types.hpp"

/// @file fixtures.hpp
/// @brief Hand-built PlanViews used across the SADG unit tests.

namespace mrs_sadg::test
{

/// @brief Following (non-opposing) two-robot instance — the f37_2 consistent
/// map: r1 and r2 both traverse A->B in the same direction, r1 before r2 at both
/// ends (H-cons holds). Ordering edges: r1#1->r2#0, r1#2->r2#1.
/// @return the plan view.
inline core::PlanView make_following_plan()
{
    // visit_order lists only SHARED locations (2+ robots) — the canonical
    // mrs.discrete_plan 4.0.0 form (sequence has minItems 2); terminal is the
    // status enum, opaque to SADG.
    core::PlanView p;
    p.instance_id = "sadg-following";
    p.plan_revision = 0;
    p.robots = {{"r1", "goal_reached", {"S1", "A", "B", "G1"}},
                {"r2", "goal_reached", {"S2", "A", "B", "G2"}}};
    p.visit_order = {{"A", {{"r1", 1}, {"r2", 1}}}, {"B", {{"r1", 2}, {"r2", 2}}}};
    return p;
}

/// @brief Following plan but with the visit order at B reversed — an H-cons
/// violation (same-direction edge A->B ordered r1<r2 at A but r2<r1 at B).
/// @return the plan view.
inline core::PlanView make_hcons_violation_plan()
{
    core::PlanView p = make_following_plan();
    p.instance_id = "sadg-hcons-violation";
    for (auto& e : p.visit_order)
    {
        if (e.location == "B")
        {
            e.sequence = {{"r2", 2}, {"r1", 2}};  // reversed vs A.
        }
    }
    return p;
}

/// @brief Feasible opposing instance (r2 first): r1 = S->A->B->G, r2 = B->A->T,
/// sharing edge A-B in opposite directions but sequenced. Produces one opposing
/// ordering edge (r2#0->r1#1, ［D7］) and one following edge (r2#1->r1#0); the
/// combined graph is acyclic.
/// @return the plan view.
inline core::PlanView make_opposing_feasible_plan()
{
    core::PlanView p;
    p.instance_id = "sadg-opposing-feasible";
    p.plan_revision = 0;
    p.robots = {{"r1", "goal_reached", {"S", "A", "B", "G"}},
                {"r2", "goal_reached", {"B", "A", "T"}}};
    p.visit_order = {{"B", {{"r2", 0}, {"r1", 2}}}, {"A", {{"r2", 1}, {"r1", 1}}}};
    return p;
}

/// @brief Crossing instance: r1 = S1->X->G1, r2 = S2->X->G2, single shared
/// vertex X, no shared edge. Either order at X is deadlock-free — used to build a
/// switch group whose every combination is acyclic.
/// @return the plan view.
inline core::PlanView make_crossing_plan()
{
    core::PlanView p;
    p.instance_id = "sadg-crossing";
    p.plan_revision = 0;
    p.robots = {{"r1", "goal_reached", {"S1", "X", "G1"}},
                {"r2", "goal_reached", {"S2", "X", "G2"}}};
    p.visit_order = {{"X", {{"r1", 1}, {"r2", 1}}}};
    return p;
}

}  // namespace mrs_sadg::test

#endif  // MRS_SADG_TEST_FIXTURES_HPP
