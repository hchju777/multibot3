// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_RELEASE_ADAPTER_HPP
#define MRS_SADG_ADAPTER_RELEASE_ADAPTER_HPP

#include <string>

#include "mrs_sadg/adapter/boundary_types.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file release_adapter.hpp
/// @brief Converts ExecutionGraph (internal) -> mrs.segment_release 2.0.0
/// (boundary, ET33), CN-8/CN-9. One artifact = one robot
/// (`segment_release.schema.json`: "한 아티팩트 = 한 로봇이다"). Only the
/// `to_boundary` direction is implemented — SADG produces this artifact and
/// never consumes it back (CN-9 permits implementing only the needed direction).
///
/// 🔴 `commit_seq` is deliberately NOT filled here, unlike `ConstraintsAdapter`
/// (which sets it from `g.commit_seq()`, immediately overwritten by the node's
/// own persistent per-compile counter — see `sadg_t0_node.cpp`). The schema
/// requires `segment_release.commit_seq` to echo `mrs.execution_constraints.
/// commit_seq` for the SAME plan revision ("이 비트들이 어느 제약 판본 위의
/// 것인가... 그 값을 되돌려 싣는다"). `ExecutionGraph::commit_seq()` is a
/// DIFFERENT counter space — it only bumps on an accepted reorder
/// (`commit_gate`/`ET12`, `execution_tick_service.cpp`) — so using it here
/// would silently conflate two disjoint sequence spaces. `commit_seq` (like
/// `instance_id`/`release_seq`/`entry_seq_ack`) is therefore the caller's
/// node-owned bookkeeping.
namespace mrs_sadg::adapter
{

/// @brief ExecutionGraph -> mrs.segment_release 2.0.0 adapter (CN-8), one robot.
class ReleaseAdapter
{
public:
    /// @brief Internal -> boundary, for one robot's segments.
    /// @param g the execution graph (read-only).
    /// @param robot the robot this artifact is for (one artifact = one robot).
    /// @param out [out] `schema`/`schema_version`/`robot`/`segments` filled
    ///            here (`segments` is replaced, not appended to);
    ///            `instance_id`/`release_seq`/`commit_seq`/`entry_seq_ack` are
    ///            the caller's node-owned counters and are left untouched.
    /// @return Status::kOk on success.
    static core::Status to_boundary(const core::ExecutionGraph& g,
                                    const std::string& robot,
                                    BoundarySegmentRelease& out);
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_RELEASE_ADAPTER_HPP
