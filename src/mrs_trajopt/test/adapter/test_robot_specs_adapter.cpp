// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/adapter/robot_specs_adapter.hpp"
#include "mrs_trajopt/io/json_io.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::adapter;

int main()
{
    // Parse a robot_specs JSON (io/), then adapt to internal RobotLimits.
    const std::string text = R"({
      "schema": "mrs.robot_specs",
      "schema_version": "1.1.0",
      "instance_id": "inst-1",
      "robots": [
        {"robot": "r0", "avg_traversal_speed_mps": 0.8, "v_max": 1.2,
         "brake_decel_min_mps2": 1.5, "curvature_max_invm": 2.0,
         "reverse_motion_allowed": false, "circumradius_m": 0.35},
        {"robot": "r1", "avg_traversal_speed_mps": 0.6, "v_max": 0.9,
         "brake_decel_min_mps2": 1.1, "curvature_max_invm": 3.0,
         "reverse_motion_allowed": true, "circumradius_m": 0.30}
      ]
    })";

    BoundaryRobotSpecs doc = mrs_trajopt::io::parse_robot_specs(text);
    CHECK(doc.robots.size() == 2);

    std::vector<RobotLimits> lims;
    CHECK(ok(RobotSpecsAdapter::from_boundary(doc, lims)));
    CHECK(lims.size() == 2);

    // The six robot-scoped fields carry across (SU08 consumption, A33).
    CHECK(lims[0].robot == "r0");
    CHECK_NEAR(lims[0].v_max, 1.2, 1e-12);
    CHECK_NEAR(lims[0].brake_decel_min_mps2, 1.5, 1e-12);
    CHECK_NEAR(lims[0].curvature_max_invm, 2.0, 1e-12);
    CHECK(lims[0].reverse_motion_allowed == false);
    CHECK_NEAR(lims[0].avg_traversal_speed_mps, 0.8, 1e-12);
    CHECK_NEAR(lims[0].circumradius_m, 0.35, 1e-12);

    CHECK(lims[1].robot == "r1");
    CHECK(lims[1].reverse_motion_allowed == true);
    CHECK_NEAR(lims[1].v_max, 0.9, 1e-12);

    return trajopt_test::summary();
}
