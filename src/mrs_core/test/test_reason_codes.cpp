// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent).
#include <cassert>
#include <cstdio>

#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_msgs/msg/stop_reason.hpp"

int main()
{
    using mrs_core_msgs::DependencyTypeFromString;
    using mrs_core_msgs::EdgeTraversalFromString;
    using mrs_core_msgs::IsLegalForReplanRequest;
    using mrs_core_msgs::IsLegalForStopDeclaration;
    using mrs_core_msgs::MappingError;
    using mrs_core_msgs::OperatorVerbFromString;
    using mrs_core_msgs::PlanTerminalFromString;
    using mrs_core_msgs::ReplanOutcomeFromString;
    using mrs_core_msgs::StopReasonFromString;
    using mrs_core_msgs::StopReasonToString;

    // V1 round-trip + the non-contiguous-subset trap (350 §3-2) — if someone
    // "fixes" IsLegalForStopDeclaration into a range check, this must fail.
    auto v = StopReasonFromString("infeasible_subgoal");
    assert(v.ok());
    assert(v.value() == mrs_msgs::msg::StopReason::REASON_INFEASIBLE_SUBGOAL);
    auto s = StopReasonToString(v.value());
    assert(s.ok());
    assert(s.value() == "infeasible_subgoal");

    assert(IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_EXOGENOUS_BLOCK));
    assert(IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_INFEASIBLE_SUBGOAL));
    assert(IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_UNRESOLVABLE_LOCALLY));
    // In-vocabulary but NOT in the {1,3,5} subset — a range check v<=5 would
    // wrongly accept value 4 (orientation_flip).
    assert(!IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_CTRL_STOP_DECLARED));
    assert(!IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_ORIENTATION_FLIP));
    assert(!IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_HORIZON_EXHAUSTING));
    assert(!IsLegalForStopDeclaration(mrs_msgs::msg::StopReason::REASON_UNKNOWN));

    assert(IsLegalForReplanRequest(mrs_msgs::msg::StopReason::REASON_HORIZON_EXHAUSTING));
    assert(!IsLegalForReplanRequest(mrs_msgs::msg::StopReason::REASON_UNKNOWN));

    // Unknown string / unknown value both report as errors, never throw.
    auto bad = StopReasonFromString("no_such_reason");
    assert(!bad.ok());
    assert(bad.error() == MappingError::kUnknownString);
    auto bad2 = StopReasonToString(200);
    assert(!bad2.ok());
    assert(bad2.error() == MappingError::kUnknownValue);

    // 🔴 F47-2 (코드 게이트 22§6) — locks the fix: LookupValue must NEVER match
    // the literal "unknown" to value 0, because none of the 15 schemas ever
    // spells it (350 D1-a's whole point is that 0 has no JSON originator). If
    // this regresses to matching "unknown", this assertion flips to failing.
    auto unk = StopReasonFromString("unknown");
    assert(!unk.ok());
    assert(unk.error() == MappingError::kUnknownString);
    // The reverse direction is allowed to keep rendering 0 as "unknown"
    // (diagnostic-only) — that half is NOT locked to fail.
    auto unk_str = StopReasonToString(mrs_msgs::msg::StopReason::REASON_UNKNOWN);
    assert(unk_str.ok());

    // Spot-check the other five vocabularies round-trip.
    assert(ReplanOutcomeFromString("fallback").ok());
    assert(OperatorVerbFromString("speed_cap").ok());
    assert(DependencyTypeFromString("ordering").ok());
    assert(PlanTerminalFromString("no_progress").ok());
    assert(EdgeTraversalFromString("backward_only").ok());

    // F47-2 same lock, all five other vocabularies — the bug was structural
    // (shared LookupValue helper), so all six shared the same defect.
    assert(!ReplanOutcomeFromString("unknown").ok());
    assert(!OperatorVerbFromString("unknown").ok());
    assert(!DependencyTypeFromString("unknown").ok());
    assert(!PlanTerminalFromString("unknown").ok());
    assert(!EdgeTraversalFromString("unknown").ok());

    std::puts("test_reason_codes: OK");
    return 0;
}
