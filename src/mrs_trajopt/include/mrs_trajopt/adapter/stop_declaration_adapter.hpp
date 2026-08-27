// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_ADAPTER_STOP_DECLARATION_ADAPTER_HPP
#define MRS_TRAJOPT_ADAPTER_STOP_DECLARATION_ADAPTER_HPP

#include <string>
#include <vector>

#include "mrs_trajopt/adapter/boundary_types.hpp"
#include "mrs_trajopt/core/declaration_regulator.hpp"
#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"

/// @file stop_declaration_adapter.hpp
/// @brief Converts internal declarations -> mrs.stop_declaration 5.0.0 (to_boundary).
///
/// The only upward channel (고정 결정 4). This module produces stop_declaration
/// and never consumes it, so only `to_boundary` is implemented (CN-9). The
/// reason string is the schema enum; the reverse=false case is
/// `infeasible_subgoal` (고정 결정 2).

namespace mrs_trajopt::adapter
{

/// @brief An internal stop declaration to be projected to the boundary.
struct InternalDeclaration
{
    std::uint64_t seq = 0;     ///< per-robot ordinal.
    std::string robot;         ///< robot identifier.
    std::string blocked_from;  ///< blocked edge `from` node id.
    std::string blocked_to;    ///< blocked edge `to` node id.
    bool declared = false;     ///< open/close.
    core::StopReason reason = core::StopReason::kUnresolvableLocally;  ///< classified reason.
};

/// @brief Adapter for the mrs.stop_declaration boundary artifact.
class StopDeclarationAdapter
{
public:
    /// @brief Project internal declarations to the boundary (to_boundary).
    /// @param decls the internal declarations.
    /// @param instance_id the instance identifier.
    /// @param out the filled boundary document (reason rendered via schema enum).
    /// @return Status::kOk on success.
    static core::Status to_boundary(const std::vector<InternalDeclaration>& decls,
                                    const std::string& instance_id,
                                    BoundaryStopDeclarations& out);
};

}  // namespace mrs_trajopt::adapter

#endif  // MRS_TRAJOPT_ADAPTER_STOP_DECLARATION_ADAPTER_HPP
