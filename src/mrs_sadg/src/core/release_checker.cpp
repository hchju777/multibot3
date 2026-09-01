// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/release_checker.hpp"

namespace mrs_sadg::core
{

std::size_t ReleaseChecker::release_check(ExecutionGraph& g)
{
    std::size_t opened = 0;
    const auto ordering = g.committed_ordering_deps();
    for (const auto& seg : g.segments())
    {
        bool permit = true;
        for (const auto& d : ordering)
        {
            if (d.to != seg.id)
            {
                continue;
            }
            if (g.is_opposing_edge(d.from, d.to))
            {
                // ［D7］ opposing (batch-drain-then-flip): the successor's arrival
                // vertex may be entered only after the predecessor has COMPLETED.
                if (!g.is_completed(d.from))
                {
                    permit = false;
                    break;
                }
            }
            else
            {
                // ［D6］ following: after the predecessor has STARTED (entered:true
                // = departed the shared vertex).
                if (!g.is_entered(d.from))
                {
                    permit = false;
                    break;
                }
            }
        }
        g.set_release(seg.id, permit);
        if (permit)
        {
            ++opened;
        }
    }
    return opened;
}

}  // namespace mrs_sadg::core
