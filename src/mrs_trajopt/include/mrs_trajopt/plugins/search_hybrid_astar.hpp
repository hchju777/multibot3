// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_PLUGINS_SEARCH_HYBRID_ASTAR_HPP
#define MRS_TRAJOPT_PLUGINS_SEARCH_HYBRID_ASTAR_HPP

#include "mrs_trajopt/core/i_trajectory_search.hpp"

/// @file search_hybrid_astar.hpp
/// @brief AB-SWAP-CTRL cell "search": free-space dynamic search (321b §3-1).
///
/// 🔴 SIMPLIFIED. A full SSIPP + Reeds-Shepp Hybrid A* is out of scope for this
/// round (see 20d "알려진 한계"). This implementation threads the subgoal pass
/// windows and, when a tube is present, may apply a bounded lateral offset
/// **inside** the tube half-width to realize an assigned yield quota — the
/// capability the gating cell lacks (handles_lateral_yield = true). It stays
/// inside the tube (hard constraint, TT01) and never reorders subgoals.

namespace mrs_trajopt::plugins
{

/// @brief The free-space dynamic-search cell (handles lateral yield).
class SearchHybridAstar : public core::ITrajectorySearch
{
public:
    /// @brief Construct with a lateral yield quota to realize [m].
    /// @param yield_quota_m lateral offset budget inside the tube [m] (>= 0).
    explicit SearchHybridAstar(double yield_quota_m = 0.0)
        : yield_quota_m_(yield_quota_m < 0.0 ? 0.0 : yield_quota_m)
    {
    }

    /// @brief Thread the subgoals, optionally applying a tube-internal offset.
    /// @param ctx the search context.
    /// @param out filled with the geometric chain and status.
    /// @return true iff a chain was produced.
    bool solve(const core::SearchContext& ctx, core::SearchOutput& out) override;

    /// @brief Capabilities: warm start, reverse, reachability, lateral yield.
    /// @return the capability declaration for this cell.
    core::LayerCapabilities capabilities() const override;

private:
    double yield_quota_m_;
};

}  // namespace mrs_trajopt::plugins

#endif  // MRS_TRAJOPT_PLUGINS_SEARCH_HYBRID_ASTAR_HPP
