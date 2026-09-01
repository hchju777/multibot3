// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/service/execution_tick_service.hpp"

#include "mrs_sadg/core/commit_arbiter.hpp"
#include "mrs_sadg/core/progress_deficit.hpp"
#include "mrs_sadg/core/release_checker.hpp"

namespace mrs_sadg::service
{

TickOutput ExecutionTickService::run_tick(const TickInput& in)
{
    TickOutput out;
    // 🔴 CN-16: the whole tick body is wrapped so no exception escapes spin().
    try
    {
        g_.set_tick_seq(in.tick_seq);

        // ── ET01/ET02: drain channels with the asymmetric defaults. ──
        // realizability lost = «불가» (safe): treat every segment as unreachable
        // so any reorder touching an in-progress segment fails AG4 (RECERT-LOCK).
        core::ReachabilityView rz = in.rz;
        if (!in.rz_present)
        {
            rz.unreachable.clear();
            for (const auto& seg : g_.segments())
            {
                rz.unreachable.push_back(seg.id);
            }
        }
        // entry event lost = «있었다» (safe): assume in-flight = committed, i.e.
        // do not accept a reorder this tick (RECERT-LOCK conservative, ET04).
        const bool entry_trustworthy = in.entry_present;

        // ── ET03: commit entered segments (E1 apply point). ──
        core::CommitArbiter::apply_entry_events(g_, in.entry_events);

        // ══ epoch start — no external output until ET33 ══
        // ── ET05-ET15: five accept gates; commit all-or-nothing or keep z_prev. ──
        if (in.t1_present && entry_trustworthy)
        {
            const core::GateId f =
                core::CommitGate::first_failure(g_, in.t1_response, rz);  // AG1-AG4
            const bool ag5 = env_.safe_check(g_, in.t1_response.combo);   // AG5 (SAFE)
            if (f == core::GateId::kNone && ag5)
            {
                g_.set_committed_combo(in.t1_response.combo);  // ET12 all-or-nothing.
                g_.bump_commit_seq();
                out.committed = true;
            }
            else
            {
                out.first_failed = f;  // ET14: which gate cut it (kNone here = AG5 cut).
                instr_.count("n_stale_discards");
                // ET15: keep z_prev ∈ Z_safe (no change to committed_combo_).
            }
        }
        else if (in.t1_present && !entry_trustworthy)
        {
            instr_.count("n_recert_lock_holds");  // z_prev held (entry channel lost).
        }
        // ══ epoch end ══

        // ── ET16/ET17: completions, then fill V_blk from upstream (§11-2③). ──
        for (const auto& id : in.completions)
        {
            if (g_.has_segment(id))
            {
                g_.mark_completed(id);  // completion implies entry; opposing release waits on it.
            }
        }
        g_.apply_blocked_delta(in.blocked_decls);

        // ── ET19/ET20: release bits (arrival-vertex entry permit) + deficit. ──
        core::ReleaseChecker::release_check(g_);
        core::ProgressDeficit::update(g_, in.deficits);

        // ── ET22: WC1 detection (mutually exclusive with the normal path). ──
        const core::WcResult wc = env_.detect_wc1(in.wc);
        out.wc1_detected = wc.detected;
        if (wc.detected)
        {
            instr_.count("n_wc1_detected");
        }

        // ── ET30: escalation ladder. ──
        core::DeficitView dv;
        dv.graph = &g_;
        for (const auto& seg : g_.segments())
        {
            dv.robots.push_back(seg.robot);
        }
        core::BlockedView bv;
        bv.count = g_.blocked_count();
        const core::Rung rung = policy_.decide(dv, bv, static_cast<core::Rung>(g_.cur_rung()));
        out.rung = rung;
        g_.set_cur_rung(static_cast<int>(rung));

        // ── ET32: EVAC binds to rung 2 (partial replan) when WC1 present. ──
        if (rung == core::Rung::kPartialReplan && wc.detected)
        {
            instr_.count("n_evac_replans");
            out.republish_needed = true;
        }

        // ── ET33: the boundary output = gab-bu bits (arrival entry permits). ──
        for (const auto& seg : g_.segments())
        {
            if (g_.is_released(seg.id))
            {
                out.released.push_back(seg.id);
            }
        }
        instr_.observe("tick_total_ms", 0.0);  // sample supplied off-core in production.
        return out;                            // ET36: publishing is the node's, not the service's.
    }
    catch (...)
    {
        // CN-16: a tick-path fault becomes a latched safe outcome, never a throw.
        instr_.count("n_fatal_latch");
        TickOutput safe;
        safe.rung = core::Rung::kNone;
        safe.republish_needed = false;
        return safe;
    }
}

}  // namespace mrs_sadg::service
