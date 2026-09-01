// SPDX-License-Identifier: Apache-2.0
#include "mrs_core_msgs/reason_codes.hpp"

#include "mrs_msgs/action/replan.hpp"
#include "mrs_msgs/msg/dependency.hpp"
#include "mrs_msgs/msg/operator_command.hpp"
#include "mrs_msgs/msg/roadmap_edge.hpp"
#include "mrs_msgs/msg/robot_plan.hpp"
#include "mrs_msgs/msg/stop_reason.hpp"

namespace mrs_core_msgs
{

namespace
{
using Result8 = mrs_core::Result<std::uint8_t, MappingError>;
using ResultStr = mrs_core::Result<std::string, MappingError>;

/// @brief Shared table-lookup helper: string -> uint8 over a fixed array of
///        (value, name) pairs.
///
/// 🔴 F47-2 (코드 게이트 22§6): value 0 (`*_UNKNOWN`, 350 D1-a) is skipped here
/// on purpose. `350`§3-1 adopted the 0-reservation *because* "0 has no JSON
/// originator" — none of the 15 schemas ever spells the literal "unknown".
/// If this lookup matched it, `StopReasonFromString("unknown")` would return
/// `ok(0)`, and the sole owner of this table (`mrs_core_msgs`) would manufacture
/// a JSON originator that does not exist on the boundary — undermining the very
/// reason D1-a was adopted. The reverse direction (`LookupString`, value ->
/// string) still renders 0 as "unknown" for diagnostics/logging only; that
/// string is never meant to appear in — or be parsed back from — the wire.
template<std::size_t N>
Result8 LookupValue(const std::string& s, const std::pair<std::uint8_t, const char*> (&table)[N])
{
    for (const auto& entry : table)
    {
        if (entry.first == 0)
        {
            continue;  // *_UNKNOWN has no JSON originator (350 D1-a) — never match it.
        }
        if (s == entry.second)
        {
            return Result8::ok_value(entry.first);
        }
    }
    return Result8::err(MappingError::kUnknownString);
}

/// @brief Shared table-lookup helper: uint8 -> string over the same table.
/// Unlike `LookupValue`, value 0 (`*_UNKNOWN`) IS rendered here — as a
/// diagnostic-only string never expected to round-trip (see `LookupValue` doc).
template<std::size_t N>
ResultStr LookupString(std::uint8_t v, const std::pair<std::uint8_t, const char*> (&table)[N])
{
    for (const auto& entry : table)
    {
        if (v == entry.first)
        {
            return ResultStr::ok_value(std::string(entry.second));
        }
    }
    return ResultStr::err(MappingError::kUnknownValue);
}
}  // namespace

// --- V1: StopReason (mrs_msgs::msg::StopReason) ---

namespace
{
using StopReasonMsg = mrs_msgs::msg::StopReason;
const std::pair<std::uint8_t, const char*> kStopReasonTable[] = {
    {StopReasonMsg::REASON_UNKNOWN, "unknown"},
    {StopReasonMsg::REASON_EXOGENOUS_BLOCK, "exogenous_block"},
    {StopReasonMsg::REASON_CTRL_STOP_DECLARED, "ctrl_stop_declared"},
    {StopReasonMsg::REASON_INFEASIBLE_SUBGOAL, "infeasible_subgoal"},
    {StopReasonMsg::REASON_ORIENTATION_FLIP, "orientation_flip"},
    {StopReasonMsg::REASON_UNRESOLVABLE_LOCALLY, "unresolvable_locally"},
    {StopReasonMsg::REASON_HORIZON_EXHAUSTING, "horizon_exhausting"},
};
}  // namespace

Result8 StopReasonFromString(const std::string& s)
{
    return LookupValue(s, kStopReasonTable);
}

ResultStr StopReasonToString(std::uint8_t v)
{
    return LookupString(v, kStopReasonTable);
}

bool IsLegalForStopDeclaration(std::uint8_t v)
{
    // 350 §3-2 V1: non-contiguous legal subset {1,3,5}. Set membership only.
    return v == StopReasonMsg::REASON_EXOGENOUS_BLOCK ||
           v == StopReasonMsg::REASON_INFEASIBLE_SUBGOAL ||
           v == StopReasonMsg::REASON_UNRESOLVABLE_LOCALLY;
}

bool IsLegalForReplanRequest(std::uint8_t v)
{
    return v == StopReasonMsg::REASON_EXOGENOUS_BLOCK ||
           v == StopReasonMsg::REASON_CTRL_STOP_DECLARED ||
           v == StopReasonMsg::REASON_INFEASIBLE_SUBGOAL ||
           v == StopReasonMsg::REASON_ORIENTATION_FLIP ||
           v == StopReasonMsg::REASON_UNRESOLVABLE_LOCALLY ||
           v == StopReasonMsg::REASON_HORIZON_EXHAUSTING;
}

// --- V2: ReplanOutcome (mrs_msgs::action::Replan_Result) ---

namespace
{
using ReplanResultMsg = mrs_msgs::action::Replan_Result;
const std::pair<std::uint8_t, const char*> kReplanOutcomeTable[] = {
    {ReplanResultMsg::OUTCOME_UNKNOWN, "unknown"},
    {ReplanResultMsg::OUTCOME_PLANNED, "planned"},
    {ReplanResultMsg::OUTCOME_FALLBACK, "fallback"},
    {ReplanResultMsg::OUTCOME_PREEMPTED, "preempted"},
    {ReplanResultMsg::OUTCOME_REJECTED, "rejected"},
};
}  // namespace

Result8 ReplanOutcomeFromString(const std::string& s)
{
    return LookupValue(s, kReplanOutcomeTable);
}

ResultStr ReplanOutcomeToString(std::uint8_t v)
{
    return LookupString(v, kReplanOutcomeTable);
}

// --- V3: OperatorVerb (mrs_msgs::msg::OperatorCommand) ---

namespace
{
using OperatorCommandMsg = mrs_msgs::msg::OperatorCommand;
const std::pair<std::uint8_t, const char*> kOperatorVerbTable[] = {
    {OperatorCommandMsg::VERB_UNKNOWN, "unknown"},
    {OperatorCommandMsg::VERB_HOLD, "hold"},
    {OperatorCommandMsg::VERB_SPEED_CAP, "speed_cap"},
    {OperatorCommandMsg::VERB_GOAL_OVERRIDE, "goal_override"},
};
}  // namespace

Result8 OperatorVerbFromString(const std::string& s)
{
    return LookupValue(s, kOperatorVerbTable);
}

ResultStr OperatorVerbToString(std::uint8_t v)
{
    return LookupString(v, kOperatorVerbTable);
}

// --- V4: DependencyType (mrs_msgs::msg::Dependency) ---

namespace
{
using DependencyMsg = mrs_msgs::msg::Dependency;
const std::pair<std::uint8_t, const char*> kDependencyTypeTable[] = {
    {DependencyMsg::DEP_UNKNOWN, "unknown"},
    {DependencyMsg::DEP_SEQUENTIAL, "sequential"},
    {DependencyMsg::DEP_ORDERING, "ordering"},
};
}  // namespace

Result8 DependencyTypeFromString(const std::string& s)
{
    return LookupValue(s, kDependencyTypeTable);
}

ResultStr DependencyTypeToString(std::uint8_t v)
{
    return LookupString(v, kDependencyTypeTable);
}

// --- V5: PlanTerminal (mrs_msgs::msg::RobotPlan) ---

namespace
{
using RobotPlanMsg = mrs_msgs::msg::RobotPlan;
const std::pair<std::uint8_t, const char*> kPlanTerminalTable[] = {
    {RobotPlanMsg::TERMINAL_UNKNOWN, "unknown"},
    {RobotPlanMsg::TERMINAL_GOAL_REACHED, "goal_reached"},
    {RobotPlanMsg::TERMINAL_PLAN_TRUNCATED, "plan_truncated"},
    {RobotPlanMsg::TERMINAL_NO_PROGRESS, "no_progress"},
};
}  // namespace

Result8 PlanTerminalFromString(const std::string& s)
{
    return LookupValue(s, kPlanTerminalTable);
}

ResultStr PlanTerminalToString(std::uint8_t v)
{
    return LookupString(v, kPlanTerminalTable);
}

// --- V6: EdgeTraversal (mrs_msgs::msg::RoadmapEdge) ---

namespace
{
using RoadmapEdgeMsg = mrs_msgs::msg::RoadmapEdge;
const std::pair<std::uint8_t, const char*> kEdgeTraversalTable[] = {
    {RoadmapEdgeMsg::TRAVERSAL_UNKNOWN, "unknown"},
    {RoadmapEdgeMsg::TRAVERSAL_BIDIRECTIONAL, "bidirectional"},
    {RoadmapEdgeMsg::TRAVERSAL_FORWARD_ONLY, "forward_only"},
    {RoadmapEdgeMsg::TRAVERSAL_BACKWARD_ONLY, "backward_only"},
};
}  // namespace

Result8 EdgeTraversalFromString(const std::string& s)
{
    return LookupValue(s, kEdgeTraversalTable);
}

ResultStr EdgeTraversalToString(std::uint8_t v)
{
    return LookupString(v, kEdgeTraversalTable);
}

}  // namespace mrs_core_msgs
