// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/adapter/roadmap_adapter.hpp"

namespace mrs_mapf::adapter
{
using mrs_mapf::core::Status;

Status RoadmapAdapter::from_boundary(const BoundaryRoadmap& doc, core::Roadmap& out)
{
    out = core::Roadmap{};
    if (doc.nodes.empty())
    {
        return Status::kError;
    }
    for (const auto& n : doc.nodes)
    {
        out.node_ids.push_back(n.id);
        out.out_arcs[n.id] = {};
    }
    out.endpoints = doc.endpoints;

    for (const auto& e : doc.edges)
    {
        if (out.out_arcs.find(e.from) == out.out_arcs.end() ||
            out.out_arcs.find(e.to) == out.out_arcs.end())
        {
            return Status::kError;
        }
        core::RoadmapArc fwd;
        fwd.from = e.from;
        fwd.to = e.to;
        fwd.length_m = e.length_m;
        fwd.width_m = e.width_m;
        fwd.capacity_robots = e.capacity_robots;
        fwd.corridor = e.corridor;

        if (e.traversal == "bidirectional")
        {
            out.out_arcs[e.from].push_back(fwd);
            core::RoadmapArc bwd = fwd;
            bwd.from = e.to;
            bwd.to = e.from;
            out.out_arcs[e.to].push_back(bwd);
        }
        else if (e.traversal == "forward_only")
        {
            out.out_arcs[e.from].push_back(fwd);
        }
        else if (e.traversal == "backward_only")
        {
            core::RoadmapArc bwd = fwd;
            bwd.from = e.to;
            bwd.to = e.from;
            out.out_arcs[e.to].push_back(bwd);
        }
        else
        {
            return Status::kError;
        }
    }
    return Status::kOk;
}

}  // namespace mrs_mapf::adapter
