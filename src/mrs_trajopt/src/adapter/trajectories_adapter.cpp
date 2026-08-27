// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/adapter/trajectories_adapter.hpp"

#include <cmath>

namespace mrs_trajopt::adapter
{

namespace
{

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

}  // namespace

core::Status TrajectoriesAdapter::to_boundary(
    const std::vector<std::vector<core::StateSample>>& chains,
    const std::vector<std::string>& robots,
    const core::FleetLimits& fleet,
    const std::string& instance_id,
    BoundaryTrajectories& out)
{
    if (chains.size() != robots.size())
    {
        return core::Status::kError;
    }
    out = BoundaryTrajectories{};
    out.instance_id = instance_id;
    out.limits.a_max = fleet.a_max;
    out.limits.j_max = fleet.j_max;
    out.limits.min_separation_m = fleet.min_separation_m;
    out.limits.omega_max_radps = fleet.omega_max_radps;
    out.limits.yaw_accel_max_radps2 = fleet.yaw_accel_max_radps2;

    for (std::size_t r = 0; r < chains.size(); ++r)
    {
        const auto& chain = chains[r];
        BoundaryTrajectory bt;
        bt.robot = robots[r];
        bt.points.reserve(chain.size());
        for (std::size_t i = 0; i < chain.size(); ++i)
        {
            const auto& s = chain[i];
            BoundaryTrajectoryPoint p;
            p.t = s.t;
            p.x = s.x;
            p.y = s.y;
            p.yaw = wrap_pi(s.theta);
            p.has_v = true;
            p.v = std::fabs(s.v);  // magnitude only — sign is internal (§3-2).
            // a = finite-difference of |v|, reported as magnitude. omega dropped.
            if (i + 1 < chain.size())
            {
                const double dt = chain[i + 1].t - s.t;
                if (dt > 0.0)
                {
                    p.has_a = true;
                    p.a = std::fabs((std::fabs(chain[i + 1].v) - std::fabs(s.v)) / dt);
                }
            }
            bt.points.push_back(p);
        }
        out.trajectories.push_back(std::move(bt));
    }
    return core::Status::kOk;
}

core::Status TrajectoriesAdapter::from_boundary(
    const BoundaryTrajectories& doc,
    std::vector<std::vector<core::StateSample>>& out_chains,
    std::vector<std::string>& out_robots)
{
    out_chains.clear();
    out_robots.clear();
    for (const auto& bt : doc.trajectories)
    {
        std::vector<core::StateSample> chain;
        chain.reserve(bt.points.size());
        for (std::size_t i = 0; i < bt.points.size(); ++i)
        {
            const auto& p = bt.points[i];
            core::StateSample s;
            s.t = p.t;
            s.x = p.x;
            s.y = p.y;
            s.theta = p.yaw;
            // Recover v sign by projecting displacement on heading (checker's
            // method): forward if the step projects positively, else reverse.
            double sign = 1.0;
            if (i + 1 < bt.points.size())
            {
                const double dx = bt.points[i + 1].x - p.x;
                const double dy = bt.points[i + 1].y - p.y;
                if (std::hypot(dx, dy) > 0.0)
                {
                    const double proj = dx * std::cos(p.yaw) + dy * std::sin(p.yaw);
                    sign = proj < 0.0 ? -1.0 : 1.0;
                }
            }
            s.v = sign * (p.has_v ? p.v : 0.0);
            // Re-derive omega by finite difference of yaw.
            if (i + 1 < bt.points.size())
            {
                const double dt = bt.points[i + 1].t - p.t;
                if (dt > 0.0)
                {
                    s.omega = wrap_pi(bt.points[i + 1].yaw - p.yaw) / dt;
                }
            }
            s.anchor = (p.has_v && p.v == 0.0);
            chain.push_back(s);
        }
        out_chains.push_back(std::move(chain));
        out_robots.push_back(bt.robot);
    }
    return core::Status::kOk;
}

}  // namespace mrs_trajopt::adapter
