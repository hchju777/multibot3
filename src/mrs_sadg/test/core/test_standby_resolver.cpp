// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/core/standby_resolver.hpp"

using namespace mrs_sadg::core;

/// @file test_standby_resolver.cpp
/// @brief RE-3 — articulation points of the roadmap graph (read-only predicate).

int main()
{
    // Path A-B-C-D: B and C are cut vertices; A and D are not.
    {
        RoadmapAdjacency adj = {{"A", {"B"}}, {"B", {"A", "C"}}, {"C", {"B", "D"}}, {"D", {"C"}}};
        auto cut = StandbyResolver::articulation_points(adj);
        CHECK(cut.count("B") == 1);
        CHECK(cut.count("C") == 1);
        CHECK(cut.count("A") == 0);
        CHECK(cut.count("D") == 0);
    }

    // Triangle A-B-C: no cut vertex (2-connected).
    {
        RoadmapAdjacency adj = {{"A", {"B", "C"}}, {"B", {"A", "C"}}, {"C", {"A", "B"}}};
        auto cut = StandbyResolver::articulation_points(adj);
        CHECK(cut.empty());
    }

    // Star with centre O: O is the only cut vertex.
    {
        RoadmapAdjacency adj = {{"O", {"A", "B", "C"}}, {"A", {"O"}}, {"B", {"O"}}, {"C", {"O"}}};
        auto cut = StandbyResolver::articulation_points(adj);
        CHECK(cut.count("O") == 1);
        CHECK(cut.size() == 1);
    }

    // RE-3: standby recoverable iff on a cut vertex with count >= D3.
    {
        RoadmapAdjacency adj = {{"A", {"B"}}, {"B", {"A", "C"}}, {"C", {"B"}}};
        CHECK(StandbyResolver::standby_recoverable(adj, "B", 2, 1));   // cut, enough slots.
        CHECK(!StandbyResolver::standby_recoverable(adj, "B", 0, 1));  // cut, too few slots.
        CHECK(!StandbyResolver::standby_recoverable(adj, "A", 5, 1));  // not a cut vertex.
    }

    return sadg_test::summary();
}
