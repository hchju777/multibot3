// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_ADAPTER_ROBOT_SPECS_ADAPTER_HPP
#define MRS_TRAJOPT_ADAPTER_ROBOT_SPECS_ADAPTER_HPP

#include <vector>

#include "mrs_trajopt/adapter/boundary_types.hpp"
#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"

/// @file robot_specs_adapter.hpp
/// @brief Converts mrs.robot_specs 1.1.0 -> internal RobotLimits (from_boundary).
///
/// Fills the per-robot six fields SU08 consumes (A33: only trajopt reads these).
/// Only the `to_boundary` direction is omitted — this module consumes robot_specs
/// and never produces it (CN-9 "필요한 방향만 구현").

namespace mrs_trajopt::adapter
{

/// @brief Adapter for the mrs.robot_specs boundary artifact.
class RobotSpecsAdapter
{
public:
    /// @brief Fill per-robot limits from the boundary robot_specs (from_boundary).
    /// @param doc the boundary robot_specs document.
    /// @param out one RobotLimits per robot (the six robot-scoped fields).
    /// @return Status::kOk on success (kError never returned here; validity is
    ///         asserted by StartupChecks, not the adapter).
    static core::Status from_boundary(const BoundaryRobotSpecs& doc,
                                      std::vector<core::RobotLimits>& out);
};

}  // namespace mrs_trajopt::adapter

#endif  // MRS_TRAJOPT_ADAPTER_ROBOT_SPECS_ADAPTER_HPP
