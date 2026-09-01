// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_TEST_FIXTURES_HPP
#define MRS_MAPF_TEST_FIXTURES_HPP

#include "mrs_mapf/core/types.hpp"

/// @file fixtures.hpp
/// @brief Shared tiny roadmaps for unit tests — A minimal-roadmap style graph
/// (nodes only at "intersections", `_workspace/multibot3-minimal-roadmap-constraint`
/// convention): a 4-node line A-B-C-D (a single corridor) plus a 3-node branch
/// used by tests that need two independent paths.

namespace mrs_mapf::test
{

/// @brief A-B-C-D bidirectional line, each edge 5.0 m.
inline core::Roadmap make_line_roadmap()
{
    core::Roadmap r;
    r.node_ids = {"A", "B", "C", "D"};
    for (const auto& n : r.node_ids)
    {
        r.out_arcs[n] = {};
    }
    r.endpoints = {"A", "D"};

    auto link = [&](const std::string& a, const std::string& b, double len)
    {
        core::RoadmapArc fwd{a, b, len, 2.0, 1, ""};
        core::RoadmapArc bwd{b, a, len, 2.0, 1, ""};
        r.out_arcs[a].push_back(fwd);
        r.out_arcs[b].push_back(bwd);
    };
    link("A", "B", 5.0);
    link("B", "C", 5.0);
    link("C", "D", 5.0);
    return r;
}

/// @brief A "Y": A-B-C line plus a second start P-B branch, both bidirectional,
/// 5.0 m each — used for two-robot opposing/independent scenarios.
inline core::Roadmap make_branch_roadmap()
{
    core::Roadmap r;
    r.node_ids = {"A", "B", "C", "P"};
    for (const auto& n : r.node_ids)
    {
        r.out_arcs[n] = {};
    }
    r.endpoints = {"A", "C", "P"};

    auto link = [&](const std::string& a, const std::string& b, double len)
    {
        core::RoadmapArc fwd{a, b, len, 2.0, 1, ""};
        core::RoadmapArc bwd{b, a, len, 2.0, 1, ""};
        r.out_arcs[a].push_back(fwd);
        r.out_arcs[b].push_back(bwd);
    };
    link("A", "B", 5.0);
    link("B", "C", 5.0);
    link("P", "B", 5.0);
    return r;
}

/// @brief `n` robots named r0..r{n-1}, each with the given average speed.
inline std::vector<core::RobotSpec> make_specs(int n, double speed_mps)
{
    std::vector<core::RobotSpec> out;
    for (int i = 0; i < n; ++i)
    {
        core::RobotSpec s;
        s.robot = "r" + std::to_string(i);
        s.avg_traversal_speed_mps = speed_mps;
        out.push_back(s);
    }
    return out;
}

}  // namespace mrs_mapf::test

#endif  // MRS_MAPF_TEST_FIXTURES_HPP
