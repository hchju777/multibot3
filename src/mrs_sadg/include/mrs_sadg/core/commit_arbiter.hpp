// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_COMMIT_ARBITER_HPP
#define MRS_SADG_CORE_COMMIT_ARBITER_HPP

#include <string>
#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"

/// @file commit_arbiter.hpp
/// @brief Applies entry events (ET03). 🔴 entered:true = segment start = commit
/// (§0-3 ③ / §9-2). A committed traversal order cannot be reversed; on observation
/// delay the safe side is "already entered" (RECERT-LOCK conservative, §6-2).

namespace mrs_sadg::core
{

/// @brief Commits entered segments into the graph (E1 commit point, with ET12).
class CommitArbiter
{
public:
    /// @brief Mark each observed entered segment as committed.
    /// @param g [in,out] the graph.
    /// @param entered_ids the segment ids observed as entered:true this tick.
    static void apply_entry_events(ExecutionGraph& g, const std::vector<std::string>& entered_ids);
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_COMMIT_ARBITER_HPP
