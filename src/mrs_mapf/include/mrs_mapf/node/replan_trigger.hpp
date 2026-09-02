// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_NODE_REPLAN_TRIGGER_HPP
#define MRS_MAPF_NODE_REPLAN_TRIGGER_HPP

#include <cstdint>
#include <optional>

/// @file replan_trigger.hpp
/// @brief Pure orchestration decision (Presentation layer, CN-23) — no ROS,
/// no algorithm change — deciding whether a newly observed `/assignment`
/// message should arm a (re)plan call into `service::MapfPlanningService`.
///
/// 🔴 [결정] `assignment.schema.json`'s `revision` field description states the
/// contract explicitly leaves this to the consumer: *"이 발행이 재계획을
/// 요구한다는 신호가 아니다 … 재계획 트리거는 소비 모듈의 것이다"* (this publish is
/// not itself a signal requiring a replan — the replan trigger belongs to the
/// consuming module). `255_mapf_canon.md`§1-1's two documented triggers (an
/// upstream `Replan` action, and the not-yet-implemented lifelong "idle robot
/// got its first goal" internal trigger, `IN2`) do not cover this case either.
/// So this file is `mapf_node`'s own node-layer policy, same status as the
/// existing "plan once all three boundary inputs have latched in" decision
/// it sits beside: **replan whenever a new `/assignment` message's `revision`
/// differs from the last one this node attempted to plan for** — no periodic
/// polling, no cooldown/period invented, and never re-planning twice for the
/// same revision (replan-storm guard).

namespace mrs_mapf::node
{

/// @brief Decide whether the node should (re)run a full-roster plan now.
/// @param inputs_complete `/roadmap` + `/robot_specs` + `/assignment` have all
///   latched in at least once.
/// @param last_attempted_revision the `/assignment.revision` this node last
///   attempted to plan for, or `std::nullopt` if it has never attempted one.
/// @param incoming_revision the `/assignment.revision` of the message just
///   received.
/// @return `true` iff a plan attempt should be made now.
inline bool ShouldReplanForAssignment(bool inputs_complete,
                                      const std::optional<std::uint64_t>& last_attempted_revision,
                                      std::uint64_t incoming_revision)
{
    if (!inputs_complete)
    {
        return false;
    }
    return !last_attempted_revision.has_value() || incoming_revision != *last_attempted_revision;
}

}  // namespace mrs_mapf::node

#endif  // MRS_MAPF_NODE_REPLAN_TRIGGER_HPP
