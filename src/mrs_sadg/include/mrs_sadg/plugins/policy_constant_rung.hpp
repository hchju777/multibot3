// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_POLICY_CONSTANT_RUNG_HPP
#define MRS_SADG_PLUGINS_POLICY_CONSTANT_RUNG_HPP

#include "mrs_sadg/core/i_escalation_policy.hpp"

/// @file policy_constant_rung.hpp
/// @brief Baseline escalation policy: never escalates (always returns kNone). The
/// "no escalation" ablation lower bound — isolates what the ladder buys.

namespace mrs_sadg::plugins
{

/// @brief Constant-rung policy (no escalation ever).
class PolicyConstantRung : public core::IEscalationPolicy
{
public:
    /// @brief Always return rung 0 (no action).
    /// @param deficit unused.
    /// @param blocked unused.
    /// @param cur unused.
    /// @return core::Rung::kNone.
    core::Rung decide(const core::DeficitView& deficit,
                      const core::BlockedView& blocked,
                      core::Rung cur) override;

    /// @brief No-op (no episode state).
    void reset_episode() override
    {
    }

    /// @brief Plugin key "mrs_sadg/PolicyConstantRung" (CN-12).
    /// @return the key.
    std::string_view key() const override;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_POLICY_CONSTANT_RUNG_HPP
