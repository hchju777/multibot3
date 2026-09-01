// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/plugins/assigner_impls.hpp"

#include <optional>

namespace mrs_mrta::plugins
{

namespace
{

/// @brief Shared candidate search — the 5-tier lexicographic tie-break
/// (`336_mrta_canon.md` §336-2-2), parameterised by the per-robot capacity
/// limit so `GreedyTailAppend` (limit = Q^ta) and `FaultyOverflow`
/// (limit = physical capacity) can reuse the identical comparison logic
/// without either inheriting from the other (`12a`§5-2 M6: has-a, not is-a).
/// @return the chosen robot, or std::nullopt if no eligible robot exists
///         (endpoint exclusivity blocks the whole release, every candidate
///         is at its capacity limit, or the distance oracle cannot reach it).
std::optional<core::RobotIndex> pick_candidate(
    const std::vector<core::GoalRecord>& goals,
    const core::AssignmentState& state,
    const ports::IDistanceOracle& distances,
    std::size_t capacity_limit,
    const std::vector<std::uint64_t>& cumulative_assigned)
{
    if (goals.empty())
    {
        return std::nullopt;
    }
    for (const core::GoalRecord& g : goals)
    {
        if (state.location_in_use(g.location))
        {
            return std::nullopt;  // A8 endpoint exclusivity — no robot may take it right now.
        }
    }

    std::optional<core::RobotIndex> best;
    std::uint64_t best_cost = 0;
    std::size_t best_residual = 0;
    std::uint64_t best_cumulative = 0;

    for (core::RobotIndex r = 0; r < state.robot_count(); ++r)
    {
        if (state.residual_len(r) + goals.size() > capacity_limit)
        {
            continue;
        }

        const core::GoalRecord* tail = state.peek_tail(r);
        const std::string from_vertex = (tail != nullptr) ? tail->location : state.robot_vertex(r);
        if (from_vertex.empty())
        {
            continue;  // No known start position yet (D04 read model unset) — not eligible.
        }

        const std::optional<std::uint64_t> cost_opt =
            distances.dist_to_endpoint(from_vertex, goals.front().location);
        if (!cost_opt.has_value())
        {
            continue;  // Unreachable.
        }

        const std::uint64_t cost = *cost_opt;
        const std::size_t residual = state.residual_len(r);
        const std::uint64_t cumulative = cumulative_assigned.at(r);

        bool better;
        if (!best.has_value())
        {
            better = true;
        }
        else if (cost != best_cost)
        {
            better = cost < best_cost;  // tier 1
        }
        else if (residual != best_residual)
        {
            better = residual < best_residual;  // tier 2
        }
        else if (cumulative != best_cumulative)
        {
            better = cumulative < best_cumulative;  // tier 3
        }
        else
        {
            better =
                state.robot_names()[r] < state.robot_names()[*best];  // tier 4 — always resolves.
        }

        if (better)
        {
            best = r;
            best_cost = cost;
            best_residual = residual;
            best_cumulative = cumulative;
        }
    }

    return best;  // tier 5 (release sequence) is never reached — see class doc.
}

}  // namespace

// ---------------------------------------------------------------- GreedyTailAppend

core::Status GreedyTailAppend::configure(const core::AssignerContext& ctx)
{
    if (ctx.distances == nullptr)
    {
        return core::Status::kError;
    }
    ctx_ = ctx;
    cumulative_assigned_.assign(ctx.robot_count, 0);
    release_seq_counter_ = 0;
    return core::Status::kOk;
}

core::AssignerStatus GreedyTailAppend::on_release(const core::ReleaseEvent& ev,
                                                  core::AssignmentState& state)
{
    ++release_seq_counter_;

    const std::optional<core::RobotIndex> best = pick_candidate(ev.goals,
                                                                state,
                                                                *ctx_.distances,
                                                                ctx_.goal_queue_capacity_ta,
                                                                cumulative_assigned_);

    if (!best.has_value())
    {
        core::PendingTask pt;
        pt.task = ev.task;
        pt.goals = ev.goals;
        state.push_pending(std::move(pt));
        return core::AssignerStatus::kPooled;
    }

    for (const core::GoalRecord& g : ev.goals)
    {
        if (state.push_tail(*best, g) != core::Status::kOk)
        {
            break;  // Physical refusal — should not happen given the capacity check above.
        }
        ++cumulative_assigned_.at(*best);
    }
    return core::AssignerStatus::kAssigned;
}

core::AssignerStatus GreedyTailAppend::on_completion(const core::CompletionEvent& ev,
                                                     core::AssignmentState& state)
{
    const core::RobotIndex r = state.index_of(ev.robot);
    if (r == core::kNoRobot)
    {
        return core::AssignerStatus::kCompletionMismatch;  // Defensive — not a modeled scenario.
    }

    const core::GoalRecord* head = state.peek_head(r);
    if (head == nullptr || head->id != ev.goal_id)
    {
        return core::AssignerStatus::kCompletionMismatch;  // canon C05-C06: not consumed, counted.
    }

    state.pop_head(r);

    // Pending-pool retry with freed capacity (`12a_arch_mrta.md` §2-2 step 3:
    // "머리 소비 + 대기 풀 재시도"). Simple FIFO discipline: try the front
    // task; if it now fits somewhere, place it and try the new front; stop at
    // the first front task that still has no candidate (does not skip ahead
    // in the pool — a documented design choice, see implementation notes).
    while (!state.pending_empty())
    {
        const core::PendingTask* front = state.peek_pending_front();
        const std::optional<core::RobotIndex> candidate =
            pick_candidate(front->goals,
                           state,
                           *ctx_.distances,
                           ctx_.goal_queue_capacity_ta,
                           cumulative_assigned_);
        if (!candidate.has_value())
        {
            break;
        }
        core::PendingTask task = state.pop_pending_front();
        for (const core::GoalRecord& g : task.goals)
        {
            if (state.push_tail(*candidate, g) != core::Status::kOk)
            {
                break;
            }
            ++cumulative_assigned_.at(*candidate);
        }
    }

    return core::AssignerStatus::kCompletedHead;
}

std::string_view GreedyTailAppend::policy_id() const
{
    return "mrs_mrta/GreedyTailAppend";
}

void GreedyTailAppend::reset(core::AssignmentState& state)
{
    for (core::RobotIndex r = 0; r < state.robot_count(); ++r)
    {
        while (state.pop_head(r).has_value())
        {
            // Draining uses only the one allowed removal operation.
        }
    }
    while (!state.pending_empty())
    {
        state.pop_pending_front();
    }
    cumulative_assigned_.assign(state.robot_count(), 0);
    release_seq_counter_ = 0;
}

// ---------------------------------------------------------------- FaultyOverflow

core::Status FaultyOverflow::configure(const core::AssignerContext& ctx)
{
    if (ctx.distances == nullptr)
    {
        return core::Status::kError;
    }
    ctx_ = ctx;
    return delegate_.configure(ctx);
}

core::AssignerStatus FaultyOverflow::on_release(const core::ReleaseEvent& ev,
                                                core::AssignmentState& state)
{
    // 🔴 The fault: candidate capacity is the *physical* limit (Q^ta + 1),
    // not the policy limit Q^ta — this plugin's own bookkeeping (not the
    // delegate's) so the fault is self-contained. Tier 3 (cumulative
    // assigned) degenerates to 0 for all robots since this plugin does not
    // track it — tiers 1/2/4 still resolve every tie.
    std::vector<std::uint64_t> zeros(state.robot_count(), 0);
    const std::optional<core::RobotIndex> best =
        pick_candidate(ev.goals, state, *ctx_.distances, state.physical_capacity(), zeros);

    if (!best.has_value())
    {
        core::PendingTask pt;
        pt.task = ev.task;
        pt.goals = ev.goals;
        state.push_pending(std::move(pt));
        return core::AssignerStatus::kPooled;
    }

    for (const core::GoalRecord& g : ev.goals)
    {
        if (state.push_tail(*best, g) != core::Status::kOk)
        {
            break;  // Even the physical limit refuses — memory-safety floor holds.
        }
    }
    return core::AssignerStatus::kAssigned;
}

core::AssignerStatus FaultyOverflow::on_completion(const core::CompletionEvent& ev,
                                                   core::AssignmentState& state)
{
    return delegate_.on_completion(ev, state);
}

std::string_view FaultyOverflow::policy_id() const
{
    return "mrs_mrta/FaultyOverflow";
}

void FaultyOverflow::reset(core::AssignmentState& state)
{
    delegate_.reset(state);
}

}  // namespace mrs_mrta::plugins
