// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_PLAN_ADAPTER_HPP
#define MRS_SADG_ADAPTER_PLAN_ADAPTER_HPP

#include "mrs_sadg/adapter/boundary_types.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/core/types.hpp"

/// @file plan_adapter.hpp
/// @brief Converts mrs.discrete_plan 4.0.0 (boundary) <-> PlanView (internal),
/// CN-8/CN-9. The H-cons intake predicate (IN01) is applied at compile time by
/// ConstraintCompiler::hcons_ok; this adapter is a lossless representation
/// conversion (round-trip tested).

namespace mrs_sadg::adapter
{

/// @brief mrs.discrete_plan 4.0.0 <-> PlanView adapter (CN-8).
class PlanAdapter
{
public:
    /// @brief Boundary -> internal.
    /// @param msg the boundary discrete plan.
    /// @param out [out] the internal plan view.
    /// @return Status::kOk on success, kError if the envelope is wrong.
    static core::Status from_boundary(const BoundaryDiscretePlan& msg, core::PlanView& out);

    /// @brief Internal -> boundary (used by the round-trip test).
    /// @param view the internal plan view.
    /// @param out [out] the boundary discrete plan.
    /// @return Status::kOk on success.
    static core::Status to_boundary(const core::PlanView& view, BoundaryDiscretePlan& out);
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_PLAN_ADAPTER_HPP
