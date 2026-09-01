// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_LAYER_FIXED_ORDER_HPP
#define MRS_SADG_PLUGINS_LAYER_FIXED_ORDER_HPP

#include "mrs_sadg/core/i_execution_layer.hpp"

/// @file layer_fixed_order.hpp
/// @brief Baseline order-search layer: `select` always returns the original order
/// (the constant function). 🔴 The "SADG's reason to exist" ablation — swap this
/// in via one line of `method.modules.sadg` and every reorder is disabled.

namespace mrs_sadg::plugins
{

/// @brief Constant-order layer (chooses alternative 0 of every group).
class LayerFixedOrder : public core::IExecutionLayer
{
public:
    /// @brief Return the original order (combo of all zeros), never improving.
    /// @param g the execution graph (const).
    /// @param rz the reachability view.
    /// @param budget the node budget (untouched — no search).
    /// @return a CommitProposal equal to the current order.
    core::Result<core::CommitProposal, core::SolveError> select(const core::ExecutionGraph& g,
                                                                const core::ReachabilityView& rz,
                                                                core::NodeBudget& budget) override;

    /// @brief No-op (no search loop to cancel).
    void request_cancel() override
    {
    }

    /// @brief Capabilities: anytime, deterministic, not global-optimal.
    /// @return the layer capabilities.
    core::LayerCapabilities capabilities() const override;

    /// @brief Plugin key "mrs_sadg/LayerFixedOrder" (CN-12).
    /// @return the key.
    std::string_view key() const override;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_LAYER_FIXED_ORDER_HPP
