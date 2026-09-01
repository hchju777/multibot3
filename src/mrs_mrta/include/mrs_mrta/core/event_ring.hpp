// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_CORE_EVENT_RING_HPP
#define MRS_MRTA_CORE_EVENT_RING_HPP

#include <algorithm>
#include <vector>

#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"

/// @file event_ring.hpp
/// @layer core [Clean Architecture: Domain].
/// @brief The one queue every event funnels through before `drain_once`
/// (`336_mrta_canon.md` §336-4: "사건 링 → drain_once 유일 진입점").

namespace mrs_mrta::core
{

/// @brief Fixed-capacity event staging area + the total-order rule
/// (`336_mrta_canon_p2.md` §336-4-2, `336b_goal_override_design.md` §336b-2-2:
/// completions first, then releases, then operator events).
///
/// Internally this is three fixed-capacity vectors (one per event kind), not
/// a single interleaved ring — the three kinds are drained in three separate,
/// independently-sorted passes by `pop_all_ordered`, which is exactly the
/// canon total order. Push order does not matter because sorting happens at
/// drain time; there is no reordering of *published* state involved (the
/// forbidden `sort` in `336`§336-3-1 is about `AssignmentState`, not this
/// staging buffer).
class EventRing
{
public:
    /// @brief All events drained from the ring in one `drain_once` call,
    /// pre-sorted into the canon total order (completions, then releases,
    /// then operator events; each group internally ordered by its own key).
    struct Drained
    {
        std::vector<CompletionEvent> completions;
        std::vector<ReleaseEvent> releases;
        std::vector<OperatorEvent> operators;
    };

    /// @brief Construct with a fixed slot capacity per event kind
    /// (`event_ring_slots`, `12a_arch_mrta.md` §12-4 V2 — [값 부재]; the
    /// caller supplies a concrete value, this class does not invent one).
    /// @param slots_per_kind Capacity for each of the three internal queues.
    explicit EventRing(std::size_t slots_per_kind) : capacity_(slots_per_kind)
    {
    }

    /// @brief Stage a completion event.
    /// @param ev The event.
    /// @return Status::kError if the completion queue is at capacity (caller
    ///         increments a drop counter; this class never grows silently).
    Status push_completion(CompletionEvent ev)
    {
        if (completions_.size() >= capacity_)
        {
            return Status::kError;
        }
        completions_.push_back(std::move(ev));
        return Status::kOk;
    }

    /// @brief Stage a release event.
    /// @param ev The event.
    /// @return Status::kError if the release queue is at capacity.
    Status push_release(ReleaseEvent ev)
    {
        if (releases_.size() >= capacity_)
        {
            return Status::kError;
        }
        releases_.push_back(std::move(ev));
        return Status::kOk;
    }

    /// @brief Stage an operator (goal_override) event.
    /// @param ev The event.
    /// @return Status::kError if the operator queue is at capacity.
    Status push_operator(OperatorEvent ev)
    {
        if (operators_.size() >= capacity_)
        {
            return Status::kError;
        }
        operators_.push_back(std::move(ev));
        return Status::kOk;
    }

    /// @brief Is the ring entirely empty (nothing to drain)?
    /// @return true iff all three internal queues are empty.
    bool empty() const noexcept
    {
        return completions_.empty() && releases_.empty() && operators_.empty();
    }

    /// @brief Drain and sort everything staged since the last call
    /// (canon `A25`-`A27`, `336b`§336b-2-2). Clears the ring.
    /// @return the drained, sorted events.
    Drained pop_all_ordered()
    {
        Drained out;
        out.completions = std::move(completions_);
        out.releases = std::move(releases_);
        out.operators = std::move(operators_);
        completions_.clear();
        releases_.clear();
        operators_.clear();

        // completions: (robot, that robot's completion order) ascending.
        // The robot-local completion order is the global `seq` restricted to
        // that robot (`336_mrta_canon_p2.md` §336-4-2) — since `seq` is
        // globally monotonic, sorting the pair (robot, seq) ascending gives
        // the same relative order within each robot as "그 로봇 안의 완료
        // 순서수 오름차순".
        std::stable_sort(out.completions.begin(),
                         out.completions.end(),
                         [](const CompletionEvent& a, const CompletionEvent& b)
                         {
                             if (a.robot != b.robot)
                             {
                                 return a.robot < b.robot;
                             }
                             return a.seq < b.seq;
                         });

        // releases: release_index ascending.
        std::stable_sort(out.releases.begin(),
                         out.releases.end(),
                         [](const ReleaseEvent& a, const ReleaseEvent& b)
                         {
                             return a.release_index < b.release_index;
                         });

        // operator events: (robot, seq) ascending (`336b`§336b-2-2 tier 3).
        std::stable_sort(out.operators.begin(),
                         out.operators.end(),
                         [](const OperatorEvent& a, const OperatorEvent& b)
                         {
                             if (a.robot != b.robot)
                             {
                                 return a.robot < b.robot;
                             }
                             return a.seq < b.seq;
                         });

        return out;
    }

private:
    std::size_t capacity_;
    std::vector<CompletionEvent> completions_;
    std::vector<ReleaseEvent> releases_;
    std::vector<OperatorEvent> operators_;
};

}  // namespace mrs_mrta::core

#endif  // MRS_MRTA_CORE_EVENT_RING_HPP
