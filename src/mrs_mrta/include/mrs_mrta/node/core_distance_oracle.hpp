// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_NODE_CORE_DISTANCE_ORACLE_HPP
#define MRS_MRTA_NODE_CORE_DISTANCE_ORACLE_HPP

#include <vector>

#include "mrs_core/graph/roadmap_distances.hpp"
#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_mrta/ports/i_distance_oracle.hpp"
#include "mrs_msgs/msg/roadmap.hpp"

/// @file core_distance_oracle.hpp
/// @layer node (CN-1/CN-23 Presentation) — the only layer of this write round
/// allowed to touch `mrs_mrta` (`367_pipeline_inputs.md` 웨이브 1-A ③). This
/// header does NOT modify `core/`, `service/`, `adapter/`, or `plugins/` —
/// it implements `ports::IDistanceOracle` (an interface those layers already
/// depend on, `i_distance_oracle.hpp` file doc: "Unit tests inject a small
/// stand-in ... a real implementation is mrs_core's to provide at
/// integration time") by wrapping `mrs_core::graph::RoadmapDistances`
/// (`347_arch_integration_delta.md` §3-1 항목 6, `12a_arch_mrta_p2.md` §8-3).
///
/// 🔴 Debt reported, not fixed here (out of node/'s reach): `12a` §8-3's
/// design intent was a *process-wide, shared* table instance so `mrta_node`
/// and `mapf_node` never compute two different distance tables. This round's
/// launch topology (`357§7-2`) runs them as two separate OS processes, not a
/// composable-node container, so this class can only own a *private* table —
/// built from the same `/roadmap` message, by the identical algorithm, but a
/// distinct instance. The two never disagree in practice (deterministic
/// algorithm, same input), but the "one shared instance" half of `12a` §8-3
/// is not realized. See `367_pipeline_inputs.md` for the fuller writeup.
namespace mrs_mrta::node
{

/// @brief `ports::IDistanceOracle` implementation backed by
/// `mrs_core::graph::RoadmapDistances`. Owns the table; `Build` is called
/// once, from `OnRoadmap`, when `/roadmap` arrives — NOT at node
/// construction time (the assigner's `configure()` call happens in the
/// constructor, before any topic has been received, so this object's
/// pointer must be valid — but its table may still be empty — for the
/// `ctx.distances != nullptr` check to pass; see `mrta_node.cpp`).
class CoreDistanceOracle : public ports::IDistanceOracle
{
public:
    /// @brief Converts a `mrs_msgs::msg::Roadmap` into the plain-typed graph
    /// `mrs_core::graph::RoadmapDistances::Build` expects, then builds the
    /// table. Edges with an unrecognized/unknown `traversal` value are
    /// dropped (not guessed at) — a malformed roadmap should not silently
    /// produce fabricated connectivity.
    /// @param msg The received `/roadmap` message.
    void Build(const mrs_msgs::msg::Roadmap& msg)
    {
        std::vector<std::string> vertices;
        vertices.reserve(msg.nodes.size());
        for (const auto& n : msg.nodes)
        {
            vertices.push_back(n.id);
        }

        std::vector<mrs_core::graph::WeightedEdge> edges;
        edges.reserve(msg.edges.size());
        for (const auto& e : msg.edges)
        {
            const auto traversal_str = mrs_core_msgs::EdgeTraversalToString(e.traversal);
            if (!traversal_str.ok())
            {
                continue;  // unknown vocabulary value — drop, do not guess.
            }
            mrs_core::graph::WeightedEdge edge;
            edge.from = e.from_id;
            edge.to = e.to_id;
            edge.length_m = e.length_m;
            if (traversal_str.value() == "bidirectional")
            {
                edge.bidirectional = true;
            }
            else if (traversal_str.value() == "forward_only")
            {
                edge.bidirectional = false;
            }
            else if (traversal_str.value() == "backward_only")
            {
                // Reverse to a forward-only edge to_id -> from_id — the plain
                // WeightedEdge type has no backward-only concept of its own.
                edge.from = e.to_id;
                edge.to = e.from_id;
                edge.bidirectional = false;
            }
            else
            {
                continue;  // TRAVERSAL_UNKNOWN — drop.
            }
            edges.push_back(edge);
        }

        table_.Build(vertices, edges, msg.endpoints);
    }

    /// @copydoc ports::IDistanceOracle::dist_to_endpoint
    std::optional<std::uint64_t> dist_to_endpoint(const std::string& from_vertex,
                                                   const std::string& to_endpoint) const override
    {
        return table_.DistToEndpoint(from_vertex, to_endpoint);
    }

    /// @brief Whether `Build` has run at least once (i.e. `/roadmap` has
    /// already been received and parsed).
    /// @return true iff the underlying table has been built.
    bool IsBuilt() const
    {
        return table_.IsBuilt();
    }

private:
    mrs_core::graph::RoadmapDistances table_;
};

}  // namespace mrs_mrta::node

#endif  // MRS_MRTA_NODE_CORE_DISTANCE_ORACLE_HPP
