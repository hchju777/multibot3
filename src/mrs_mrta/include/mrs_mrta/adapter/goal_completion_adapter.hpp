// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_GOAL_COMPLETION_ADAPTER_HPP
#define MRS_MRTA_ADAPTER_GOAL_COMPLETION_ADAPTER_HPP

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file goal_completion_adapter.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief `GoalCompletionAdapter` — `from_boundary` only (this module never
/// produces `mrs.goal_completion`; its producer is
/// `mrs_sim/completion_monitor_node`, `341_arch_mrta_addendum.md` §341-6).
namespace mrs_mrta::adapter
{

class GoalCompletionAdapter
{
public:
    /// @brief Convert a boundary completion event to an internal event.
    /// @param msg The boundary message.
    /// @param out Output internal event (fields overwritten in place).
    /// @return core::Status::kOk always (no fields are conditionally absent).
    static core::Status from_boundary(const BoundaryGoalCompletion& msg,
                                      core::CompletionEvent& out);
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_GOAL_COMPLETION_ADAPTER_HPP
