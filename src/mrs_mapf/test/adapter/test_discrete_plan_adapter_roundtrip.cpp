// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_mapf/adapter/discrete_plan_adapter.hpp"

using namespace mrs_mapf::core;
using namespace mrs_mapf::adapter;

/// @file test_discrete_plan_adapter_roundtrip.cpp
/// @brief Internal -> boundary -> internal round-trip for the ONE artifact
/// this module both produces and consumes. Also proves the envelope-mismatch
/// guard (`from_boundary` rejects a stale schema_version rather than silently
/// accepting it — the counterpart of 함정 ③ inside the adapter itself).

int main()
{
    DiscretePlanDraft original;
    original.instance_id = "roundtrip-test";
    original.plan_revision = 7;

    RobotPlan p0;
    p0.robot = "r0";
    p0.terminal = Terminal::kGoalReached;
    p0.steps = {{0, "A"}, {1, "B"}, {2, "C"}};
    RobotPlan p1;
    p1.robot = "r1";
    p1.terminal = Terminal::kPlanTruncated;
    p1.steps = {{0, "D"}, {1, "C"}};
    original.plans = {p0, p1};

    VisitOrderEntry ve;
    ve.location = "C";
    ve.sequence = {{"r1", 1}, {"r0", 2}};
    original.visit_order = {ve};

    BoundaryDiscretePlan boundary;
    CHECK(ok(DiscretePlanAdapter::to_boundary(original, boundary)));
    CHECK(boundary.schema == "mrs.discrete_plan");
    CHECK(boundary.schema_version == "4.0.0");
    CHECK(boundary.plan_revision == 7);
    CHECK(boundary.plans.size() == 2);

    DiscretePlanDraft recovered;
    CHECK(ok(DiscretePlanAdapter::from_boundary(boundary, recovered)));

    CHECK(recovered.instance_id == original.instance_id);
    CHECK(recovered.plan_revision == original.plan_revision);
    CHECK(recovered.plans.size() == original.plans.size());
    for (std::size_t i = 0; i < original.plans.size(); ++i)
    {
        CHECK(recovered.plans[i].robot == original.plans[i].robot);
        CHECK(recovered.plans[i].terminal == original.plans[i].terminal);
        CHECK(recovered.plans[i].steps.size() == original.plans[i].steps.size());
        for (std::size_t j = 0; j < original.plans[i].steps.size(); ++j)
        {
            CHECK(recovered.plans[i].steps[j].index == original.plans[i].steps[j].index);
            CHECK(recovered.plans[i].steps[j].location == original.plans[i].steps[j].location);
        }
    }
    CHECK(recovered.visit_order.size() == original.visit_order.size());
    CHECK(recovered.visit_order[0].location == original.visit_order[0].location);
    CHECK(recovered.visit_order[0].sequence.size() == original.visit_order[0].sequence.size());

    // Envelope-mismatch guard: a stale schema_version is rejected, not silently accepted.
    BoundaryDiscretePlan stale = boundary;
    stale.schema_version = "2.0.0";
    DiscretePlanDraft rejected;
    CHECK(!ok(DiscretePlanAdapter::from_boundary(stale, rejected)));

    return mapf_test::summary();
}
