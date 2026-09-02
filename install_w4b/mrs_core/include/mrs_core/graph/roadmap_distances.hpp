// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_CORE_GRAPH_ROADMAP_DISTANCES_HPP
#define MRS_CORE_GRAPH_ROADMAP_DISTANCES_HPP

#include <cstdint>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/// @file roadmap_distances.hpp
/// @layer mrs_core_pure (CN-2/CN-3) — no ROS, no rosidl, no JSON, no pluginlib.
/// @brief Shared roadmap shortest-distance table (`347_arch_integration_delta.md`
/// §3-1 항목 6 · `12a_arch_mrta_p2.md` §8-3 "변형 채택" — 소유자를 `mrs_core`로).
///
/// `mrs_mrta` and `mrs_mapf` each build a private instance of this class from
/// the same `/roadmap` message (this header does not know `mrs_msgs` — the
/// ROS-message-to-plain-vector conversion is each consumer's own `node/`
/// layer's job, CN-1). The two instances are computed by the identical
/// algorithm, so they never disagree — `12a` §8-3's actual requirement
/// ("두 소비자가 같은 구현을 써야 두 값이 갈리지 않는다") is satisfied by a
/// shared *implementation*, not a shared *process-wide singleton instance*
/// (the plain-executable launch topology this round uses — `357§7-2` — does
/// not compose `mrta_node`/`mapf_node` into one process, so a literal pointer-
/// shared singleton is not reachable without a sibling-package dependency,
/// which `CN-21` forbids). 🔴 Reported as a debt in `367_pipeline_inputs.md`
/// — the shared-*instance* half of the 47차 design intent is not realized
/// this round.
namespace mrs_core::graph
{

/// @brief One directed or bidirectional roadmap edge, plain-typed (no
/// `mrs_msgs::msg::RoadmapEdge` — the caller converts).
struct WeightedEdge
{
    std::string from;          ///< Source vertex id.
    std::string to;            ///< Target vertex id.
    double length_m = 0.0;     ///< Edge length in metres, >= 0.
    bool bidirectional = true;  ///< true: usable from either endpoint. false: from -> to only.
};

/// @brief Build-once, query-many shortest-distance table from any roadmap
/// vertex to any roadmap endpoint (`mrs_mrta::ports::IDistanceOracle`'s
/// contract: integer millimetres, `std::nullopt` if unreachable).
///
/// Algorithm: for each endpoint, one Dijkstra pass over the *reversed*
/// adjacency (edge `u->v` reverses to `v->u`; a bidirectional edge reverses
/// to itself) rooted at that endpoint. `dist[v]` after that pass is the
/// shortest path length from `v` to the endpoint in the original graph —
/// this is the standard "single-destination shortest paths" reduction. With
/// roadmap sizes in the tens of vertices/edges and endpoint counts in the
/// single digits (this round's scenarios), this is `O(|E| = endpoints)`
/// Dijkstra passes and is not a p99 concern (12a §8-4 already found the
/// assignment rule itself, not distance lookup, dominates the budget).
class RoadmapDistances
{
public:
    /// @brief (Re)builds the table from a plain roadmap graph. Not
    /// incremental — the whole table is discarded and recomputed. Safe to
    /// call multiple times (e.g. if a run republishes `/roadmap`, though
    /// `139:215` documents it as latched + run-invariant).
    /// @param vertices All roadmap vertex ids (including endpoints).
    /// @param edges Roadmap edges, plain-typed.
    /// @param endpoints The subset of `vertices` that are legal
    ///        `dist_to_endpoint` targets (`mrs.roadmap.endpoints`).
    void Build(const std::vector<std::string>& vertices,
               const std::vector<WeightedEdge>& edges,
               const std::vector<std::string>& endpoints)
    {
        dist_from_endpoint_.clear();
        built_ = false;

        std::unordered_map<std::string, std::vector<std::pair<std::string, std::uint64_t>>>
            reverse_adjacency;
        for (const auto& v : vertices)
        {
            reverse_adjacency[v];  // ensure every vertex has an (possibly empty) entry
        }
        for (const auto& e : edges)
        {
            const auto weight_mm = MetresToMillimetres(e.length_m);
            // Forward edge e.from -> e.to reverses to e.to -> e.from.
            reverse_adjacency[e.to].emplace_back(e.from, weight_mm);
            if (e.bidirectional)
            {
                // Forward edge e.to -> e.from (the other direction) reverses to
                // e.from -> e.to.
                reverse_adjacency[e.from].emplace_back(e.to, weight_mm);
            }
        }

        for (const auto& endpoint : endpoints)
        {
            dist_from_endpoint_[endpoint] = DijkstraFrom(endpoint, reverse_adjacency);
        }
        built_ = true;
    }

    /// @brief Shortest-path cost from @p from_vertex to @p to_endpoint, in
    /// millimetres (`ports::IDistanceOracle::dist_to_endpoint` contract).
    /// @param from_vertex Roadmap vertex the query originates at.
    /// @param to_endpoint Roadmap endpoint the query targets.
    /// @return the integer-millimetre cost, or `std::nullopt` if unreachable,
    ///         if @p to_endpoint was not in the `endpoints` list passed to
    ///         the last `Build`, or if `Build` was never called.
    std::optional<std::uint64_t> DistToEndpoint(const std::string& from_vertex,
                                                 const std::string& to_endpoint) const
    {
        if (!built_)
        {
            return std::nullopt;
        }
        const auto endpoint_it = dist_from_endpoint_.find(to_endpoint);
        if (endpoint_it == dist_from_endpoint_.end())
        {
            return std::nullopt;
        }
        const auto vertex_it = endpoint_it->second.find(from_vertex);
        if (vertex_it == endpoint_it->second.end())
        {
            return std::nullopt;
        }
        return vertex_it->second;
    }

    /// @brief Whether `Build` has been called at least once. `mrta_node`/
    /// `mapf_node` use this to decide whether a task-release/plan request
    /// arrived before `/roadmap` did (a real race — `/roadmap` is
    /// `TRANSIENT_LOCAL` but `/task_release` is not, `qos.yaml` `reliable_
    /// bounded`).
    /// @return true iff at least one `Build` call has completed.
    bool IsBuilt() const
    {
        return built_;
    }

private:
    /// @brief Converts a length in metres to integer millimetres, rounding to
    /// nearest and clamping negative input to 0 (a malformed roadmap should
    /// not produce a negative edge weight for Dijkstra).
    /// @param length_m Length in metres.
    /// @return Length in millimetres, rounded to nearest integer.
    static std::uint64_t MetresToMillimetres(double length_m)
    {
        if (length_m < 0.0)
        {
            length_m = 0.0;
        }
        return static_cast<std::uint64_t>(length_m * 1000.0 + 0.5);
    }

    /// @brief Single-source Dijkstra over @p reverse_adjacency rooted at
    /// @p source.
    /// @param source The root vertex (an endpoint, in reversed-graph terms).
    /// @param reverse_adjacency Reversed adjacency list (vertex -> (neighbor,
    ///        weight_mm) pairs).
    /// @return Map of every reachable vertex to its distance from @p source,
    ///         in millimetres.
    static std::unordered_map<std::string, std::uint64_t> DijkstraFrom(
        const std::string& source,
        const std::unordered_map<std::string, std::vector<std::pair<std::string, std::uint64_t>>>&
            reverse_adjacency)
    {
        std::unordered_map<std::string, std::uint64_t> dist;
        using QueueItem = std::pair<std::uint64_t, std::string>;
        std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> frontier;

        dist[source] = 0;
        frontier.emplace(0, source);
        while (!frontier.empty())
        {
            const auto [distance, vertex] = frontier.top();
            frontier.pop();

            const auto known_it = dist.find(vertex);
            if (known_it != dist.end() && distance > known_it->second)
            {
                continue;  // stale queue entry
            }

            const auto adjacency_it = reverse_adjacency.find(vertex);
            if (adjacency_it == reverse_adjacency.end())
            {
                continue;
            }
            for (const auto& [neighbor, weight] : adjacency_it->second)
            {
                const auto candidate = distance + weight;
                const auto neighbor_it = dist.find(neighbor);
                if (neighbor_it == dist.end() || candidate < neighbor_it->second)
                {
                    dist[neighbor] = candidate;
                    frontier.emplace(candidate, neighbor);
                }
            }
        }
        return dist;
    }

    bool built_ = false;
    std::unordered_map<std::string, std::unordered_map<std::string, std::uint64_t>>
        dist_from_endpoint_;
};

}  // namespace mrs_core::graph

#endif  // MRS_CORE_GRAPH_ROADMAP_DISTANCES_HPP
