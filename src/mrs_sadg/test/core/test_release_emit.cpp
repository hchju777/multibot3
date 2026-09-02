// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <string>

#include "../test_util.hpp"
#include "mrs_sadg/adapter/release_adapter.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/release_checker.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/io/json_io.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::adapter;

/// @file test_release_emit.cpp
/// @brief Boundary-artifact checker test: emit a real mrs.segment_release 2.0.0
/// document (ReleaseAdapter::to_boundary on a graph that has actually run
/// ReleaseChecker::release_check, mirroring what sadg_t0_node's tick loop does)
/// next to the test binary, so check_schema_purity.py runs against a real
/// production of this module's node-wiring layer (mirrors test_boundary_emit.cpp
/// for execution_constraints/discrete_plan).

int main(int argc, char** argv)
{
    const std::string dir = argc > 1 ? std::string(argv[1]) : std::string(".");

    ExecutionGraph g;
    g.set_instance_id("release-emit-fixture");
    g.add_segment({"r1#0", "r1", "S1", "X"});
    g.add_segment({"r1#1", "r1", "X", "G1"});
    g.add_fixed_dep({"r1#0", "r1#1", DepType::kSequential});
    g.mark_entered("r1#0");
    ReleaseChecker::release_check(g);
    CHECK(g.is_released("r1#1"));  // sequential predecessor started -> permitted.

    BoundarySegmentRelease doc;
    CHECK(ok(ReleaseAdapter::to_boundary(g, "r1", doc)));
    doc.instance_id = g.instance_id();
    doc.release_seq = 0;
    doc.commit_seq = 0;
    doc.entry_seq_ack = 1;

    const std::string j = mrs_sadg::io::to_json(doc);
    CHECK(j.find("\"schema\": \"mrs.segment_release\"") != std::string::npos);
    CHECK(j.find("\"schema_version\": \"2.0.0\"") != std::string::npos);
    std::ofstream(dir + "/segment_release.json") << j;

    return sadg_test::summary();
}
