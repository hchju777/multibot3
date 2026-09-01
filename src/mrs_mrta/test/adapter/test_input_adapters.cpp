// SPDX-License-Identifier: Apache-2.0
// CN-22: test/adapter/ — from_boundary correctness for the four consumed
// channels (task_release, goal_completion, operator_command, roadmap).
#include <string>

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/adapter/goal_completion_adapter.hpp"
#include "mrs_mrta/adapter/operator_command_adapter.hpp"
#include "mrs_mrta/adapter/roadmap_adapter.hpp"
#include "mrs_mrta/adapter/task_release_adapter.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "test_util.hpp"

using namespace mrs_mrta::adapter;  // NOLINT — small test file, all four adapters used throughout.
using mrs_mrta::core::CompletionEvent;
using mrs_mrta::core::OperatorEvent;
using mrs_mrta::core::ReleaseEvent;
using mrs_mrta::core::RoadmapView;
using mrs_mrta::core::Status;

namespace
{

void test_task_release_adapter()
{
    BoundaryTaskRelease msg;
    msg.release_index = 3;
    msg.task = "t1";
    msg.goals = {{"g1", "L1"}, {"g2", "L2"}};

    ReleaseEvent ev;
    MRTA_CHECK(TaskReleaseAdapter::from_boundary(msg, ev) == Status::kOk);
    MRTA_CHECK(ev.release_index == 3);
    MRTA_CHECK(ev.task == "t1");
    MRTA_CHECK(ev.goals.size() == 2);
    MRTA_CHECK(ev.goals[0].id == "g1" && ev.goals[0].location == "L1" && ev.goals[0].task == "t1");
    MRTA_CHECK(ev.goals[1].id == "g2" && ev.goals[1].location == "L2");

    BoundaryTaskRelease empty;
    empty.task = "t2";
    ReleaseEvent ev2;
    MRTA_CHECK(TaskReleaseAdapter::from_boundary(empty, ev2) == Status::kError);  // goals empty.
}

void test_goal_completion_adapter()
{
    BoundaryGoalCompletion msg;
    msg.seq = 4;
    msg.robot = "r1";
    msg.goal_id = "g9";
    msg.revision = 2;

    CompletionEvent ev;
    MRTA_CHECK(GoalCompletionAdapter::from_boundary(msg, ev) == Status::kOk);
    MRTA_CHECK(ev.seq == 4 && ev.robot == "r1" && ev.goal_id == "g9" && ev.revision == 2);
}

void test_operator_command_adapter()
{
    BoundaryOperatorCommand ok_active;
    ok_active.seq = 1;
    ok_active.robot = "r1";
    ok_active.verb = "goal_override";
    ok_active.active = true;
    ok_active.goal_location = "L5";

    OperatorEvent ev;
    MRTA_CHECK(OperatorCommandAdapter::from_boundary(ok_active, ev) == Status::kOk);
    MRTA_CHECK(ev.robot == "r1" && ev.active && ev.goal_location == "L5" && ev.seq == 1);

    BoundaryOperatorCommand ok_inactive;
    ok_inactive.seq = 2;
    ok_inactive.robot = "r1";
    ok_inactive.verb = "goal_override";
    ok_inactive.active = false;
    OperatorEvent ev2;
    MRTA_CHECK(OperatorCommandAdapter::from_boundary(ok_inactive, ev2) == Status::kOk);
    MRTA_CHECK(!ev2.active && ev2.goal_location.empty());

    BoundaryOperatorCommand wrong_verb;
    wrong_verb.verb = "hold";
    wrong_verb.active = true;
    OperatorEvent ev3;
    MRTA_CHECK(OperatorCommandAdapter::from_boundary(wrong_verb, ev3) == Status::kError);

    BoundaryOperatorCommand missing_location;
    missing_location.verb = "goal_override";
    missing_location.active = true;
    // goal_location left at its default "" — the wire absence sentinel (F46-1 fix).
    // Layer-2 conditional-required violated.
    OperatorEvent ev4;
    MRTA_CHECK(OperatorCommandAdapter::from_boundary(missing_location, ev4) == Status::kError);
}

void test_roadmap_adapter()
{
    BoundaryRoadmap msg;
    msg.node_ids = {"V1", "V2", "L1"};
    msg.endpoints = {"L1"};

    RoadmapView view;
    MRTA_CHECK(RoadmapAdapter::from_boundary(msg, view) == Status::kOk);
    MRTA_CHECK(view.has_node("V1"));
    MRTA_CHECK(!view.has_node("nope"));
    MRTA_CHECK(view.is_endpoint("L1"));
    MRTA_CHECK(!view.is_endpoint("V1"));

    BoundaryRoadmap empty;
    RoadmapView view2;
    MRTA_CHECK(RoadmapAdapter::from_boundary(empty, view2) == Status::kError);  // nodes empty.
}

}  // namespace

int main()
{
    test_task_release_adapter();
    test_goal_completion_adapter();
    test_operator_command_adapter();
    test_roadmap_adapter();
    return 0;
}
