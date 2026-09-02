// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_core/i_steady_clock.hpp"
#include "mrs_trajopt/plugins/peer_channel_impls.hpp"
#include "mrs_trajopt/plugins/search_fixed_path_gating.hpp"
#include "mrs_trajopt/plugins/search_hybrid_astar.hpp"
#include "mrs_trajopt/plugins/subgoal_candidates.hpp"
#include "mrs_trajopt/service/control_tick_service.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::plugins;
using namespace mrs_trajopt::service;

namespace
{

/// @brief 368_p3 — a settable `mrs_core::ISteadyClock` test double. Not
/// production code: the node's `RclcppSteadyClock` is the only concrete
/// clock this package ships (CN-4/CN-18 — the service never builds one).
class FakeSteadyClock : public mrs_core::ISteadyClock
{
public:
    double NowSeconds() const override
    {
        return now_s_;
    }

    void set(double now_s)
    {
        now_s_ = now_s;
    }

private:
    double now_s_ = 0.0;
};

RobotLimits make_lim()
{
    RobotLimits l;
    l.robot = "r0";
    l.v_max = 1.0;
    l.brake_decel_min_mps2 = 1.0;
    l.curvature_max_invm = 2.0;
    l.reverse_motion_allowed = false;
    l.avg_traversal_speed_mps = 0.8;
    l.circumradius_m = 0.3;
    return l;
}

FleetLimits make_fleet()
{
    FleetLimits f;
    f.a_max = 1.0;
    f.j_max = 5.0;
    f.omega_max_radps = 0.5;
    f.yaw_accel_max_radps2 = 1.0;
    f.min_separation_m = 0.6;
    return f;
}

TrajoptConfig make_cfg()
{
    TrajoptConfig c;
    c.traj_replan_period_ticks = 2;
    c.subgoal_replan_period_trajcycles = 3;  // subgoal tick every 6 control ticks.
    c.round_cap_budget = 4;
    c.backtrack_budget = 2;
    c.peer_board_rounds_max = 2;
    return c;
}

}  // namespace

int main()
{
    // --- Gating cell: runs ticks; a subgoal tick commits a trajectory. ---
    {
        SearchFixedPathGating search;
        SubgoalFreeSpaceDisk subgoals(0.5, 0.2);
        PeerChannelIdeal channel;
        auto pred = [](const StateSample&, double)
        {
            return true;
        };
        SafetyMonitor safety(pred);

        ServiceWiring w;
        w.search = &search;
        w.subgoals = &subgoals;
        w.channel = &channel;
        w.safety = &safety;

        Pose2 goal;
        goal.x = 5.0;
        goal.y = 0.0;
        ControlTickService svc("r0", w, make_cfg(), make_lim(), make_fleet(), goal);

        TickInput in;
        in.pose.x = 0.0;
        in.pose.y = 0.0;
        // Run 6 control ticks — tick 6 is the first subgoal tick.
        for (int i = 0; i < 6; ++i)
        {
            TickOutput out = svc.run_tick(in);
            CHECK(!out.has_stop);  // no spurious stop in a clear corridor.
        }
        CHECK(svc.tick_seq() == 6);
        CHECK(!svc.committed().empty());  // a trajectory was committed.
        CHECK(svc.publish_count() >= 1);  // a tube was published (ST30).
        // Fairness observables were recorded (328 six).
        CHECK(svc.fairness().subgoal_sequence_hash != 0);
        CHECK(svc.fairness().tube_radius_m > 0.0);
        CHECK(svc.fairness().subgoal_order_deviation == 0);  // FC-3.
        CHECK(svc.fairness().tube_exit_ticks == 0);          // FC-3 (gating stays on path).
    }

    // --- Fairness: both cells receive the same subgoal sequence hash (FC-1). ---
    {
        SubgoalFreeSpaceDisk subgoals(0.5, 0.2);
        PeerChannelIdeal ch_a, ch_b;
        auto pred = [](const StateSample&, double)
        {
            return true;
        };
        SafetyMonitor safety(pred);
        Pose2 goal;
        goal.x = 5.0;

        SearchFixedPathGating gating;
        SearchHybridAstar hybrid(0.0);  // zero yield => same path as gating.

        ServiceWiring wa;
        wa.search = &gating;
        wa.subgoals = &subgoals;
        wa.channel = &ch_a;
        wa.safety = &safety;
        ControlTickService a("r0", wa, make_cfg(), make_lim(), make_fleet(), goal);

        ServiceWiring wb;
        wb.search = &hybrid;
        wb.subgoals = &subgoals;
        wb.channel = &ch_b;
        wb.safety = &safety;
        ControlTickService b("r0", wb, make_cfg(), make_lim(), make_fleet(), goal);

        TickInput in;
        for (int i = 0; i < 6; ++i)
        {
            a.run_tick(in);
            b.run_tick(in);
        }
        // Same subgoal source + same start/goal => identical subgoal sequence (FC-1).
        CHECK(a.fairness().subgoal_sequence_hash == b.fairness().subgoal_sequence_hash);
    }

    // --- 368_p3: CT15 sampling advances by elapsed time (regression lock).
    // If this reverts to the old "always chain.front()" behavior, the
    // `!= chain.front().v` check below must FAIL. ---
    {
        FakeSteadyClock clock;
        SearchFixedPathGating search;
        SubgoalFreeSpaceDisk subgoals(0.5, 0.2);
        PeerChannelIdeal channel;
        auto pred = [](const StateSample&, double)
        {
            return true;
        };
        SafetyMonitor safety(pred);

        ServiceWiring w;
        w.search = &search;
        w.subgoals = &subgoals;
        w.channel = &channel;
        w.safety = &safety;
        w.clock = &clock;  // 368_p3 — without this, run_tick() falls back to now_s=0.0.

        TrajoptConfig cfg;
        cfg.traj_replan_period_ticks = 5;
        cfg.subgoal_replan_period_trajcycles = 1;  // subgoal tick also at tick 5 (5*1).
        cfg.round_cap_budget = 4;
        cfg.backtrack_budget = 2;
        cfg.peer_board_rounds_max = 2;

        Pose2 goal;
        goal.x = 5.0;
        goal.y = 0.0;
        ControlTickService svc("r0", w, cfg, make_lim(), make_fleet(), goal);

        TickInput in;
        in.pose.x = 0.0;
        in.pose.y = 0.0;

        clock.set(0.0);
        for (int i = 0; i < 4; ++i)
        {
            svc.run_tick(in);  // ticks 1-4: 4 % 5 != 0, no trajectory/subgoal tick yet.
        }
        CHECK(svc.committed().empty());  // nothing adopted yet — confirms the setup.

        clock.set(10.0);
        TickOutput out5 = svc.run_tick(in);  // tick 5: subgoal tick -> adopts, ref time = 10.0.
        CHECK(!out5.has_stop);
        const std::vector<StateSample> chain = svc.committed();
        CHECK(chain.size() >= 2);  // need an anchor + a moving sample to prove advance.

        // elapsed == 0 (now == adoption time) -> CT15 must sample the start anchor.
        clock.set(10.0);
        TickOutput at0 = svc.run_tick(in);  // tick 6: 6 % 5 != 0, no re-adopt.
        CHECK_NEAR(at0.cmd.t, chain.front().t, 1e-9);
        CHECK_NEAR(at0.cmd.v, chain.front().v, 1e-9);

        // elapsed == chain[1].t (a real value THIS run produced, not a
        // fabricated constant) -> CT15 must sample chain[1], not front().
        clock.set(10.0 + chain[1].t);
        TickOutput at1 = svc.run_tick(in);  // tick 7: 7 % 5 != 0, no re-adopt.
        CHECK_NEAR(at1.cmd.t, chain[1].t, 1e-6);
        CHECK_NEAR(at1.cmd.v, chain[1].v, 1e-9);
        CHECK(at1.cmd.v != chain.front().v);  // the regression lock itself.

        // elapsed far past the end -> holds at the last (safe) sample, not a crash.
        clock.set(10.0 + chain.back().t + 1000.0);
        TickOutput at_end = svc.run_tick(in);  // tick 8: 8 % 5 != 0, no re-adopt.
        CHECK_NEAR(at_end.cmd.t, chain.back().t, 1e-9);

        // A null clock (no wiring) degrades to now_s=0.0 -- always front() --
        // rather than throwing (368_p3 ServiceWiring::clock doc).
        ServiceWiring w_noclock;
        w_noclock.search = &search;
        w_noclock.subgoals = &subgoals;
        w_noclock.channel = &channel;
        w_noclock.safety = &safety;
        // w_noclock.clock left null on purpose.
        ControlTickService svc_noclock("r0", w_noclock, cfg, make_lim(), make_fleet(), goal);
        TickInput in2;
        for (int i = 0; i < 5; ++i)
        {
            svc_noclock.run_tick(in2);  // tick 5 adopts (now_s=0.0 both times -> elapsed=0).
        }
        TickOutput noclock_out = svc_noclock.run_tick(in2);  // tick 6, still elapsed=0.
        CHECK(!svc_noclock.committed().empty());
        CHECK_NEAR(noclock_out.cmd.v, svc_noclock.committed().front().v, 1e-9);
    }

    return trajopt_test::summary();
}
