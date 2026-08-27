// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_THREE_CLOCK_GATE_HPP
#define MRS_TRAJOPT_CORE_THREE_CLOCK_GATE_HPP

#include <cstdint>

/// @file three_clock_gate.hpp
/// @brief The three nested clocks (U42-2, RQ5-2): control ⊃ trajectory ⊃ subgoal.
///
/// One timer, two integer-multiple gates (321b §1-1, K21). Values of the two
/// periods are **[값 부재]** — the spike measures candidates; this class only
/// enforces the structural invariant. Key names follow orchestrator disposition
/// ⑴ and the freeze: `traj_replan_period_ticks` (n^traj) and
/// `subgoal_replan_period_trajcycles` (n^re). 🔴 The old key
/// `subgoal_replan_period_ticks` is FORBIDDEN (고정 결정 1) — it is not defined
/// here nor accepted anywhere.

namespace mrs_trajopt::core
{

/// @brief Deterministic gate deciding which of the three ticks fire at a seq.
///
/// The subgoal tick is a subset of the trajectory tick, which is a subset of
/// the control tick (nesting invariant, 321b §1-1). When all three predicates
/// are true simultaneously the tick is the **worst-case tick** — the budget
/// owner (§4).
class ThreeClockGate
{
public:
    /// @brief Construct from the two periods.
    /// @param traj_replan_period_ticks n^traj: trajectory recompute period, in
    ///        control ticks. Must be >= 1 (SU01), else `valid()` is false.
    /// @param subgoal_replan_period_trajcycles n^re: subgoal period, in
    ///        trajectory cycles. Must be >= 1 (SU02), else `valid()` is false.
    ThreeClockGate(std::int64_t traj_replan_period_ticks,
                   std::int64_t subgoal_replan_period_trajcycles)
        : n_traj_(traj_replan_period_ticks), n_re_(subgoal_replan_period_trajcycles)
    {
    }

    /// @brief Are the periods valid (both >= 1)?
    /// @return true iff n^traj >= 1 and n^re >= 1. A false here is a startup
    ///         refusal (SU01/SU02): a zero period makes `tick_seq % 0` UB.
    bool valid() const
    {
        return n_traj_ >= 1 && n_re_ >= 1;
    }

    /// @brief Trajectory recompute period n^traj (in control ticks).
    /// @return n^traj.
    std::int64_t traj_period_ticks() const
    {
        return n_traj_;
    }

    /// @brief Subgoal period n^re (in trajectory cycles).
    /// @return n^re.
    std::int64_t subgoal_period_trajcycles() const
    {
        return n_re_;
    }

    /// @brief Subgoal firing period in control ticks (= n^traj * n^re).
    /// @return the control-tick period at which the subgoal tick fires.
    std::int64_t subgoal_period_ticks() const
    {
        return n_traj_ * n_re_;
    }

    /// @brief Does the trajectory tick fire at @p tick_seq? (CT22)
    /// @param tick_seq the current control-tick sequence number (>= 1).
    /// @return true iff tick_seq % n^traj == 0. False when !valid().
    bool is_trajectory_tick(std::int64_t tick_seq) const
    {
        return valid() && (tick_seq % n_traj_) == 0;
    }

    /// @brief Does the subgoal+publish tick fire at @p tick_seq? (TT14)
    /// @param tick_seq the current control-tick sequence number (>= 1).
    /// @return true iff tick_seq % (n^traj * n^re) == 0. False when !valid().
    bool is_subgoal_tick(std::int64_t tick_seq) const
    {
        return valid() && (tick_seq % subgoal_period_ticks()) == 0;
    }

private:
    std::int64_t n_traj_;
    std::int64_t n_re_;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_THREE_CLOCK_GATE_HPP
