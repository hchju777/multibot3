// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_constraint_compiler.cpp
/// @brief IN## — segment compilation, E0, RG-3 mapping, ［D5］/［D7］ opposing,
/// H-cons intake reject.

namespace
{

bool has_dep(const ExecutionGraph& g, const std::string& from, const std::string& to, DepType t)
{
    for (const auto& d : g.fixed_deps())
    {
        if (d.from == from && d.to == to && d.type == t)
        {
            return true;
        }
    }
    return false;
}

}  // namespace

int main()
{
    AltHeadLift gen;

    // ── following plan: segments, E0, contract-mapped ordering edges ──────────
    {
        ConstraintCompiler cc(gen);
        ExecutionGraph g;
        CompileReport rep;
        CHECK(ok(cc.compile(mrs_sadg::test::make_following_plan(), g, rep)));
        CHECK(rep.outcome == CompileOutcome::kOk);
        CHECK(g.segments().size() == 6);  // 3 per robot (steps-1).
        // E0 sequential chains.
        CHECK(has_dep(g, "r1#0", "r1#1", DepType::kSequential));
        CHECK(has_dep(g, "r1#1", "r1#2", DepType::kSequential));
        CHECK(has_dep(g, "r2#0", "r2#1", DepType::kSequential));
        // RG-3 contract mapping 퇴거->진입 at A and B.
        CHECK(has_dep(g, "r1#1", "r2#0", DepType::kOrdering));  // A: r1 before r2.
        CHECK(has_dep(g, "r1#2", "r2#1", DepType::kOrdering));  // B: r1 before r2.
        CHECK(rep.n_ordering_edges == 2);
        CHECK(rep.n_opposing_excluded == 0);  // pure following.
        CHECK(g.is_acyclic({}));
    }

    // ── opposing feasible plan: one opposing edge, still acyclic ──────────────
    {
        ConstraintCompiler cc(gen);
        ExecutionGraph g;
        CompileReport rep;
        CHECK(ok(cc.compile(mrs_sadg::test::make_opposing_feasible_plan(), g, rep)));
        CHECK(rep.outcome == CompileOutcome::kOk);
        // The opposing edge exists as a fixed ordering dep (checker expects it)
        // AND is flagged opposing (release waits on completion, ［D7］).
        CHECK(has_dep(g, "r2#0", "r1#1", DepType::kOrdering));
        CHECK(g.is_opposing_edge("r2#0", "r1#1"));
        CHECK(rep.n_opposing_excluded == 1);
        CHECK(!g.is_opposing_edge("r2#1", "r1#0"));  // the following edge is not opposing.
        CHECK(g.is_acyclic({}));
    }

    // ── H-cons violation: rejected + counted (not silently folded) ────────────
    {
        ConstraintCompiler cc(gen);
        ExecutionGraph g;
        CompileReport rep;
        CHECK(!ok(cc.compile(mrs_sadg::test::make_hcons_violation_plan(), g, rep)));
        CHECK(rep.outcome == CompileOutcome::kHconsViolation);
        CHECK(!rep.messages.empty());
    }

    return sadg_test::summary();
}
