// SPDX-License-Identifier: Apache-2.0
// CN-22: test/adapter/ — AssignmentAdapter::to_boundary field-identity check
// (assumption A-TA5, `336_mrta_canon_p2.md` §336-5-3: the adapter is an
// identity translation for goal_id/task/location, no gap). Since `mrta` never
// consumes `mrs.assignment`, there is no from_boundary to round-trip against
// (CN-9) — this test instead verifies the one-directional transform is
// lossless and order-preserving, which is the property `to_boundary` alone
// must uphold.
#include <string>

#include "mrs_mrta/adapter/assignment_adapter.hpp"
#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "test_util.hpp"

using mrs_mrta::adapter::AssignmentAdapter;
using mrs_mrta::adapter::BoundaryAssignment;
using mrs_mrta::core::AssignmentView;
using mrs_mrta::core::GoalRecord;
using mrs_mrta::core::RobotGoalsView;
using mrs_mrta::core::Status;

namespace
{

GoalRecord goal(const std::string& id, const std::string& task, const std::string& loc)
{
    GoalRecord g;
    g.id = id;
    g.task = task;
    g.location = loc;
    return g;
}

void test_identity_and_order_preserved()
{
    AssignmentView view;
    view.revision = 7;

    RobotGoalsView ra;
    ra.robot = "r1";
    ra.start = "V3";
    ra.goals = {goal("g1", "t1", "L1"), goal("g2", "t1", "L2")};  // Multi-leg, order matters.
    RobotGoalsView rb;
    rb.robot = "r2";
    rb.start = "V9";
    // Empty goals — an idle robot is a legal, common case (schema does not require minItems on
    // goals).
    view.robots = {ra, rb};
    view.unassigned_tasks = {"t9"};

    BoundaryAssignment out;
    out.instance_id = "keep-me";  // Envelope field the adapter must not touch.
    MRTA_CHECK(AssignmentAdapter::to_boundary(view, out) == Status::kOk);

    MRTA_CHECK(out.instance_id == "keep-me");  // Untouched, as documented.
    MRTA_CHECK(out.schema == "mrs.assignment");
    MRTA_CHECK(out.schema_version == "2.0.0");
    MRTA_CHECK(out.revision == 7);
    MRTA_CHECK(out.assignments.size() == 2);

    MRTA_CHECK(out.assignments[0].robot == "r1");
    MRTA_CHECK(out.assignments[0].start == "V3");
    MRTA_CHECK(out.assignments[0].goals.size() == 2);
    // Order preserved (leg order is the boundary's within-task order, task_release 2.0.0 regime).
    MRTA_CHECK(out.assignments[0].goals[0].goal_id == "g1");
    MRTA_CHECK(out.assignments[0].goals[0].task == "t1");
    MRTA_CHECK(out.assignments[0].goals[0].location == "L1");
    MRTA_CHECK(out.assignments[0].goals[1].goal_id == "g2");

    MRTA_CHECK(out.assignments[1].robot == "r2");
    MRTA_CHECK(out.assignments[1].goals.empty());  // Idle robot still appears with goals: [].

    MRTA_CHECK(out.unassigned_tasks.size() == 1 && out.unassigned_tasks[0] == "t9");
}

void test_no_gap_no_fabrication()
{
    // A view with no robots and no unassigned tasks converts cleanly to empty arrays —
    // the adapter never invents an entry.
    AssignmentView view;
    view.revision = 0;
    BoundaryAssignment out;
    MRTA_CHECK(AssignmentAdapter::to_boundary(view, out) == Status::kOk);
    MRTA_CHECK(out.assignments.empty());
    MRTA_CHECK(out.unassigned_tasks.empty());
}

}  // namespace

int main()
{
    test_identity_and_order_preserved();
    test_no_gap_no_fabrication();
    return 0;
}
