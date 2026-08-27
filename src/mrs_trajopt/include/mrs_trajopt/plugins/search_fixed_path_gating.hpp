// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_PLUGINS_SEARCH_FIXED_PATH_GATING_HPP
#define MRS_TRAJOPT_PLUGINS_SEARCH_FIXED_PATH_GATING_HPP

#include "mrs_trajopt/core/i_trajectory_search.hpp"

/// @file search_fixed_path_gating.hpp
/// @brief AB-SWAP-CTRL cell "gating": fixed-path forward/stop gating (321b §3-1).
///
/// The "search-off" cell. It follows the fixed subgoal path with **no lateral
/// maneuver** (handles_lateral_yield = false): it can only advance or stop, then
/// escalate via `stop_declaration`. This is the SC-14 (ii) phenomenon itself,
/// not an artifact (328 §328-1 정정 2). It receives the *same* subgoal sequence
/// as the search cell (fairness FC-1).

namespace mrs_trajopt::plugins
{

/// @brief The fixed-path gating cell (no lateral yield).
class SearchFixedPathGating : public core::ITrajectorySearch
{
public:
    /// @brief Thread the subgoals along the fixed path (no offset).
    /// @param ctx the search context.
    /// @param out filled with the geometric chain and status.
    /// @return true iff a chain was produced.
    bool solve(const core::SearchContext& ctx, core::SearchOutput& out) override;

    /// @brief Capabilities: warm start, reverse, reachability, NO lateral yield.
    /// @return the capability declaration for this cell.
    core::LayerCapabilities capabilities() const override;
};

}  // namespace mrs_trajopt::plugins

#endif  // MRS_TRAJOPT_PLUGINS_SEARCH_FIXED_PATH_GATING_HPP
