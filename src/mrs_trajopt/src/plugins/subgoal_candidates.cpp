// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/plugins/subgoal_candidates.hpp"

#include <cmath>

namespace mrs_trajopt::plugins
{

namespace
{

/// @brief Place pass windows along start->goal spaced by stride, radius r.
bool line_windows(const core::SubgoalContext& ctx,
                  double stride,
                  double r,
                  std::vector<core::PassWindow>& out)
{
    out.clear();
    const double dx = ctx.goal.x - ctx.start.x;
    const double dy = ctx.goal.y - ctx.start.y;
    const double len = std::hypot(dx, dy);
    if (len <= 0.0 || stride <= 0.0)
    {
        return false;
    }
    const double dir = std::atan2(dy, dx);
    const int n = static_cast<int>(len / stride);
    for (int i = 1; i <= n; ++i)
    {
        const double s = static_cast<double>(i) * stride;
        core::PassWindow w;
        w.center.x = ctx.start.x + dx * (s / len);
        w.center.y = ctx.start.y + dy * (s / len);
        w.center.theta = dir;
        w.radius_win_m = r;
        w.theta_lo = dir - 0.5;
        w.theta_hi = dir + 0.5;
        out.push_back(w);
    }
    // Always terminate at the goal.
    core::PassWindow g;
    g.center = ctx.goal;
    g.center.theta = dir;
    g.radius_win_m = r;
    g.theta_lo = dir - 0.5;
    g.theta_hi = dir + 0.5;
    out.push_back(g);
    return true;
}

}  // namespace

bool SubgoalFreeSpaceDisk::generate(const core::SubgoalContext& ctx,
                                    std::vector<core::PassWindow>& out)
{
    return line_windows(ctx, stride_m_, window_radius_m_, out);
}

bool SubgoalRoadmapProjected::generate(const core::SubgoalContext& ctx,
                                       std::vector<core::PassWindow>& out)
{
    return line_windows(ctx, stride_m_, window_radius_m_, out);
}

}  // namespace mrs_trajopt::plugins
