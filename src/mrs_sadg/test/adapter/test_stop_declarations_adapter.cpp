// SPDX-License-Identifier: Apache-2.0
#include <string>
#include <utility>
#include <vector>

#include "../test_util.hpp"
#include "mrs_sadg/adapter/stop_declarations_adapter.hpp"
#include "mrs_sadg/core/status.hpp"

using namespace mrs_sadg::adapter;

/// @file test_stop_declarations_adapter.cpp
/// @brief mrs.stop_declaration 5.0.1 -> (from,to) blocked-edge pairs (ET17). See
/// stop_declarations_adapter.hpp file doc for the reason-scoping + declared:false
/// scope notes this test locks in.

int main()
{
    // declared:true relays the edge.
    {
        BoundaryStopDeclarations msg;
        msg.declarations.push_back({0, "r1", "X", "Y", true, "exogenous_block"});
        std::vector<std::pair<std::string, std::string>> out;
        CHECK(mrs_sadg::core::ok(StopDeclarationsAdapter::from_boundary(msg, out)));
        CHECK(out.size() == 1);
        CHECK(out[0].first == "X" && out[0].second == "Y");
    }

    // declared:false is NOT relayed (no removal API — additive-only V_blk).
    {
        BoundaryStopDeclarations msg;
        msg.declarations.push_back({1, "r1", "X", "Y", false, "exogenous_block"});
        std::vector<std::pair<std::string, std::string>> out;
        CHECK(mrs_sadg::core::ok(StopDeclarationsAdapter::from_boundary(msg, out)));
        CHECK(out.empty());
    }

    // all three legal reasons relay identically (core has one flat V_blk with
    // no reason-scoped consumer today — see file-doc scope note).
    {
        BoundaryStopDeclarations msg;
        msg.declarations.push_back({0, "r1", "A", "B", true, "infeasible_subgoal"});
        msg.declarations.push_back({1, "r1", "C", "D", true, "unresolvable_locally"});
        std::vector<std::pair<std::string, std::string>> out;
        CHECK(mrs_sadg::core::ok(StopDeclarationsAdapter::from_boundary(msg, out)));
        CHECK(out.size() == 2);
    }

    // envelope mismatch -> kError.
    {
        BoundaryStopDeclarations msg;
        msg.schema_version = "1.0.0";
        std::vector<std::pair<std::string, std::string>> out;
        CHECK(!mrs_sadg::core::ok(StopDeclarationsAdapter::from_boundary(msg, out)));
    }

    return sadg_test::summary();
}
