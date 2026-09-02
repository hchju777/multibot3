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

double VelocityProfiler::edge_curvature(const std::vector<GeometricNode>& path, std::size_t e)
{
    // 368_p8 (370 CVN-1/CVN-4): forward difference over edge e's own span —
    // NOT the old node-centered central difference (368_p4/p5/p7's diagnosed
    // defect: that form integrated over edges e-1 and e combined, but ZOH
    // only ever executes edge e's worth of it).
    const double plen = seg_len(path[e].pose, path[e + 1].pose);  // CVN-4 chord approx.
    if (plen <= 0.0)
    {
        return 0.0;
    }
    const double dyaw = wrap_pi(path[e + 1].pose.theta - path[e].pose.theta);
    return dyaw / plen;
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
    const std::size_t num_edges = n - 1;

    // Design margin: parameterize to 90% of every kinodynamic limit. The
    // checker (and the real robot) judge limits on a *sampled finite difference*
    // of position/heading, which overestimates the true analytic peak on curved
    // segments — designing exactly to a limit fails the checker the moment the
    // grid is refined (references/trajopt.md §흔한 결함, §4 설계 목표 90-95%).
    // 🔴 368_p8/370§7 tail: this margin's justification shifts (yr is now
    // exactly the planned/checked quantity, CVN-2) but the VALUE is unchanged
    // — 370 explicitly says not to change kDesignMargin.
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

    // Edge chord lengths (== plen_e under CVN-4's conservative/current choice).
    std::vector<double> ds(num_edges, 0.0);
    for (std::size_t e = 0; e < num_edges; ++e)
    {
        ds[e] = seg_len(path[e].pose, path[e + 1].pose);
    }

    // 368_p8 (370 CVN-1): signed EDGE curvature, one per edge.
    std::vector<double> crv(num_edges, 0.0);
    for (std::size_t e = 0; e < num_edges; ++e)
    {
        crv[e] = edge_curvature(path, e);
    }

    // Per-node speed ceiling — 370§10-1's conservative, per-node SUFFICIENT
    // condition: cap a node's speed by whichever of its (up to two) adjacent
    // EDGE curvatures is larger in magnitude, so that edge's held speed
    // (== the average of its two endpoint node speeds, see below) respects
    // omega_max on BOTH sides. This keeps the existing "every pass only
    // lowers speeds ⇒ monotone ⇒ converges" argument intact (370§10-1,
    // explicitly preserved). Anchors force 0 (unchanged, 368_p6).
    std::vector<double> vcap(n, lim.v_max);
    for (std::size_t i = 0; i < n; ++i)
    {
        double crv_mag = 0.0;
        bool have_edge = false;
        if (i < num_edges)  // outgoing edge i exists.
        {
            crv_mag = std::max(crv_mag, std::fabs(crv[i]));
            have_edge = true;
        }
        if (i > 0)  // incoming edge i-1 exists.
        {
            crv_mag = std::max(crv_mag, std::fabs(crv[i - 1]));
            have_edge = true;
        }
        if (have_edge && crv_mag > 0.0 && fleet.omega_max_radps > 0.0)
        {
            vcap[i] = std::min(vcap[i], kDesignMargin * fleet.omega_max_radps / crv_mag);
        }
        if (path[i].anchor)
        {
            vcap[i] = 0.0;
        }
    }
    // Endpoints: start uses |start_v| (368_p6 warm-start — node 0 is no
    // longer forced to be an anchor by the search plugins), end is 0 before
    // the braking tail.
    vcap.front() = std::min(vcap.front(), std::fabs(start_v));
    vcap.back() = 0.0;

    const double a = (fleet.a_max > 0.0 ? fleet.a_max : 1.0) * kDesignMargin;
    const double alpha_max = fleet.yaw_accel_max_radps2 * kDesignMargin;  // yaw-accel cap.

    // Iteratively lower speeds until BOTH bounds hold:
    //   (1) linear accel |v[i+1]^2 - v[i]^2| <= 2*a*ds   (a_max) — UNCHANGED,
    //       this still models the physical ramp between NODES.
    //   (2) yaw accel |yr_i - yr_{i-1}| / dtau_i <= alpha_max at INTERNAL
    //       node i (370§10-2), where yr_e = crv_e * vbar_e is edge e's TRIAL
    //       held yaw-rate under the current v[] and dtau_i is the average of
    //       the two adjacent edges' durations. Violating this scales all
    //       three node speeds v[i-1],v[i],v[i+1] by the same factor
    //       s=sqrt(alpha_max/ya) — this scales vbar (hence yr) of BOTH
    //       adjacent edges by s and both edges' Δt by 1/s, so ya scales by
    //       s^2, exactly the same monotone-shrink argument the old (node-
    //       pair) form used (comment preserved below).
    // Every pass only lowers speeds (takes min / scales down), so the
    // iteration is monotone decreasing and converges.
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
        // (2) yaw-accel bound, edge-pair form (368_p8).
        bool yaw_changed = false;
        if (alpha_max > 0.0 && num_edges >= 2)
        {
            for (std::size_t i = 1; i + 1 < n; ++i)  // internal nodes: need both edges i-1, i.
            {
                const double vbar_prev = 0.5 * (v[i - 1] + v[i]);
                const double vbar_next = 0.5 * (v[i] + v[i + 1]);
                if (vbar_prev <= 1e-9 || vbar_next <= 1e-9 || ds[i - 1] <= 0.0 || ds[i] <= 0.0)
                {
                    continue;
                }
                const double dt_prev = ds[i - 1] / vbar_prev;
                const double dt_next = ds[i] / vbar_next;
                const double dtau = 0.5 * (dt_prev + dt_next);
                if (dtau <= 1e-9)
                {
                    continue;
                }
                const double yr_prev = crv[i - 1] * vbar_prev;
                const double yr_next = crv[i] * vbar_next;
                const double ya = std::fabs(yr_next - yr_prev) / dtau;
                if (ya > alpha_max * (1.0 + 1e-9))
                {
                    const double s = std::sqrt(alpha_max / ya);
                    v[i - 1] *= s;
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

    // Integrate time and build the chain. 368_p8 (370 CVN-1..3, representation
    // A): node i (i < num_edges) carries edge i's HELD values — a single
    // constant (v_hold_i, yr_i) applied over the whole edge duration, matching
    // exactly what CT15's zero-order-hold sampler executes. v_hold_i reduces
    // to vbar_i = 0.5*(v[i]+v[i+1]) (the SAME quantity already used below to
    // integrate Δt_i = ds_i/vbar_i) — 370§8-2 confirms this numerically. The
    // LAST node (i == n-1) has no outgoing edge, so it holds nothing: yr=0,
    // v=0 (370§6-1 candidate A's ruling on the "undefined last-node value").
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
        if (i < num_edges)
        {
            const double v_hold =
                0.5 * (v[i] + v[i + 1]);  // == plen_i/Δt_i (CVN-3, verified 370§8-2).
            s.v = sign * v_hold;
            s.omega = sign * v_hold * crv[i];  // yr_i = crv_i * v_hold_i (CVN-1..3).
        }
        else
        {
            s.v = 0.0;
            s.omega = 0.0;
        }
        s.anchor = path[i].anchor || s.v == 0.0;
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
    // 🔴 368_p8: uses the internal per-node ramp array v[] (unchanged) — its
    // peak is >= any edge-held vbar, so this stays a safe (if now slightly
    // more conservative) upper bound.
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
