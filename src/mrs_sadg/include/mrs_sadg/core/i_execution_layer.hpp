// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_I_EXECUTION_LAYER_HPP
#define MRS_SADG_CORE_I_EXECUTION_LAYER_HPP

#include <string_view>

#include "mrs_sadg/core/extension_types.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file i_execution_layer.hpp
/// @brief Extension point 1 (337 §4-1): the order-search layer (안3), the
/// `method.modules.sadg` ablation axis. Pure virtual, no pluginlib include
/// (CN-10). Implementations live in `plugins/` (CN-5): LayerBranchBound (정본),
/// LayerFixedOrder (ablation lower bound).

namespace mrs_sadg::core
{

class ExecutionGraph;

/// @brief Chooses a switch-group combination (order z) for the tick to accept.
///
/// 🔴 E1 enforced by type: `select` takes the graph as `const&` and returns a
/// `CommitProposal`, so no implementation can commit — only
/// `ExecutionTickService` applies (337 §4-1).
class IExecutionLayer
{
public:
    virtual ~IExecutionLayer() = default;

    /// @brief Configure with static parameters and the alternative generator.
    /// @param cfg static layer parameters (from config/sadg.yaml).
    void configure(const LayerConfig& cfg)
    {
        cfg_ = cfg;
    }

    /// @brief Search for an improving order under a deterministic node budget.
    /// @param g the execution graph (const — cannot be committed to, E1).
    /// @param rz reachability view (Rz + rz_seq).
    /// @param budget node-expansion deadline (no wall clock, RS-2).
    /// @return an improving CommitProposal, or a SolveError (RS-5 -> caller keeps z_prev).
    virtual Result<CommitProposal, SolveError> select(const ExecutionGraph& g,
                                                      const ReachabilityView& rz,
                                                      NodeBudget& budget) = 0;

    /// @brief Request cancellation; checked inside the search loop (RS-5).
    virtual void request_cancel() = 0;

    /// @brief What this layer promises about its search.
    /// @return the layer capabilities.
    virtual LayerCapabilities capabilities() const = 0;

    /// @brief The pluginlib key "{pkg}/{ClassName}" (CN-12).
    /// @return the plugin key.
    virtual std::string_view key() const = 0;

protected:
    LayerConfig cfg_{};  ///< static parameters injected by configure().
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_I_EXECUTION_LAYER_HPP
