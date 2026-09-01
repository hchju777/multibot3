// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/policy_deficit_ladder.hpp"

namespace mrs_sadg::plugins
{

namespace
{

/// @brief Situation fingerprint from the deficit/blocked views (EP03).
std::uint64_t situation_hash(bool over_threshold, std::size_t blocked_count)
{
    return (static_cast<std::uint64_t>(blocked_count) << 1) | (over_threshold ? 1u : 0u);
}

/// @brief One rung up (never skips), clamped to the global-replan rung.
core::Rung step_up(core::Rung cur)
{
    switch (cur)
    {
        case core::Rung::kNone:
            return core::Rung::kReorder;
        case core::Rung::kReorder:
            return core::Rung::kPartialReplan;
        case core::Rung::kPartialReplan:
            return core::Rung::kGlobalReplan;
        case core::Rung::kGlobalReplan:
            return core::Rung::kGlobalReplan;  // EP06 caps escalation anyway.
    }
    return cur;
}

}  // namespace

core::Rung PolicyDeficitLadder::decide(const core::DeficitView& deficit,
                                       const core::BlockedView& blocked,
                                       core::Rung cur)
{
    const bool over = deficit.any_over_threshold();
    const bool trigger = over || blocked.count > 0;

    // EP01 rule2: no downward assignment — resolution is the trigger vanishing,
    // never an assignment that lowers the rung.
    if (!trigger)
    {
        return cur;
    }

    // EP02 rule3: no escalation while the current rung's action is pending.
    if (pending_)
    {
        return cur;
    }

    // EP04-05 rule4: no duplicate situation (full compare on hash match).
    const std::uint64_t sig = situation_hash(over, blocked.count);
    if (issued_this_episode_.count(sig) != 0)
    {
        return cur;  // same fingerprint already escalated this episode.
    }

    // EP06 rule5: at most 3 escalations per episode.
    if (episode_escalation_count_ >= 3)
    {
        return cur;
    }

    // EP09 rule1: one rung up only.
    const core::Rung rung = step_up(cur);
    issued_this_episode_.insert(sig);
    ++episode_escalation_count_;
    pending_ = true;
    return rung;
}

void PolicyDeficitLadder::reset_episode()
{
    issued_this_episode_.clear();
    episode_escalation_count_ = 0;
    pending_ = false;
}

void PolicyDeficitLadder::mark_resolved()
{
    pending_ = false;
}

std::string_view PolicyDeficitLadder::key() const
{
    return "mrs_sadg/PolicyDeficitLadder";
}

}  // namespace mrs_sadg::plugins
