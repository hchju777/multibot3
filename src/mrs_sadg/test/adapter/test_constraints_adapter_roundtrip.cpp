// SPDX-License-Identifier: Apache-2.0
#include <set>

#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/adapter/constraints_adapter.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/io/json_io.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::adapter;
using mrs_sadg::plugins::AltHeadLift;

/// @file test_constraints_adapter_roundtrip.cpp
/// @brief Adapter round-trip: ExecutionGraph -> boundary -> JSON -> boundary
/// preserves the segments and dependencies (id/robot/from/to and edge set).

int main()
{
    AltHeadLift gen;
    ConstraintCompiler cc(gen);
    ExecutionGraph g;
    CompileReport rep;
    CHECK(ok(cc.compile(mrs_sadg::test::make_following_plan(), g, rep)));

    BoundaryExecutionConstraints doc;
    CHECK(ok(ConstraintsAdapter::to_boundary(g, doc)));
    const std::string json = mrs_sadg::io::to_json(doc);
    const BoundaryExecutionConstraints back = mrs_sadg::io::parse_execution_constraints(json);

    CHECK(back.schema == "mrs.execution_constraints");
    CHECK(back.schema_version == "3.0.0");
    CHECK(back.instance_id == g.instance_id());
    CHECK(back.segments.size() == g.segments().size());

    std::set<std::string> seg_before;
    for (const auto& s : g.segments())
    {
        seg_before.insert(s.id + "|" + s.robot + "|" + s.from + "|" + s.to);
    }
    std::set<std::string> seg_after;
    for (const auto& s : back.segments)
    {
        seg_after.insert(s.id + "|" + s.robot + "|" + s.from + "|" + s.to);
    }
    CHECK(seg_before == seg_after);

    std::set<std::string> dep_before;
    for (const auto& d : g.fixed_deps())
    {
        dep_before.insert(d.from + "->" + d.to + ":" +
                          (d.type == DepType::kSequential ? "sequential" : "ordering"));
    }
    std::set<std::string> dep_after;
    for (const auto& d : back.dependencies)
    {
        dep_after.insert(d.from + "->" + d.to + ":" + d.type);
    }
    CHECK(dep_before == dep_after);

    return sadg_test::summary();
}
