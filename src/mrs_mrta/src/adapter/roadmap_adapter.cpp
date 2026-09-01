// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/adapter/roadmap_adapter.hpp"

namespace mrs_mrta::adapter
{

core::Status RoadmapAdapter::from_boundary(const BoundaryRoadmap& msg, core::RoadmapView& out)
{
    if (msg.node_ids.empty())
    {
        return core::Status::kError;
    }
    out.node_ids = msg.node_ids;
    out.endpoints = msg.endpoints;
    return core::Status::kOk;
}

}  // namespace mrs_mrta::adapter
