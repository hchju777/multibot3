// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/core/release_checker.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_release_checker.cpp
/// @brief ET19 — the gab-bu bit = arrival-vertex entry permit. ［D6］ following
/// opens on predecessor START; ［D7］ opposing opens only on predecessor COMPLETE.

int main()
{
    AltHeadLift gen;
    ConstraintCompiler cc(gen);

    // Following: r2#0 has ordering predecessor r1#1 (［D6］). Its arrival permit
    // opens once r1#1 has STARTED (entered).
    {
        ExecutionGraph g;
        CompileReport rep;
        cc.compile(mrs_sadg::test::make_following_plan(), g, rep);
        ReleaseChecker::release_check(g);
        CHECK(!g.is_released("r2#0"));  // r1#1 not entered yet.
        g.mark_entered("r1#1");
        ReleaseChecker::release_check(g);
        CHECK(g.is_released("r2#0"));  // now permitted (start suffices, ［D6］).
    }

    // Opposing: r1#1 has opposing predecessor r2#0 (［D7］). START of r2#0 is NOT
    // enough — its arrival permit opens only on COMPLETION.
    {
        ExecutionGraph g;
        CompileReport rep;
        cc.compile(mrs_sadg::test::make_opposing_feasible_plan(), g, rep);
        CHECK(g.is_opposing_edge("r2#0", "r1#1"));
        g.mark_entered("r2#0");  // started, not completed.
        ReleaseChecker::release_check(g);
        CHECK(!g.is_released("r1#1"));  // ［D7］: start is not enough.
        g.mark_completed("r2#0");
        ReleaseChecker::release_check(g);
        CHECK(g.is_released("r1#1"));  // completion opens it.
    }

    return sadg_test::summary();
}
