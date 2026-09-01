// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_CORE_I_ASSIGNER_HPP
#define MRS_MRTA_CORE_I_ASSIGNER_HPP

#include <string>
#include <string_view>

#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "mrs_mrta/ports/i_distance_oracle.hpp"

/// @file i_assigner.hpp
/// @layer core [Clean Architecture: Domain]. Extension point interface — pure
/// virtual, no `pluginlib` include (CN-10). Implementations (`plugins/`) may
/// only reach the state through `push_tail`/`pop_head`/the pending-pool
/// methods on `AssignmentState` — the "없는 연산 여섯" (`336_mrta_canon.md`
/// §336-3) holds regardless of which plugin is loaded, because the forbidden
/// six member functions simply do not exist on `AssignmentState`.

namespace mrs_mrta::core
{

/// @brief Outcome of one `on_release` / `on_completion` call.
enum class AssignerStatus
{
    kAssigned,            ///< on_release: task placed on a robot's tail.
    kPooled,              ///< on_release: no eligible robot; task went to the pending pool.
    kCompletedHead,       ///< on_completion: head matched; popped.
    kCompletionMismatch,  ///< on_completion: head did not match; not consumed (counted, not an
                          ///< error).
    kRejected             ///< configure()-time or physical-capacity refusal.
};

/// @brief Fixed sizing and shared resources handed to `IAssigner::configure` once,
/// at startup (`12a_arch_mrta.md` §5-1). Never re-issued mid-run.
struct AssignerContext
{
    std::size_t robot_count = 0;             ///< N (`12a_arch_mrta.md` §5-1 ctx = N + Q^ta + ...).
    std::size_t goal_queue_capacity_ta = 0;  ///< Q^ta (`336_mrta_canon_p3.md` §336-9 cites 4).
    const ports::IDistanceOracle* distances = nullptr;  ///< Non-owning; must outlive the assigner.
};

/// @brief The assignment policy extension point (`12a_arch_mrta.md` §5-1) —
/// pluginlib base class `mrs_mrta::core::IAssigner`, 5 pure-virtual functions.
///
/// A plugin's *only* means of mutating assignment state are the methods this
/// header re-exposes on `AssignmentState` (push_tail / pop_head / the pending-
/// pool operations) — there is no path to `erase`, `insert_at`, `swap`,
/// `move_to_robot`, `sort`, or `replace` because those functions are not
/// members of `AssignmentState` (see `types.hpp`).
class IAssigner
{
public:
    virtual ~IAssigner() = default;

    /// @brief One-time startup configuration. All sizing decisions happen here
    /// — no allocation-affecting call is made afterward.
    /// @param ctx Capacity and shared-resource context.
    /// @return Status::kError if @p ctx is unusable (e.g. distances == nullptr).
    virtual Status configure(const AssignerContext& ctx) = 0;

    /// @brief Handle one task-release event: pick a candidate robot (or pool
    /// the task) and mutate @p state via push_tail / the pending-pool methods
    /// only.
    /// @param ev The release event (task id + ordered leg list).
    /// @param state The assignment state to mutate.
    /// @return kAssigned or kPooled.
    virtual AssignerStatus on_release(const ReleaseEvent& ev, AssignmentState& state) = 0;

    /// @brief Handle one completion event: pop the head iff it matches, and
    /// attempt to retry the pending pool with freed capacity.
    /// @param ev The completion event (robot + goal_id).
    /// @param state The assignment state to mutate.
    /// @return kCompletedHead or kCompletionMismatch.
    virtual AssignerStatus on_completion(const CompletionEvent& ev, AssignmentState& state) = 0;

    /// @brief Stable policy identifier for `result.json:assignment_policy_id`
    /// (a CK-D comparison key — never published on the boundary, `12a` §5-1).
    /// @return the policy id (e.g. "greedy_tail_append").
    virtual std::string_view policy_id() const = 0;

    /// @brief Reset all algorithm-owned counters/state at a run boundary,
    /// *without* releasing the fixed allocation (`12a` §5-1: "사전 할당을
    /// 유지한 채 내용만 비운다").
    /// @param state The assignment state to reset (queues emptied, pool cleared).
    virtual void reset(AssignmentState& state) = 0;
};

}  // namespace mrs_mrta::core

#endif  // MRS_MRTA_CORE_I_ASSIGNER_HPP
