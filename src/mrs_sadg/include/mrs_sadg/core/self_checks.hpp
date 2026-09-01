// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_SELF_CHECKS_HPP
#define MRS_SADG_CORE_SELF_CHECKS_HPP

#include <string>
#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/types.hpp"

/// @file self_checks.hpp
/// @brief IN14 production-side seam self-check — the *same* convention as
/// `check_sadg_acyclic.py`'s seam check (a)~(d). 🔴 mandatory because a wrong
/// contract mapping is acyclic and therefore passes the checker's exit 0 when
/// `--discrete-plan` is not supplied (296§296-4-2, `_workspace/artifacts/f37_2/`
/// measured). This recomputes the expected sequential + ordering edges from the
/// plan by the contract mapping 퇴거->진입 and compares them to what the graph
/// actually installed.

namespace mrs_sadg::core
{

/// @brief Detail for a seam self-check.
struct SeamReport
{
    std::vector<std::string> messages;  ///< each mismatch, empty on success.
};

/// @brief ［D5］ opposing predicate on an ordering arc from -> to.
/// @param from predecessor segment (departs the shared vertex).
/// @param to successor segment (arrives the shared vertex).
/// @return true iff segments[to].from == segments[from].to (they cross the same
///   edge in opposite directions).
bool is_opposing(const Segment& from, const Segment& to);

/// @brief Recompute the expected committed ordering edges from visit_order.
/// @param plan the plan view.
/// @param graph the compiled graph (for the ［D5］ opposing lookup).
/// @return the expected non-opposing ordering edges (from-id, to-id).
std::vector<std::pair<SegmentId, SegmentId>> expected_ordering_edges(const PlanView& plan,
                                                                     const ExecutionGraph& graph);

/// @brief IN14 seam self-check: segments (a), sequential chain (b), ordering
/// mapping (c), H-cons (d).
/// @param plan the plan view.
/// @param graph the compiled graph.
/// @param report [out] filled with mismatch detail on failure.
/// @return true iff the graph is the contract-mapped graph of @p plan.
bool seam_selfcheck(const PlanView& plan, const ExecutionGraph& graph, SeamReport& report);

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_SELF_CHECKS_HPP
