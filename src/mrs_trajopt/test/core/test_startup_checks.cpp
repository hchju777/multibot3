// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/startup_checks.hpp"

using namespace mrs_trajopt::core;

namespace
{

std::vector<RobotLimits> good_specs()
{
    RobotLimits r;
    r.robot = "r0";
    r.v_max = 1.0;
    r.brake_decel_min_mps2 = 1.0;
    r.curvature_max_invm = 2.0;
    r.reverse_motion_allowed = false;
    r.avg_traversal_speed_mps = 0.8;
    r.circumradius_m = 0.3;
    return {r};
}

TrajoptConfig good_cfg()
{
    TrajoptConfig c;
    c.traj_replan_period_ticks = 2;
    c.subgoal_replan_period_trajcycles = 3;
    c.round_cap_budget = 4;
    c.backtrack_budget = 2;
    c.peer_board_rounds_max = 2;
    return c;
}

FleetLimits good_fleet()
{
    FleetLimits f;
    f.min_separation_m = 0.6;  // >= 2 * 0.3.
    return f;
}

bool throws(const std::vector<RobotLimits>& s, const FleetLimits& f, const TrajoptConfig& c)
{
    try
    {
        StartupChecks::run(s, f, c);
        return false;
    }
    catch (const ContractViolation&)
    {
        return true;
    }
}

}  // namespace

int main()
{
    // All good => passes.
    CHECK(!throws(good_specs(), good_fleet(), good_cfg()));

    // SU01: n^traj = 0 => refusal (a zero period is UB, not just a bad value).
    {
        TrajoptConfig c = good_cfg();
        c.traj_replan_period_ticks = 0;
        CHECK(throws(good_specs(), good_fleet(), c));
    }
    // SU02: n^re = 0 => refusal.
    {
        TrajoptConfig c = good_cfg();
        c.subgoal_replan_period_trajcycles = 0;
        CHECK(throws(good_specs(), good_fleet(), c));
    }
    // SU03: round_cap_budget = 0 => refusal.
    {
        TrajoptConfig c = good_cfg();
        c.round_cap_budget = 0;
        CHECK(throws(good_specs(), good_fleet(), c));
    }
    // SU06: an independent eta_slf key => refusal (▲7, derived value).
    {
        TrajoptConfig c = good_cfg();
        c.config_key_names = {"peer_board_rounds_max", "eta_slf_k"};
        CHECK(throws(good_specs(), good_fleet(), c));
        TrajoptConfig c2 = good_cfg();
        c2.config_key_names = {"self_trigger_threshold"};
        CHECK(throws(good_specs(), good_fleet(), c2));
    }
    // SU08: a missing/invalid robot field => refusal.
    {
        auto s = good_specs();
        s[0].circumradius_m = 0.0;  // must be > 0.
        CHECK(throws(s, good_fleet(), good_cfg()));
    }
    // SU11: min_separation < 2 * min circumradius => refusal (C-41).
    {
        FleetLimits f = good_fleet();
        f.min_separation_m = 0.5;  // < 2 * 0.3.
        CHECK(throws(good_specs(), f, good_cfg()));
    }

    return trajopt_test::summary();
}
