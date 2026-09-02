// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_SERVICE_CONTROL_TICK_SERVICE_HPP
#define MRS_TRAJOPT_SERVICE_CONTROL_TICK_SERVICE_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "mrs_core/i_steady_clock.hpp"
#include "mrs_trajopt/core/declaration_regulator.hpp"
#include "mrs_trajopt/core/i_peer_channel.hpp"
#include "mrs_trajopt/core/i_subgoal_candidates.hpp"
#include "mrs_trajopt/core/i_trajectory_search.hpp"
#include "mrs_trajopt/core/instrumentation.hpp"
#include "mrs_trajopt/core/round_ledger.hpp"
#include "mrs_trajopt/core/safety_monitor.hpp"
#include "mrs_trajopt/core/startup_checks.hpp"
#include "mrs_trajopt/core/three_clock_gate.hpp"
#include "mrs_trajopt/core/trajectory_buffer.hpp"
#include "mrs_trajopt/core/types.hpp"
#include "mrs_trajopt/core/velocity_profiler.hpp"

/// @file control_tick_service.hpp
/// @brief The three-clock orchestrator (322 §322-1..3). One robot's onboard
/// service: control tick (50 ms) ⊃ trajectory tick (n^traj) ⊃ subgoal tick (n^re).
///
/// 🔴 SIMPLIFIED where a full algorithm is out of scope (round back-off,
/// check-recheck gate) — those places are marked in code and in 20d "알려진
/// 한계". The structure (gates, INV-2 hold, reverse-infeasible declaration,
/// puncture advance) is faithful and testable. This is a `service/` class
/// (CN-23 Application layer, the tick use case): no ROS, no JSON, no boundary
/// types (the adapter is called by the node layer, not here). It depends on
/// `core/` (Domain) only.
///
/// 🔴 368_p3 — CT00/CT15 now read a real clock (`CN-18`: the one seam,
/// `mrs_core::ISteadyClock`). This service NEVER constructs a clock itself
/// (`w_.clock` is wiring, supplied by the node — CN-4/CN-18); see
/// `ServiceWiring::clock` and `trajectory_buffer.hpp`'s `sample_at()`.

namespace mrs_trajopt::service
{

/// @brief Wiring for one robot's onboard service.
struct ServiceWiring
{
    core::ITrajectorySearch* search = nullptr;     ///< extension point (i).
    core::ISubgoalCandidates* subgoals = nullptr;  ///< extension point (ii).
    core::IPeerChannel* channel = nullptr;         ///< extension point (iv).
    core::IInstrSink* instr = nullptr;             ///< instrumentation sink (may be null).
    core::VelocityProfiler profiler;               ///< core class (§3-4).
    core::SafetyMonitor* safety = nullptr;         ///< braking-filter dynamic half.
    /// 🆕 368_p3 — CT00's "시계 1" (CN-18 seam). NOT owned here; the node
    /// wires a concrete `mrs_core::ISteadyClock` (e.g. rclcpp::Clock-backed,
    /// use_sim_time-respecting) at composition time. If left null, CT00/CT15
    /// degrade to `now_s = 0.0` (the pre-368_p3 always-chain.front()
    /// behavior) rather than throw — a wiring gap is the caller's
    /// responsibility, not a tick-path fatal (CN-16 keeps run_tick() total).
    mrs_core::ISteadyClock* clock = nullptr;
};

/// @brief One control-tick input (CT-in).
struct TickInput
{
    core::Pose2 pose;                       ///< current odom pose.
    double v = 0.0;                         ///< current signed speed [m/s].
    bool staged_constraints_fresh = false;  ///< A5: fresh staged constraints?
    bool tube_pierced = false;              ///< CT09a: was the promise tube pierced this tick?
};

/// @brief One control-tick output (CT-out).
struct TickOutput
{
    core::StateSample cmd;  ///< CT20: the command sample to track.
    bool has_stop = false;  ///< CT25: is a stop declaration emitted?
    core::StopReason stop_reason =
        core::StopReason::kUnresolvableLocally;  ///< the reason if has_stop.
    bool horizon_truncated = false;              ///< CT19: was the horizon truncated this tick?
};

/// @brief Orchestrates one robot's three nested ticks.
class ControlTickService
{
public:
    /// @brief Construct with wiring, config, limits, and the segment goal.
    /// @param self this robot's identifier.
    /// @param wiring the extension points and core components.
    /// @param cfg the trajopt config (periods, budgets).
    /// @param lim this robot's per-robot limits.
    /// @param fleet the fleet-folded limits.
    /// @param goal the current segment goal pose.
    ControlTickService(std::string self,
                       ServiceWiring wiring,
                       core::TrajoptConfig cfg,
                       core::RobotLimits lim,
                       core::FleetLimits fleet,
                       core::Pose2 goal);

    /// @brief Run one control tick (CT00-CT28). Never throws (CN-16).
    ///
    /// All top-level work is wrapped so no exception escapes to the executor —
    /// a tick-path violation becomes a fatal latch + stop declaration instead.
    /// @param in the control-tick input.
    /// @return the control-tick output (command, optional stop, truncation flag).
    TickOutput run_tick(const TickInput& in);

    /// @brief The current control-tick sequence number.
    /// @return tick_seq.
    std::int64_t tick_seq() const
    {
        return tick_seq_;
    }

    /// @brief Read-only view of the committed trajectory buffer.
    /// @return const reference to the committed state chain.
    const std::vector<core::StateSample>& committed() const
    {
        return buffer_.view();
    }

    /// @brief The fairness metrics accumulated so far (328 six observables).
    /// @return const reference to this robot's fairness metrics.
    const core::FairnessMetrics& fairness() const
    {
        return fairness_;
    }

    /// @brief The count of committed publications (for tests / spike).
    /// @return number of times a new tube was published.
    std::int64_t publish_count() const
    {
        return publish_count_;
    }

private:
    void recompute_trajectory(const TickInput& in, double now_s);             // TT00-TT19
    void regenerate_subgoals_and_publish(const TickInput& in, double now_s);  // ST00-ST34

    std::string self_;
    ServiceWiring w_;
    core::TrajoptConfig cfg_;
    core::RobotLimits lim_;
    core::FleetLimits fleet_;
    core::Pose2 goal_;
    core::ThreeClockGate gate_;
    core::TrajectoryBuffer buffer_;
    core::RoundLedger ledger_;
    core::FairnessMetrics fairness_;

    std::int64_t tick_seq_ = 0;
    bool subgoal_advance_req_ = false;  // CT09b puncture advance request.
    std::vector<core::StateSample> cand_traj_;
    std::vector<core::PassWindow> subgoals_;
    std::int64_t publish_count_ = 0;
    std::vector<core::PassWindow> committed_subgoals_;  // for order-deviation check.
};

}  // namespace mrs_trajopt::service

#endif  // MRS_TRAJOPT_SERVICE_CONTROL_TICK_SERVICE_HPP
