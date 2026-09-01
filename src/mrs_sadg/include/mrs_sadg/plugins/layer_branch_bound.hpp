// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_LAYER_BRANCH_BOUND_HPP
#define MRS_SADG_PLUGINS_LAYER_BRANCH_BOUND_HPP

#include "mrs_sadg/core/i_execution_layer.hpp"

/// @file layer_branch_bound.hpp
/// @brief Canonical order-search layer (RS-1): a railway-style branch-and-bound
/// over switch-group combinations under a deterministic node budget (no clock,
/// RS-2). Greedy initial (anytime fallback), envelope-admits gate (RS-3), strict
/// Phi decrease >= theta_gain to accept a reorder (RS-4), z_prev on timeout /
/// cancel (RS-5). 🔴 select takes the graph as const& (E1): it cannot commit.
/// 🔴 The objective is a deterministic *proxy* for cumulative delay J pending the
/// measured delay model (V-2) — see 20c_impl_sadg.md L-list.

namespace mrs_sadg::plugins
{

/// @brief Bounded branch-and-bound order search.
class LayerBranchBound : public core::IExecutionLayer
{
public:
    /// @brief Search for an improving order under a node budget.
    /// @param g the execution graph (const — E1, cannot commit).
    /// @param rz the reachability view.
    /// @param budget node-expansion deadline (charged per combination).
    /// @return an improving CommitProposal, or a SolveError (caller keeps z_prev).
    core::Result<core::CommitProposal, core::SolveError> select(const core::ExecutionGraph& g,
                                                                const core::ReachabilityView& rz,
                                                                core::NodeBudget& budget) override;

    /// @brief Request cancellation; honoured inside the search loop (RS-5).
    void request_cancel() override
    {
        cancel_ = true;
    }

    /// @brief Capabilities: anytime, deterministic, not global-optimal.
    /// @return the layer capabilities.
    core::LayerCapabilities capabilities() const override;

    /// @brief Plugin key "mrs_sadg/LayerBranchBound" (CN-12).
    /// @return the key.
    std::string_view key() const override;

private:
    bool cancel_ = false;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_LAYER_BRANCH_BOUND_HPP
