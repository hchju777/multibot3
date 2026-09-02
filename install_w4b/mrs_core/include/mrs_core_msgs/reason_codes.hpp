// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_CORE_MSGS_REASON_CODES_HPP
#define MRS_CORE_MSGS_REASON_CODES_HPP

#include <cstdint>
#include <string>

#include "mrs_core/status.hpp"

/// @file reason_codes.hpp
/// @brief The one string<->uint8 mapping table for every closed vocabulary
///        rendered as `.msg` constants (`_workspace/350_msg_idl_decisions.md`
///        §3-2, §3-4). `mrs_core_msgs` is the sole owner (CN-3) — modules do
///        not keep their own copy of this table (350 §3-4, §9-2 item 2).
///
/// Six vocabularies. `capacity_robots` (V7) and `capacity_rule_id` (V8) are
/// NOT vocabularies (350 §3-2) and have no entries here.
///
/// 🔴 `StopReason` (V1) is shared by two channels with DIFFERENT, NON-
/// contiguous legal subsets (350 D1-c, §3-2). `IsLegalForStopDeclaration` /
/// `IsLegalForReplanRequest` check SET MEMBERSHIP — never validate with a
/// range check (350 §3-2, §4, §8 item 2... actually item covers other things,
/// see §3-2 explicit warning).
///
/// Error classification (CN-13/CN-14): parse failures return
/// `mrs_core::Result<uint8_t, MappingError>` / `Result<std::string,
/// MappingError>` — never throw (this is boundary-crossing code, not the
/// startup path CN-15 reserves exceptions for).
///
/// 🔴 F47-2 (코드 게이트 `22_code_review.md`§6): every `*FromString` below
/// REJECTS the literal `"unknown"` — value 0 (`*_UNKNOWN`, 350 D1-a) has no
/// JSON originator in any of the 15 schemas, and D1-a was adopted precisely
/// *because* 0 cannot come from the wire. If `FromString("unknown")` returned
/// `ok(0)`, this table — the sole owner (350 §3-4) — would manufacture a
/// boundary value that does not exist, undermining its own adoption reason.
/// The corresponding `*ToString(0)` still renders `"unknown"` — diagnostic-only
/// (logs, `result.json`), never meant to round-trip back through `FromString`.

namespace mrs_core_msgs
{

/// @brief Why a reason-code mapping lookup failed.
enum class MappingError
{
    kUnknownString,  ///< The string is not a member of the vocabulary at all.
    kUnknownValue    ///< The uint8 value is not a member of the vocabulary at all.
};

// --- V1: StopReason — mrs_msgs::msg::StopReason (350 §3-2 V1, D1-c). ---
// Shared by mrs.stop_declaration.declarations[].reason and
// mrs.replan_request.reason. 0 is reserved REASON_UNKNOWN (350 D1-a/b).

/// @brief Parse a StopReason string into its uint8 value.
/// @param s One of the seven StopReason strings (350 §3-2 V1).
/// @return ok(value) if @p s is a member of the shared vocabulary, else
///         err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> StopReasonFromString(const std::string& s);

/// @brief Render a StopReason value as its canonical string.
/// @param v A StopReason uint8 value.
/// @return ok(string) if @p v is a member of the shared vocabulary
///         ({0..6}), else err(kUnknownValue).
mrs_core::Result<std::string, MappingError> StopReasonToString(std::uint8_t v);

/// @brief Is @p v in the `mrs.stop_declaration` legal subset {1,3,5}?
/// @param v A StopReason uint8 value (untrusted boundary input).
/// @return true iff @p v is exogenous_block, infeasible_subgoal, or
///         unresolvable_locally. Set membership, NOT a range check (350 §3-2)
///         — the subset is non-contiguous.
bool IsLegalForStopDeclaration(std::uint8_t v);

/// @brief Is @p v in the `mrs.replan_request` legal subset {1..6}?
/// @param v A StopReason uint8 value (untrusted boundary input).
/// @return true iff @p v is any non-zero, in-vocabulary value.
bool IsLegalForReplanRequest(std::uint8_t v);

// --- V2: ReplanOutcome — mrs_msgs::action::Replan_Result (350 §3-2 V2~V6). ---

/// @brief Parse a ReplanOutcome string.
/// @param s One of "planned"/"fallback"/"preempted"/"rejected".
/// @return ok(value) or err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> ReplanOutcomeFromString(const std::string& s);

/// @brief Render a ReplanOutcome value.
/// @param v A ReplanOutcome uint8 value.
/// @return ok(string) or err(kUnknownValue).
mrs_core::Result<std::string, MappingError> ReplanOutcomeToString(std::uint8_t v);

// --- V3: OperatorVerb — mrs_msgs::msg::OperatorCommand (350 §3-2 V2~V6). ---

/// @brief Parse an OperatorVerb string.
/// @param s One of "hold"/"speed_cap"/"goal_override".
/// @return ok(value) or err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> OperatorVerbFromString(const std::string& s);

/// @brief Render an OperatorVerb value.
/// @param v An OperatorVerb uint8 value.
/// @return ok(string) or err(kUnknownValue).
mrs_core::Result<std::string, MappingError> OperatorVerbToString(std::uint8_t v);

// --- V4: DependencyType — mrs_msgs::msg::Dependency (350 §3-2 V2~V6). ---

/// @brief Parse a DependencyType string.
/// @param s One of "sequential"/"ordering".
/// @return ok(value) or err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> DependencyTypeFromString(const std::string& s);

/// @brief Render a DependencyType value.
/// @param v A DependencyType uint8 value.
/// @return ok(string) or err(kUnknownValue).
mrs_core::Result<std::string, MappingError> DependencyTypeToString(std::uint8_t v);

// --- V5: PlanTerminal — mrs_msgs::msg::RobotPlan (350 §3-2 V2~V6). ---

/// @brief Parse a PlanTerminal string.
/// @param s One of "goal_reached"/"plan_truncated"/"no_progress".
/// @return ok(value) or err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> PlanTerminalFromString(const std::string& s);

/// @brief Render a PlanTerminal value.
/// @param v A PlanTerminal uint8 value.
/// @return ok(string) or err(kUnknownValue).
mrs_core::Result<std::string, MappingError> PlanTerminalToString(std::uint8_t v);

// --- V6: EdgeTraversal — mrs_msgs::msg::RoadmapEdge (350 §3-2 V2~V6). ---

/// @brief Parse an EdgeTraversal string.
/// @param s One of "bidirectional"/"forward_only"/"backward_only".
/// @return ok(value) or err(kUnknownString).
mrs_core::Result<std::uint8_t, MappingError> EdgeTraversalFromString(const std::string& s);

/// @brief Render an EdgeTraversal value.
/// @param v An EdgeTraversal uint8 value.
/// @return ok(string) or err(kUnknownValue).
mrs_core::Result<std::string, MappingError> EdgeTraversalToString(std::uint8_t v);

}  // namespace mrs_core_msgs

#endif  // MRS_CORE_MSGS_REASON_CODES_HPP
