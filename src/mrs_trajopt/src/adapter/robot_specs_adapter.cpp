// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/adapter/robot_specs_adapter.hpp"

namespace mrs_trajopt::adapter
{

core::Status RobotSpecsAdapter::from_boundary(const BoundaryRobotSpecs& doc,
                                              std::vector<core::RobotLimits>& out)
{
    out.clear();
    out.reserve(doc.robots.size());
    for (const auto& r : doc.robots)
    {
        core::RobotLimits lim;
        lim.robot = r.robot;
        lim.v_max = r.v_max;
        lim.brake_decel_min_mps2 = r.brake_decel_min_mps2;
        lim.curvature_max_invm = r.curvature_max_invm;
        lim.reverse_motion_allowed = r.reverse_motion_allowed;
        lim.avg_traversal_speed_mps = r.avg_traversal_speed_mps;
        lim.circumradius_m = r.circumradius_m;
        out.push_back(lim);
    }
    return core::Status::kOk;
}

}  // namespace mrs_trajopt::adapter
