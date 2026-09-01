// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_SERVICE_ORDER_SEARCH_SERVICE_HPP
#define MRS_SADG_SERVICE_ORDER_SEARCH_SERVICE_HPP

#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/extension_types.hpp"
#include "mrs_sadg/core/i_execution_layer.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file order_search_service.hpp
/// @brief OS## — the order-search use case (안3). Runs asynchronously in the
/// separate `sadg_t1` process, off the tick budget (T-DIV). 🔴 It receives the
/// graph as `const&` and returns a CommitProposal; it never commits (E1). On
/// failure / timeout it surfaces a SolveError so the caller keeps z_prev (RS-5) —
/// a performance loss, not a safety violation (T-DIV).

namespace mrs_sadg::service
{

/// @brief One reorder request (a graph snapshot + reachability + node budget).
struct ReorderRequest
{
    const core::ExecutionGraph* graph = nullptr;  ///< const snapshot (cannot commit).
    core::ReachabilityView rz;                    ///< Rz + rz_seq.
    core::NodeBudget budget;                      ///< deterministic node-count deadline.
};

/// @brief Wraps an IExecutionLayer as an asynchronous reorder use case.
class OrderSearchService
{
public:
    /// @brief Construct with the (swappable) order-search layer.
    /// @param layer the execution layer (extension point 1).
    explicit OrderSearchService(core::IExecutionLayer& layer) : layer_(layer)
    {
    }

    /// @brief Run one reorder search (OS00-OS18).
    /// @param req the reorder request (graph snapshot, rz, budget).
    /// @return an improving CommitProposal, or a SolveError (caller keeps z_prev).
    core::Result<core::CommitProposal, core::SolveError> run(const ReorderRequest& req);

    /// @brief Request cancellation of the in-flight search (RS-5).
    void request_cancel()
    {
        layer_.request_cancel();
    }

private:
    core::IExecutionLayer& layer_;
};

}  // namespace mrs_sadg::service

#endif  // MRS_SADG_SERVICE_ORDER_SEARCH_SERVICE_HPP
