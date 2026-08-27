// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/plugins/search_hybrid_astar.hpp"

#include <algorithm>
#include <cmath>

namespace mrs_trajopt::plugins
{

namespace
{

double heading_to(const core::Pose2& a, const core::Pose2& b)
{
    return std::atan2(b.y - a.y, b.x - a.x);
}

/// @brief Nearest lateral distance from p to the tube centerline.
double tube_lateral(const core::Tube& tube, const core::Pose2& p)
{
    if (tube.empty())
    {
        return 0.0;
    }
    double best = 1e300;
    for (const auto& c : tube.centerline)
    {
        best = std::min(best, std::hypot(p.x - c.x, p.y - c.y));
    }
    return best;
}

}  // namespace

bool SearchHybridAstar::solve(const core::SearchContext& ctx, core::SearchOutput& out)
{
    out = core::SearchOutput{};
    if (ctx.subgoals.empty())
    {
        out.found = false;
        out.cause = core::SearchFailCause::kEmptyCandidates;
        return false;
    }

    std::vector<core::Pose2> poses;
    poses.push_back(ctx.start);

    // Lateral offset budget: quota, but clamped so the offset stays inside the
    // tube half-width (hard constraint TT01). This is the tube-internal lateral
    // maneuver the gating cell cannot do.
    const double half = ctx.tube.half_width_m;
    const double offset = ctx.tube.empty() ? yield_quota_m_ : std::min(yield_quota_m_, half);

    for (std::size_t i = 0; i < ctx.subgoals.size(); ++i)
    {
        core::Pose2 c = ctx.subgoals[i].center;
        if (offset > 0.0 && i + 1 < ctx.subgoals.size())
        {
            // Offset perpendicular to the local travel direction, then verify
            // it stays inside the tube (else drop the offset for this node).
            const double dir = heading_to(c, ctx.subgoals[i + 1].center);
            core::Pose2 shifted = c;
            shifted.x += -std::sin(dir) * offset;
            shifted.y += std::cos(dir) * offset;
            if (ctx.tube.empty() || tube_lateral(ctx.tube, shifted) <= half + 1e-9)
            {
                c = shifted;
            }
        }
        poses.push_back(c);
    }

    // Fill headings (each node points at the next; last keeps prior heading).
    out.chain.reserve(poses.size());
    for (std::size_t i = 0; i < poses.size(); ++i)
    {
        core::GeometricNode node;
        node.pose = poses[i];
        node.reverse = false;  // forward-only (INV-2-GEO(C) safe).
        if (i + 1 < poses.size())
        {
            node.pose.theta = heading_to(poses[i], poses[i + 1]);
        }
        else if (i > 0)
        {
            node.pose.theta = out.chain.back().pose.theta;
        }
        node.anchor = (i == 0 || i + 1 == poses.size());  // start & goal are anchors.
        out.chain.push_back(node);
    }
    out.found = true;
    return true;
}

core::LayerCapabilities SearchHybridAstar::capabilities() const
{
    core::LayerCapabilities caps;
    caps.supports_warm_start = true;
    caps.handles_reverse = true;
    caps.emits_reachability_witness = true;
    caps.node_budget_max = 0;           // [값 부재] — set from config/spike.
    caps.handles_lateral_yield = true;  // 🔴 F-4 [잠정]: this cell realizes yield.
    return caps;
}

}  // namespace mrs_trajopt::plugins
