// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/control_tick_service.hpp"
#include "mrs_trajopt/plugins/peer_channel_impls.hpp"
#include "mrs_trajopt/plugins/search_fixed_path_gating.hpp"
#include "mrs_trajopt/plugins/search_hybrid_astar.hpp"
#include "mrs_trajopt/plugins/subgoal_candidates.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::plugins;

namespace
{

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

    return trajopt_test::summary();
}
