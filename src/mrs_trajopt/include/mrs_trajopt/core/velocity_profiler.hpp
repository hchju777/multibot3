// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_VELOCITY_PROFILER_HPP
#define MRS_TRAJOPT_CORE_VELOCITY_PROFILER_HPP

#include <vector>

#include "mrs_trajopt/core/types.hpp"

/// @file velocity_profiler.hpp
/// @brief Speed/time parameterization of a fixed geometric path (321b §3-4).
///
/// A **core class**, not a 5th extension point (K25) — promotion is an
/// algorithm decision left to 321a. Input is the search's geometric chain
/// (poses + anchor flags); output is the full state chain ③ (t,x,y,theta,v,
/// omega) plus a braking tail (RQ5-10, U42-9). INV-2-GEO binds here (221-P7):
///   (A) tube radius ⊇ heading-error arc excursion,
///   (B) braking tail ⊇ over-speed braking distance,
///   (C) reverse=false => forward arcs only.
/// Curvature and yaw-rate caps are honored *by construction*: at each node the
/// speed is capped at min(v_max, omega_max/|kappa|), so the produced omega
/// never exceeds omega_max.

namespace mrs_trajopt::core
{

/// @brief One waypoint of the geometric search chain fed to the profiler.
struct GeometricNode
{
    Pose2 pose;            ///< position + heading.
    bool anchor = false;   ///< true iff a v=0 marking (AnchorSet member).
    bool reverse = false;  ///< true iff this edge is traversed in reverse.
};

/// @brief Result of parameterization.
struct ProfileResult
{
    std::vector<StateSample> chain;   ///< the full state chain including braking tail.
    bool ok = false;                  ///< false iff the path was empty / degenerate.
    double braking_tail_len_m = 0.0;  ///< length of the appended braking tail [m].
    /// true iff the yaw-accel relaxation reached a fixpoint within the iteration
    /// cap. false => the profile may retain a small yaw-accel excess (residual,
    /// analysis-only) — never emit such a chain without noting it (20d L-list).
    bool yaw_accel_relaxation_converged = true;
};

/// @brief Turns a geometric path into a timed, speed-limited state chain.
class VelocityProfiler
{
public:
    /// @brief Parameterize a geometric chain into a full state chain (TT04).
    ///
    /// Trapezoidal forward/back sweep bounded by the per-robot limits. Speed at
    /// each node is capped at min(v_max, omega_max/|kappa|); accel/decel bounded
    /// by a_max; a braking tail (length ⊇ v^2/(2*brake_decel)) is appended so
    /// the chain ends at v=0 (INV-2-GEO(B)). Reverse edges keep v<0 internally;
    /// if @p lim.reverse_motion_allowed is false, a reverse edge yields ok=false
    /// (INV-2-GEO(C)).
    /// @param path the geometric chain (>= 2 nodes for a non-degenerate result).
    /// @param lim per-robot limits (v_max, brake_decel, curvature, reverse).
    /// @param fleet fleet-folded limits (a_max for accel bound; omega_max cap).
    /// @param start_v initial signed speed [m/s] at the first node.
    /// @return the parameterized chain; ok=false on empty/degenerate/forbidden-reverse.
    ProfileResult parameterize(const std::vector<GeometricNode>& path,
                               const RobotLimits& lim,
                               const FleetLimits& fleet,
                               double start_v) const;

private:
    /// @brief Signed path curvature kappa at node i (|dtheta|/ds), 0 at ends.
    static double node_curvature(const std::vector<GeometricNode>& path, std::size_t i);
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_VELOCITY_PROFILER_HPP
