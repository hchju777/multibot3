// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_ADAPTER_BOUNDARY_TYPES_HPP
#define MRS_TRAJOPT_ADAPTER_BOUNDARY_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file boundary_types.hpp
/// @brief Plain structs mirroring the frozen boundary schemas (contract-v1.0.0).
///
/// These are the ONLY place internal types meet the boundary shape. They carry
/// exactly the schema fields — no solver cost, timestep, k_robustness, horizon,
/// seed, or timeout leaks here (invariant 1). JSON (de)serialization lives in
/// `io/`, not here, so `adapter/` stays free of a JSON dependency.

namespace mrs_trajopt::adapter
{

/// @brief One point of mrs.trajectories 2.0.0 (t,x,y,yaw required; v,a optional).
struct BoundaryTrajectoryPoint
{
    double t = 0.0;      ///< time from run start [s] (strict-ascending).
    double x = 0.0;      ///< map x [m].
    double y = 0.0;      ///< map y [m].
    double yaw = 0.0;    ///< heading [rad], (-pi, pi].
    bool has_v = false;  ///< is v present? (mirrors x-ros-optional-flag has_v).
    double v = 0.0;      ///< reported speed MAGNITUDE [m/s] (>= 0). No sign (§3-2).
    bool has_a = false;  ///< is a present? (mirrors has_a).
    double a = 0.0;      ///< reported accel MAGNITUDE [m/s^2] (>= 0). No sign.
};

/// @brief One robot's boundary trajectory.
struct BoundaryTrajectory
{
    std::string robot;                            ///< robot identifier.
    std::vector<BoundaryTrajectoryPoint> points;  ///< time-ordered points.
};

/// @brief mrs.trajectories 2.0.0 limits block (fleet-folded set).
struct BoundaryLimits
{
    double a_max = 0.0;
    double j_max = 0.0;
    double min_separation_m = 0.0;
    double omega_max_radps = 0.0;
    double yaw_accel_max_radps2 = 0.0;
};

/// @brief mrs.trajectories 2.0.0 document.
struct BoundaryTrajectories
{
    std::string instance_id;
    BoundaryLimits limits;
    std::vector<BoundaryTrajectory> trajectories;
};

/// @brief One robot spec of mrs.robot_specs 1.1.0 (the seven required fields).
struct BoundaryRobotSpec
{
    std::string robot;
    double avg_traversal_speed_mps = 0.0;
    double v_max = 0.0;
    double brake_decel_min_mps2 = 0.0;
    double curvature_max_invm = 0.0;
    bool reverse_motion_allowed = false;
    double circumradius_m = 0.0;
};

/// @brief mrs.robot_specs 1.1.0 document.
struct BoundaryRobotSpecs
{
    std::string instance_id;
    std::vector<BoundaryRobotSpec> robots;
};

/// @brief One directed blocked edge of mrs.stop_declaration 5.0.1.
struct BoundaryBlockedEdge
{
    std::string from;  ///< roadmap node id.
    std::string to;    ///< roadmap node id.
};

/// @brief One declaration of mrs.stop_declaration 5.0.1.
struct BoundaryStopDeclaration
{
    std::uint64_t seq = 0;             ///< per-robot ordinal.
    std::string robot;                 ///< robot identifier.
    BoundaryBlockedEdge blocked_edge;  ///< the blocked directed edge.
    bool declared = false;             ///< open/close.
    std::string reason;  ///< one of exogenous_block/infeasible_subgoal/unresolvable_locally.
};

/// @brief mrs.stop_declaration 5.0.1 document.
struct BoundaryStopDeclarations
{
    std::string instance_id;
    std::vector<BoundaryStopDeclaration> declarations;
};

}  // namespace mrs_trajopt::adapter

#endif  // MRS_TRAJOPT_ADAPTER_BOUNDARY_TYPES_HPP
