// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_CONSTRAINTS_ADAPTER_HPP
#define MRS_SADG_ADAPTER_CONSTRAINTS_ADAPTER_HPP

#include "mrs_sadg/adapter/boundary_types.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file constraints_adapter.hpp
/// @brief Converts ExecutionGraph (internal) -> mrs.execution_constraints 3.0.0
/// (boundary), CN-8/CN-9. Only the graph's segments, fixed dependencies and
/// switch groups cross — never the release bits, Rz, budgets, theta_gain or any
/// solver state (those are internal / config; check_schema_purity.py guards it).

namespace mrs_sadg::adapter
{

/// @brief ExecutionGraph -> mrs.execution_constraints 3.0.0 adapter (CN-8).
class ConstraintsAdapter
{
public:
    /// @brief Internal -> boundary.
    /// @param g the compiled execution graph.
    /// @param out [out] the boundary execution-constraints document.
    /// @return Status::kOk on success.
    static core::Status to_boundary(const core::ExecutionGraph& g,
                                    BoundaryExecutionConstraints& out);
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_CONSTRAINTS_ADAPTER_HPP
