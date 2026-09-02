// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_STOP_DECLARATIONS_ADAPTER_HPP
#define MRS_SADG_ADAPTER_STOP_DECLARATIONS_ADAPTER_HPP

#include <string>
#include <utility>
#include <vector>

#include "mrs_sadg/adapter/boundary_types.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file stop_declarations_adapter.hpp
/// @brief Converts mrs.stop_declaration 5.0.1 (boundary, upstream trajopt ->
/// sadg_t0) into the (from,to) edge pairs `ExecutionGraph::apply_blocked_delta`
/// (ET17, §11-2③) consumes, CN-8/CN-9.
///
/// 🔴 Scope note (reported, not silently narrowed): the three legal `reason`
/// values differ in whether the block is a world fact (`exogenous_block`) or
/// robot-local (`infeasible_subgoal`/`unresolvable_locally` —
/// `stop_declaration.schema.json`: "«이 로봇이» from->to 방향으로 지금 나아가지
/// 못한다 ... «그 간선이 모든 로봇에 대해 차단됐다»는 뜻이 아니다"). But
/// `ExecutionGraph::apply_blocked_delta` (`execution_graph.cpp`) already treats
/// V_blk as one flat, robot-agnostic edge set, and its only reader today is
/// `blocked_count()` — a scalar into the escalation ladder, not a per-edge or
/// per-robot query. This adapter therefore relays all three `declared:true`
/// reasons undifferentiated, matching the existing core consumer's granularity;
/// it does not invent a per-robot blocked-edge representation that no core code
/// reads yet.
///
/// 🔴 `declared:false` is NOT relayed as a removal: `apply_blocked_delta` is
/// additive-only (no removal API exists in `ExecutionGraph` today — reported as
/// a known limitation in `372_sadg_tick_wiring.md`, not fixed here, since
/// changing that core semantics is outside this wiring round's scope).

namespace mrs_sadg::adapter
{

/// @brief mrs.stop_declaration 5.0.1 -> blocked-edge-pair adapter (CN-8).
class StopDeclarationsAdapter
{
public:
    /// @brief Boundary -> internal (declared:true entries only; additive).
    /// @param msg the boundary stop-declarations document.
    /// @param out [out] (from,to) pairs to accumulate into V_blk. Appends to
    ///            any existing contents (caller drains between ticks).
    /// @return Status::kOk on success, kError if the envelope is wrong.
    static core::Status from_boundary(const BoundaryStopDeclarations& msg,
                                      std::vector<std::pair<std::string, std::string>>& out);
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_STOP_DECLARATIONS_ADAPTER_HPP
