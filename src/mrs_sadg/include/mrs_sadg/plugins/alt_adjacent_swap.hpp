// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_PLUGINS_ALT_ADJACENT_SWAP_HPP
#define MRS_SADG_PLUGINS_ALT_ADJACENT_SWAP_HPP

#include "mrs_sadg/core/i_alternative_generator.hpp"

/// @file alt_adjacent_swap.hpp
/// @brief Legacy alternative generator (12c AltAdjacentSwap). 🔴 Kept, not
/// deleted (arch §2 — ablation / rollback). It produces the same contract-mapped
/// committed ordering edges as AltHeadLift (both must match the checker's seam
/// (c) consecutive-pair mapping); the two differ in declared capabilities and in
/// the *alternative enumeration* used by the reorder search (deferred — see
/// 20c_impl_sadg.md L-list).

namespace mrs_sadg::plugins
{

/// @brief Adjacent-swap alternative generator (`heads_complete = false`).
class AltAdjacentSwap : public core::IAlternativeGenerator
{
public:
    /// @brief Capabilities: not head-complete, preserves intra-robot order.
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

    /// @brief Plugin key "mrs_sadg/AltAdjacentSwap" (CN-12).
    /// @return the key.
    std::string_view key() const override;
};

}  // namespace mrs_sadg::plugins

#endif  // MRS_SADG_PLUGINS_ALT_ADJACENT_SWAP_HPP
