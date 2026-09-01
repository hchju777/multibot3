// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_RELEASE_CHECKER_HPP
#define MRS_SADG_CORE_RELEASE_CHECKER_HPP

#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"

/// @file release_checker.hpp
/// @brief ET19 (ET-REL) — computes the ga-bu (release) bits. 🔴 The bit means
/// "arrival-vertex entry permit" (§0-3 ①, F): NOT "departure forbidden". A
/// segment v's arrival vertex may be entered once every committed ordering
/// predecessor (u -> v) has *started* (［D6］ following; entered:true). Opposing
/// (［D7］) predecessors are excluded from the committed set (［D8］) so all
/// committed ordering deps are following. Consumption is onboard (trajopt); SADG
/// only makes the rule and never reads the three distances (298§298-1-2).

namespace mrs_sadg::core
{

/// @brief Recomputes arrival-vertex entry permits from the committed graph.
class ReleaseChecker
{
public:
    /// @brief Set the release bit of each segment (ET19). No clock, no time.
    /// @param g [in,out] the graph.
    /// @return the number of segments whose arrival-vertex entry is permitted.
    static std::size_t release_check(ExecutionGraph& g);
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_RELEASE_CHECKER_HPP
