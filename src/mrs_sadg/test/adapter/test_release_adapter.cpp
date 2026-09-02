// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/adapter/release_adapter.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/io/json_io.hpp"

using namespace mrs_sadg::adapter;
using namespace mrs_sadg::core;

/// @file test_release_adapter.cpp
/// @brief ExecutionGraph -> mrs.segment_release 2.0.0 (ET33), one robot per
/// artifact. Only `to_boundary` exists (CN-9, SADG never consumes this back) —
/// hence no "_roundtrip" suffix (see CMakeLists.txt comment for the naming note).

int main()
{
    ExecutionGraph g;
    g.add_segment({"r1#0", "r1", "S1", "X"});
    g.add_segment({"r1#1", "r1", "X", "G1"});
    g.add_segment({"r2#0", "r2", "S2", "X"});
    g.set_release("r1#0", true);

    // to_boundary: only this robot's segments; released bit copied faithfully;
    // commit_seq is NOT set here (node-owned, see release_adapter.hpp file doc).
    {
        BoundarySegmentRelease out;
        CHECK(ok(ReleaseAdapter::to_boundary(g, "r1", out)));
        CHECK(out.schema == BoundarySegmentRelease::kSchema);
        CHECK(out.schema_version == BoundarySegmentRelease::kSchemaVersion);
        CHECK(out.robot == "r1");
        CHECK(out.commit_seq == 0);       // untouched — caller's job.
        CHECK(out.segments.size() == 2);  // r1#0, r1#1 only -- not r2#0.
        bool found_r1_0 = false;
        bool found_r1_1 = false;
        for (const auto& s : out.segments)
        {
            CHECK(s.id != "r2#0");
            if (s.id == "r1#0")
            {
                found_r1_0 = true;
                CHECK(s.released);
            }
            if (s.id == "r1#1")
            {
                found_r1_1 = true;
                CHECK(!s.released);
            }
        }
        CHECK(found_r1_0 && found_r1_1);
    }

    // r2 gets its own, disjoint artifact (one artifact = one robot).
    {
        BoundarySegmentRelease out;
        CHECK(ok(ReleaseAdapter::to_boundary(g, "r2", out)));
        CHECK(out.robot == "r2");
        CHECK(out.segments.size() == 1);
        CHECK(out.segments[0].id == "r2#0");
        CHECK(!out.segments[0].released);
    }

    // schema-purity of the emitted JSON — no algorithm/solver keys.
    {
        BoundarySegmentRelease out;
        out.instance_id = "inst1";
        out.release_seq = 3;
        out.commit_seq = 7;
        out.entry_seq_ack = 2;
        CHECK(ok(ReleaseAdapter::to_boundary(g, "r2", out)));
        const std::string j = mrs_sadg::io::to_json(out);
        CHECK(j.find("\"schema\": \"mrs.segment_release\"") != std::string::npos);
        CHECK(j.find("\"schema_version\": \"2.0.0\"") != std::string::npos);
        CHECK(j.find("\"commit_seq\": 7") != std::string::npos);
        CHECK(j.find("theta_gain") == std::string::npos);
        CHECK(j.find("budget") == std::string::npos);
        CHECK(j.find("rz_seq") == std::string::npos);
        CHECK(j.find("solver") == std::string::npos);
        CHECK(j.find("node_budget") == std::string::npos);
    }

    return sadg_test::summary();
}
