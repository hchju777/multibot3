// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_ROBOT_SPECS_ADAPTER_HPP
#define MRS_MAPF_ADAPTER_ROBOT_SPECS_ADAPTER_HPP

#include <vector>

#include "mrs_mapf/adapter/boundary_types.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file robot_specs_adapter.hpp
/// @brief Converts mrs.robot_specs 1.1.0 -> internal RobotSpec (from_boundary
/// only — CN-9). A33: only `robot` and `avg_traversal_speed_mps` are copied
/// into the internal type; the other five fields are legal for
/// `BoundaryRobotSpec` to hold (round-trip fidelity in tests) but never reach
/// `core::RobotSpec`.

namespace mrs_mapf::adapter
{

/// @brief Adapter for the mrs.robot_specs boundary artifact.
class RobotSpecsAdapter
{
public:
    /// @brief Fill the internal per-robot speed roster.
    /// @param doc the boundary robot_specs document.
    /// @param out one `RobotSpec` per robot (`robot`, `avg_traversal_speed_mps`).
    /// @return Status::kOk on success; Status::kError if @p doc.robots is empty.
    static core::Status from_boundary(const BoundaryRobotSpecs& doc,
                                      std::vector<core::RobotSpec>& out);
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_ROBOT_SPECS_ADAPTER_HPP
