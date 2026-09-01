// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/layer_fixed_order.hpp"

#include "mrs_sadg/core/execution_graph.hpp"

namespace mrs_sadg::plugins
{

core::Result<core::CommitProposal, core::SolveError> LayerFixedOrder::select(
    const core::ExecutionGraph& g,
    const core::ReachabilityView& rz,
    core::NodeBudget& budget)
{
    (void)budget;  // no search performed.
    core::CommitProposal p;
    p.combo.assign(g.switch_groups().size(), 0);  // original order.
    p.rz_seq = rz.rz_seq;
    p.predicted_delay = 0.0;
    return core::Result<core::CommitProposal, core::SolveError>::ok_value(p);
}

core::LayerCapabilities LayerFixedOrder::capabilities() const
{
    core::LayerCapabilities c;
    c.anytime = true;
    c.deterministic = true;
    c.global_optimal = false;
    return c;
}

std::string_view LayerFixedOrder::key() const
{
    return "mrs_sadg/LayerFixedOrder";
}

}  // namespace mrs_sadg::plugins
