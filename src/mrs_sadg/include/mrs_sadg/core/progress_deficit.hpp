// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_PROGRESS_DEFICIT_HPP
#define MRS_SADG_CORE_PROGRESS_DEFICIT_HPP

#include <map>
#include <string>

#include "mrs_sadg/core/execution_graph.hpp"

/// @file progress_deficit.hpp
/// @brief ET20 — stores per-robot progress deficit D_r = now - tau_nom[frontier].
/// 🔴 The clock lives outside core (CN-18): the deficit scalar is computed by the
/// caller and only stored here (time is an analysis variable, never boundary).
/// A hold robot's D_r grows but is absorbed as a normal dependency wait (§0-4) —
/// no special-casing here.

namespace mrs_sadg::core
{

/// @brief Records per-robot progress deficits into the graph.
class ProgressDeficit
{
public:
    /// @brief Store the deficits (no clock access — values are supplied).
    /// @param g [in,out] the graph.
    /// @param deficits robot -> D_r (already computed against a steady clock).
    static void update(ExecutionGraph& g, const std::map<std::string, double>& deficits);
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_PROGRESS_DEFICIT_HPP
