// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/adapter/goal_completion_adapter.hpp"

namespace mrs_mrta::adapter
{

core::Status GoalCompletionAdapter::from_boundary(const BoundaryGoalCompletion& msg,
                                                  core::CompletionEvent& out)
{
    out.robot = msg.robot;
    out.goal_id = msg.goal_id;
    out.seq = msg.seq;
    out.revision = msg.revision;
    return core::Status::kOk;
}

}  // namespace mrs_mrta::adapter
