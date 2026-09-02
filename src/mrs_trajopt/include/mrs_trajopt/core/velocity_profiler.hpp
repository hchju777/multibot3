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
///
/// 🔴 368_p8 (`_workspace/370_curvature_sample_convention.md`, applying the
/// `math-formalist`'s confirmed convention `CVN-1..CVN-6`): curvature,
/// yaw-rate, AND linear speed are now **edge quantities** — edge e (between
/// node e and node e+1) holds a SINGLE constant (yr_e, v_hold_e) for its
/// entire duration [t_e, t_{e+1}), matching exactly what `CT15`'s
/// zero-order-hold sampler (`TrajectoryBuffer::sample_at`) actually executes
/// (368_p7's isolated diagnosis: the OLD node-centered central-difference
/// computed a turn assuming it would be integrated over TWO edges' arc
/// length, but ZOH only ever applies ONE edge's worth of it — chronically
/// under-rotating, worse as the required turn grows, up to full divergence).
/// `T-CVN1`/`T-CVN2` (370§5) prove `yr_e·Δt_e = Δyaw_e` (and the same for
/// arc length/`v_hold_e`) holds unconditionally — "정의 구간 = 적용 구간" —
/// for exactly this form, and ONLY this form (up to the representation
/// choice A≡C, 370§1-3). Curvature/yaw-rate caps are honored *by
/// construction*: a node's speed is capped so BOTH adjacent edges' curvature
/// respect omega_max (a conservative, per-node sufficient condition, 370§10-1
/// — the exact edge-pair joint cap is not implemented, this is deliberately
/// stronger/safer, not the tight bound).

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
    /// Trapezoidal forward/back sweep (on an internal per-NODE speed array)
    /// bounded by the per-robot limits, unchanged since 368_p4/p6 — this
    /// still models how fast the robot can physically ramp node-to-node.
    /// 🔴 368_p8: what changes is the OUTPUT. Edge e's held yaw-rate/speed
    /// (`CVN-1..3`) are `yr_e = crv_e·v_hold_e` and `v_hold_e = plen_e/Δt_e`,
    /// where `crv_e = Δyaw_e/plen_e` is the edge's own (forward-difference,
    /// chord-approximated per `CVN-4`) signed curvature and `v_hold_e`
    /// reduces to the SAME `vbar_e = 0.5·(v[e]+v[e+1])` already used to
    /// integrate `Δt_e = plen_e/vbar_e` (370§8-2 confirms this numerically:
    /// `v_hold_0 = 0.5/3.9453 = 0.1267 = vbar_0` exactly). Node i (i<n-1)
    /// carries edge i's held values (representation A, 370§6-1); the LAST
    /// node carries yr=0, v=0 (370§6-1 candidate A's ruling on "the
    /// undefined last-node value": there is no outgoing edge, so nothing is
    /// held — this is also already what an anchor goal node needs). A
    /// braking tail (length ⊇ v^2/(2*brake_decel)) is appended so the chain
    /// ends at v=0 (INV-2-GEO(B)). Reverse edges keep v<0 internally; if
    /// @p lim.reverse_motion_allowed is false, a reverse edge yields
    /// ok=false (INV-2-GEO(C)).
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
    /// @brief Signed EDGE curvature crv_e = Δyaw_e/plen_e (368_p8, `CVN-1`+
    /// `CVN-4`) for edge e (between node e and node e+1), forward difference,
    /// chord-length approximation of plen_e (370§7 `CVN-4`'s explicit
    /// "conservative/safe" choice — over-reports curvature, never hides a
    /// violation; do not switch to the arc-length form without also
    /// revisiting `check_traj_limits.py`, which uses the same chord form).
    /// 0 if edge e has zero chord length (degenerate). e must be < path.size()-1.
    /// @param path the geometric chain.
    /// @param e the edge index (0 .. path.size()-2).
    /// @return the signed curvature of edge e [1/m].
    static double edge_curvature(const std::vector<GeometricNode>& path, std::size_t e);
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_VELOCITY_PROFILER_HPP
