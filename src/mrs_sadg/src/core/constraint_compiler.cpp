// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/constraint_compiler.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <utility>

#include "mrs_sadg/core/self_checks.hpp"

namespace mrs_sadg::core
{

namespace
{

/// @brief "{robot}#{k}".
std::string seg_id(const std::string& robot, std::size_t k)
{
    return robot + "#" + std::to_string(k);
}

}  // namespace

bool ConstraintCompiler::hcons_ok(const PlanView& plan, std::vector<std::string>& messages)
{
    // Same rule as check_sadg_acyclic.py seam (d): each directed edge used by
    // >= 2 robots must have a consistent restricted visit order at both ends.
    // users of edge (A,B) = { (robot, k) : steps[k]=A, steps[k+1]=B }.
    std::map<std::pair<std::string, std::string>, std::set<std::pair<std::string, std::uint32_t>>>
        traversals;
    for (const auto& r : plan.robots)
    {
        for (std::size_t k = 0; k + 1 < r.locations.size(); ++k)
        {
            traversals[{r.locations[k], r.locations[k + 1]}].insert(
                {r.robot, static_cast<std::uint32_t>(k)});
        }
    }

    std::map<std::string, std::vector<VisitItem>> vo;
    for (const auto& e : plan.visit_order)
    {
        vo[e.location] = e.sequence;
    }

    bool ok = true;
    for (const auto& [edge, users] : traversals)
    {
        std::set<std::string> robots;
        for (const auto& u : users)
        {
            robots.insert(u.first);
        }
        if (robots.size() < 2)
        {
            continue;  // H-cons only binds when 2+ robots share a direction.
        }
        const std::string& a = edge.first;
        const std::string& b = edge.second;
        auto ia = vo.find(a);
        auto ib = vo.find(b);
        if (ia == vo.end() || ib == vo.end())
        {
            messages.push_back("[H-cons] 간선 " + a + "->" + b + "의 끝 정점이 visit_order에 없다");
            ok = false;
            continue;
        }
        // list_a: A's visit order restricted to users (departure index).
        std::vector<std::pair<std::string, std::uint32_t>> list_a;
        for (const auto& it : ia->second)
        {
            if (users.count({it.robot, it.index}))
            {
                list_a.push_back({it.robot, it.index});
            }
        }
        // list_b: B's visit order restricted, arrival index shifted back by 1.
        std::vector<std::pair<std::string, std::uint32_t>> list_b;
        for (const auto& it : ib->second)
        {
            if (it.index >= 1 && users.count({it.robot, it.index - 1}))
            {
                list_b.push_back({it.robot, it.index - 1});
            }
        }
        if (list_a != list_b)
        {
            messages.push_back("[H-cons] 간선 " + a + "->" + b +
                               "의 양끝 방문 순서가 다르다 (추월 불가 통로 반쪽 반전)");
            ok = false;
        }
    }
    return ok;
}

Status ConstraintCompiler::compile(const PlanView& plan,
                                   ExecutionGraph& out,
                                   CompileReport& report) const
{
    report = CompileReport{};
    out.reset_runtime();
    out.set_plan_revision(plan.plan_revision);
    out.set_instance_id(plan.instance_id);

    // ── IN01-IN03: H-cons intake. Do not silently fold — reject + declare. ──
    if (!hcons_ok(plan, report.messages))
    {
        report.outcome = CompileOutcome::kHconsViolation;
        return Status::kError;
    }

    // ── IN04-IN06: segments (steps -> segments). count = steps - 1. ──
    for (const auto& r : plan.robots)
    {
        for (std::size_t k = 0; k + 1 < r.locations.size(); ++k)
        {
            Segment s;
            s.id = seg_id(r.robot, k);
            s.robot = r.robot;
            s.from = r.locations[k];
            s.to = r.locations[k + 1];
            out.add_segment(s);
        }
    }

    // ── IN07: fixed E0 sequential chain {r}#{k} -> {r}#{k+1}. ──
    for (const auto& r : plan.robots)
    {
        for (std::size_t k = 0; k + 2 < r.locations.size(); ++k)
        {
            Dependency d;
            d.from = seg_id(r.robot, k);
            d.to = seg_id(r.robot, k + 1);
            d.type = DepType::kSequential;
            out.add_fixed_dep(d);
        }
    }

    // ── IN08-IN13: committed ordering deps via generator + ［D5］/［D8］. ──
    for (const auto& entry : plan.visit_order)
    {
        VisitOrderView view;
        view.entry = &entry;
        std::vector<AlternativeSlot> slots;
        auto res = gen_.generate(view, out.segments(), slots);
        if (!res.ok())
        {
            report.outcome = CompileOutcome::kMalformedPlan;
            report.messages.push_back("[IN] 대안 생성 실패: " + entry.location);
            return Status::kError;
        }
        for (const auto& slot : slots)
        {
            // The ordering EDGE exists for both following (［D6］) and opposing
            // (［D7］) pairs — the checker's seam (c) expects the edge either way.
            // ［D5］/［D8］ distinguish only (a) release semantics (start vs
            // completion) and (b) switchability (opposing pairs are irreversible,
            // so they never become a reorderable switch alternative).
            out.add_fixed_dep(slot.dep);
            ++report.n_ordering_edges;
            if (slot.opposing)
            {
                out.mark_opposing_edge(slot.dep.from, slot.dep.to);
                ++report.n_opposing_excluded;  // kept fixed, excluded from switch alts (［D8］).
            }
        }
    }

    // ── IN14: production-side seam self-check (296§296-4-2 — a wrong mapping is
    // acyclic yet passes exit 0, so this is mandatory). ──
    SeamReport seam;
    if (!seam_selfcheck(plan, out, seam))
    {
        report.outcome = CompileOutcome::kSeamInconsistent;
        report.messages = seam.messages;
        return Status::kError;
    }

    report.outcome = CompileOutcome::kOk;
    return Status::kOk;
}

}  // namespace mrs_sadg::core
