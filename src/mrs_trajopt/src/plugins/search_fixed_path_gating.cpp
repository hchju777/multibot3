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
        // 368_p4 FIX: node 0 keeps ctx.start.theta (the robot's REAL current
        // heading, already copied via poses[0] = ctx.start) instead of being
        // unconditionally overwritten by heading_to(start, next). Overwriting
        // it discarded the actual heading and made every node share the same
        // constant "path direction" theta whenever the path is a straight
        // line to the goal — VelocityProfiler::node_curvature's central
        // difference then saw zero heading change everywhere (dtheta=0 at
        // every interior node too, since neighbors were identical), so
        // s.omega = v*kappa was 0 for the WHOLE chain, and CT20 forwards
        // only (v, omega) to /cmd_vel — never theta. The robot's actual
        // orientation (integrated by mrs_sim/state_integrator from commanded
        // omega) then never turned at all, regardless of which way the path
        // pointed. Preserving the real start heading here lets node 1's
        // curvature (computed from node 0 and node 2) see the true mismatch
        // between "where I'm facing" and "where the path points," producing
        // a real nonzero omega during forward motion — a turn-while-moving,
        // NOT an in-place rotation subgoal (RQ-2 unaffected: no subgoal-list
        // element is added; this only changes what theta node 0 reports).
        if (i > 0 && i + 1 < poses.size())
        {
            node.pose.theta = heading_to(poses[i], poses[i + 1]);
        }
        else if (i > 0)
        {
            node.pose.theta = out.chain.back().pose.theta;
        }
        // 368_p6 FIX (321c_trajopt_formal_r2.md:146/:162/:165, "warm-start가
        // 직전 ③의 꼬리를 초기값으로"): node 0 is NOT unconditionally an
        // AnchorSet member anymore. Only the goal (the last node) is. Forcing
        // node 0 to be an anchor made VelocityProfiler zero vcap[0]
        // regardless of the caller's real current speed (ctx.start_v) —
        // every trajectory-tick recompute restarted from rest even while the
        // robot was already moving, contradicting the formal doc's "재계산
        // 이음(n^traj)은 v≠0 가능" (368_p5's "crawl, then permanent fixpoint"
        // finding traced to exactly this). VelocityProfiler already has
        // `vcap.front() = min(vcap.front(), fabs(start_v))` (unchanged) —
        // removing the forced anchor here lets THAT line do its job: a
        // genuinely-at-rest start (start_v==0, e.g. mission genesis, §321c-2
        // "기저") still yields vcap.front()==0 with no new anchor flag
        // needed, and a warm (start_v!=0) recompute now keeps moving.
        node.anchor = (i + 1 == poses.size());
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
