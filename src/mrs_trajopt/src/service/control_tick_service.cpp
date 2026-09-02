// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/service/control_tick_service.hpp"

#include <cmath>
#include <cstring>
#include <utility>

namespace mrs_trajopt::service
{

namespace
{

/// @brief FNV-1a hash of a subgoal sequence (fairness subgoal_sequence_hash).
std::uint64_t hash_subgoals(const std::vector<core::PassWindow>& sgs)
{
    std::uint64_t h = 1469598103934665603ull;
    auto mix = [&h](double d)
    {
        std::uint64_t bits = 0;
        static_assert(sizeof(bits) == sizeof(d), "double is 8 bytes");
        std::memcpy(&bits, &d, sizeof(bits));
        for (int b = 0; b < 8; ++b)
        {
            h ^= (bits >> (b * 8)) & 0xffu;
            h *= 1099511628211ull;
        }
    };
    for (const auto& s : sgs)
    {
        mix(s.center.x);
        mix(s.center.y);
        mix(s.center.theta);
        mix(s.radius_win_m);
    }
    return h;
}

}  // namespace

ControlTickService::ControlTickService(std::string self,
                                       ServiceWiring wiring,
                                       core::TrajoptConfig cfg,
                                       core::RobotLimits lim,
                                       core::FleetLimits fleet,
                                       core::Pose2 goal)
    : self_(std::move(self)),
      w_(std::move(wiring)),
      cfg_(std::move(cfg)),
      lim_(std::move(lim)),
      fleet_(std::move(fleet)),
      goal_(goal),
      gate_(cfg_.traj_replan_period_ticks, cfg_.subgoal_replan_period_trajcycles),
      obs_ledger_(core::DeclarationLedgerConfig{cfg_.obs_n_open,
                                                cfg_.obs_n_close,
                                                cfg_.obs_n_hold})  // 376.
{
}

TickOutput ControlTickService::run_tick(const TickInput& in)
{
    TickOutput out;
    try  // CN-16: no exception escapes the tick path.
    {
        // CT00 t0 = clock_->now_steady() (322 §322-1 CT00, "시계 1"). 368_p3:
        // w_.clock is wiring (CN-18/CN-4) — a null clock degrades to 0.0
        // (ServiceWiring::clock doc), it never fabricates a clock here.
        const double now_s = (w_.clock != nullptr) ? w_.clock->NowSeconds() : 0.0;

        // CT01 tick_seq += 1.
        tick_seq_ += 1;

        // CT09a-CT09b puncture-advance hook (RQ5-4, U42-2 sole exception): a
        // pierced tube is not an INV-2 fallback, so advance the subgoal tick.
        if (in.tube_pierced)
        {
            subgoal_advance_req_ = true;  // CT09b.
        }

        // CT15 sample the committed state chain (t,x,y,theta,v,omega),
        // advanced by elapsed time since adoption (368_p3 — see
        // trajectory_buffer.hpp sample_at() doc for the narrow reading).
        core::StateSample s_now = buffer_.sample_at(now_s, in.pose);

        // CT16-CT19 braking-filter dynamic half: truncate at last stoppable.
        if (w_.safety != nullptr && !buffer_.view().empty())
        {
            if (!w_.safety->remaining_stoppable(buffer_.view(),
                                                lim_.brake_decel_min_mps2))  // CT16.
            {
                std::size_t k =
                    w_.safety->last_stoppable_index(buffer_.view(),
                                                    lim_.brake_decel_min_mps2);  // CT18.
                buffer_.truncate(k);                                             // CT19.
                out.horizon_truncated = true;
                if (w_.instr != nullptr)
                {
                    w_.instr->count("horizon_truncated_count");
                }
            }
        }

        // CT20 command = tracked sample (the command leaves ONLY here).
        out.cmd = s_now;

        // CT22 gate: is this a trajectory tick? (n^traj)
        if (gate_.is_trajectory_tick(tick_seq_))
        {
            recompute_trajectory(in, now_s);  // §322-2.
        }

        // CT24-CT25 (322 §322-1) — 376: runs EVERY control tick (NOT gated by
        // n^traj/n^re — the observation channel is independent of the
        // trajectory/subgoal nesting, 355§5 "판정 비용"). SIMPLIFIED trigger
        // (see file doc): "some evidence fired this tick" stands in for the
        // unimplemented fallback ladder's staged-candidate gate; the 3-way
        // total order (Q1 > Q2 > Q3) itself is exact.
        const bool q1_raw = core::IsQ1RawPredicate(in.obs, cfg_.obs_max_age_ticks);
        core::DeclarationEvidence ev;
        ev.edge_impassable_for_any_robot = obs_ledger_.update(q1_raw);  // N2.
        ev.remains_after_removing_others = pending_q2_evidence_;
        pending_q2_evidence_ = false;  // one-shot deposit, consumed (OBS-6 #4).
        if (ev.edge_impassable_for_any_robot || ev.remains_after_removing_others)
        {
            out.has_stop = true;                                         // N4.
            out.stop_reason = core::DeclarationRegulator::classify(ev);  // N3.
        }
    }
    catch (const std::exception&)
    {
        // CN-16: a tick-path violation becomes a stop declaration, not a crash.
        out.has_stop = true;
        out.stop_reason = core::StopReason::kUnresolvableLocally;
    }
    return out;
}

void ControlTickService::recompute_trajectory(const TickInput& in, double now_s)
{
    // TT00 tube = my last published promise (INV-2 fallback source).
    core::Tube tube;
    if (!buffer_.view().empty())
    {
        tube.centerline = buffer_.view();
        tube.half_width_m = fleet_.min_separation_m;  // simplified half-width.
    }

    // TT01-TT02 two-stage dynamic search inside the tube (hard constraint).
    core::SearchContext sctx;
    sctx.start = in.pose;
    sctx.start_v = in.v;
    sctx.subgoals = subgoals_;
    sctx.tube = tube;
    sctx.limits = lim_;

    core::SearchOutput res;
    bool found = false;
    if (w_.search != nullptr && !subgoals_.empty())
    {
        found = w_.search->solve(sctx, res);  // TT02.
    }

    if (found)  // TT03 Found = F^srch != empty (RQ5-EX witness).
    {
        // TT04 profiler fills (t,v,omega) + braking tail.
        core::ProfileResult prof = w_.profiler.parameterize(res.chain, lim_, fleet_, in.v);
        if (prof.ok)
        {
            cand_traj_ = prof.chain;  // TT07 candidate state chain.
            // Fairness: tube containment check (search cell must stay in tube).
            if (!tube.empty())
            {
                for (const auto& s : cand_traj_)
                {
                    double best = 1e300;
                    for (const auto& c : tube.centerline)
                    {
                        best = std::min(best, std::hypot(s.x - c.x, s.y - c.y));
                    }
                    if (best > tube.half_width_m + 1e-6)
                    {
                        fairness_.tube_exit_ticks += 1;  // FC-3 observable.
                    }
                }
            }
        }
        else
        {
            // TT12-TT13 reverse=false robot's infeasibility -> infeasible_subgoal.
            buffer_.hold_previous();  // TT10 (INV-2 a1).
            if (!lim_.reverse_motion_allowed)
            {
                // 🔴 고정 결정 2 / 관문 324: reverse forbidden => infeasible_subgoal.
                // 376 (OBS-6 #4): deposit evidence, do not assign the final
                // reason here — classify() runs once at CT24-CT25 below so a
                // Q1 confirmed LATER this tick is not overwritten by this
                // earlier Q2 evidence (total order, schema :54 "Q1 before Q2").
                pending_q2_evidence_ = true;
                (void)core::DeclarationRegulator::reverse_forbidden_infeasible();
            }
        }
    }
    else  // TT09-TT10 not found inside the tube => hold previous remainder.
    {
        buffer_.hold_previous();  // TT10 (INV-2 a1, BT-FAIL-SOUND: no partial).
        if (res.reverse_required && !lim_.reverse_motion_allowed)  // TT12.
        {
            // 376 (OBS-6 #4): same evidence-deposit treatment as TT13 above.
            pending_q2_evidence_ = true;
            (void)core::DeclarationRegulator::reverse_forbidden_infeasible();  // TT13.
        }
    }

    // TT14 subgoal tick OR puncture advance consumes the subgoal regeneration.
    if (gate_.is_subgoal_tick(tick_seq_) || subgoal_advance_req_)
    {
        subgoal_advance_req_ = false;                // TT15 reset the advance flag.
        regenerate_subgoals_and_publish(in, now_s);  // §322-3.
    }
    else if (!cand_traj_.empty())
    {
        // TT18 inside-tube recompute is adopted without re-check (induction).
        buffer_.adopt(cand_traj_, now_s);  // 368_p3: CT15's new reference point.
    }
}

void ControlTickService::regenerate_subgoals_and_publish(const TickInput& in, double now_s)
{
    // ST01 preliminary yield allocation is upstream-pure (roadmap/plan/iota
    // only); st.yaw is NOT read here (221-W1 defense). Simplified: no yield in
    // this single-robot corridor build — see 20d "알려진 한계".

    // ST02 ledger reset.
    ledger_.reset();

    // Generate the subgoal pass windows for this tick (ST07 candidate source).
    if (w_.subgoals != nullptr)
    {
        core::SubgoalContext sc;
        sc.start = in.pose;
        sc.goal = goal_;
        std::vector<core::PassWindow> generated;
        if (w_.subgoals->generate(sc, generated))
        {
            subgoals_ = generated;
        }
    }

    // 🔴 SIMPLIFIED round loop: a single-robot corridor converges in one round.
    // The full inter-robot back-off (ST03-ST23, U42-8) drives multiple robots
    // through RoundLedger + YieldHandles; those are implemented and unit-tested
    // separately (test_round_ledger.cpp) but not wired into this single-robot
    // path — see 20d "알려진 한계".
    core::RoundOutcome outcome = core::RoundOutcome::kConverged;  // ST12.

    if (outcome == core::RoundOutcome::kConverged)
    {
        // ST25 subgoals = pass windows (adopt).
        // ST26 recompute trajectory with the new subgoals.
        core::SearchContext sctx;
        sctx.start = in.pose;
        sctx.start_v = in.v;
        sctx.subgoals = subgoals_;
        sctx.limits = lim_;
        core::SearchOutput res;
        if (w_.search != nullptr && !subgoals_.empty() && w_.search->solve(sctx, res))
        {
            core::ProfileResult prof = w_.profiler.parameterize(res.chain, lim_, fleet_, in.v);
            if (prof.ok)
            {
                cand_traj_ = prof.chain;
                // Fairness: did realization change subgoal visit order? (FC-3)
                // Simplified: order preserved by construction => deviation 0.
                fairness_.subgoal_order_deviation += 0;
                // ST27-ST29 commit gate (check-recheck): SIMPLIFIED to accept.
                // The full gate re-reads publications arriving mid-computation
                // (316b①) — a stub here, see 20d "알려진 한계".
                buffer_.adopt(cand_traj_, now_s);  // ST29 promise <- pending (368_p3 ref point).
                committed_subgoals_ = subgoals_;
                fairness_.subgoal_sequence_hash = hash_subgoals(subgoals_);  // FC-1 observable.
                fairness_.tube_radius_m = fleet_.min_separation_m;  // FC-2 observable (simplified).
                publish_count_ += 1;                                // ST30 publish the tube.
                if (w_.channel != nullptr && !cand_traj_.empty())
                {
                    core::Pose2 anchor;
                    anchor.x = cand_traj_.front().x;
                    anchor.y = cand_traj_.front().y;
                    anchor.theta = cand_traj_.front().theta;
                    w_.channel->publish(self_, /*rd=*/0, anchor, /*iota=*/tick_seq_);
                }
            }
        }
    }
    else  // ST21-ST23 cap reached => sequential downgrade (hold + fallback).
    {
        buffer_.hold_previous();  // ST22 (INV-2 a1).
        if (w_.instr != nullptr)
        {
            w_.instr->count("round_truncate_consecutive_ticks");
        }
        fairness_.n_escalation_by_cell += 1;  // 328 observable.
    }
}

}  // namespace mrs_trajopt::service
