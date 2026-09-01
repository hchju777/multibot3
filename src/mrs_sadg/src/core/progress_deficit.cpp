// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/progress_deficit.hpp"

namespace mrs_sadg::core
{

void ProgressDeficit::update(ExecutionGraph& g, const std::map<std::string, double>& deficits)
{
    for (const auto& [robot, d] : deficits)
    {
        g.set_deficit(robot, d);
    }
}

}  // namespace mrs_sadg::core
