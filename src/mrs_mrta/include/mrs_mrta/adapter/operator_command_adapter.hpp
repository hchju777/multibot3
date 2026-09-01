// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_OPERATOR_COMMAND_ADAPTER_HPP
#define MRS_MRTA_ADAPTER_OPERATOR_COMMAND_ADAPTER_HPP

#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file operator_command_adapter.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief `OperatorCommandAdapter` — `from_boundary` only
/// (`341_arch_mrta_addendum.md` §341-4: "mrta는 이 채널을 소비만 한다").
namespace mrs_mrta::adapter
{

class OperatorCommandAdapter
{
public:
    /// @brief Convert one `goal_override` command to an internal event.
    ///
    /// The `verb != goal_override` filter is the *caller's* job
    /// (`336b_goal_override_design.md` §2-1: "필터가 service/ 층에 선다") —
    /// this function assumes it has already been applied and returns an
    /// error if it was not, rather than silently ignoring the mismatch.
    /// @param msg One element of `operator_command.commands[]`.
    /// @param out Output internal event (fields overwritten in place).
    /// @return core::Status::kError if @p msg.verb != "goal_override", or if
    ///         @p msg.active is true but @p msg.goal_location is the wire
    ///         absence sentinel `""` (layer-2 conditional-required, schema
    ///         x-layer2-note (2); `""` is the same absence convention the
    ///         schema uses elsewhere, e.g. `roadmap.edges[].corridor` — not
    ///         an internal presence flag, F46-1 fix).
    static core::Status from_boundary(const BoundaryOperatorCommand& msg, core::OperatorEvent& out);
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_OPERATOR_COMMAND_ADAPTER_HPP
