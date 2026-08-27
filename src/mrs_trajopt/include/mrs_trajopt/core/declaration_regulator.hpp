// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_DECLARATION_REGULATOR_HPP
#define MRS_TRAJOPT_CORE_DECLARATION_REGULATOR_HPP

#include <cstdint>
#include <string>

/// @file declaration_regulator.hpp
/// @brief The upward channel: `mrs.stop_declaration` 5.0.0 only (고정 결정 4).
///
/// This is the ONLY upward channel — `replan_request` is not emitted by this
/// module (CT25 철회). The reason is chosen by the schema's 3-way total-order
/// discriminant (stop_declaration.schema.json :54, Q1/Q2/Q3). 🔴 The
/// reverse=false-infeasibility path is `infeasible_subgoal` (관문 324 확정,
/// 고정 결정 2) — NOT `unresolvable_locally` (that 321b draft value was
/// disproved). This header is the SC-16 discriminant/counter piece.

namespace mrs_trajopt::core
{

/// @brief The legal reason vocabulary (subset 3 of stop_declaration 5.0.0).
///
/// Exactly the enum of stop_declaration.schema.json :52 — no more, no less.
enum class StopReason
{
    kExogenousBlock,      ///< Q1: edge is impassable independent of robot identity.
    kInfeasibleSubgoal,   ///< Q2: remains after removing all others' / upstream decisions.
    kUnresolvableLocally  ///< Q3: everything else.
};

/// @brief Local observations feeding the 3-way discriminant.
struct DeclarationEvidence
{
    /// Q1 witness: the blocking edge is impassable for ANY robot (world fact),
    /// not merely this one. True => exogenous_block.
    bool edge_impassable_for_any_robot = false;
    /// Q2 witness: even with all peers' and upstream (V^blk) decisions removed,
    /// this robot still cannot proceed by its own physics. The reverse=false
    /// case sets this: "the wall is passable by another chassis, so Q2 is true"
    /// (schema :54). True (and !Q1) => infeasible_subgoal.
    bool remains_after_removing_others = false;
};

/// @brief Classifies a stop declaration's reason by the schema 3-way order.
///
/// The order is total (Q1 before Q2 before Q3), so classification is a chain
/// of the two witnesses. No absolute time, no cost, no preference — this is a
/// classification of the *opening*, not a policy (schema :54).
class DeclarationRegulator
{
public:
    /// @brief Classify by the 3-way total order (Q1 -> Q2 -> Q3).
    /// @param ev the two local witnesses.
    /// @return exogenous_block if Q1; else infeasible_subgoal if Q2; else
    ///         unresolvable_locally.
    static StopReason classify(const DeclarationEvidence& ev)
    {
        if (ev.edge_impassable_for_any_robot)
        {
            return StopReason::kExogenousBlock;  // Q1
        }
        if (ev.remains_after_removing_others)
        {
            return StopReason::kInfeasibleSubgoal;  // Q2
        }
        return StopReason::kUnresolvableLocally;  // Q3
    }

    /// @brief The reason for a reverse=false robot whose subgoal is unrealizable.
    ///
    /// 🔴 고정 결정 2 / 관문 324 발견 #1: a robot that cannot reverse and has
    /// no forward-realizable trajectory declares `infeasible_subgoal` — Q2 is
    /// true because the obstacle is passable by a different chassis (schema :54).
    /// TT13 calls this. `unresolvable_locally` would make SADG attempt a futile
    /// local reorder, so it is wrong here.
    /// @return StopReason::kInfeasibleSubgoal.
    static StopReason reverse_forbidden_infeasible()
    {
        return StopReason::kInfeasibleSubgoal;
    }

    /// @brief Canonical schema string for a reason (adapter / IO consume this).
    /// @param r the reason.
    /// @return the exact enum string of stop_declaration.schema.json :52.
    static std::string to_schema_string(StopReason r)
    {
        switch (r)
        {
            case StopReason::kExogenousBlock:
                return "exogenous_block";
            case StopReason::kInfeasibleSubgoal:
                return "infeasible_subgoal";
            case StopReason::kUnresolvableLocally:
                return "unresolvable_locally";
        }
        return "unresolvable_locally";
    }
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_DECLARATION_REGULATOR_HPP
