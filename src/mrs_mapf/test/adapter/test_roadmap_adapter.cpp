// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_mapf/adapter/roadmap_adapter.hpp"

using namespace mrs_mapf::core;
using namespace mrs_mapf::adapter;

/// @file test_roadmap_adapter.cpp
/// @brief mrs.roadmap 3.0.0 -> internal Roadmap: `bidirectional` expands to
/// two directed arcs, `forward_only`/`backward_only` to one, and an edge
/// referencing an undeclared node is rejected (Status::kError), matching
/// `check_mapf_solution.py`'s `read_roadmap` semantics this package's search
/// must agree with.

int main()
{
    BoundaryRoadmap doc;
    doc.instance_id = "adapter-test";
    doc.nodes = {{"A", 0.0, 0.0, 1.0}, {"B", 5.0, 0.0, 1.0}, {"C", 10.0, 0.0, 1.0}};
    doc.edges = {
        {"A", "B", "bidirectional", 5.0, 2.0, 1, ""},
        {"B", "C", "forward_only", 5.0, 2.0, 1, ""},
    };
    doc.endpoints = {"A", "C"};

    Roadmap rm;
    CHECK(ok(RoadmapAdapter::from_boundary(doc, rm)));
    CHECK(rm.node_ids.size() == 3);
    CHECK(rm.endpoints.size() == 2);

    // bidirectional A-B => arcs both ways.
    CHECK(rm.out_arcs.at("A").size() == 1);
    CHECK(rm.out_arcs.at("A")[0].to == "B");
    CHECK(rm.out_arcs.at("B").size() == 2);  // one from A-B(bidir), one from B-C(forward).
    bool has_b_to_a = false;
    for (const auto& arc : rm.out_arcs.at("B"))
    {
        if (arc.to == "A")
        {
            has_b_to_a = true;
        }
    }
    CHECK(has_b_to_a);

    // forward_only B-C => arc B->C only, no C->B.
    CHECK(rm.out_arcs.at("C").empty());

    // an edge to an undeclared node is a Status::kError, not a silent drop.
    BoundaryRoadmap bad = doc;
    bad.edges.push_back({"A", "GHOST", "bidirectional", 1.0, 1.0, 1, ""});
    Roadmap rm2;
    CHECK(!ok(RoadmapAdapter::from_boundary(bad, rm2)));

    // an empty node list is a Status::kError.
    BoundaryRoadmap empty_doc;
    Roadmap rm3;
    CHECK(!ok(RoadmapAdapter::from_boundary(empty_doc, rm3)));

    return mapf_test::summary();
}
