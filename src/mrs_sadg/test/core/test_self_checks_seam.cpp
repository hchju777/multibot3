// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/core/self_checks.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_self_checks_seam.cpp
/// @brief IN14 — the production-side seam self-check catches a wrong contract
/// mapping (which would otherwise be acyclic and pass the checker's exit 0).

int main()
{
    AltHeadLift gen;

    // A correctly compiled graph passes the seam self-check.
    {
        ConstraintCompiler cc(gen);
        ExecutionGraph g;
        CompileReport rep;
        CHECK(ok(cc.compile(mrs_sadg::test::make_following_plan(), g, rep)));
        SeamReport seam;
        CHECK(seam_selfcheck(mrs_sadg::test::make_following_plan(), g, seam));
        CHECK(seam.messages.empty());
    }

    // 🔴 Inject a WRONG mapping (퇴거->퇴거 instead of 퇴거->진입): the graph is
    // still acyclic, but the seam self-check must reject it — this is exactly the
    // f37_2 failure the deployed checker missed without --discrete-plan.
    {
        ExecutionGraph g;
        auto plan = mrs_sadg::test::make_following_plan();
        // Build segments + sequential by hand.
        for (const auto& r : plan.robots)
        {
            for (std::size_t k = 0; k + 1 < r.locations.size(); ++k)
            {
                g.add_segment({r.robot + "#" + std::to_string(k),
                               r.robot,
                               r.locations[k],
                               r.locations[k + 1]});
            }
            for (std::size_t k = 0; k + 2 < r.locations.size(); ++k)
            {
                g.add_fixed_dep({r.robot + "#" + std::to_string(k),
                                 r.robot + "#" + std::to_string(k + 1),
                                 DepType::kSequential});
            }
        }
        // Wrong ordering: 퇴거->퇴거 (r1#1 -> r2#1) instead of 퇴거->진입 (r1#1 -> r2#0).
        g.add_fixed_dep({"r1#1", "r2#1", DepType::kOrdering});
        g.add_fixed_dep({"r1#2", "r2#1", DepType::kOrdering});
        CHECK(g.is_acyclic({}));  // acyclic — yet wrong.
        SeamReport seam;
        CHECK(!seam_selfcheck(plan, g, seam));  // the self-check catches it.
        CHECK(!seam.messages.empty());
    }

    return sadg_test::summary();
}
