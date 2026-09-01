// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <string>

#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/adapter/constraints_adapter.hpp"
#include "mrs_sadg/adapter/plan_adapter.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/io/json_io.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::adapter;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_boundary_emit.cpp
/// @brief Boundary-artifact checker test: run the full pipeline (compiler ->
/// adapter -> io) on a following instance and write mrs.execution_constraints
/// 3.0.0 + the matching mrs.discrete_plan 4.0.0 next to the test binary, so
/// check_sadg_acyclic.py (--discrete-plan, seam + H-cons) / check_schema_purity.py
/// run against a real production of this module's own code. Also emits a
/// switch-group instance so the exhaustive acyclic-combination check has
/// alternatives to enumerate.

namespace
{

void assert_pure(const std::string& j)
{
    // Schema purity — no algorithm/solver/internal keys anywhere in the text.
    CHECK(j.find("timestep") == std::string::npos);
    CHECK(j.find("k_robustness") == std::string::npos);
    CHECK(j.find("solver") == std::string::npos);
    CHECK(j.find("theta_gain") == std::string::npos);
    CHECK(j.find("budget") == std::string::npos);
    CHECK(j.find("rz_seq") == std::string::npos);
    CHECK(j.find("seed") == std::string::npos);
    CHECK(j.find("horizon") == std::string::npos);
    CHECK(j.find("suboptimality") == std::string::npos);
    CHECK(j.find("k_robust") == std::string::npos);
}

std::string emit_constraints(const PlanView& plan)
{
    AltHeadLift gen;
    ConstraintCompiler cc(gen);
    ExecutionGraph g;
    CompileReport rep;
    CHECK(ok(cc.compile(plan, g, rep)));
    CHECK(rep.outcome == CompileOutcome::kOk);
    BoundaryExecutionConstraints doc;
    CHECK(ok(ConstraintsAdapter::to_boundary(g, doc)));
    return mrs_sadg::io::to_json(doc);
}

std::string emit_plan(const PlanView& plan)
{
    BoundaryDiscretePlan doc;
    CHECK(ok(PlanAdapter::to_boundary(plan, doc)));
    return mrs_sadg::io::to_json(doc);
}

// A crossing graph with a genuine switch group (both orders acyclic).
std::string emit_switch_constraints(const PlanView& plan)
{
    ExecutionGraph g;
    g.set_instance_id(plan.instance_id);
    for (const auto& r : plan.robots)
    {
        for (std::size_t k = 0; k + 1 < r.locations.size(); ++k)
        {
            g.add_segment(
                {r.robot + "#" + std::to_string(k), r.robot, r.locations[k], r.locations[k + 1]});
        }
        for (std::size_t k = 0; k + 2 < r.locations.size(); ++k)
        {
            g.add_fixed_dep({r.robot + "#" + std::to_string(k),
                             r.robot + "#" + std::to_string(k + 1),
                             DepType::kSequential});
        }
    }
    SwitchGroup grp;
    grp.id = "gX";
    grp.location = "X";
    grp.alternatives = {{"r1_first", {{"r1#1", "r2#0", DepType::kOrdering}}},
                        {"r2_first", {{"r2#1", "r1#0", DepType::kOrdering}}}};
    g.add_switch_group(grp);
    BoundaryExecutionConstraints doc;
    ConstraintsAdapter::to_boundary(g, doc);
    return mrs_sadg::io::to_json(doc);
}

}  // namespace

int main(int argc, char** argv)
{
    const std::string dir = argc > 1 ? std::string(argv[1]) : std::string(".");

    // Following (committed) instance — fully validates seam (a-d) + H-cons.
    const std::string ec = emit_constraints(mrs_sadg::test::make_following_plan());
    const std::string dp = emit_plan(mrs_sadg::test::make_following_plan());
    CHECK(ec.find("\"schema\": \"mrs.execution_constraints\"") != std::string::npos);
    CHECK(ec.find("\"schema_version\": \"3.0.0\"") != std::string::npos);
    CHECK(dp.find("\"schema\": \"mrs.discrete_plan\"") != std::string::npos);
    CHECK(dp.find("\"schema_version\": \"4.0.0\"") != std::string::npos);
    assert_pure(ec);
    assert_pure(dp);
    std::ofstream(dir + "/execution_constraints.json") << ec;
    std::ofstream(dir + "/discrete_plan.json") << dp;

    // Switch-group (crossing) instance — exercises the acyclic-combination check.
    const std::string ecs = emit_switch_constraints(mrs_sadg::test::make_crossing_plan());
    const std::string dps = emit_plan(mrs_sadg::test::make_crossing_plan());
    assert_pure(ecs);
    std::ofstream(dir + "/ec_switch.json") << ecs;
    std::ofstream(dir + "/dp_switch.json") << dps;

    return sadg_test::summary();
}
