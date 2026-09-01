// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_I_ALTERNATIVE_GENERATOR_HPP
#define MRS_SADG_CORE_I_ALTERNATIVE_GENERATOR_HPP

#include <string_view>
#include <vector>

#include "mrs_sadg/core/extension_types.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file i_alternative_generator.hpp
/// @brief Extension point 2 (337 §4-2): candidate (alternative) generation
/// (안1). 🔴 F37-1 opposing/non-opposing decision lives inside implementations:
/// ［D5］ predicate `segments[to].from == segments[from].to`, opposing pairs are
/// NOT installed as switch alternatives (［D8］). Pure virtual, no pluginlib
/// include (CN-10). Implementations in `plugins/`: AltHeadLift, AltAdjacentSwap.

namespace mrs_sadg::core
{

/// @brief Generates the ordering alternatives for one shared location.
///
/// The generator is given a visit-order view and the segment lookup needed to
/// evaluate the ［D5］ opposing predicate, and fills a caller-owned span (no
/// allocation of the graph — RG-3 mapping is `out(r,i) -> in(s,j) = r#i ->
/// s#(j-1)`).
class IAlternativeGenerator
{
public:
    virtual ~IAlternativeGenerator() = default;

    /// @brief Configure with static parameters (from config/sadg.yaml).
    /// @param cfg generator parameters.
    void configure(const GeneratorConfig& cfg)
    {
        cfg_ = cfg;
    }

    /// @brief What this generator promises.
    /// @return the generator capabilities.
    virtual GeneratorCapabilities capabilities() const = 0;

    /// @brief Generate the ordering slots for one shared location.
    /// @param view the visit-order view of the shared location.
    /// @param segments all compiled segments (to evaluate ［D5］ opposing).
    /// @param out [out] filled with the ordering slots (opposing flag set per ［D5］).
    /// @return the number of slots written, or a CompileError.
    virtual Result<std::uint32_t, CompileError> generate(
        const VisitOrderView& view,
        const std::vector<Segment>& segments,
        std::vector<AlternativeSlot>& out) const = 0;

    /// @brief The pluginlib key "{pkg}/{ClassName}" (CN-12).
    /// @return the plugin key.
    virtual std::string_view key() const = 0;

protected:
    GeneratorConfig cfg_{};  ///< static parameters injected by configure().
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_I_ALTERNATIVE_GENERATOR_HPP
