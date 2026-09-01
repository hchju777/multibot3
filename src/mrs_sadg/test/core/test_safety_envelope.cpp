// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/core/safety_envelope.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_safety_envelope.cpp
/// @brief SE## — safe_check (acyclic ∧ PROP-ACCEPT, broader than acyclicity) and
/// detect_wc1 (STRUCT_WC1).

int main()
{
    AltHeadLift gen;
    ConstraintCompiler cc(gen);
    SafetyEnvelope env;

    // RE-1 accept: the compiled following graph is safe.
    {
        ExecutionGraph g;
        CompileReport rep;
        cc.compile(mrs_sadg::test::make_following_plan(), g, rep);
        CHECK(env.safe_check(g, {}));
    }

    // PROP-ACCEPT (broader than acyclicity): a committed successor placed behind
    // an uncommitted predecessor is unsafe even though the graph is acyclic.
    {
        ExecutionGraph g;
        CompileReport rep;
        cc.compile(mrs_sadg::test::make_following_plan(), g, rep);
        g.mark_entered("r2#0");  // successor committed...
        // ...but its ordering predecessor r1#1 is NOT entered -> A-LOCK inversion.
        CHECK(g.is_acyclic({}));        // still acyclic.
        CHECK(!env.safe_check(g, {}));  // yet rejected (RE-4 broader).
    }

    // detect_wc1: exit occupant with a reversed-only residual first edge.
    {
        WcInput in;
        Corridor q;
        q.id = "q0";
        q.entry_node = "a";
        q.exit_node = "e";
        q.occupants = {"r3"};
        q.exit_occupant = "r4";
        q.exit_first_edge_reversed = true;  // DT.5.
        in.corridors.push_back(q);
        WcResult r = env.detect_wc1(in);
        CHECK(r.detected);
        CHECK(r.corridor == "q0");
    }

    // detect_wc1: empty corridor -> not detected (DT.2 drain is finite).
    {
        WcInput in;
        Corridor q;
        q.id = "q1";
        q.exit_occupant = "r4";
        q.exit_first_edge_reversed = true;  // but no occupants.
        in.corridors.push_back(q);
        CHECK(!env.detect_wc1(in).detected);
    }

    return sadg_test::summary();
}
