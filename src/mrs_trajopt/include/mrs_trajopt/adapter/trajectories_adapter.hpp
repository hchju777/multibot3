// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_ADAPTER_TRAJECTORIES_ADAPTER_HPP
#define MRS_TRAJOPT_ADAPTER_TRAJECTORIES_ADAPTER_HPP

#include <string>
#include <vector>

#include "mrs_trajopt/adapter/boundary_types.hpp"
#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"

/// @file trajectories_adapter.hpp
/// @brief Converts the internal state chain ③ <-> mrs.trajectories 2.0.0 (CN-8/9).
///
/// 🔴 The down-projection is the physical realization of module replaceability
/// (CT26, orchestrator disposition ⑵): internal (t,x,y,theta,v,omega) ->
/// boundary (t,x,y,yaw,+v,a). omega is DROPPED at the boundary and re-derived by
/// finite difference on consumption. v becomes a magnitude (sign is internal
/// only, §3-2). This adapter is the single seam; the algorithm core never
/// touches BoundaryTrajectory*.

namespace mrs_trajopt::adapter
{

/// @brief Adapter for the mrs.trajectories boundary artifact.
class TrajectoriesAdapter
{
public:
    /// @brief Project internal state chains down to the boundary (to_boundary).
    ///
    /// theta -> yaw; signed internal v -> |v| magnitude (has_v = true); a is
    /// derived by finite difference of |v| and reported as a magnitude
    /// (has_a = true); omega is NOT emitted (down-projection, ⑵). No solver
    /// parameter is written — purity by construction.
    /// @param chains internal state chains, one per robot.
    /// @param robots the robot identifier for each chain (same length).
    /// @param fleet fleet-folded limits for the boundary `limits` block.
    /// @param instance_id the instance identifier.
    /// @param out the filled boundary document.
    /// @return Status::kOk, or kError if @p chains and @p robots differ in length.
    static core::Status to_boundary(const std::vector<std::vector<core::StateSample>>& chains,
                                    const std::vector<std::string>& robots,
                                    const core::FleetLimits& fleet,
                                    const std::string& instance_id,
                                    BoundaryTrajectories& out);

    /// @brief Reconstruct internal state chains from the boundary (from_boundary).
    ///
    /// yaw -> theta; time/position copied exactly; v sign recovered by projecting
    /// displacement on heading (the checker's method, §3-2); omega re-derived by
    /// finite difference of yaw. This is the inverse used by the roundtrip test:
    /// t/x/y/theta and |v| are preserved exactly; sign and omega are
    /// reconstructed (they are intentionally not on the boundary).
    /// @param doc the boundary document.
    /// @param out_chains reconstructed chains, one per robot.
    /// @param out_robots the robot identifier for each chain.
    /// @return Status::kOk on success.
    static core::Status from_boundary(const BoundaryTrajectories& doc,
                                      std::vector<std::vector<core::StateSample>>& out_chains,
                                      std::vector<std::string>& out_robots);
};

}  // namespace mrs_trajopt::adapter

#endif  // MRS_TRAJOPT_ADAPTER_TRAJECTORIES_ADAPTER_HPP
