// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_mapf/adapter/replan_request_adapter.hpp"

using namespace mrs_mapf::core;
using namespace mrs_mapf::adapter;

/// @file test_replan_request_adapter.cpp
/// @brief mrs.replan_request 7.0.0 -> `ReplanRequestFields`: `affected_robots`
/// (the floor), `blocked`, `ordering_constraints` (received prefix),
/// `baseline`, and `plan_revision` (Clause A — §255-4-3 / D-3's selector) all
/// flatten losslessly.

int main()
{
    BoundaryReplanRequest doc;
    doc.instance_id = "adapter-test";
    doc.reason = "exogenous_block";
    doc.affected_robots = {"r0", "r1"};
    doc.blocked = {{"B", "C"}};
    doc.blocked_hash = "opaque";
    doc.plan_revision = 3;

    BoundaryOrderingConstraint oc;
    oc.location = "B";
    oc.sequence = {{"r2", 4}};
    doc.ordering_constraints = {oc};

    BoundaryBaselineEntry be;
    be.robot = "r0";
    be.committed_segment_count = 2;
    doc.baseline = {be};

    ReplanRequestFields out;
    CHECK(ok(ReplanRequestAdapter::from_boundary(doc, out)));
    CHECK(out.reason == "exogenous_block");
    CHECK(out.affected_robots.size() == 2);
    CHECK(out.blocked.size() == 1);
    CHECK(out.blocked[0].from == "B");
    CHECK(out.blocked[0].to == "C");
    CHECK(out.plan_revision == 3);
    CHECK(out.ordering_constraints.size() == 1);
    CHECK(out.ordering_constraints[0].location == "B");
    CHECK(out.ordering_constraints[0].sequence.size() == 1);
    CHECK(out.ordering_constraints[0].sequence[0].robot == "r2");
    CHECK(out.ordering_constraints[0].sequence[0].index == 4);
    CHECK(out.baseline.size() == 1);
    CHECK(out.baseline[0].robot == "r0");
    CHECK(out.baseline[0].committed_segment_count == 2);

    // Empty affected_robots/blocked is legal (§6) — not an error.
    BoundaryReplanRequest empty_doc;
    ReplanRequestFields empty_out;
    CHECK(ok(ReplanRequestAdapter::from_boundary(empty_doc, empty_out)));
    CHECK(empty_out.affected_robots.empty());
    CHECK(empty_out.blocked.empty());

    return mapf_test::summary();
}
