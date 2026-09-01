// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_ROADMAP_ADAPTER_HPP
#define MRS_MRTA_ADAPTER_ROADMAP_ADAPTER_HPP

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file roadmap_adapter.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief `RoadmapAdapter` — `from_boundary` only. This module never
/// produces a roadmap (CN-9).
///
/// 🔴 **Known limitation**: `12a_arch_mrta.md` §8-3 places the shared
/// distance table in `mrs_core` (absent from this checkout) — this adapter
/// only extracts the node/endpoint id sets a startup check (`C-10`/`C-11`)
/// would use. It is not wired into `service::AssignmentService` this round
/// (that wiring is `node/`'s, which is a skeleton, brief §1) — see the
/// implementation notes' "알려진 한계".
namespace mrs_mrta::adapter
{

class RoadmapAdapter
{
public:
    /// @brief Convert a boundary roadmap message to the internal read view.
    /// @param msg The boundary message (only `node_ids`/`endpoints` are read).
    /// @param out Output internal view (fields overwritten in place).
    /// @return core::Status::kError if @p msg.node_ids is empty.
    static core::Status from_boundary(const BoundaryRoadmap& msg, core::RoadmapView& out);
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_ROADMAP_ADAPTER_HPP
