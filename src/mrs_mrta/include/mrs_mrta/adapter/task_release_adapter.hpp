// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_TASK_RELEASE_ADAPTER_HPP
#define MRS_MRTA_ADAPTER_TASK_RELEASE_ADAPTER_HPP

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file task_release_adapter.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief `TaskReleaseAdapter` — `from_boundary` only (this module never
/// produces `mrs.task_release`, CN-9).
namespace mrs_mrta::adapter
{

class TaskReleaseAdapter
{
public:
    /// @brief Convert a boundary task-release message to an internal event.
    ///
    /// Identity copy for `goal_id`/`location`/`task` (assumption A-TA5,
    /// `336_mrta_canon_p2.md` §336-5-3) and leg order preservation ("goals의
    /// 순서가 규범이다", `task_release` 2.0.0 schema) — this adapter performs
    /// no reordering, no filtering, no derived computation.
    /// @param msg The boundary message.
    /// @param out Output internal event (fields overwritten in place).
    /// @return core::Status::kError if @p msg.goals is empty (layer-1
    ///         `minItems:1` should already forbid this upstream; this is a
    ///         defensive re-check, not a parser).
    static core::Status from_boundary(const BoundaryTaskRelease& msg, core::ReleaseEvent& out);
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_TASK_RELEASE_ADAPTER_HPP
