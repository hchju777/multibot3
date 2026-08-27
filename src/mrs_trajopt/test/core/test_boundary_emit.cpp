// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <string>
#include <vector>

#include "../test_util.hpp"
#include "mrs_trajopt/adapter/stop_declaration_adapter.hpp"
#include "mrs_trajopt/adapter/trajectories_adapter.hpp"
#include "mrs_trajopt/core/declaration_regulator.hpp"
#include "mrs_trajopt/core/velocity_profiler.hpp"
#include "mrs_trajopt/io/json_io.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::adapter;

/// @file test_boundary_emit.cpp
/// @brief Boundary-artifact checker test: emit a schema-valid mrs.trajectories
/// and mrs.stop_declaration + a matching mrs.robot_specs, so the python checkers
/// (check_traj_limits.py, check_schema_purity.py) can be run against them. The
/// files are written next to the test binary (or argv[1] dir). See the checker
/// table in 20d.

namespace
{

RobotLimits lim(const std::string& r, double v_max)
{
    RobotLimits l;
    l.robot = r;
    l.v_max = v_max;
    l.brake_decel_min_mps2 = 1.0;
    l.curvature_max_invm = 2.0;
    l.reverse_motion_allowed = false;
    l.avg_traversal_speed_mps = 0.8;
    l.circumradius_m = 0.25;
    return l;
}

std::vector<GeometricNode> straight(double y0, int n, double dx)
{
    std::vector<GeometricNode> path;
    for (int i = 0; i <= n; ++i)
    {
        GeometricNode nd;
        nd.pose.x = static_cast<double>(i) * dx;
        nd.pose.y = y0;
        nd.pose.theta = 0.0;
        path.push_back(nd);
    }
    return path;
}

// A curved lane (arc, radius 2 => curvature 0.5) far from the straight lanes, so
// the augmented checker's derived omega / yaw-accel judgment runs on real curved
// motion (not only omega=0 straight lanes).
std::vector<GeometricNode> arc(double cx, double cy, double R, int n)
{
    std::vector<GeometricNode> path;
    for (int i = 0; i <= n; ++i)
    {
        const double a = static_cast<double>(i) * 0.1;
        GeometricNode nd;
        nd.pose.x = cx + R * std::sin(a);
        nd.pose.y = cy - R * std::cos(a);
        nd.pose.theta = a;
        path.push_back(nd);
    }
    return path;
}

std::string robot_specs_json()
{
    return R"({
  "schema": "mrs.robot_specs",
  "schema_version": "1.1.0",
  "instance_id": "spike-emit",
  "robots": [
    {"robot": "r0", "avg_traversal_speed_mps": 0.8, "v_max": 1.0,
     "brake_decel_min_mps2": 1.0, "curvature_max_invm": 2.0,
     "reverse_motion_allowed": false, "circumradius_m": 0.25},
    {"robot": "r1", "avg_traversal_speed_mps": 0.8, "v_max": 1.0,
     "brake_decel_min_mps2": 1.0, "curvature_max_invm": 2.0,
     "reverse_motion_allowed": false, "circumradius_m": 0.25},
    {"robot": "r2", "avg_traversal_speed_mps": 0.8, "v_max": 1.0,
     "brake_decel_min_mps2": 1.0, "curvature_max_invm": 2.0,
     "reverse_motion_allowed": false, "circumradius_m": 0.25}
  ]
})";
}

}  // namespace

int main(int argc, char** argv)
{
    const std::string dir = argc > 1 ? std::string(argv[1]) : std::string(".");

    FleetLimits fleet;
    fleet.a_max = 1.0;
    fleet.j_max = 5.0;
    fleet.min_separation_m = 0.6;
    fleet.omega_max_radps = 0.5;
    fleet.yaw_accel_max_radps2 = 1.0;

    VelocityProfiler prof;
    // Two parallel lanes 1.0 m apart (>> min_separation 0.6) — no collision.
    ProfileResult p0 = prof.parameterize(straight(0.0, 10, 0.5), lim("r0", 1.0), fleet, 0.0);
    ProfileResult p1 = prof.parameterize(straight(1.0, 10, 0.5), lim("r1", 1.0), fleet, 0.0);
    // r2: an arc far from the lanes (center (0,8), radius 2) — exercises the
    // checker's derived omega / yaw-accel judgment on curved motion.
    ProfileResult p2 = prof.parameterize(arc(0.0, 8.0, 2.0, 12), lim("r2", 1.0), fleet, 0.0);
    CHECK(p0.ok && p1.ok && p2.ok);
    CHECK(p2.yaw_accel_relaxation_converged);  // emitted arc has bounded yaw accel.

    BoundaryTrajectories doc;
    CHECK(ok(TrajectoriesAdapter::to_boundary({p0.chain, p1.chain, p2.chain},
                                              {"r0", "r1", "r2"},
                                              fleet,
                                              "spike-emit",
                                              doc)));
    const std::string tj = mrs_trajopt::io::to_json(doc);
    CHECK(tj.find("\"schema\": \"mrs.trajectories\"") != std::string::npos);
    CHECK(tj.find("\"schema_version\": \"2.0.0\"") != std::string::npos);
    CHECK(tj.find("has_v") == std::string::npos);  // no extra keys (additionalProperties:false).

    // A stop declaration: reverse-forbidden infeasibility => infeasible_subgoal.
    std::vector<InternalDeclaration> decls;
    InternalDeclaration d;
    d.seq = 0;
    d.robot = "r0";
    d.blocked_from = "n3";
    d.blocked_to = "n4";
    d.declared = true;
    d.reason = DeclarationRegulator::reverse_forbidden_infeasible();
    decls.push_back(d);
    BoundaryStopDeclarations sd;
    CHECK(ok(StopDeclarationAdapter::to_boundary(decls, "spike-emit", sd)));
    const std::string sj = mrs_trajopt::io::to_json(sd);
    CHECK(sj.find("\"infeasible_subgoal\"") != std::string::npos);
    CHECK(sj.find("\"schema_version\": \"5.0.0\"") != std::string::npos);

    // Write the artifacts for the python checkers.
    std::ofstream(dir + "/emitted_trajectories.json") << tj;
    std::ofstream(dir + "/emitted_stop_declaration.json") << sj;
    std::ofstream(dir + "/emitted_robot_specs.json") << robot_specs_json();

    return trajopt_test::summary();
}
