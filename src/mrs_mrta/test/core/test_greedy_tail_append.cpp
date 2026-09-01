// SPDX-License-Identifier: Apache-2.0
// CN-22: test/core/ — algorithm-core unit tests for the two IAssigner
// implementations (`336_mrta_canon.md` §336-2 tie-break, §336-3-3 pending
// pool retry, `12a_arch_mrta.md` §2-3 physical-vs-policy capacity).
#include <memory>
#include <string>

#include "mrs_mrta/core/i_assigner.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "mrs_mrta/plugins/assigner_impls.hpp"
#include "test_util.hpp"

using mrs_mrta::core::AssignerContext;
using mrs_mrta::core::AssignerStatus;
using mrs_mrta::core::AssignmentState;
using mrs_mrta::core::CompletionEvent;
using mrs_mrta::core::GoalRecord;
using mrs_mrta::core::ReleaseEvent;
using mrs_mrta::core::Status;
using mrs_mrta::plugins::FaultyOverflow;
using mrs_mrta::plugins::GreedyTailAppend;
using mrs_mrta::testutil::MapDistanceOracle;

namespace
{

ReleaseEvent single_leg_release(std::uint64_t idx,
                                const std::string& task,
                                const std::string& goal_id,
                                const std::string& location)
{
    ReleaseEvent ev;
    ev.release_index = idx;
    ev.task = task;
    GoalRecord g;
    g.id = goal_id;
    g.task = task;
    g.location = location;
    ev.goals.push_back(g);
    return ev;
}

void test_tie_break_tiers()
{
    // Roster r1 < r2 < r3 (lexicographic — fixes tier 4).
    AssignmentState state({"r1", "r2", "r3"}, 4);
    state.set_robot_vertex(state.index_of("r1"), "V0");
    state.set_robot_vertex(state.index_of("r2"), "V0");
    state.set_robot_vertex(state.index_of("r3"), "V0");

    MapDistanceOracle distances;
    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 3;
    ctx.goal_queue_capacity_ta = 4;
    ctx.distances = &distances;
    MRTA_CHECK(assigner.configure(ctx) == Status::kOk);
    MRTA_CHECK(assigner.policy_id() == "mrs_mrta/GreedyTailAppend");

    // Tier 1 (cost): give each robot a distinct current vertex so the oracle can assign each a
    // different cost to the same pickup target, deterministically.
    ReleaseEvent relA = single_leg_release(1, "taskA", "gA", "PICKA");
    state.set_robot_vertex(state.index_of("r1"), "V1");
    state.set_robot_vertex(state.index_of("r2"), "V2");
    state.set_robot_vertex(state.index_of("r3"), "V3");
    distances.set("V1", "PICKA", 900);
    distances.set("V2", "PICKA", 100);  // r2: cheapest.
    distances.set("V3", "PICKA", 900);
    MRTA_CHECK(assigner.on_release(relA, state) == AssignerStatus::kAssigned);
    MRTA_CHECK(state.residual_len(state.index_of("r2")) == 1);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 0);
    MRTA_CHECK(state.residual_len(state.index_of("r3")) == 0);

    // Step B: tier 2 (residual length) — r1 and r3 tie on cost, but r3 already has 1 queued
    // (from a prior release) so r1 (shorter residual) must win.
    distances.set("V1", "PICKB", 300);
    distances.set("V3", "PICKB", 300);
    // Give r3 one prior goal so its residual_len is 1 > r1's 0.
    ReleaseEvent preload = single_leg_release(2, "preload", "gPre", "PRELOAD");
    distances.set("V3", "PRELOAD", 1);  // trivially cheapest for r3 alone at this point.
    distances.set("V1", "PRELOAD", 100000);
    distances.set("V2", "PRELOAD", 100000);
    MRTA_CHECK(assigner.on_release(preload, state) == AssignerStatus::kAssigned);
    MRTA_CHECK(state.residual_len(state.index_of("r3")) == 1);

    ReleaseEvent relB = single_leg_release(3, "taskB", "gB", "PICKB");
    distances.set("V2", "PICKB", 100000);  // r2 not competitive here.
    MRTA_CHECK(assigner.on_release(relB, state) == AssignerStatus::kAssigned);
    // r1 (residual 0) must win over r3 (residual 1) despite equal cost.
    MRTA_CHECK(state.peek_tail(state.index_of("r1"))->id == "gB");
    MRTA_CHECK(state.residual_len(state.index_of("r3")) == 1);  // r3 unchanged.
}

void test_tier4_robot_name_breaks_remaining_ties()
{
    AssignmentState state({"a1", "a2"}, 4);
    state.set_robot_vertex(state.index_of("a1"), "X");
    state.set_robot_vertex(state.index_of("a2"), "X");

    MapDistanceOracle distances;
    distances.set("X",
                  "PICK",
                  42);  // identical cost, identical residual (0), identical cumulative (0).

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 2;
    ctx.goal_queue_capacity_ta = 4;
    ctx.distances = &distances;
    assigner.configure(ctx);

    ReleaseEvent rel = single_leg_release(1, "t", "g", "PICK");
    MRTA_CHECK(assigner.on_release(rel, state) == AssignerStatus::kAssigned);
    // "a1" < "a2" lexicographically -> a1 must win the final tier.
    MRTA_CHECK(state.residual_len(state.index_of("a1")) == 1);
    MRTA_CHECK(state.residual_len(state.index_of("a2")) == 0);
}

void test_a8_exclusivity_blocks_release()
{
    AssignmentState state({"r1", "r2"}, 4);
    state.set_robot_vertex(state.index_of("r1"), "V");
    state.set_robot_vertex(state.index_of("r2"), "V");
    MapDistanceOracle distances;
    distances.set("V", "SHARED", 10);

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 2;
    ctx.goal_queue_capacity_ta = 4;
    ctx.distances = &distances;
    assigner.configure(ctx);

    ReleaseEvent first = single_leg_release(1, "t1", "g1", "SHARED");
    MRTA_CHECK(assigner.on_release(first, state) == AssignerStatus::kAssigned);

    // A different task targeting the same (still uncompleted) location must be pooled, not
    // assigned to another robot, per A8 endpoint exclusivity.
    ReleaseEvent second = single_leg_release(2, "t2", "g2", "SHARED");
    MRTA_CHECK(assigner.on_release(second, state) == AssignerStatus::kPooled);
    MRTA_CHECK(state.pending_size() == 1);
}

void test_capacity_pools_when_full()
{
    AssignmentState state({"r1"}, 1);  // Q^ta = 1.
    state.set_robot_vertex(state.index_of("r1"), "V");
    MapDistanceOracle distances;
    distances.set("V", "P1", 5);
    distances.set("V", "P2", 5);

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 1;
    ctx.distances = &distances;
    assigner.configure(ctx);

    MRTA_CHECK(assigner.on_release(single_leg_release(1, "t1", "g1", "P1"), state) ==
               AssignerStatus::kAssigned);
    // r1 is now at Q^ta=1 — the only robot, so the next release must pool.
    MRTA_CHECK(assigner.on_release(single_leg_release(2, "t2", "g2", "P2"), state) ==
               AssignerStatus::kPooled);
    MRTA_CHECK(state.pending_size() == 1);
}

void test_completion_mismatch_and_match_with_pool_retry()
{
    AssignmentState state({"r1"}, 1);
    state.set_robot_vertex(state.index_of("r1"), "V");
    MapDistanceOracle distances;
    distances.set("V", "P1", 5);
    distances.set("V", "P2", 7);

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 1;
    ctx.distances = &distances;
    assigner.configure(ctx);

    assigner.on_release(single_leg_release(1, "t1", "g1", "P1"), state);
    assigner.on_release(single_leg_release(2, "t2", "g2", "P2"), state);  // pooled — r1 is full.
    MRTA_CHECK(state.pending_size() == 1);

    CompletionEvent wrong;
    wrong.robot = "r1";
    wrong.goal_id = "not-the-head";
    MRTA_CHECK(assigner.on_completion(wrong, state) == AssignerStatus::kCompletionMismatch);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 1);  // Not consumed.

    CompletionEvent right;
    right.robot = "r1";
    right.goal_id = "g1";
    MRTA_CHECK(assigner.on_completion(right, state) == AssignerStatus::kCompletedHead);
    // Pool retry must have placed g2 on the freed slot.
    MRTA_CHECK(state.pending_size() == 0);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 1);
    MRTA_CHECK(state.peek_head(state.index_of("r1"))->id == "g2");
}

void test_reset_clears_everything()
{
    AssignmentState state({"r1"}, 2);
    state.set_robot_vertex(state.index_of("r1"), "V");
    MapDistanceOracle distances;
    distances.set("V", "P1", 5);

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 2;
    ctx.distances = &distances;
    assigner.configure(ctx);

    assigner.on_release(single_leg_release(1, "t1", "g1", "P1"), state);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 1);

    assigner.reset(state);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 0);
    MRTA_CHECK(state.pending_empty());
}

void test_faulty_overflow_exceeds_policy_cap_but_not_physical()
{
    AssignmentState state({"r1"}, 1);  // Q^ta=1, physical=2.
    state.set_robot_vertex(state.index_of("r1"), "V");
    MapDistanceOracle distances;
    distances.set("V", "P1", 1);
    // Once r1 has a tail goal, the cost query originates from the tail location
    // (`336_mrta_canon.md` §336-2-2), not "V" — so P1->P2 and P2->P3 must also be registered.
    distances.set("P1", "P2", 1);
    distances.set("P2", "P3", 1);

    FaultyOverflow assigner;
    AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 1;
    ctx.distances = &distances;
    MRTA_CHECK(assigner.configure(ctx) == Status::kOk);
    MRTA_CHECK(assigner.policy_id() == "mrs_mrta/FaultyOverflow");

    MRTA_CHECK(assigner.on_release(single_leg_release(1, "t1", "g1", "P1"), state) ==
               AssignerStatus::kAssigned);
    // 🔴 The fault: a second task is accepted past Q^ta=1, up to the physical limit (2) —
    // GreedyTailAppend would have pooled this.
    MRTA_CHECK(assigner.on_release(single_leg_release(2, "t2", "g2", "P2"), state) ==
               AssignerStatus::kAssigned);
    MRTA_CHECK(state.residual_len(state.index_of("r1")) == 2);
    // Physical capacity is still the floor — a third task cannot fit.
    MRTA_CHECK(assigner.on_release(single_leg_release(3, "t3", "g3", "P3"), state) ==
               AssignerStatus::kPooled);
}

}  // namespace

int main()
{
    test_tie_break_tiers();
    test_tier4_robot_name_breaks_remaining_ties();
    test_a8_exclusivity_blocks_release();
    test_capacity_pools_when_full();
    test_completion_mismatch_and_match_with_pool_retry();
    test_reset_clears_everything();
    test_faulty_overflow_exceeds_policy_cap_but_not_physical();
    return 0;
}
