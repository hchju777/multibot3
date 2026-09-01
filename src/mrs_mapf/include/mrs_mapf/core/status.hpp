// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_STATUS_HPP
#define MRS_MAPF_CORE_STATUS_HPP

#include <mrs_core/status.hpp>

/// @file status.hpp
/// @brief Error-handling vocabulary (CN-13/CN-14/CN-15) — now an alias.
///
/// 47차 통합(P1/D15, `_workspace/347_arch_integration_delta.md` §3-2): `mrs_core`
/// has landed. The local `Status`/`Result<T,E>`/`ContractViolation` definitions
/// that used to live here are deleted; this header now only aliases the
/// canonical `mrs_core` vocabulary into `mrs_mapf::core`, textually — every
/// call site that wrote `mrs_mapf::core::Status` etc. keeps compiling
/// unchanged (D15: "using" 별칭 형태, not a rename).

namespace mrs_mapf::core
{

using mrs_core::ok;
using mrs_core::Status;

template<typename T, typename E>
using Result = mrs_core::Result<T, E>;

using mrs_core::ContractViolation;

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_STATUS_HPP
