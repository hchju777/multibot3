// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/plugins/search_fixed_path_gating.hpp"

#include <cmath>

namespace mrs_trajopt::plugins
{

namespace
{

double heading_to(const core::Pose2& a, const core::Pose2& b)
{
    return std::atan2(b.y - a.y, b.x - a.x);
}

}  // namespace

bool SearchFixedPathGating::solve(const core::SearchContext& ctx, core::SearchOutput& out)
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
    for (const auto& sg : ctx.subgoals)
    {
        poses.push_back(sg.center);  // fixed path — NO lateral offset.
    }

    out.chain.reserve(poses.size());
    for (std::size_t i = 0; i < poses.size(); ++i)
    {
        core::GeometricNode node;
        node.pose = poses[i];
        node.reverse = false;
        if (i + 1 < poses.size())
        {
            node.pose.theta = heading_to(poses[i], poses[i + 1]);
        }
        else if (i > 0)
        {
            node.pose.theta = out.chain.back().pose.theta;
        }
        node.anchor = (i == 0 || i + 1 == poses.size());
        out.chain.push_back(node);
    }
    out.found = true;
    return true;
}

core::LayerCapabilities SearchFixedPathGating::capabilities() const
{
    core::LayerCapabilities caps;
    caps.supports_warm_start = true;
    caps.handles_reverse = true;
    caps.emits_reachability_witness = true;
    caps.node_budget_max = 0;
    caps.handles_lateral_yield = false;  // 🔴 F-4 [잠정]: gating cell cannot yield laterally.
    return caps;
}

}  // namespace mrs_trajopt::plugins
