// SPDX-License-Identifier: Apache-2.0
#include <cmath>

#include "../test_util.hpp"
#include "mrs_trajopt/adapter/trajectories_adapter.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::adapter;

int main()
{
    // Build a forward-moving curved internal chain (t,x,y,theta,v,omega).
    std::vector<StateSample> chain;
    const double R = 2.0, w = 0.3;  // circle radius 2, yaw rate 0.3 rad/s.
    for (int i = 0; i <= 12; ++i)
    {
        const double t = static_cast<double>(i) * 0.1;
        StateSample s;
        s.t = t;
        s.theta = w * t;
        s.x = R * std::sin(s.theta);
        s.y = R * (1.0 - std::cos(s.theta));
        s.v = R * w;  // constant speed on the circle (forward, +).
        s.omega = w;  // constant yaw rate.
        chain.push_back(s);
    }

    FleetLimits fleet;
    fleet.a_max = 1.0;
    fleet.j_max = 5.0;
    fleet.min_separation_m = 0.6;
    fleet.omega_max_radps = 0.5;
    fleet.yaw_accel_max_radps2 = 1.0;

    // internal -> boundary -> internal.
    BoundaryTrajectories doc;
    CHECK(ok(TrajectoriesAdapter::to_boundary({chain}, {"r0"}, fleet, "inst-1", doc)));
    CHECK(doc.trajectories.size() == 1);
    CHECK(doc.trajectories[0].robot == "r0");
    CHECK(doc.limits.omega_max_radps == fleet.omega_max_radps);

    std::vector<std::vector<StateSample>> back;
    std::vector<std::string> robots;
    CHECK(ok(TrajectoriesAdapter::from_boundary(doc, back, robots)));
    CHECK(back.size() == 1);
    CHECK(robots.size() == 1 && robots[0] == "r0");

    const auto& rc = back[0];
    CHECK(rc.size() == chain.size());

    // Boundary-relevant meaning preserved exactly: t, x, y, theta, |v|.
    for (std::size_t i = 0; i < chain.size(); ++i)
    {
        CHECK_NEAR(rc[i].t, chain[i].t, 1e-9);
        CHECK_NEAR(rc[i].x, chain[i].x, 1e-9);
        CHECK_NEAR(rc[i].y, chain[i].y, 1e-9);
        CHECK_NEAR(rc[i].theta, chain[i].theta, 1e-9);
        CHECK_NEAR(std::fabs(rc[i].v), std::fabs(chain[i].v), 1e-9);  // magnitude preserved.
        CHECK(rc[i].v >= 0.0);  // forward chain reconstructs a positive sign.
    }

    // omega is dropped at the boundary and re-derived by yaw finite difference —
    // it reconstructs to the original within finite-difference tolerance.
    for (std::size_t i = 0; i + 1 < chain.size(); ++i)
    {
        CHECK_NEAR(rc[i].omega, chain[i].omega, 1e-6);
    }

    // Length mismatch => kError (defensive contract).
    BoundaryTrajectories bad;
    CHECK(TrajectoriesAdapter::to_boundary({chain}, {}, fleet, "x", bad) == Status::kError);

    return trajopt_test::summary();
}
