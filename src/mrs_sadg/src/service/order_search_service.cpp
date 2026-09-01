// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/service/order_search_service.hpp"

namespace mrs_sadg::service
{

core::Result<core::CommitProposal, core::SolveError> OrderSearchService::run(
    const ReorderRequest& req)
{
    if (req.graph == nullptr)
    {
        return core::Result<core::CommitProposal, core::SolveError>::err(
            core::SolveError::kOutsideEnvelope);
    }
    // OS00: the graph is const& — the layer cannot commit (E1, compile-time).
    core::NodeBudget budget = req.budget;
    return layer_.select(*req.graph, req.rz, budget);
}

}  // namespace mrs_sadg::service
