// SPDX-License-Identifier: Apache-2.0
#include <cmath>

#include "../test_util.hpp"
#include "mrs_trajopt/core/velocity_profiler.hpp"

using namespace mrs_trajopt::core;

namespace
{

RobotLimits make_limits(bool reverse)
{
    RobotLimits l;
    l.robot = "r0";
    l.v_max = 1.0;
    l.brake_decel_min_mps2 = 1.0;
    l.curvature_max_invm = 2.0;
    l.reverse_motion_allowed = reverse;
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

}  // namespace

int main()
{
    VelocityProfiler prof;
    const RobotLimits lim = make_limits(false);
    const FleetLimits fleet = make_fleet();

    // --- Straight path: speed within v_max, endpoints at v=0, time increases. ---
    {
        std::vector<GeometricNode> path;
        for (int i = 0; i <= 10; ++i)
        {
            GeometricNode n;
            n.pose.x = static_cast<double>(i) * 0.5;  // 0.5 m spacing, 5 m total.
            n.pose.y = 0.0;
            n.pose.theta = 0.0;
            path.push_back(n);
        }
        ProfileResult r = prof.parameterize(path, lim, fleet, 0.0);
        CHECK(r.ok);
        CHECK(r.chain.size() == path.size());
        // 368_p8 (370 CVN-1..3): node 0's value is now edge 0's HELD speed —
        // the average of the ramp's two endpoint speeds (0 and whatever v[1]
        // reaches), NOT the old node point-value 0. A start-from-rest chain
        // that actually moves during its first edge now correctly reports a
        // NONZERO first held speed (this is exactly the fix: the old code
        // held v=0 for the whole first edge's duration, i.e. the robot sat
        // still while the plan said it was already moving — 370§5-3/§8-2).
        CHECK(r.chain.front().v > 0.0);
        CHECK(r.chain.front().v <= lim.v_max + 1e-9);
        CHECK_NEAR(r.chain.back().v, 0.0, 1e-9);  // end v=0 (no outgoing edge, 370§6-1 cand. A).
        double t_prev = -1.0, vmax_seen = 0.0;
        for (const auto& s : r.chain)
        {
            CHECK(s.t > t_prev);  // strictly ascending time.
            t_prev = s.t;
            CHECK(std::fabs(s.v) <= lim.v_max + 1e-9);  // v_max honored.
            vmax_seen = std::max(vmax_seen, std::fabs(s.v));
        }
        CHECK(vmax_seen > 0.0);             // it actually moves.
        CHECK(r.braking_tail_len_m > 0.0);  // braking tail reserved (INV-2-GEO(B)).
    }

    // --- Curved path: produced omega must not exceed omega_max (by construction). ---
    {
        std::vector<GeometricNode> path;
        const double R = 1.0;  // radius 1 m => curvature 1.0 (< curvature_max 2.0).
        for (int i = 0; i <= 20; ++i)
        {
            const double a = static_cast<double>(i) * 0.05;  // arc up to ~1 rad.
            GeometricNode n;
            n.pose.x = R * std::sin(a);
            n.pose.y = R * (1.0 - std::cos(a));
            n.pose.theta = a;
            path.push_back(n);
        }
        ProfileResult r = prof.parameterize(path, lim, fleet, 0.0);
        CHECK(r.ok);
        double omega_max_seen = 0.0;
        for (const auto& s : r.chain)
        {
            omega_max_seen = std::max(omega_max_seen, std::fabs(s.omega));
        }
        // omega generated <= omega_max (0.5): the profiler caps v at omega_max/kappa.
        CHECK(omega_max_seen <= fleet.omega_max_radps + 1e-6);
    }

    // --- 368_p4 regression lock: a RIGHT-curving (clockwise, negative
    //     dtheta) path must produce NEGATIVE omega, not just some nonzero
    //     magnitude. node_curvature() used to be std::fabs(dtheta)/(2*ds) —
    //     unsigned — so s.omega = sign*v*kappa was ALWAYS >= 0 regardless of
    //     which way the path actually turned. A live end-to-end run found
    //     this: a robot needing a right turn instead turned increasingly
    //     LEFT every replan and diverged from its goal (368_p4 diagnosis).
    //     If this reverts to the unsigned form, the CHECK below (omega < 0)
    //     must fail. ---
    {
        std::vector<GeometricNode> path;
        const double R = 1.0;
        for (int i = 0; i <= 20; ++i)
        {
            const double a = -static_cast<double>(i) * 0.05;  // NEGATIVE arc (clockwise).
            GeometricNode n;
            n.pose.x = R * std::sin(a);
            n.pose.y = R * (1.0 - std::cos(a));
            n.pose.theta = a;
            path.push_back(n);
        }
        ProfileResult r = prof.parameterize(path, lim, fleet, 0.0);
        CHECK(r.ok);
        bool saw_negative_omega = false;
        for (const auto& s : r.chain)
        {
            if (s.omega < -1e-6)
            {
                saw_negative_omega = true;
            }
            CHECK(s.omega <= 1e-6);  // a right turn must never report a positive rate.
        }
        CHECK(saw_negative_omega);  // the regression lock itself — not merely "nonzero".
    }

    // --- reverse=false + reverse edge => ok=false (INV-2-GEO(C)). ---
    {
        std::vector<GeometricNode> path;
        for (int i = 0; i <= 3; ++i)
        {
            GeometricNode n;
            n.pose.x = static_cast<double>(i) * 0.5;
            n.reverse = (i == 2);  // one reverse edge.
            path.push_back(n);
        }
        ProfileResult r = prof.parameterize(path, make_limits(false), fleet, 0.0);
        CHECK(!r.ok);  // forbidden reverse rejected.
        ProfileResult r2 = prof.parameterize(path, make_limits(true), fleet, 0.0);
        CHECK(r2.ok);  // reverse allowed => accepted.
    }

    // --- Degenerate: fewer than 2 nodes => ok=false. ---
    {
        std::vector<GeometricNode> one(1);
        ProfileResult r = prof.parameterize(one, lim, fleet, 0.0);
        CHECK(!r.ok);
    }

    // --- Yaw-accel bound BINDS: straight -> tight arc with generous omega_max
    //     but tight yaw_accel_max. Without enforcement the omega jump at the
    //     junction gives a huge alpha; the profiler must slow the arc down so
    //     the derived yaw acceleration stays under yaw_accel_max_radps2. ---
    {
        FleetLimits sharp = make_fleet();
        sharp.omega_max_radps = 2.0;       // generous — does NOT bind on this arc.
        sharp.yaw_accel_max_radps2 = 0.5;  // tight — MUST bind.

        std::vector<GeometricNode> path;
        // 5 straight nodes (theta=0), 0.1 m spacing.
        for (int i = 0; i < 5; ++i)
        {
            GeometricNode n;
            n.pose.x = static_cast<double>(i) * 0.1;
            n.pose.y = 0.0;
            n.pose.theta = 0.0;
            path.push_back(n);
        }
        // Tight arc: radius 1.0 (curvature ~1.0 < curvature_max 2.0), 0.1 rad steps.
        const double x0 = 0.4, R = 1.0;
        for (int j = 1; j <= 12; ++j)
        {
            const double a = static_cast<double>(j) * 0.1;
            GeometricNode n;
            n.pose.x = x0 + R * std::sin(a);
            n.pose.y = R * (1.0 - std::cos(a));
            n.pose.theta = a;
            path.push_back(n);
        }

        ProfileResult r = prof.parameterize(path, make_limits(false), sharp, 0.0);
        CHECK(r.ok);
        CHECK(r.yaw_accel_relaxation_converged);  // settled within the iteration cap.

        // Derived yaw accel must be within the cap — 368_p8 (370 §10-2): at
        // INTERNAL node i, compare edge (i-1)'s held yr (= chain[i-1].omega)
        // against edge i's held yr (= chain[i].omega), divided by the
        // AVERAGE of the two edges' durations (dtau_i) — the same formula
        // check_traj_limits.py's alpha^fd uses (mid-point timestamps). This
        // is NOT simply (chain[i+1].omega-chain[i].omega)/(chain[i+1].t-
        // chain[i].t) — that compares the WRONG pair of edges against ONE
        // edge's duration (a pre-368_p8 test bug this round also fixes).
        double alpha_max_seen = 0.0;
        double omega_max_seen = 0.0;
        for (std::size_t i = 1; i + 1 < r.chain.size(); ++i)
        {
            const double dt_prev = r.chain[i].t - r.chain[i - 1].t;
            const double dt_next = r.chain[i + 1].t - r.chain[i].t;
            if (dt_prev <= 0.0 || dt_next <= 0.0)
            {
                continue;
            }
            const double dtau = 0.5 * (dt_prev + dt_next);
            const double dalpha = std::fabs(r.chain[i].omega - r.chain[i - 1].omega) / dtau;
            alpha_max_seen = std::max(alpha_max_seen, dalpha);
        }
        for (const auto& s : r.chain)
        {
            omega_max_seen = std::max(omega_max_seen, std::fabs(s.omega));
        }
        CHECK(alpha_max_seen <= sharp.yaw_accel_max_radps2 + 1e-6);  // enforced.
        CHECK(omega_max_seen <= sharp.omega_max_radps + 1e-6);       // still within omega cap.

        // The constraint actually BOUND: arc speed was pushed BELOW the omega
        // ceiling (omega_max/kappa = 2.0/1.0 = 2.0, and v_max=1.0 => vcap=1.0).
        // If yaw-accel had not bound, the arc would run near v=1.0.
        double arc_vmax = 0.0;
        for (std::size_t i = 5; i < r.chain.size(); ++i)
        {
            arc_vmax = std::max(arc_vmax, std::fabs(r.chain[i].v));
        }
        CHECK(arc_vmax < 1.0 - 1e-6);  // yaw-accel reduced the arc speed (non-vacuous).
    }

    // --- 368_p8 regression lock (370 T-CVN1/T-CVN2, "항별" — itemwise, NOT
    //     total-sum): for EVERY edge e, yr_e*Δt_e == Δyaw_e (rotation) and
    //     v_hold_e*Δt_e == chd_e (arc length). The coordinator's original
    //     "total rotation preserved" bar was refuted by math-formalist: a
    //     chain whose start/goal yaw are both 0 preserves a total of 0
    //     whether or not anything is fixed. This test checks the per-edge
    //     (equivalently every prefix sum) form instead — the one `T-CVN1`
    //     actually proves is necessary and sufficient. If this reverts to
    //     the pre-368_p8 (node-centered, central-difference) form, it MUST
    //     fail (confirmed in 368_p8_trajopt_tick_wiring_p8.md by reverting
    //     and re-running). Path: an explicit 90-degree turn (mirrors the
    //     coordinator's own (10,5,0)->(10,10) live-launch scenario, but
    //     given directly as GeometricNodes here — a core/ unit test, no
    //     search/service involved, CN-22). ---
    {
        std::vector<GeometricNode> path(3);
        path[0].pose.x = 0.0;
        path[0].pose.y = 0.0;
        path[0].pose.theta = 0.0;
        path[1].pose.x = 0.0;
        path[1].pose.y = 0.5;
        path[1].pose.theta = M_PI / 2.0;  // 90 degrees — matches the coordinator's scenario.
        path[2].pose.x = 0.0;
        path[2].pose.y = 1.0;
        path[2].pose.theta = M_PI / 2.0;
        path[2].anchor = true;  // goal.

        ProfileResult r = prof.parameterize(path, lim, fleet, 0.0);
        CHECK(r.ok);
        CHECK(r.chain.size() == path.size());

        const std::size_t num_edges = path.size() - 1;
        for (std::size_t e = 0; e < num_edges; ++e)
        {
            const double dyaw_e =
                path[e + 1].pose.theta - path[e].pose.theta;  // no wrap needed here.
            const double chd_e = std::hypot(path[e + 1].pose.x - path[e].pose.x,
                                            path[e + 1].pose.y - path[e].pose.y);
            const double dt_e = r.chain[e + 1].t - r.chain[e].t;
            CHECK(dt_e > 0.0);
            // T-CVN1 (나): yr_e * Δt_e == Δyaw_e, itemwise — not a total sum.
            CHECK_NEAR(r.chain[e].omega * dt_e, dyaw_e, 1e-6);
            // T-CVN2 (the same theorem for arc length / v_hold).
            CHECK_NEAR(r.chain[e].v * dt_e, chd_e, 1e-6);
        }
        // The coordinator's expected numbers (370§1, §8-2): 90 degrees exactly
        // (not the pre-368_p8 0.35226 rad), and 0.5 m of real progress in the
        // first held edge (not the pre-368_p8 0 m).
        CHECK_NEAR(r.chain[0].omega * (r.chain[1].t - r.chain[0].t), M_PI / 2.0, 1e-6);
        CHECK(r.chain[0].v > 0.0);  // real progress in edge 0 — not "sits still".
    }

    return trajopt_test::summary();
}
