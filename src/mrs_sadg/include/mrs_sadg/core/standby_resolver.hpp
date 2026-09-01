// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_STANDBY_RESOLVER_HPP
#define MRS_SADG_CORE_STANDBY_RESOLVER_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

/// @file standby_resolver.hpp
/// @brief RE-3 / U37-3 — derives articulation points of the roadmap graph as a
/// *predicate* (337 §4-3 StandbyResolver). 🔴 It never mutates the roadmap: a
/// standby ("z-axis stack") node stacked onto the physical roadmap would make it
/// robot-dependent and break the minimal-roadmap invariant (307§307-1). This is
/// a pure graph query over `mrs.roadmap.nodes`, read-only.

namespace mrs_sadg::core
{

/// @brief An undirected roadmap adjacency (node id -> neighbour ids).
using RoadmapAdjacency = std::map<std::string, std::vector<std::string>>;

/// @brief Derives articulation points of a roadmap graph (read-only predicate).
class StandbyResolver
{
public:
    /// @brief Articulation points (cut vertices) of an undirected graph.
    /// @param adj the roadmap adjacency.
    /// @return the set of node ids whose removal increases the component count.
    static std::set<std::string> articulation_points(const RoadmapAdjacency& adj);

    /// @brief RE-3: does a junction have a standby slot on a cut vertex with the
    /// required count?
    /// @param adj the roadmap adjacency.
    /// @param junction the unsatisfied junction node.
    /// @param standby_count the number of standby slots available at @p junction.
    /// @param d3 the required standby count (D3, [값 부재] from config).
    /// @return true iff @p junction is a cut vertex and @p standby_count >= @p d3.
    static bool standby_recoverable(const RoadmapAdjacency& adj,
                                    const std::string& junction,
                                    int standby_count,
                                    int d3);
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_STANDBY_RESOLVER_HPP
