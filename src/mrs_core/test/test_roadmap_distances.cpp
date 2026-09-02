// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent).
#include <cassert>
#include <cstdio>

#include "mrs_core/graph/roadmap_distances.hpp"

namespace
{

/// @brief A tiny diamond graph: S -bidir(3m)- A -bidir(4m)- E1, S -bidir(2m)- B
/// -one-way(B->E2, 1m)- E2. Endpoints: E1, E2. Isolated vertex I has no edges.
void TestDiamond()
{
    using mrs_core::graph::RoadmapDistances;
    using mrs_core::graph::WeightedEdge;

    RoadmapDistances table;
    const std::vector<std::string> vertices = {"S", "A", "B", "E1", "E2", "I"};
    const std::vector<WeightedEdge> edges = {
        {"S", "A", 3.0, true},
        {"A", "E1", 4.0, true},
        {"S", "B", 2.0, true},
        {"B", "E2", 1.0, false},  // directed B -> E2 only
    };
    const std::vector<std::string> endpoints = {"E1", "E2"};

    table.Build(vertices, edges, endpoints);
    assert(table.IsBuilt());

    // S -> E1 via A: 3 + 4 = 7 m = 7000 mm.
    auto d_s_e1 = table.DistToEndpoint("S", "E1");
    assert(d_s_e1.has_value());
    assert(*d_s_e1 == 7000);

    // S -> E2 via B: 2 + 1 = 3 m = 3000 mm.
    auto d_s_e2 = table.DistToEndpoint("S", "E2");
    assert(d_s_e2.has_value());
    assert(*d_s_e2 == 3000);

    // Endpoint to itself: 0.
    auto d_e1_e1 = table.DistToEndpoint("E1", "E1");
    assert(d_e1_e1.has_value());
    assert(*d_e1_e1 == 0);

    // B -> E2 exists forward on the directed edge: 1 m = 1000 mm.
    auto d_b_e2 = table.DistToEndpoint("B", "E2");
    assert(d_b_e2.has_value());
    assert(*d_b_e2 == 1000);
    auto d_e2_from_e2 = table.DistToEndpoint("E2", "E2");
    assert(d_e2_from_e2.has_value());
    assert(*d_e2_from_e2 == 0);

    // Isolated vertex I is unreachable to any endpoint.
    assert(!table.DistToEndpoint("I", "E1").has_value());
    assert(!table.DistToEndpoint("I", "E2").has_value());

    // Unknown endpoint (not in the endpoints list) -> nullopt.
    assert(!table.DistToEndpoint("S", "NOT_AN_ENDPOINT").has_value());
}

/// @brief A one-way edge C->D does not let D reach C in reverse — the
/// distance table must not silently treat directed edges as bidirectional.
void TestDirectedEdgeBlocksReverse()
{
    using mrs_core::graph::RoadmapDistances;
    using mrs_core::graph::WeightedEdge;

    RoadmapDistances table;
    // Endpoint is C (the source side of the one-way edge). D has no other
    // edges, so D reaching C would require traversing C->D backwards.
    table.Build({"C", "D"}, {{"C", "D", 2.0, false}}, {"C"});

    // C -> C: 0 (endpoint to itself).
    assert(*table.DistToEndpoint("C", "C") == 0);
    // D -> C: unreachable — the only edge is directed C->D.
    assert(!table.DistToEndpoint("D", "C").has_value());
}

/// @brief `IsBuilt()` is false before the first `Build` call, and queries on
/// an unbuilt table return `std::nullopt` rather than crashing — this is the
/// race `mrta_node`'s node-layer glue depends on (task_release before
/// /roadmap arrives).
void TestUnbuiltIsSafe()
{
    using mrs_core::graph::RoadmapDistances;

    RoadmapDistances table;
    assert(!table.IsBuilt());
    assert(!table.DistToEndpoint("S", "E1").has_value());
}

/// @brief Rebuilding discards the previous table rather than merging into it.
void TestRebuildReplaces()
{
    using mrs_core::graph::RoadmapDistances;
    using mrs_core::graph::WeightedEdge;

    RoadmapDistances table;
    table.Build({"S", "E1"}, {{"S", "E1", 5.0, true}}, {"E1"});
    assert(*table.DistToEndpoint("S", "E1") == 5000);

    // Second build with a disjoint vertex set — "S" no longer exists.
    table.Build({"X", "E2"}, {{"X", "E2", 1.0, true}}, {"E2"});
    assert(!table.DistToEndpoint("S", "E1").has_value());  // E1 no longer an endpoint
    assert(*table.DistToEndpoint("X", "E2") == 1000);
}

}  // namespace

int main()
{
    TestDiamond();
    TestDirectedEdgeBlocksReverse();
    TestUnbuiltIsSafe();
    TestRebuildReplaces();
    std::puts("test_roadmap_distances: OK");
    return 0;
}
