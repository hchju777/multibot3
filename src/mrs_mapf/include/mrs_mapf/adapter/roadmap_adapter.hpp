// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_ROADMAP_ADAPTER_HPP
#define MRS_MAPF_ADAPTER_ROADMAP_ADAPTER_HPP

#include "mrs_mapf/adapter/boundary_types.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file roadmap_adapter.hpp
/// @brief Converts mrs.roadmap 3.0.0 -> internal Roadmap (from_boundary only —
/// this module consumes roadmap and never produces it, CN-9).

namespace mrs_mapf::adapter
{

/// @brief Adapter for the mrs.roadmap boundary artifact.
class RoadmapAdapter
{
public:
    /// @brief Build the internal directed-arc graph from the boundary roadmap.
    /// A `bidirectional` edge expands into two `RoadmapArc`s (one per
    /// direction); `forward_only`/`backward_only` expand into one.
    /// @param doc the boundary roadmap document.
    /// @param out the internal roadmap graph.
    /// @return Status::kError if @p doc.nodes is empty or an edge references an
    ///   undeclared node or an unrecognised `traversal` value; Status::kOk otherwise.
    static core::Status from_boundary(const BoundaryRoadmap& doc, core::Roadmap& out);
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_ROADMAP_ADAPTER_HPP
