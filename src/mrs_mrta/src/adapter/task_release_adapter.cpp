// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/adapter/task_release_adapter.hpp"

namespace mrs_mrta::adapter
{

core::Status TaskReleaseAdapter::from_boundary(const BoundaryTaskRelease& msg,
                                               core::ReleaseEvent& out)
{
    if (msg.goals.empty())
    {
        return core::Status::kError;
    }

    out.release_index = msg.release_index;
    out.task = msg.task;
    out.goals.clear();
    out.goals.reserve(msg.goals.size());
    for (const BoundaryTaskReleaseGoal& g : msg.goals)
    {
        core::GoalRecord gr;
        gr.id = g.goal_id;
        gr.task = msg.task;
        gr.location = g.location;
        out.goals.push_back(std::move(gr));
    }
    return core::Status::kOk;
}

}  // namespace mrs_mrta::adapter
