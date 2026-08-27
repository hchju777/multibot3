// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_CONTROL_TICK_SERVICE_HPP
#define MRS_TRAJOPT_CORE_CONTROL_TICK_SERVICE_HPP

#include <cstdint>
#include <string>
#include <vector>

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
/// puncture advance) is faithful and testable. This is a `core/` class: no ROS,
/// no JSON, no boundary types (the adapter is called by the node layer, not here).

namespace mrs_trajopt::core
{

/// @brief Wiring for one robot's onboard service.
struct ServiceWiring
{
    ITrajectorySearch* search = nullptr;     ///< extension point (i).
    ISubgoalCandidates* subgoals = nullptr;  ///< extension point (ii).
    IPeerChannel* channel = nullptr;         ///< extension point (iv).
    IInstrSink* instr = nullptr;             ///< instrumentation sink (may be null).
    VelocityProfiler profiler;               ///< core class (§3-4).
    SafetyMonitor* safety = nullptr;         ///< braking-filter dynamic half.
};

/// @brief One control-tick input (CT-in).
struct TickInput
{
    Pose2 pose;                             ///< current odom pose.
    double v = 0.0;                         ///< current signed speed [m/s].
    bool staged_constraints_fresh = false;  ///< A5: fresh staged constraints?
    bool tube_pierced = false;              ///< CT09a: was the promise tube pierced this tick?
};

/// @brief One control-tick output (CT-out).
struct TickOutput
{
    StateSample cmd;        ///< CT20: the command sample to track.
    bool has_stop = false;  ///< CT25: is a stop declaration emitted?
    StopReason stop_reason = StopReason::kUnresolvableLocally;  ///< the reason if has_stop.
    bool horizon_truncated = false;  ///< CT19: was the horizon truncated this tick?
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
                       TrajoptConfig cfg,
                       RobotLimits lim,
                       FleetLimits fleet,
                       Pose2 goal);

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
    const std::vector<StateSample>& committed() const
    {
        return buffer_.view();
    }

    /// @brief The fairness metrics accumulated so far (328 six observables).
    /// @return const reference to this robot's fairness metrics.
    const FairnessMetrics& fairness() const
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
    void recompute_trajectory(const TickInput& in);             // TT00-TT19
    void regenerate_subgoals_and_publish(const TickInput& in);  // ST00-ST34

    std::string self_;
    ServiceWiring w_;
    TrajoptConfig cfg_;
    RobotLimits lim_;
    FleetLimits fleet_;
    Pose2 goal_;
    ThreeClockGate gate_;
    TrajectoryBuffer buffer_;
    RoundLedger ledger_;
    FairnessMetrics fairness_;

    std::int64_t tick_seq_ = 0;
    bool subgoal_advance_req_ = false;  // CT09b puncture advance request.
    std::vector<StateSample> cand_traj_;
    std::vector<PassWindow> subgoals_;
    std::int64_t publish_count_ = 0;
    std::vector<PassWindow> committed_subgoals_;  // for order-deviation check.
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_CONTROL_TICK_SERVICE_HPP
