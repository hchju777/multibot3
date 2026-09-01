// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_SERVICE_ASSIGNMENT_SERVICE_HPP
#define MRS_MRTA_SERVICE_ASSIGNMENT_SERVICE_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "mrs_mrta/core/event_ring.hpp"
#include "mrs_mrta/core/i_assigner.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "mrs_mrta/ports/i_override_goal_id_resolver.hpp"
#include "mrs_mrta/service/override_latch.hpp"

/// @file assignment_service.hpp
/// @layer service [Clean Architecture: Application].
/// @brief `drain_once` — the single entry point that ever mutates assignment
/// state (`336_mrta_canon_p2.md` §336-4: "drain_once가 유일한 배정 진입점").
///
/// Owns: the fixed `core::AssignmentState`, the `core::EventRing`, the
/// `service::OverrideLatch` (`341_arch_mrta_addendum.md` §341-2 places all
/// three here, not in `core` or a plugin). `IAssigner` and
/// `IOverrideGoalIdResolver` are injected, non-owning references — this class
/// does not know which policy is loaded or how override goal_ids are reserved.
namespace mrs_mrta::service
{

class AssignmentService
{
public:
    /// @brief Construct with a fixed roster and fixed capacities.
    /// @param robot_names Robot roster, ascending lexicographic order.
    /// @param goal_queue_capacity_ta Policy capacity Q^ta.
    /// @param event_ring_slots Capacity for each of the three event kinds
    ///        (`event_ring_slots`, [값 부재] — caller-supplied, not invented here).
    /// @param assigner The loaded policy plugin (must outlive this service).
    /// @param goal_id_resolver Override goal_id seam (may be nullptr — then
    ///        overrides never project onto the boundary; see the port's doc).
    AssignmentService(std::vector<std::string> robot_names,
                      std::size_t goal_queue_capacity_ta,
                      std::size_t event_ring_slots,
                      core::IAssigner& assigner,
                      ports::IOverrideGoalIdResolver* goal_id_resolver);

    /// @brief Stage a task-release event (`on_task_release`, canon `A04`).
    /// @param ev The release event.
    /// @return Status::kError iff the event ring is at capacity (dropped, counted).
    core::Status ingest_release(core::ReleaseEvent ev);

    /// @brief Stage a completion event (`on_completion_event`, canon `A11`).
    /// @param ev The completion event.
    /// @return Status::kError iff the event ring is at capacity (dropped, counted).
    core::Status ingest_completion(core::CompletionEvent ev);

    /// @brief Stage a filtered operator `goal_override` event
    /// (`on_operator_command`, `336b`§336b-2-1 — the caller has already
    /// discarded `hold`/`speed_cap` commands before calling this).
    /// @param ev The operator event.
    /// @return Status::kError iff the event ring is at capacity (dropped, counted).
    core::Status ingest_operator(core::OperatorEvent ev);

    /// @brief Update a robot's last-known vertex (`on_robot_states`, canon
    /// `A16`-`A19`, `D04`). 🔴 Does **not** trigger `drain_once` — a robot
    /// state message never wakes assignment (`336_mrta_canon.md` §336-0 #6).
    /// @param robot Robot identifier (must be in the roster; unknown robots
    ///        are ignored, not thrown — this is a runtime callback path, CN-16).
    /// @param vertex The robot's current/last-sampled roadmap vertex.
    void set_robot_vertex(const std::string& robot, std::string vertex);

    /// @brief The single entry point that ever mutates assignment state. Sorts
    /// and processes everything staged since the last call in the canon total
    /// order (completions, then releases, then operator events), then decides
    /// whether to publish.
    ///
    /// 🔴 The publish decision is a **content** comparison against the last
    /// *published* view, not a "did something happen" flag (F46-2 fix) — a
    /// latch change alone (e.g. an override activation whose goal_id the
    /// resolver seam could not resolve, `build_view`'s fallback-to-base path)
    /// does not by itself justify a `revision` bump if the resulting goals[]
    /// content is unchanged from what was last published.
    /// @return the view to publish, or std::nullopt if nothing observable
    ///         changed (no-op publish suppression, canon `A32`).
    std::optional<core::AssignmentView> drain_once();

    /// @brief Current publish revision (0-based, increments only on publish).
    /// @return the revision counter.
    std::uint64_t revision() const noexcept
    {
        return revision_;
    }

    /// @brief Count of completion events whose head did not match (not consumed).
    /// @return the counter (`completion_mismatch`, canon `C05`-`C06`).
    std::size_t completion_mismatch_count() const noexcept
    {
        return completion_mismatch_count_;
    }

    /// @brief Count of override activations that could not be projected
    /// because the goal_id seam returned nothing (fell back to base queue).
    /// @return the counter.
    std::size_t override_goal_id_unresolved_count() const noexcept
    {
        return override_goal_id_unresolved_count_;
    }

    /// @brief Read-only access to the underlying algorithm state, for tests
    /// and diagnostics only — never handed to an adapter directly (`12a`§4).
    /// @return a const reference to the assignment state.
    const core::AssignmentState& state() const noexcept
    {
        return state_;
    }

    /// @brief Is robot @p robot currently under an active operator override?
    /// @param robot Robot identifier.
    /// @return true iff an active `goal_override` latch is in effect.
    bool override_active(const std::string& robot) const;

private:
    core::AssignmentView build_view();

    /// @brief Content equality for the publish decision (F46-2) — compares
    /// `robots[].{robot,start,goals}` and `unassigned_tasks` only. `revision`
    /// is deliberately excluded (it is the thing being decided, not an input).
    /// @param a One candidate/published view.
    /// @param b The other.
    /// @return true iff the two views would look identical on the boundary.
    static bool content_equal(const core::AssignmentView& a, const core::AssignmentView& b);

    core::AssignmentState state_;
    core::EventRing ring_;
    OverrideLatch latch_;
    core::IAssigner& assigner_;
    ports::IOverrideGoalIdResolver* goal_id_resolver_;  ///< Non-owning; may be nullptr.

    std::uint64_t revision_ = 0;
    std::size_t completion_mismatch_count_ = 0;
    std::size_t override_goal_id_unresolved_count_ = 0;
    /// Last **published** view content, for the F46-2 no-op-suppression
    /// comparison. std::nullopt until the first publish.
    std::optional<core::AssignmentView> last_published_;
};

}  // namespace mrs_mrta::service

#endif  // MRS_MRTA_SERVICE_ASSIGNMENT_SERVICE_HPP
