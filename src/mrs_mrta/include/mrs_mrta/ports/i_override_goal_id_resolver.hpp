// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_PORTS_I_OVERRIDE_GOAL_ID_RESOLVER_HPP
#define MRS_MRTA_PORTS_I_OVERRIDE_GOAL_ID_RESOLVER_HPP

#include <optional>
#include <string>

#include "mrs_mrta/core/types.hpp"

/// @file i_override_goal_id_resolver.hpp
/// @layer ports [Clean Architecture: Application, driven seam].
/// @brief Injection point for the operator-override `goal_id` reservation —
/// deliberately left a **seam, not a decision**.
///
/// 🔴 `336b_goal_override_design.md` §336b-4-2 is explicit that the reservation
/// rule (which unit the instance generator reserves an override `goal_id` by —
/// `(robot, endpoint)` vs. a per-robot slot列) is `[결정 부재]`, owned by a
/// future generator-spec round, and that this module must not invent it
/// ("goal_id 예약은 생성기 사양 라운드의 것" — this developer does not choose).
/// `service::AssignmentService` therefore never mints or picks a `goal_id`
/// itself (A-TA3 stays with the instance generator, `336b`§4-1); it only
/// *asks* this seam, and if the seam returns nothing, the override is not
/// projected onto the boundary (falls back to the base queue) rather than
/// publishing an artifact with a fabricated identifier — see
/// `AssignmentService::build_view` and the "알려진 한계" section of the
/// implementation notes.
namespace mrs_mrta::ports
{

/// @brief Resolves a reserved override `goal_id` for one `(robot, goal_location)`
/// pair. A real implementation is the instance generator's (`mrs_eval/generator`),
/// not this module's — see file doc.
class IOverrideGoalIdResolver
{
public:
    virtual ~IOverrideGoalIdResolver() = default;

    /// @brief Resolve the reserved goal_id for an override target.
    /// @param robot Robot identifier.
    /// @param goal_location Roadmap endpoint the operator is redirecting to.
    /// @return the reserved goal_id, or std::nullopt if none is reserved /
    ///         the reservation is exhausted (the caller must not fabricate one).
    virtual std::optional<core::GoalId> resolve(const std::string& robot,
                                                const std::string& goal_location) = 0;
};

}  // namespace mrs_mrta::ports

#endif  // MRS_MRTA_PORTS_I_OVERRIDE_GOAL_ID_RESOLVER_HPP
