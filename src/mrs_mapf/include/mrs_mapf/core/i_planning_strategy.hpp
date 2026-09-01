// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_I_PLANNING_STRATEGY_HPP
#define MRS_MAPF_CORE_I_PLANNING_STRATEGY_HPP

#include <map>
#include <string>
#include <vector>

#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file i_planning_strategy.hpp
/// @brief The swap point (CN-10) — this module's algorithm family extension
/// point. `service/` is a wrapping procedure (§255-1: "본체는 감싸는 절차, 탐색
/// 규칙은 플러그인 계열") and never contains search logic itself; every concrete
/// search family lives under `plugins/` and links `core` only (CN-2/CN-5).
///
/// The canon (`255`§255-2-2) names four families — a continuous-time
/// constraint-based search (default), a continuous-time focal bounded-suboptimal
/// variant, a priority + safe-interval search, and a collision-agnostic
/// baseline. This round implements exactly ONE concrete plugin
/// (`plugins::PrioritySafeIntervalSearch`, the third family) — see
/// `20b_impl_mapf.md` "알려진 한계" for why the other three are not implemented
/// this round; the interface below is deliberately family-agnostic so a second
/// plugin can be added without touching `service/`.

namespace mrs_mapf::core
{

/// @brief One robot's search failure detail, for diagnostics only (never on the boundary).
struct PlanFailure
{
    std::string
        robot;  ///< the robot the strategy could not find a feasible path for (empty if global).
    std::string detail;  ///< human-readable reason.
};

/// @brief A freshly-solved robot's plan PLUS the strategy's own actual
/// (wait-respecting, reservation-consistent) arrival-time schedule.
///
/// 🔴 The `arrival_s` vector never crosses to the boundary — it exists purely
/// so `service/`'s `visit_order` assembly can use the SAME schedule the search
/// itself used to avoid conflicts, instead of an independently recomputed
/// waitless estimate. Recomputing a naive (no-wait) nominal schedule for
/// ordering purposes is WRONG whenever the search actually inserted a wait
/// (this was caught by this package's own tests — a two-robot single-lane
/// swap where one robot must wait produced a spurious segment-graph cycle
/// until `visit_order` was made to consume the search's real schedule instead
/// of re-deriving one).
struct TimedRobotPlan
{
    RobotPlan plan;
    std::vector<double>
        arrival_s;  ///< arrival_s[i] == time this robot enters plan.steps[i].location.
};

/// @brief Owned by `core/` (Domain) per CN-23 — plugins (Infrastructure) implement it.
class IPlanningStrategy
{
public:
    virtual ~IPlanningStrategy() = default;

    /// @brief Plan feasible step sequences for exactly the robots in @p scope,
    /// given the full planning request (roadmap, blocked edges, ordering
    /// constraints, and — for out-of-scope robots' occupancy — the request's
    /// `previous_plan` + robot specs, which `RB-1`/§255-8-3 reconstructs into
    /// reservations internally).
    ///
    /// No timestep, cost, k-robustness, suboptimality or solver-parameter value
    /// crosses this call's boundary — everything here is internal (invariant 1
    /// of `CLAUDE.md`).
    ///
    /// @param request the full planning request (roster, roadmap, constraints).
    /// @param scope the robots to (re)plan; must be a subset of the request's roster.
    /// @return on success, one `TimedRobotPlan` per robot in @p scope; on
    ///   failure, the robot (if identifiable) and a diagnostic message.
    virtual Result<std::map<std::string, TimedRobotPlan>, PlanFailure> solve(
        const PlanningRequest& request,
        const std::vector<std::string>& scope) = 0;
};

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_I_PLANNING_STRATEGY_HPP
