// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/adapter/assignment_adapter.hpp"

namespace mrs_mapf::adapter
{
using mrs_mapf::core::Status;

Status AssignmentAdapter::from_boundary(const BoundaryAssignment& doc,
                                        std::vector<core::AssignmentEntry>& out)
{
    out.clear();
    for (const auto& a : doc.assignments)
    {
        core::AssignmentEntry e;
        e.robot = a.robot;
        e.start = a.start;
        for (const auto& g : a.goals)
        {
            e.goal_locations.push_back(g.location);
        }
        out.push_back(e);
    }
    return Status::kOk;
}

}  // namespace mrs_mapf::adapter
