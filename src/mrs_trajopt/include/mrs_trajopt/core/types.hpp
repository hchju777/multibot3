// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_TYPES_HPP
#define MRS_TRAJOPT_CORE_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file types.hpp
/// @brief Internal representations for the trajopt core. **These are free** —
/// the boundary schema (`mrs.trajectories` etc.) is reached only through
/// `adapter/`. Nothing here appears on a boundary artifact (invariant 6).
///
/// Granularity vocabulary (U42-9, RQ5-1): three distinct types keep the three
/// grains apart so "노드" is never used unqualified —
///   ① subgoal   = PassWindow (a gate, no speed)          — 몇 m
///   ② search node = SearchNode (geometry only, no speed) — 수십 cm (search/)
///   ③ state chain = StateSample (t,x,y,theta,v,omega)    — time samples, speed

namespace mrs_trajopt::core
{

/// @brief A planar pose (position + heading) in the shared map frame.
struct Pose2
{
    double x = 0.0;      ///< map x [m].
    double y = 0.0;      ///< map y [m].
    double theta = 0.0;  ///< heading [rad], (-pi, pi].
};

/// @brief One sample of the internal state chain ③ (RQ5-10, U42-9).
///
/// The **full** internal form is (t, x, y, theta, v, omega). The boundary
/// down-projection (t, x, y, yaw, +v, a) drops omega — the adapter derives it
/// back by finite difference (orchestrator disposition ⑵, CT26). See
/// TrajectoriesAdapter.
struct StateSample
{
    double t = 0.0;       ///< time from run start [s], strictly ascending.
    double x = 0.0;       ///< map x [m].
    double y = 0.0;       ///< map y [m].
    double theta = 0.0;   ///< heading [rad].
    double v = 0.0;       ///< signed body speed [m/s]. NEGATIVE = reverse (internal only).
    double omega = 0.0;   ///< yaw rate [rad/s].
    bool anchor = false;  ///< true iff this sample is a v=0 anchor node (AnchorSet member).
};

/// @brief Per-robot physical limits, filled by RobotSpecsAdapter::from_boundary.
///
/// The **six** robot-scoped fields SU08 requires. Fleet-folded values
/// (a_max, j_max, omega_max, yaw_accel_max, min_separation_m) live in
/// FleetLimits — kept apart because folding a per-robot quantity would let a
/// slow robot's brake distance conservatize the whole fleet (robot_specs :65).
struct RobotLimits
{
    std::string robot;                    ///< robot identifier (world fact).
    double v_max = 0.0;                   ///< speed cap [m/s], magnitude.
    double brake_decel_min_mps2 = 0.0;    ///< guaranteed-min braking decel lower bound [m/s^2].
    double curvature_max_invm = 0.0;      ///< path-curvature cap [1/m].
    bool reverse_motion_allowed = false;  ///< reverse permitted?
    double avg_traversal_speed_mps =
        0.0;                      ///< nominal average speed [m/s] (not consumed by profiler).
    double circumradius_m = 0.0;  ///< circumscribed radius [m] (occupancy-distance derivation).
};

/// @brief Fleet-folded physical limits (mrs.trajectories.limits, one set).
struct FleetLimits
{
    double a_max = 0.0;                 ///< accel cap [m/s^2], fleet-min.
    double j_max = 0.0;                 ///< jerk cap [m/s^3], fleet-min.
    double omega_max_radps = 0.0;       ///< yaw-rate cap [rad/s], fleet-min.
    double yaw_accel_max_radps2 = 0.0;  ///< yaw-accel cap [rad/s^2], fleet-min.
    double min_separation_m = 0.0;      ///< pairwise separation lower bound [m], fleet-max.
};

/// @brief Subgoal = a pass window ① (PassWindow twin, RQ5-8). **Not an anchor.**
struct PassWindow
{
    Pose2 center;               ///< nominal center pose.
    double radius_win_m = 0.0;  ///< position tolerance radius r^win_g [m].
    double theta_lo = 0.0;      ///< heading window lower bound [rad].
    double theta_hi = 0.0;      ///< heading window upper bound [rad].
};

/// @brief The published promise tube: a swept region + a heading corridor.
///
/// Used as a **hard constraint** in the trajectory tick (TT01): the new
/// trajectory must lie inside it, which makes the previous publication an
/// always-available fallback (INV-2 a1). Represented as the centerline state
/// chain plus a lateral half-width.
struct Tube
{
    std::vector<StateSample> centerline;  ///< the previously published state chain.
    double half_width_m = 0.0;            ///< lateral half-width [m].

    bool empty() const
    {
        return centerline.empty();
    }
};

/// @brief Ordinal age of a peer publication (TR-N1). **Never seconds** (221-W2).
struct PeerAge
{
    std::int64_t delta_k = 0;   ///< control-tick difference (ordinal).
    std::int64_t delta_rd = 0;  ///< round difference (ordinal).
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_TYPES_HPP
