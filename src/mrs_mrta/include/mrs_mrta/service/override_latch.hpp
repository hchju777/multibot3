// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_SERVICE_OVERRIDE_LATCH_HPP
#define MRS_MRTA_SERVICE_OVERRIDE_LATCH_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "mrs_mrta/core/types.hpp"

/// @file override_latch.hpp
/// @layer service [Clean Architecture: Application].
/// @brief highest-seq-wins latch for the operator `goal_override` channel
/// (`336b_goal_override_design.md` §336b-6).
///
/// 🔴 Placed in `service/`, not `core/` or a plugin — `341_arch_mrta_addendum.md`
/// §341-2 sets this deliberately: the override is not an assignment *policy*,
/// so `IAssigner` implementations never see it (a faulty/normal plugin swap
/// cannot change override behaviour). This is a plain struct with no ROS/JSON
/// dependency, so the placement does not pull anything into the `core` link
/// unit — it simply is not reachable from `IAssigner`.
namespace mrs_mrta::service
{

/// @brief One robot's override latch state (a `(robot, goal_override)` key,
/// `336b`§336b-6-1 — the module never sees `hold`/`speed_cap`, so the `verb`
/// component of that key is always `goal_override` here and is dropped).
struct OverrideState
{
    std::uint64_t seq = 0;
    bool active = false;
    std::string goal_location;
    bool has_state = false;  ///< false until the first event for this robot arrives.
};

/// @brief Per-robot `highest-seq-wins` latch set, fixed-size over the roster.
class OverrideLatch
{
public:
    /// @brief Construct over a fixed robot roster.
    /// @param robot_names Roster, index-aligned with `core::AssignmentState`.
    explicit OverrideLatch(std::vector<std::string> robot_names);

    /// @brief Apply one operator event to robot @p r's latch
    /// (`336b`§336b-6-1 pseudocode: "if cmd.seq <= latch[key].seq: return").
    /// @param ev The operator event (already filtered to verb == goal_override).
    /// @param r Robot index (must be < roster size).
    /// @return true iff the observable projected state actually changed
    ///         (used for the no-op-publish-suppression `dirty` flag).
    bool apply(const core::OperatorEvent& ev, core::RobotIndex r);

    /// @brief Is robot @p r's override currently active?
    /// @param r Robot index.
    /// @return true iff an active `goal_override` latch is in effect.
    bool is_active(core::RobotIndex r) const;

    /// @brief The overridden goal location for robot @p r.
    /// @param r Robot index.
    /// @return the location string (only meaningful if is_active(r)).
    const std::string& goal_location(core::RobotIndex r) const;

private:
    std::vector<OverrideState> latches_;
};

}  // namespace mrs_mrta::service

#endif  // MRS_MRTA_SERVICE_OVERRIDE_LATCH_HPP
