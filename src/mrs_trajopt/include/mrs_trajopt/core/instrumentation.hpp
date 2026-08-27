// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_INSTRUMENTATION_HPP
#define MRS_TRAJOPT_CORE_INSTRUMENTATION_HPP

#include <cstdint>
#include <string>

/// @file instrumentation.hpp
/// @brief The CN-20 instrumentation keys this module produces (§322-4).
///
/// 🔴 These are **result.json metrics, not boundary fields** — measured
/// quantities, not inter-robot messages (§R5-E:146). The producer of each key
/// is this module (CN-20 uniqueness). Includes the six fairness observables
/// defined by 328 (§328-6): they are instrumentation, kept out of the boundary.

namespace mrs_trajopt::core
{

/// @brief The six fairness observables of 328 (cell A/B swap fairness).
///
/// 🔴 328 §328-6: all six are `result.json` instrumentation (경계 아님). Names
/// are taken from 328 §328-1..§328-3 verbatim.
struct FairnessMetrics
{
    /// 🆕 tube radius used [m] (robot/scenario). Must be equal across both cells
    /// for a fair run (FC-2); differing => the run is comparison-invalid.
    double tube_radius_m = 0.0;
    /// 🆕 hash of the subgoal sequence received. Must be equal across both cells
    /// (FC-1); differing => "상류 고정" violated.
    std::uint64_t subgoal_sequence_hash = 0;
    /// 🆕 count of times realization changed the subgoal visit order. Must be 0
    /// for both cells to be fair (FC-3): != 0 => upstream-fix violated.
    std::int64_t subgoal_order_deviation = 0;
    /// 🆕 number of ticks the trajectory left the published tube. Must be 0 for
    /// the search cell to be fair (FC-3): > 0 => tube containment violated.
    std::int64_t tube_exit_ticks = 0;
    /// 🆕 count of encounters the search cell resolved by a tube-internal
    /// lateral maneuver. The gating cell is 0 by definition.
    std::int64_t lateral_yield_realized = 0;
    /// 🆕 number of escalations (stop -> SADG) this cell made. The gating cell
    /// escalating more often is the *measurement target* (fair, SC-14 (ii)).
    std::int64_t n_escalation_by_cell = 0;
};

/// @brief Sink for latency and count instrumentation (CN-18/CN-19 seam).
///
/// The core takes this by reference so it never calls a clock or allocates in
/// the tick path directly. The concrete sink lives outside the pure core.
class IInstrSink
{
public:
    virtual ~IInstrSink() = default;

    /// @brief Record a duration sample under @p key [milliseconds].
    /// @param key metric key (e.g. "traj_search_ms").
    /// @param ms the measured duration in milliseconds.
    virtual void observe_ms(const std::string& key, double ms) = 0;

    /// @brief Increment a counter under @p key by one.
    /// @param key counter key (e.g. "n_commit_withdrawn").
    virtual void count(const std::string& key) = 0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_INSTRUMENTATION_HPP
