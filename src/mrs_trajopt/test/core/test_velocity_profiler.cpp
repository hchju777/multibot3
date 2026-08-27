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
        CHECK_NEAR(r.chain.front().v, 0.0, 1e-9);  // start anchor v=0.
        CHECK_NEAR(r.chain.back().v, 0.0, 1e-9);   // end v=0.
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

        // Derived yaw accel (finite diff of omega) must be within the cap — the
        // same quantity the augmented check_traj_limits.py judges.
        double alpha_max_seen = 0.0;
        double omega_max_seen = 0.0;
        for (std::size_t i = 0; i + 1 < r.chain.size(); ++i)
        {
            const double dt = r.chain[i + 1].t - r.chain[i].t;
            if (dt <= 0.0)
            {
                continue;
            }
            const double dalpha = std::fabs(r.chain[i + 1].omega - r.chain[i].omega) / dt;
            alpha_max_seen = std::max(alpha_max_seen, dalpha);
            omega_max_seen = std::max(omega_max_seen, std::fabs(r.chain[i].omega));
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

    return trajopt_test::summary();
}
