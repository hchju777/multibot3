// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_ALT_HEAD_LIFT_HPP
#define MRS_SADG_PLUGINS_ALT_HEAD_LIFT_HPP

#include "mrs_sadg/core/i_alternative_generator.hpp"

/// @file alt_head_lift.hpp
/// @brief Canonical alternative generator (안1). Emits the committed ordering
/// edges of a shared location by the contract mapping 퇴거->진입 (consecutive
/// pairs, matching `check_sadg_acyclic.py`'s seam (c)), flagging opposing pairs
/// per ［D5］ so the compiler excludes them (［D8］).

namespace mrs_sadg::plugins
{

/// @brief Head-complete alternative generator (`heads_complete = true`).
class AltHeadLift : public core::IAlternativeGenerator
{
public:
    /// @brief Capabilities: head-complete, preserves intra-robot order.
    /// @return the generator capabilities.
    core::GeneratorCapabilities capabilities() const override;

    /// @brief Generate the committed ordering slots for one shared location.
    /// @param view the visit-order view of the shared location.
    /// @param segments all compiled segments (for the ［D5］ opposing lookup).
    /// @param out [out] the ordering slots (opposing flag set per ［D5］).
    /// @return the number of slots written, or a CompileError.
    core::Result<std::uint32_t, core::CompileError> generate(
        const core::VisitOrderView& view,
        const std::vector<core::Segment>& segments,
        std::vector<core::AlternativeSlot>& out) const override;

    /// @brief Plugin key "mrs_sadg/AltHeadLift" (CN-12).
    /// @return the key.
    std::string_view key() const override;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_ALT_HEAD_LIFT_HPP
