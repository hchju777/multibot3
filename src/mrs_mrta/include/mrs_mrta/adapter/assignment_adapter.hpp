// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_ASSIGNMENT_ADAPTER_HPP
#define MRS_MRTA_ADAPTER_ASSIGNMENT_ADAPTER_HPP

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file assignment_adapter.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief `AssignmentAdapter` — `to_boundary` only (CN-8/CN-9).
///
/// `mrs_mrta` never *consumes* `mrs.assignment` (it is this module's only
/// produced artifact, `336_mrta_canon.md` §336-0 #1) — so there is no
/// `from_boundary` here, per CN-9 "필요한 방향만 구현해도 된다".
namespace mrs_mrta::adapter
{

class AssignmentAdapter
{
public:
    /// @brief Convert an internal read-only view to the boundary shape.
    ///
    /// Fills `revision`, `assignments`, and `unassigned_tasks` only — the
    /// envelope fields (`schema`, `schema_version`, `instance_id`) are left
    /// at whatever @p out already holds (the contract's `instance_id` is
    /// itself an open item, x-todo B-9 in the schema; it is not something
    /// this internal view carries, `12a_arch_mrta.md` §4 근거 3).
    /// @param view The internal view (`service::AssignmentService::build_view`).
    /// @param out Output boundary struct (fields overwritten in place).
    /// @return core::Status::kOk always — this direction cannot fail on a
    ///         well-formed @p view (no parsing, only field copying).
    static core::Status to_boundary(const core::AssignmentView& view, BoundaryAssignment& out);
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_ASSIGNMENT_ADAPTER_HPP
