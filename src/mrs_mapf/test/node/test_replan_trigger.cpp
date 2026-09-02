// SPDX-License-Identifier: Apache-2.0
#include <optional>

#include "../test_util.hpp"
#include "mrs_mapf/node/replan_trigger.hpp"

using mrs_mapf::node::ShouldReplanForAssignment;

/// @file test_replan_trigger.cpp
/// @brief Locks the 48차 fix: `/assignment.revision` advancing (re)arms a
/// plan attempt, and the same revision is never planned for twice. Pure
/// function — no ROS, no rclcpp::spin, exercises `mapf_node`'s node-layer
/// orchestration decision directly (§48-1-C — `mapf_node.cpp:204`'s
/// `published_once_` gate previously stuck `/discrete_plan.plan_revision` at
/// 0 across 8 `/assignment` updates).

int main()
{
    // --- inputs incomplete: never replan regardless of revision state ---
    CHECK(!ShouldReplanForAssignment(false, std::nullopt, 0));
    CHECK(!ShouldReplanForAssignment(false, 5, 7));

    // --- first ever attempt (no prior attempt recorded): always plan ---
    CHECK(ShouldReplanForAssignment(true, std::nullopt, 0));
    CHECK(ShouldReplanForAssignment(true, std::nullopt, 41));

    // --- the bug this round closes: revision ADVANCES beyond what was last
    // attempted -> must replan (this is what makes plan_revision keep pace
    // with 8 successive /assignment updates instead of sticking at 0) ---
    CHECK(ShouldReplanForAssignment(true, 0, 1));
    CHECK(ShouldReplanForAssignment(true, 3, 4));
    CHECK(ShouldReplanForAssignment(true, 0, 7));  // several revisions skipped between messages.

    // --- replan-storm guard: same revision arriving again -> do NOT replan ---
    CHECK(!ShouldReplanForAssignment(true, 3, 3));
    CHECK(!ShouldReplanForAssignment(true, 0, 0));

    // --- revision going "backwards" relative to the last attempt is still
    // treated as "differs" (the contract does not promise redelivery order;
    // this function only ever sees what the node layer hands it) ---
    CHECK(ShouldReplanForAssignment(true, 5, 2));

    return mapf_test::summary();
}
