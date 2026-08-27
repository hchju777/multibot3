// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/core/velocity_profiler.hpp"

#include <algorithm>
#include <cmath>

namespace mrs_trajopt::core
{

namespace
{

/// @brief Wrap an angle difference into (-pi, pi].
double wrap_pi(double a)
{
    while (a > M_PI)
    {
        a -= 2.0 * M_PI;
    }
    while (a <= -M_PI)
    {
        a += 2.0 * M_PI;
    }
    return a;
}

double seg_len(const Pose2& a, const Pose2& b)
{
    return std::hypot(b.x - a.x, b.y - a.y);
}

}  // namespace

double VelocityProfiler::node_curvature(const std::vector<GeometricNode>& path, std::size_t i)
{
    if (i == 0 || i + 1 >= path.size())
    {
        return 0.0;
    }
    const double ds =
        0.5 * (seg_len(path[i - 1].pose, path[i].pose) + seg_len(path[i].pose, path[i + 1].pose));
    if (ds <= 0.0)
    {
        return 0.0;
    }
    const double dtheta = wrap_pi(path[i + 1].pose.theta - path[i - 1].pose.theta);
    return std::fabs(dtheta) / (2.0 * ds);
}

ProfileResult VelocityProfiler::parameterize(const std::vector<GeometricNode>& path,
                                             const RobotLimits& lim,
                                             const FleetLimits& fleet,
                                             double start_v) const
{
    ProfileResult out;
    if (path.size() < 2)
    {
        return out;
    }

    const std::size_t n = path.size();

    // Design margin: parameterize to 90% of every kinodynamic limit. The
    // checker (and the real robot) judge limits on a *sampled finite difference*
    // of position/heading, which overestimates the true analytic peak on curved
    // segments — designing exactly to a limit fails the checker the moment the
    // grid is refined (references/trajopt.md §흔한 결함, §4 설계 목표 90-95%).
    constexpr double kDesignMargin = 0.9;

    // (C) reverse=false => forward arcs only (INV-2-GEO(C)).
    for (const auto& node : path)
    {
        if (node.reverse && !lim.reverse_motion_allowed)
        {
            out.ok = false;
            return out;
        }
    }

    // Per-node speed ceiling: min(v_max, omega_max/|kappa|) so produced omega
    // never exceeds omega_max. Anchors force 0.
    std::vector<double> vcap(n, lim.v_max);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double kappa = node_curvature(path, i);
        if (kappa > 0.0 && fleet.omega_max_radps > 0.0)
        {
            vcap[i] = std::min(vcap[i], kDesignMargin * fleet.omega_max_radps / kappa);
        }
        if (path[i].anchor)
        {
            vcap[i] = 0.0;
        }
    }
    // Endpoints stop (start uses |start_v|, end is 0 before the braking tail).
    vcap.front() = std::min(vcap.front(), std::fabs(start_v));
    vcap.back() = 0.0;

    // Segment lengths.
    std::vector<double> ds(n - 1, 0.0);
    for (std::size_t i = 0; i + 1 < n; ++i)
    {
        ds[i] = seg_len(path[i].pose, path[i + 1].pose);
    }

    const double a = (fleet.a_max > 0.0 ? fleet.a_max : 1.0) * kDesignMargin;
    const double alpha_max = fleet.yaw_accel_max_radps2 * kDesignMargin;  // yaw-accel cap.

    // Per-node signed curvature (cached for the yaw-accel relaxation).
    std::vector<double> kap(n, 0.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        kap[i] = node_curvature(path, i);
    }

    // Iteratively lower speeds until BOTH bounds hold:
    //   (1) linear accel |v[i+1]^2 - v[i]^2| <= 2*a*ds        (a_max),
    //   (2) yaw accel   |omega[i+1] - omega[i]| / dt <= alpha_max,
    //       where omega[j] = v[j]*kappa[j] and dt = ds / vbar.
    // Every pass only lowers speeds (takes min / scales down), so the iteration
    // is monotone decreasing and converges. Lowering v for (1) never creates a
    // (2) violation (it shrinks omega and grows dt), and vice versa, so the two
    // passes interleave to a common fixpoint.
    std::vector<double> v = vcap;
    bool yaw_converged = true;
    for (int iter = 0; iter < 256; ++iter)
    {
        // (1) forward accel bound.
        for (std::size_t i = 0; i + 1 < n; ++i)
        {
            double reach = std::sqrt(v[i] * v[i] + 2.0 * a * ds[i]);
            v[i + 1] = std::min(v[i + 1], reach);
        }
        // (1) backward decel bound.
        for (std::size_t i = n - 1; i > 0; --i)
        {
            double reach = std::sqrt(v[i] * v[i] + 2.0 * a * ds[i - 1]);
            v[i - 1] = std::min(v[i - 1], reach);
        }
        // (2) yaw-accel bound. Reducing both endpoint speeds of a segment by a
        // factor s scales the omega jump by s and dt by 1/s, so alpha scales by
        // s^2 — set s = sqrt(alpha_max/alpha) to bring a violating segment to
        // the cap. Iterate because neighbors couple.
        bool yaw_changed = false;
        if (alpha_max > 0.0)
        {
            for (std::size_t i = 0; i + 1 < n; ++i)
            {
                const double vbar = 0.5 * (v[i] + v[i + 1]);
                if (vbar <= 1e-9 || ds[i] <= 0.0)
                {
                    continue;
                }
                const double dt = ds[i] / vbar;
                const double alpha = std::fabs(v[i + 1] * kap[i + 1] - v[i] * kap[i]) / dt;
                if (alpha > alpha_max * (1.0 + 1e-9))
                {
                    const double s = std::sqrt(alpha_max / alpha);
                    v[i] *= s;
                    v[i + 1] *= s;
                    yaw_changed = true;
                }
            }
        }
        if (!yaw_changed)
        {
            yaw_converged = true;
            break;  // both bounds hold and nothing lowered this pass.
        }
        yaw_converged = false;  // still relaxing; may hit the iteration cap.
    }
    // Residual guard: if the relaxation did not settle within the cap, the
    // profile may retain a small yaw-accel excess. Report it (analysis-only) so
    // it is never silently emitted (see 20d L-list).
    out.yaw_accel_relaxation_converged = yaw_converged;

    // Integrate time and build the chain.
    out.chain.reserve(n);
    double t = 0.0;
    for (std::size_t i = 0; i < n; ++i)
    {
        StateSample s;
        s.t = t;
        s.x = path[i].pose.x;
        s.y = path[i].pose.y;
        s.theta = path[i].pose.theta;
        const double sign = path[i].reverse ? -1.0 : 1.0;
        s.v = sign * v[i];
        const double kappa = node_curvature(path, i);
        s.omega = sign * v[i] * kappa;  // yaw rate = v * signed curvature.
        s.anchor = path[i].anchor || v[i] == 0.0;
        out.chain.push_back(s);
        if (i + 1 < n)
        {
            const double vbar = 0.5 * (v[i] + v[i + 1]);
            const double dt = vbar > 1e-9 ? ds[i] / vbar : 0.0;
            t += dt;
        }
    }

    // (B) braking tail: from the last non-zero speed decelerate to 0 over
    // v^2/(2*brake_decel). The path already ends at v=0 by vcap.back()=0, so
    // the tail length is the *reserved* distance the tube must contain.
    double vmax_on_path = 0.0;
    for (double vi : v)
    {
        vmax_on_path = std::max(vmax_on_path, vi);
    }
    const double bd = lim.brake_decel_min_mps2 > 0.0 ? lim.brake_decel_min_mps2 : a;
    out.braking_tail_len_m = (vmax_on_path * vmax_on_path) / (2.0 * bd);

    out.ok = true;
    return out;
}

}  // namespace mrs_trajopt::core
