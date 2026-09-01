// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_SERVICE_MAPF_PLANNING_SERVICE_HPP
#define MRS_MAPF_SERVICE_MAPF_PLANNING_SERVICE_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "mrs_mapf/core/i_planning_strategy.hpp"
#include "mrs_mapf/core/self_checks.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file mapf_planning_service.hpp
/// @brief The wrapping procedure — `run_once()` — that owns NO search logic
/// itself (§255-1: "본체는 감싸는 절차, 탐색 규칙은 플러그인 계열"). Implements a
/// SINGLE-SHOT slice of the `255`§255-1-1 flow (R02/R04 pass-through, R06
/// report-only reachability, R12 scope = the received floor only — no ㉠/㉡
/// expansion this round, R18/R23 partial-then-global escalation, R29 safety-stop
/// fallback, X-block export, self-check, adapter, revision).
///
/// 🔴 NOT implemented this round (`20b_impl_mapf.md` known limitations): the
/// full `286` lifelong incremental loop (`LL`/`PX`/`IN`/`BC`/`SV`/`PB`/`RP`/`IM`
/// state machine — corridor batch composition, T-2 mid-path insertion closure,
/// `A-fifo′` overtake budget). This service performs a single from-scratch solve
/// per call, not an incremental insertion.

namespace mrs_mapf::service
{

/// @brief Distinct outcomes of one `run_once()` call — §255-8-2 decision 3's
/// "상태 타입" discipline applied at the run level, not just the self-check level.
enum class RunOutcome
{
    kPublished,  ///< a self-check-passing plan was produced (planned scope, global, or fallback).
    kNoBaselinePlan,  ///< D-3(a): baseline requires a retained plan this call does not have, or its
                      ///< plan_revision does not match the request's — explicit failure, no
                      ///< publish.
    kFallbackNotAcyclic  ///< even the safety-stop fallback failed self-check ⓐ/ⓑ — §255-13's U-1
                         ///< (unresolved by the user); this module follows the frozen contract text
                         ///< ("발행하지 않고 run을 중단한다") as the default disposition — see
                         ///< limitations.
};

/// @brief Everything one `run_once()` call reports, beyond the draft itself.
struct RunResult
{
    RunOutcome outcome = RunOutcome::kPublished;
    core::DiscretePlanDraft draft;     ///< valid only if outcome == kPublished.
    core::SelfCheckReport self_check;  ///< detail of the LAST self-check attempt.
    bool escalated_to_global =
        false;  ///< R23 — did the requested (floor) scope fail and require full-roster replanning?
    bool used_safety_stop_fallback =
        false;  ///< R29 — did even global planning fail, forcing the no-search fallback?
    std::vector<std::string> reachability_warnings;  ///< R06 — report-only; never changes behavior.
};

/// @brief The wrapping planning procedure (Application layer, CN-23).
class MapfPlanningService
{
public:
    /// @param strategy the swap-target search plugin (CN-5) — never owned here.
    explicit MapfPlanningService(core::IPlanningStrategy& strategy);

    /// @brief Run one planning cycle (§255-1-1's `run_once`).
    /// @param request the fully-adapted planning request (roster, roadmap,
    ///   blocked edges, ordering constraints, baseline, previous_plan).
    /// @param next_plan_revision the revision number this publish would carry
    ///   if it succeeds (PB02 — the caller/`system-architect`'s persisted
    ///   monotonic counter; this service does not own that state, §286-13
    ///   "다음 스텝 전달 사항").
    /// @param instance_id copied verbatim into the draft.
    /// @return the run's outcome and (if published) the draft plan.
    RunResult run_once(const core::PlanningRequest& request,
                       std::uint64_t next_plan_revision,
                       const std::string& instance_id);

    /// @brief How many times D-3(a) has explicitly failed a request this
    /// service instance's lifetime (an internal counter — CN-20: exposed for a
    /// caller to fold into `result.json`, never written there directly by this
    /// module).
    std::uint64_t no_baseline_plan_failures() const
    {
        return no_baseline_plan_failures_;
    }

private:
    core::IPlanningStrategy& strategy_;
    std::uint64_t no_baseline_plan_failures_ = 0;
};

}  // namespace mrs_mapf::service

#endif  // MRS_MAPF_SERVICE_MAPF_PLANNING_SERVICE_HPP
