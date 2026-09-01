// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/adapter/assignment_adapter.hpp"

namespace mrs_mrta::adapter
{

core::Status AssignmentAdapter::to_boundary(const core::AssignmentView& view,
                                            BoundaryAssignment& out)
{
    out.revision = view.revision;

    out.assignments.clear();
    out.assignments.reserve(view.robots.size());
    for (const core::RobotGoalsView& rv : view.robots)
    {
        BoundaryRobotAssignment ba;
        ba.robot = rv.robot;
        ba.start = rv.start;
        ba.goals.reserve(rv.goals.size());
        for (const core::GoalRecord& g : rv.goals)
        {
            BoundaryGoal bg;
            bg.goal_id = g.id;  // Identity copy — A-TA5, no translation.
            bg.task = g.task;
            bg.location = g.location;
            ba.goals.push_back(std::move(bg));
        }
        out.assignments.push_back(std::move(ba));
    }

    out.unassigned_tasks = view.unassigned_tasks;

    return core::Status::kOk;
}

}  // namespace mrs_mrta::adapter
