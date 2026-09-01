// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/extension_types.hpp"

#include <algorithm>

#include "mrs_sadg/core/execution_graph.hpp"

namespace mrs_sadg::core
{

bool ReachabilityView::reachable(const SegmentId& id) const
{
    return std::find(unreachable.begin(), unreachable.end(), id) == unreachable.end();
}

bool DeficitView::any_over_threshold() const
{
    if (graph == nullptr)
    {
        return false;
    }
    for (const auto& r : robots)
    {
        if (graph->deficit(r) > stall_threshold && stall_threshold > 0.0)
        {
            return true;
        }
    }
    return false;
}

}  // namespace mrs_sadg::core
