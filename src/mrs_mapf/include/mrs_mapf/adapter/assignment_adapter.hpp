// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_ASSIGNMENT_ADAPTER_HPP
#define MRS_MAPF_ADAPTER_ASSIGNMENT_ADAPTER_HPP

#include <vector>

#include "mrs_mapf/adapter/boundary_types.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file assignment_adapter.hpp
/// @brief Converts mrs.assignment 2.0.0 -> internal AssignmentEntry roster
/// (from_boundary only — this module consumes assignment and never produces
/// it, CN-9).

namespace mrs_mapf::adapter
{

/// @brief Adapter for the mrs.assignment boundary artifact.
class AssignmentAdapter
{
public:
    /// @brief Flatten each robot's assignment into `(robot, start, goal locations)`.
    /// `task`/`goal_id` are dropped (this module needs only the ordered
    /// location sequence — §255-1: the module has no cost function over tasks).
    /// @param doc the boundary assignment document.
    /// @param out one AssignmentEntry per robot, in `doc.assignments` order
    ///   (order carries no meaning per §5-3 — callers must not rely on it).
    /// @return Status::kOk always (an empty roster is a legal, if degenerate, input).
    static core::Status from_boundary(const BoundaryAssignment& doc,
                                      std::vector<core::AssignmentEntry>& out);
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_ASSIGNMENT_ADAPTER_HPP
