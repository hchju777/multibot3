// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/adapter/robot_specs_adapter.hpp"

namespace mrs_mapf::adapter
{
using mrs_mapf::core::Status;

Status RobotSpecsAdapter::from_boundary(const BoundaryRobotSpecs& doc,
                                        std::vector<core::RobotSpec>& out)
{
    out.clear();
    if (doc.robots.empty())
    {
        return Status::kError;
    }
    for (const auto& r : doc.robots)
    {
        core::RobotSpec s;
        s.robot = r.robot;
        s.avg_traversal_speed_mps =
            r.avg_traversal_speed_mps;  // A33: the only legal field besides `robot`.
        out.push_back(s);
    }
    return Status::kOk;
}

}  // namespace mrs_mapf::adapter
