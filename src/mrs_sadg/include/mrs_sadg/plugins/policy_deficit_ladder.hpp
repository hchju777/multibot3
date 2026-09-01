// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_POLICY_DEFICIT_LADDER_HPP
#define MRS_SADG_PLUGINS_POLICY_DEFICIT_LADDER_HPP

#include <cstdint>
#include <set>

#include "mrs_sadg/core/i_escalation_policy.hpp"

/// @file policy_deficit_ladder.hpp
/// @brief Canonical escalation policy (EP##). 🔴 The five ladder rules each on a
/// line: EP09 one step up · EP01 no downward assignment · EP02 no escalation
/// while pending · EP04-05 no duplicate situation (full compare on hash match) ·
/// EP06 <= 3 per episode. `decide` is const-in-spirit: it only reads views and
/// mutates its own episode memory (not the graph).

namespace mrs_sadg::plugins
{

/// @brief Deficit/blocked-driven escalation ladder.
class PolicyDeficitLadder : public core::IEscalationPolicy
{
public:
    /// @brief Decide the next rung under the five ladder rules.
    /// @param deficit read-only progress-deficit view.
    /// @param blocked read-only blocked-edge-set view.
    /// @param cur the current rung.
    /// @return the next rung (never more than one step up per call).
    core::Rung decide(const core::DeficitView& deficit,
                      const core::BlockedView& blocked,
                      core::Rung cur) override;

    /// @brief Begin a new episode (clears duplicate memory + the 3-count).
    void reset_episode() override;

    /// @brief Mark the pending escalation resolved (its action completed).
    void mark_resolved();

    /// @brief Plugin key "mrs_sadg/PolicyDeficitLadder" (CN-12).
    /// @return the key.
    std::string_view key() const override;

private:
    std::set<std::uint64_t> issued_this_episode_;
    int episode_escalation_count_ = 0;
    bool pending_ = false;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_POLICY_DEFICIT_LADDER_HPP
