// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_SELF_CHECKS_HPP
#define MRS_MAPF_CORE_SELF_CHECKS_HPP

#include <string>
#include <vector>

#include "mrs_mapf/core/segment_graph.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file self_checks.hpp
/// @brief The two pre-publish self-checks the contract requires ("생산 측은 발행
/// 전 자기 검사를 같은 관례로 수행해야 한다", `discrete_plan` 4.0.0 `visit_order`
/// description) — 🔴 **함정 ①(발주서): ⓐ와 ⓑ는 별개의 술어**
/// (`255`§255-9 · `309`§309-5 · `313` · `314`§314-4). Implementing only ⓑ misses
/// the "sentinel pile-up" failure mode of a naive safety-stop fallback that
/// parks every robot on a shared placeholder vertex instead of its own current
/// position — ⓑ (a segment-graph acyclicity check) is silent about that because
/// a length-1 plan per robot has zero segments and therefore zero edges to
/// cycle through.
///
/// §255-8-2 decision 3: the self-check returns a status type distinguishing
/// *why* it failed (collision vs cycle vs untested-empty-graph vs malformed
/// Type-2 mapping) rather than a single boolean — folding these into one bit
/// hides exactly the case §255-9-4 found the deployed checker missing.

namespace mrs_mapf::core
{

/// @brief Distinct self-check outcomes (decision 3, §255-8-2).
enum class SelfCheckOutcome
{
    kOk,                    ///< both ⓐ and ⓑ pass, and ⓑ's graph had >= 1 edge (tested).
    kStartVertexCollision,  ///< ⓐ — two different robots share a `steps[0].location`.
    kCycleDetected,         ///< ⓑ — the A5 segment graph (Type-1+Type-2) has a cycle.
    kMalformedType2Edge,    ///< ⓑ — a visit_order pair could not be mapped to a segment edge
                            ///< (stricter than the deployed checker's silent skip — §255-9-4).
    kUntested               ///< ⓑ — the segment graph has zero edges (e.g. every robot's plan
                            ///< has <= 1 step, as in a fallback with no movement). Not a pass:
                            ///< the invariant was never exercised (03_formulation.md §2-9 rule,
                            ///< the same rule `check_mapf_solution.py` applies).
};

/// @brief Detail attached to a `SelfCheckOutcome`.
struct SelfCheckReport
{
    SelfCheckOutcome outcome = SelfCheckOutcome::kOk;
    std::vector<std::string> collisions;       ///< ⓐ: locations shared by >=2 robots' steps[0].
    std::vector<SegmentId> cycle;              ///< ⓑ: the cycle, if kCycleDetected.
    std::vector<std::string> malformed_edges;  ///< ⓑ: messages, if kMalformedType2Edge.
};

/// @brief ⓐ — start-vertex uniqueness: no two different robots' `steps[0].location`
/// coincide. Robots with an empty `steps` are skipped (already reported elsewhere).
/// @param plan the draft plan.
/// @return the set of locations (if any) shared by >= 2 robots.
std::vector<std::string> check_start_vertex_uniqueness(const DiscretePlanDraft& plan);

/// @brief ⓑ — A5 acyclicity over the segment graph (Type-1 + "퇴거->진입" Type-2).
/// @param plan the draft plan.
/// @param report [out] filled with the cycle or malformed-edge detail on failure.
/// @return true iff the graph is acyclic AND has >= 1 tested edge.
bool check_segment_graph_acyclic(const DiscretePlanDraft& plan, SelfCheckReport& report);

/// @brief Runs ⓐ then ⓑ, in that order (ⓐ first — a start-vertex collision is
/// cheaper to explain and does not require building the segment graph).
/// @param plan the draft plan.
/// @param report [out] filled with the outcome and its detail.
/// @return the outcome (also stored in @p report.outcome).
SelfCheckOutcome run_self_checks(const DiscretePlanDraft& plan, SelfCheckReport& report);

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_SELF_CHECKS_HPP
