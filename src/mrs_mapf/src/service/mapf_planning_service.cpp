// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/service/mapf_planning_service.hpp"

#include <algorithm>
#include <map>
#include <set>

#include "mrs_mapf/core/visit_order.hpp"

namespace mrs_mapf::service
{
using namespace mrs_mapf::core;  // NOLINT

namespace
{

/// @brief R06 (§255-1-1) — report-only: which declared endpoints are not
/// reachable (directed) from ANY robot's resolved start vertex. Never changes
/// behavior — only feeds `RunResult::reachability_warnings`.
std::vector<std::string> reachability_warnings(const Roadmap& roadmap,
                                               const std::vector<AssignmentEntry>& roster)
{
    std::set<std::string> reachable;
    std::vector<std::string> frontier;
    for (const auto& a : roster)
    {
        if (roadmap.has_node(a.start) && reachable.insert(a.start).second)
        {
            frontier.push_back(a.start);
        }
    }
    while (!frontier.empty())
    {
        std::string u = frontier.back();
        frontier.pop_back();
        auto it = roadmap.out_arcs.find(u);
        if (it == roadmap.out_arcs.end())
        {
            continue;
        }
        for (const auto& arc : it->second)
        {
            if (reachable.insert(arc.to).second)
            {
                frontier.push_back(arc.to);
            }
        }
    }
    std::vector<std::string> warnings;
    for (const auto& ep : roadmap.endpoints)
    {
        if (reachable.count(ep) == 0)
        {
            warnings.push_back("대기 지점 " + ep +
                               " 가 어느 로봇의 시작 정점에서도 도달 불가하다(보고만 — 거동 불변)");
        }
    }
    return warnings;
}

/// @brief D-3 (§286-3-2) — resolve each robot's frozen planning-start vertex
/// from `baseline`. Returns false (a) if baseline is non-empty but no matching
/// retained plan is held (PX10), leaving @p out untouched.
bool resolve_frozen_starts(const PlanningRequest& request, std::map<std::string, std::string>& out)
{
    out.clear();
    for (const auto& a : request.assignments)
    {
        out[a.robot] = a.start;  // default: assignment's own start.
    }
    if (request.baseline.empty())
    {
        return true;  // no baseline consumption requested — every robot uses its assignment start.
    }
    if (!request.previous_plan.has_value() ||
        request.previous_plan->plan_revision != request.request_plan_revision)
    {
        return false;  // PX10 — "그 가정이 깨지는 배치" (D-3(a): explicit failure).
    }
    std::map<std::string, const RobotPlan*> prev_by_robot;
    for (const auto& p : request.previous_plan->plans)
    {
        prev_by_robot[p.robot] = &p;
    }
    for (const auto& b : request.baseline)
    {
        auto it = prev_by_robot.find(b.robot);
        if (it == prev_by_robot.end() || b.committed_segment_count >= it->second->steps.size())
        {
            return false;  // "이 필드는 해석 불가가 된다" — the retained plan cannot resolve k.
        }
        out[b.robot] = it->second->steps[b.committed_segment_count].location;
    }
    return true;
}

/// @brief §255-1-2's guarantee ③ (부분성): copy an out-of-scope robot's
/// retained plan verbatim ("구성상 항등" — byte copy, not recomputed).
RobotPlan byte_copy(const RobotPlan& retained)
{
    return retained;
}

/// @brief R29 (§255-1-1) — the no-search safety-stop fallback: park every
/// robot at its resolved start vertex.
std::vector<RobotPlan> safety_stop_plan(const std::vector<AssignmentEntry>& roster,
                                        const std::map<std::string, std::string>& starts)
{
    std::vector<RobotPlan> plans;
    for (const auto& a : roster)
    {
        RobotPlan p;
        p.robot = a.robot;
        p.terminal = Terminal::kNoProgress;  // R29: no search occurred.
        StepEntry s;
        s.index = 0;
        auto it = starts.find(a.robot);
        s.location = (it != starts.end()) ? it->second : a.start;
        p.steps.push_back(s);
        plans.push_back(p);
    }
    return plans;
}

/// @brief Combine a scope-solve's fresh plans with byte-copied out-of-scope
/// plans (or a degenerate length-1 plan if no retained plan exists for a robot
/// outside scope — an off-normal-path defensive fallback; see known limitations).
/// @param known_schedules [out] robot -> the search's own actual arrival-time
///   schedule, for every FRESHLY-solved robot (see `TimedRobotPlan` doc — this
///   is what makes `build_visit_order` agree with the schedule the search
///   actually used to avoid conflicts, instead of an independently recomputed
///   waitless estimate).
std::vector<RobotPlan> combine_plans(const std::vector<AssignmentEntry>& roster,
                                     const std::map<std::string, TimedRobotPlan>& fresh,
                                     const std::optional<DiscretePlanDraft>& previous_plan,
                                     const std::map<std::string, std::string>& starts,
                                     std::map<std::string, std::vector<double>>& known_schedules)
{
    std::map<std::string, const RobotPlan*> retained;
    if (previous_plan.has_value())
    {
        for (const auto& p : previous_plan->plans)
        {
            retained[p.robot] = &p;
        }
    }
    std::vector<RobotPlan> out;
    for (const auto& a : roster)
    {
        auto fit = fresh.find(a.robot);
        if (fit != fresh.end())
        {
            out.push_back(fit->second.plan);
            known_schedules[a.robot] = fit->second.arrival_s;
            continue;
        }
        auto rit = retained.find(a.robot);
        if (rit != retained.end())
        {
            out.push_back(byte_copy(*rit->second));
            continue;
        }
        // Defensive fallback (documented limitation): a roster robot outside
        // scope with no retained plan — stay put rather than invent motion.
        RobotPlan p;
        p.robot = a.robot;
        p.terminal = Terminal::kNoProgress;
        StepEntry s;
        s.index = 0;
        auto it = starts.find(a.robot);
        s.location = (it != starts.end()) ? it->second : a.start;
        p.steps.push_back(s);
        out.push_back(p);
    }
    return out;
}

}  // namespace

MapfPlanningService::MapfPlanningService(core::IPlanningStrategy& strategy) : strategy_(strategy)
{
}

RunResult MapfPlanningService::run_once(const PlanningRequest& request,
                                        std::uint64_t next_plan_revision,
                                        const std::string& instance_id)
{
    RunResult result;

    // R06 — report-only, never gates behavior.
    result.reachability_warnings = reachability_warnings(request.roadmap, request.assignments);

    // D-3 / PX09~PX10.
    std::map<std::string, std::string> starts;
    if (!resolve_frozen_starts(request, starts))
    {
        ++no_baseline_plan_failures_;
        result.outcome = RunOutcome::kNoBaselinePlan;
        return result;
    }

    // Build a request whose assignments carry the resolved (frozen) starts —
    // §255-6-1's "축의 원점은 지금" applied per-robot via baseline.
    PlanningRequest resolved = request;
    for (auto& a : resolved.assignments)
    {
        auto it = starts.find(a.robot);
        if (it != starts.end())
        {
            a.start = it->second;
        }
    }

    std::map<std::string, double> speed_of;
    for (const auto& rs : resolved.robot_specs)
    {
        speed_of[rs.robot] = rs.avg_traversal_speed_mps;
    }

    // Assemble a draft from a set of fresh (scope) plans + everyone else
    // byte-copied/parked, run self-checks ⓐⓑ on it, and report both.
    //
    // 🔴 A self-check failure here is NOT necessarily a bug in the search — the
    // implemented family (priority + safe-interval, feasible-only, NO
    // completeness guarantee — `255`§255-2-2 「셋째」) can produce a
    // continuous-time-safe schedule whose discrete, order-only representation
    // is still cyclic: waiting at an intermediate vertex can make a robot's
    // ENTRY order (what `visit_order` records) diverge from its DEPARTURE
    // order relative to a different shared vertex, and the retreat->진입
    // mapping only encodes entry order. This package's own tests found this
    // directly (a corridor swap requiring one robot to wait). Treating a
    // self-check failure as "this scope attempt failed" and escalating
    // (exactly like a raw search infeasibility) is the correct response for a
    // family with no completeness guarantee — see `20b_impl_mapf.md` known
    // limitations for why a fuller fix (joint continuous+discrete search) is
    // out of this round's scope.
    auto assemble_and_check = [&](const std::vector<RobotPlan>& plans,
                                  const std::map<std::string, std::vector<double>>& known_schedules,
                                  DiscretePlanDraft& draft_out,
                                  SelfCheckReport& report_out) -> SelfCheckOutcome
    {
        draft_out = DiscretePlanDraft{};
        draft_out.instance_id = instance_id;
        draft_out.plan_revision = next_plan_revision;
        draft_out.plans = plans;
        draft_out.visit_order = build_visit_order(plans,
                                                  speed_of,
                                                  resolved.roadmap,
                                                  resolved.ordering_constraints,
                                                  known_schedules);
        return run_self_checks(draft_out, report_out);
    };

    // R12 (§255-3, this round's floor-only simplification — no ㉠/㉡ expansion).
    std::vector<std::string> scope = resolved.affected_robots;

    // R18 — solve(scope = floor).
    auto scope_solve = strategy_.solve(resolved, scope);
    if (scope_solve.ok())
    {
        std::map<std::string, std::vector<double>> known_schedules;
        std::vector<RobotPlan> plans = combine_plans(resolved.assignments,
                                                     scope_solve.value(),
                                                     resolved.previous_plan,
                                                     starts,
                                                     known_schedules);
        DiscretePlanDraft draft;
        SelfCheckOutcome outcome =
            assemble_and_check(plans, known_schedules, draft, result.self_check);
        if (outcome == SelfCheckOutcome::kOk || outcome == SelfCheckOutcome::kUntested)
        {
            result.outcome = RunOutcome::kPublished;
            result.draft = draft;
            return result;
        }
        // Discrete-representation infeasibility — treat like a search failure, escalate.
        // 48차 진단(`369_p2`) — snapshot BEFORE the global attempt overwrites
        // result.self_check with its own (possibly different) report.
        result.scope_self_check = result.self_check;
    }
    else
    {
        // 48차 진단(`369_p2`) — record WHY, never fed back into the decision.
        result.scope_failure = scope_solve.error();
    }

    // R20/R23 — escalate to global (every roster robot in scope).
    result.escalated_to_global = true;
    std::vector<std::string> all_robots;
    for (const auto& a : resolved.assignments)
    {
        all_robots.push_back(a.robot);
    }
    auto global_solve = strategy_.solve(resolved, all_robots);
    if (global_solve.ok())
    {
        std::map<std::string, std::vector<double>> known_schedules;
        std::vector<RobotPlan> plans = combine_plans(resolved.assignments,
                                                     global_solve.value(),
                                                     resolved.previous_plan,
                                                     starts,
                                                     known_schedules);
        DiscretePlanDraft draft;
        SelfCheckOutcome outcome =
            assemble_and_check(plans, known_schedules, draft, result.self_check);
        if (outcome == SelfCheckOutcome::kOk || outcome == SelfCheckOutcome::kUntested)
        {
            result.outcome = RunOutcome::kPublished;
            result.draft = draft;
            return result;
        }
        // 48차 진단(`369_p2`) — snapshot BEFORE the fallback attempt overwrites
        // result.self_check with its own report.
        result.global_self_check = result.self_check;
    }
    else
    {
        // 48차 진단(`369_p2`) — record WHY, never fed back into the decision.
        result.global_failure = global_solve.error();
    }

    // R29 — safety-stop fallback (no search).
    result.used_safety_stop_fallback = true;
    std::vector<RobotPlan> fallback_plans = safety_stop_plan(resolved.assignments, starts);
    DiscretePlanDraft fallback_draft;
    SelfCheckOutcome fallback_outcome =
        assemble_and_check(fallback_plans, {}, fallback_draft, result.self_check);
    if (fallback_outcome == SelfCheckOutcome::kOk ||
        fallback_outcome == SelfCheckOutcome::kUntested)
    {
        result.outcome = RunOutcome::kPublished;
        result.draft = fallback_draft;
        return result;
    }

    // §255-13 (unresolved by the user — U-1): follow the FROZEN CONTRACT text
    // literally ("그것조차 비순환이 아니면 아티팩트를 발행하지 않고 run을 중단한다",
    // discrete_plan.schema.json plans[].description) as this module's default —
    // documented as an open design point, not a value invented.
    result.outcome = RunOutcome::kFallbackNotAcyclic;
    return result;
}

}  // namespace mrs_mapf::service
