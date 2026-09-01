// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/service/override_latch.hpp"

#include "mrs_mrta/core/status.hpp"

namespace mrs_mrta::service
{

OverrideLatch::OverrideLatch(std::vector<std::string> robot_names) : latches_(robot_names.size())
{
}

bool OverrideLatch::apply(const core::OperatorEvent& ev, core::RobotIndex r)
{
    OverrideState& latch = latches_.at(r);
    if (latch.has_state && ev.seq <= latch.seq)
    {
        return false;  // stale or duplicate — highest-seq-wins, ignored.
    }

    const bool was_active = latch.has_state && latch.active;
    const bool changed =
        (was_active != ev.active) ||
        (ev.active && (!latch.has_state || latch.goal_location != ev.goal_location));

    latch.seq = ev.seq;
    latch.active = ev.active;
    latch.goal_location = ev.active ? ev.goal_location : std::string{};
    latch.has_state = true;

    return changed;
}

bool OverrideLatch::is_active(core::RobotIndex r) const
{
    return latches_.at(r).active;
}

const std::string& OverrideLatch::goal_location(core::RobotIndex r) const
{
    return latches_.at(r).goal_location;
}

}  // namespace mrs_mrta::service
