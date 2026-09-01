// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/core/commit_gate.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/instr_ring.hpp"
#include "mrs_sadg/core/safety_envelope.hpp"
#include "mrs_sadg/plugins/policy_deficit_ladder.hpp"
#include "mrs_sadg/service/execution_tick_service.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::service;
using mrs_sadg::plugins::PolicyDeficitLadder;

/// @file test_execution_tick.cpp
/// @brief ET## — the five accept gates each cut independently, the asymmetric
/// drain defaults, all-or-nothing commit, and apply_blocked_delta.

namespace
{

/// @brief Crossing graph: sequentials + one switch group at X (two orders).
ExecutionGraph make_switch_graph()
{
    ExecutionGraph g;
    g.add_segment({"r1#0", "r1", "S1", "X"});
    g.add_segment({"r1#1", "r1", "X", "G1"});
    g.add_segment({"r2#0", "r2", "S2", "X"});
    g.add_segment({"r2#1", "r2", "X", "G2"});
    g.add_fixed_dep({"r1#0", "r1#1", DepType::kSequential});
    g.add_fixed_dep({"r2#0", "r2#1", DepType::kSequential});
    SwitchGroup grp;
    grp.id = "gX";
    grp.location = "X";
    grp.alternatives = {{"r1_first", {{"r1#1", "r2#0", DepType::kOrdering}}},
                        {"r2_first", {{"r2#1", "r1#0", DepType::kOrdering}}}};
    g.add_switch_group(grp);
    return g;
}

CommitProposal prop(std::vector<std::size_t> combo, std::uint64_t rz_seq)
{
    CommitProposal p;
    p.combo = std::move(combo);
    p.rz_seq = rz_seq;
    return p;
}

ReachabilityView rz_view(std::uint64_t seq)
{
    ReachabilityView rz;
    rz.rz_seq = seq;
    return rz;
}

}  // namespace

int main()
{
    // ── AG1 acyclic: a combo that closes a cycle with a fixed dep is cut. ──────
    {
        ExecutionGraph g = make_switch_graph();
        g.add_fixed_dep({"r2#0", "r1#1", DepType::kOrdering});  // makes alt r1_first cyclic.
        CHECK(!CommitGate::gate_acyclic(g, prop({0}, 0)));      // r1#1->r2#0->r1#1.
        CHECK(CommitGate::gate_acyclic(g, prop({1}, 0)));       // r2_first is acyclic.
    }

    // ── AG2 release-unopened: reordering an already-open release is cut. ──────
    {
        ExecutionGraph g = make_switch_graph();
        g.set_release("r2#0", true);  // arrival permit already open...
        CHECK(!CommitGate::gate_release_unopened(g, prop({0}, 0)));  // r1#1 not entered.
        ExecutionGraph fresh = make_switch_graph();
        CHECK(CommitGate::gate_release_unopened(fresh, prop({0}, 0)));
    }

    // ── AG3 A-LOCK: a committed segment placed behind an uncommitted one is cut.
    {
        ExecutionGraph g = make_switch_graph();
        g.mark_entered("r2#0");                                 // committed successor...
        CHECK(!CommitGate::gate_alock_order(g, prop({0}, 0)));  // ...behind uncommitted r1#1.
        ExecutionGraph fresh = make_switch_graph();
        CHECK(CommitGate::gate_alock_order(fresh, prop({0}, 0)));
    }

    // ── AG4 RECERT-LOCK: stale rz_seq or a de-certified entered segment is cut.
    {
        ExecutionGraph g = make_switch_graph();
        CHECK(!CommitGate::gate_recert_inprogress(g, prop({0}, 4), rz_view(5)));  // stale.
        CHECK(CommitGate::gate_recert_inprogress(g, prop({0}, 5), rz_view(5)));   // fresh.
        g.mark_entered("r1#0");
        ReachabilityView rz = rz_view(5);
        rz.unreachable = {"r1#0"};  // de-certifies an in-progress segment.
        CHECK(!CommitGate::gate_recert_inprogress(g, prop({0}, 5), rz));
    }

    // ── AG5 SAFE: cyclic proposal rejected by the envelope. ───────────────────
    {
        ExecutionGraph g = make_switch_graph();
        g.add_fixed_dep({"r2#0", "r1#1", DepType::kOrdering});
        SafetyEnvelope env;
        CHECK(!env.safe_check(g, {0}));  // cyclic.
        CHECK(env.safe_check(g, {1}));   // acyclic + PROP-ACCEPT.
    }

    // ── tick: all five pass -> all-or-nothing commit, commit_seq bumps. ───────
    {
        ExecutionGraph g = make_switch_graph();
        SafetyEnvelope env;
        PolicyDeficitLadder policy;
        InstrRing instr;
        ExecutionTickService svc(g, env, policy, instr);
        TickInput in;
        in.tick_seq = 1;
        in.entry_present = true;
        in.rz_present = true;
        in.rz = rz_view(0);
        in.t1_present = true;
        in.t1_response = prop({1}, 0);  // r2_first, acyclic.
        TickOutput out = svc.run_tick(in);
        CHECK(out.committed);
        CHECK(g.commit_seq() == 1);
        CHECK(g.committed_combo() == std::vector<std::size_t>{1});
    }

    // ── tick asymmetric default: realizability lost => reorder not committed. ─
    {
        ExecutionGraph g = make_switch_graph();
        g.mark_entered("r1#0");  // an in-progress segment.
        SafetyEnvelope env;
        PolicyDeficitLadder policy;
        InstrRing instr;
        ExecutionTickService svc(g, env, policy, instr);
        TickInput in;
        in.tick_seq = 2;
        in.rz_present = false;  // «불가» default -> all unreachable -> AG4 cuts.
        in.t1_present = true;
        in.t1_response = prop({0}, 0);  // r1_first: no A-LOCK on the entered r1#0.
        TickOutput out = svc.run_tick(in);
        CHECK(!out.committed);
        CHECK(out.first_failed == GateId::kRecertStale);  // AG4 (de-certified entered seg).
    }

    // ── tick asymmetric default: entry lost => «있었다» recert-lock, no reorder.
    {
        ExecutionGraph g = make_switch_graph();
        SafetyEnvelope env;
        PolicyDeficitLadder policy;
        InstrRing instr;
        ExecutionTickService svc(g, env, policy, instr);
        TickInput in;
        in.tick_seq = 3;
        in.entry_present = false;  // «있었다» -> hold z_prev.
        in.rz_present = true;
        in.t1_present = true;
        in.t1_response = prop({1}, 0);
        TickOutput out = svc.run_tick(in);
        CHECK(!out.committed);
        CHECK(instr.counter("n_recert_lock_holds") == 1);
    }

    // ── tick: apply_blocked_delta fills V_blk and the ladder escalates. ───────
    {
        ExecutionGraph g = make_switch_graph();
        SafetyEnvelope env;
        PolicyDeficitLadder policy;
        InstrRing instr;
        ExecutionTickService svc(g, env, policy, instr);
        TickInput in;
        in.tick_seq = 4;
        in.blocked_decls = {{"X", "G1"}};  // upstream reported a blocked edge.
        TickOutput out = svc.run_tick(in);
        CHECK(g.blocked_count() == 1);
        CHECK(out.rung == Rung::kReorder);  // one step up on a blocked trigger.
    }

    return sadg_test::summary();
}
