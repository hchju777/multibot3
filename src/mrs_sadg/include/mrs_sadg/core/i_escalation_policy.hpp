// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_I_ESCALATION_POLICY_HPP
#define MRS_SADG_CORE_I_ESCALATION_POLICY_HPP

#include <string_view>

#include "mrs_sadg/core/extension_types.hpp"

/// @file i_escalation_policy.hpp
/// @brief Extension point 3 (337 §4): the escalation ladder (0/1/2/3). `decide`
/// is `const` + views (no state, no clock, no allocation, 12c §A-5-3). The five
/// ladder rules (EP##) are enforced by implementations. Pure virtual, no
/// pluginlib include (CN-10). Implementations: PolicyDeficitLadder,
/// PolicyConstantRung.

namespace mrs_sadg::core
{

/// @brief Decides the next escalation rung from deficit/blocked views.
class IEscalationPolicy
{
public:
    virtual ~IEscalationPolicy() = default;

    /// @brief Decide the next rung. Never publishes; never mutates state.
    /// @param deficit read-only progress-deficit view.
    /// @param blocked read-only blocked-edge-set view.
    /// @param cur the current rung.
    /// @return the next rung (rules: one step up, no downward assignment,
    ///   no escalation while pending, no duplicate situation, <= 3 per episode).
    virtual Rung decide(const DeficitView& deficit, const BlockedView& blocked, Rung cur) = 0;

    /// @brief Begin a new episode (resets duplicate memory + the 3-per-episode counter).
    virtual void reset_episode() = 0;

    /// @brief The pluginlib key "{pkg}/{ClassName}" (CN-12).
    /// @return the plugin key.
    virtual std::string_view key() const = 0;
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_I_ESCALATION_POLICY_HPP
