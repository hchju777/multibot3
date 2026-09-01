// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_REPLAN_REQUEST_ADAPTER_HPP
#define MRS_MAPF_ADAPTER_REPLAN_REQUEST_ADAPTER_HPP

#include "mrs_mapf/adapter/boundary_types.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file replan_request_adapter.hpp
/// @brief Converts mrs.replan_request 7.0.0 -> the pieces `service/` folds into
/// a `core::PlanningRequest` (from_boundary only — CN-9). `reason` and
/// `blocked_hash` are copied through for diagnostics but not branched on this
/// round (known limitation — no `LL03`-style 4/6-value reason classifier).

namespace mrs_mapf::adapter
{

/// @brief The subset of `PlanningRequest` this artifact fills.
struct ReplanRequestFields
{
    std::string reason;
    std::vector<std::string> affected_robots;
    std::vector<core::BlockedEdge> blocked;
    std::vector<core::OrderingConstraint> ordering_constraints;
    std::vector<core::BaselineEntry> baseline;
    std::uint64_t plan_revision = 0;
};

/// @brief Adapter for the mrs.replan_request boundary artifact.
class ReplanRequestAdapter
{
public:
    /// @brief Flatten the boundary replan_request into `ReplanRequestFields`.
    /// @param doc the boundary replan_request document.
    /// @param out the flattened fields.
    /// @return Status::kOk always (an empty `affected_robots`/`blocked` is legal per §6).
    static core::Status from_boundary(const BoundaryReplanRequest& doc, ReplanRequestFields& out);
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_REPLAN_REQUEST_ADAPTER_HPP
