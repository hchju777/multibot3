// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/commit_arbiter.hpp"

namespace mrs_sadg::core
{

void CommitArbiter::apply_entry_events(ExecutionGraph& g,
                                       const std::vector<std::string>& entered_ids)
{
    for (const auto& id : entered_ids)
    {
        if (g.has_segment(id))
        {
            g.mark_entered(id);
        }
    }
}

}  // namespace mrs_sadg::core
