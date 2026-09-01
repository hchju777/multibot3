// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/service/assignment_service.hpp"

#include <algorithm>

namespace mrs_mrta::service
{

AssignmentService::AssignmentService(std::vector<std::string> robot_names,
                                     std::size_t goal_queue_capacity_ta,
                                     std::size_t event_ring_slots,
                                     core::IAssigner& assigner,
                                     ports::IOverrideGoalIdResolver* goal_id_resolver)
    : state_(robot_names, goal_queue_capacity_ta),
      ring_(event_ring_slots),
      latch_(robot_names),
      assigner_(assigner),
      goal_id_resolver_(goal_id_resolver)
{
}

core::Status AssignmentService::ingest_release(core::ReleaseEvent ev)
{
    return ring_.push_release(std::move(ev));
}

core::Status AssignmentService::ingest_completion(core::CompletionEvent ev)
{
    return ring_.push_completion(std::move(ev));
}

core::Status AssignmentService::ingest_operator(core::OperatorEvent ev)
{
    return ring_.push_operator(std::move(ev));
}

void AssignmentService::set_robot_vertex(const std::string& robot, std::string vertex)
{
    const core::RobotIndex r = state_.index_of(robot);
    if (r == core::kNoRobot)
    {
        return;  // Unknown robot — defensive no-op on a runtime callback path (CN-16).
    }
    state_.set_robot_vertex(r, std::move(vertex));
}

bool AssignmentService::override_active(const std::string& robot) const
{
    const core::RobotIndex r = state_.index_of(robot);
    if (r == core::kNoRobot)
    {
        return false;
    }
    return latch_.is_active(r);
}

std::optional<core::AssignmentView> AssignmentService::drain_once()
{
    if (ring_.empty())
    {
        return std::nullopt;
    }

    core::EventRing::Drained drained = ring_.pop_all_ordered();

    // "touched" is a cheap pre-filter (did anything happen at all?), NOT the
    // publish decision by itself — F46-2: a latch change can leave the
    // *published* content unchanged (e.g. an override activation whose
    // goal_id the resolver seam could not resolve falls back to the base
    // queue, which may be exactly what was already published). The real
    // no-op-publish-suppression decision happens below, by content
    // comparison against `last_published_`.
    bool touched = false;

    // Tier 1: completions (`92` A3, `336_mrta_canon_p2.md` §336-4-2).
    for (const core::CompletionEvent& c : drained.completions)
    {
        const core::RobotIndex r = state_.index_of(c.robot);
        if (r == core::kNoRobot)
        {
            continue;  // Defensive — not a modeled scenario, never thrown on this path.
        }
        const core::AssignerStatus st = assigner_.on_completion(c, state_);
        if (st == core::AssignerStatus::kCompletionMismatch)
        {
            ++completion_mismatch_count_;
        }
        else if (st == core::AssignerStatus::kCompletedHead)
        {
            touched = true;
        }
    }

    // Tier 2: releases (`92` A3, release_index ascending already applied by
    // EventRing). Every release mutates base state (assigned or pooled).
    for (const core::ReleaseEvent& rel : drained.releases)
    {
        (void)assigner_.on_release(rel, state_);
        touched = true;
    }

    // Tier 3: operator goal_override events (`336b`§336b-2-2 — base state is
    // never touched here; only the projection latch moves).
    for (const core::OperatorEvent& op : drained.operators)
    {
        const core::RobotIndex r = state_.index_of(op.robot);
        if (r == core::kNoRobot)
        {
            continue;
        }
        if (latch_.apply(op, r))
        {
            touched = true;
        }
    }

    if (!touched)
    {
        return std::nullopt;  // Nothing happened at all — trivially no content change either.
    }

    core::AssignmentView candidate = build_view();

    // F46-2: publish only if the content actually differs from what was last
    // published — a latch touch (e.g. the unresolved-goal_id fallback) does
    // not by itself justify a revision bump.
    if (last_published_.has_value() && content_equal(candidate, *last_published_))
    {
        return std::nullopt;  // No-op publish suppression (canon A32/step 6).
    }

    ++revision_;
    candidate.revision = revision_;
    last_published_ = candidate;
    return candidate;
}

bool AssignmentService::content_equal(const core::AssignmentView& a, const core::AssignmentView& b)
{
    if (a.unassigned_tasks != b.unassigned_tasks)
    {
        return false;
    }
    if (a.robots.size() != b.robots.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < a.robots.size(); ++i)
    {
        const core::RobotGoalsView& ra = a.robots[i];
        const core::RobotGoalsView& rb = b.robots[i];
        if (ra.robot != rb.robot || ra.start != rb.start || ra.goals != rb.goals)
        {
            return false;
        }
    }
    return true;
}

core::AssignmentView AssignmentService::build_view()
{
    core::AssignmentView view;
    // `revision` is left at its default here — the caller (drain_once) only
    // commits a real revision number after deciding, by content comparison,
    // that this candidate is actually going to be published (F46-2).

    const std::vector<std::string>& names = state_.robot_names();
    view.robots.reserve(names.size());

    for (core::RobotIndex r = 0; r < names.size(); ++r)
    {
        core::RobotGoalsView rv;
        rv.robot = names[r];
        rv.start = state_.robot_vertex(r);

        bool projected = false;
        if (latch_.is_active(r))
        {
            std::optional<core::GoalId> gid;
            if (goal_id_resolver_ != nullptr)
            {
                gid = goal_id_resolver_->resolve(names[r], latch_.goal_location(r));
            }
            if (gid.has_value())
            {
                core::GoalRecord g;
                g.id = *gid;
                g.task.clear();  // The operator names a destination, not a task (336b§4-1).
                g.location = latch_.goal_location(r);
                rv.goals.push_back(std::move(g));
                projected = true;
            }
            else
            {
                // 🔴 The reservation rule is [결정 부재] (336b§336b-4-2). Rather
                // than publish an artifact with a fabricated goal_id, fall back
                // to the base queue and count it — see the port's file doc.
                ++override_goal_id_unresolved_count_;
            }
        }

        if (!projected)
        {
            rv.goals = state_.residual_view(r);
        }

        view.robots.push_back(std::move(rv));
    }

    std::vector<std::string> pending = state_.pending_task_ids();
    std::sort(pending.begin(), pending.end());
    view.unassigned_tasks = std::move(pending);

    return view;
}

}  // namespace mrs_mrta::service
