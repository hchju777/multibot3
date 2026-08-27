// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/declaration_regulator.hpp"

using namespace mrs_trajopt::core;

int main()
{
    // Q1 first: edge impassable for any robot => exogenous_block (even if Q2 too).
    {
        DeclarationEvidence e;
        e.edge_impassable_for_any_robot = true;
        e.remains_after_removing_others = true;
        CHECK(DeclarationRegulator::classify(e) == StopReason::kExogenousBlock);
    }
    // Q2: not Q1, but remains after removing others => infeasible_subgoal.
    {
        DeclarationEvidence e;
        e.edge_impassable_for_any_robot = false;
        e.remains_after_removing_others = true;
        CHECK(DeclarationRegulator::classify(e) == StopReason::kInfeasibleSubgoal);
    }
    // Q3: neither => unresolvable_locally.
    {
        DeclarationEvidence e;
        CHECK(DeclarationRegulator::classify(e) == StopReason::kUnresolvableLocally);
    }

    // 🔴 고정 결정 2 / 관문 324: reverse-forbidden infeasibility is
    // infeasible_subgoal, NOT unresolvable_locally.
    CHECK(DeclarationRegulator::reverse_forbidden_infeasible() == StopReason::kInfeasibleSubgoal);

    // Schema strings match the enum of stop_declaration.schema.json :52 exactly.
    CHECK(DeclarationRegulator::to_schema_string(StopReason::kExogenousBlock) == "exogenous_block");
    CHECK(DeclarationRegulator::to_schema_string(StopReason::kInfeasibleSubgoal) ==
          "infeasible_subgoal");
    CHECK(DeclarationRegulator::to_schema_string(StopReason::kUnresolvableLocally) ==
          "unresolvable_locally");

    return trajopt_test::summary();
}
