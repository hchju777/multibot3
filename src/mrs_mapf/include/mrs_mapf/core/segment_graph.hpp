// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_SEGMENT_GRAPH_HPP
#define MRS_MAPF_CORE_SEGMENT_GRAPH_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "mrs_mapf/core/types.hpp"

/// @file segment_graph.hpp
/// @brief The A5 判定 graph — nodes are SEGMENTS `(robot,i) = steps[i]->steps[i+1]`,
/// not visit items `(robot,index)`. This is the exact graph the contract's
/// `x-layer2-note` on `mrs.discrete_plan` 4.0.0 specifies and that
/// `check_mapf_solution.py` §(e) measures — the "퇴거 → 진입"(departure -> entry)
/// mapping, NOT "진입 → 진입"(entry -> entry).
///
/// 🔴 함정 ②(발주서): the deployed checker's own docstring records that an
/// entry->entry graph passes a plan the contract's departure->entry mapping
/// rejects as a length-4 cycle (`274_algo_gate.md` F-1). This module's
/// self-check ⓑ (`self_checks.hpp`) builds ONLY the graph below.

namespace mrs_mapf::core
{

/// @brief A segment node `(robot, i)` — the i-th move of that robot
/// (`steps[i] -> steps[i+1]`).
struct SegmentId
{
    std::string robot;
    std::uint32_t i = 0;

    bool operator==(const SegmentId& o) const
    {
        return robot == o.robot && i == o.i;
    }

    bool operator<(const SegmentId& o) const
    {
        return robot != o.robot ? robot < o.robot : i < o.i;
    }
};

/// @brief The A5 segment graph built from a `DiscretePlanDraft`.
struct SegmentGraph
{
    std::vector<SegmentId> nodes;
    std::map<SegmentId, std::vector<SegmentId>> adjacency;
    std::size_t edge_count = 0;
};

/// @brief Build the A5 segment graph from a draft plan.
///
/// Type-1 edges: `(r,i) -> (r,i+1)` for consecutive segments of the same robot
/// (mandatory — dropping these lets a corridor-both-ends-flipped plan's 4-cycle
/// fail to close; `281`§281-3 実行 confirmation, reproduced in
/// `test_segment_graph_self_checks.cpp`).
///
/// Type-2 edges: for each location's `visit_order` resolved sequence, a
/// consecutive pair `(r,i),(s,j)` with `r != s` yields `(r,i) -> (s,j-1)` — the
/// DEPARTURE segment of the leading item points at the ENTRY segment of the
/// following item (one slot earlier than the following item itself). This is
/// the "퇴거 → 진입" contract mapping. Pairs whose departure/entry segment does
/// not exist (leading item is the robot's terminal step, or following item is
/// index 0) are reported via @p missing_edges rather than silently dropped —
/// this module's self-check is intentionally STRICTER than the deployed
/// checker's silent skip (`255`§255-9-4 finding), because a missing Type-2 edge
/// here signals a malformed `visit_order` this module itself produced.
///
/// @param plan the draft plan (already-fully-assembled: steps + visit_order).
/// @param missing_edges [out] appended with a human-readable note per pair that
///   could not be turned into an edge (contract §2-2 alternatives shape issue).
/// @return the built segment graph.
SegmentGraph build_segment_graph(const DiscretePlanDraft& plan,
                                 std::vector<std::string>& missing_edges);

/// @brief Find one cycle in @p graph, if any (DFS, deterministic node order).
/// @param graph the segment graph.
/// @param cycle_out [out] the cycle as a sequence of segment ids (first == last) if found.
/// @return true iff a cycle was found.
bool find_cycle(const SegmentGraph& graph, std::vector<SegmentId>& cycle_out);

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_SEGMENT_GRAPH_HPP
