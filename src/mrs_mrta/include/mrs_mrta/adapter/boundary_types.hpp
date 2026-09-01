// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_ADAPTER_BOUNDARY_TYPES_HPP
#define MRS_MRTA_ADAPTER_BOUNDARY_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file boundary_types.hpp
/// @layer adapter [Clean Architecture: Infrastructure].
/// @brief Boundary-shaped structs — the C++ mirror of the five schemas this
/// module touches, field-for-field, no JSON dependency (CN-8/CN-9).
///
/// These are what `mrs_msgs` `.msg` types would deserialize into on a ROS 2
/// machine (`node/` is a skeleton in this checkout, brief §1). Field names
/// and required/optional-ness follow the schema files in
/// `multibot3/src/mrs_msgs/schema/` exactly — this header does not add,
/// rename, or drop a field. Algorithm parameters (Q^ta, tie-break, capacities)
/// never appear here (`336_mrta_canon.md` §336-9 "경계로 나가는 것과 내부에
/// 남는 것").
namespace mrs_mrta::adapter
{

// ---------------------------------------------------------------- mrs.assignment 2.0.0 (output)

/// @brief `mrs_msgs/msg/Goal` as it appears inside `assignments[].goals[]`.
struct BoundaryGoal
{
    std::string goal_id;
    std::string task;
    std::string location;
};

/// @brief `mrs_msgs/msg/RobotAssignment`.
struct BoundaryRobotAssignment
{
    std::string robot;
    std::string start;
    std::vector<BoundaryGoal> goals;
};

/// @brief `mrs.assignment` 2.0.0 top level.
struct BoundaryAssignment
{
    std::string schema = "mrs.assignment";
    std::string schema_version = "2.0.0";
    std::string instance_id;
    std::uint64_t revision = 0;
    std::vector<BoundaryRobotAssignment> assignments;
    std::vector<std::string> unassigned_tasks;
};

// ---------------------------------------------------------------- mrs.task_release 2.0.0 (input)

/// @brief One element of `task_release.goals[]`.
struct BoundaryTaskReleaseGoal
{
    std::string goal_id;
    std::string location;
};

/// @brief `mrs.task_release` 2.0.0 top level.
struct BoundaryTaskRelease
{
    std::string schema = "mrs.task_release";
    std::string schema_version = "2.0.0";
    std::string instance_id;
    std::uint64_t release_index = 0;
    std::string task;
    std::vector<BoundaryTaskReleaseGoal> goals;
};

// ---------------------------------------------------------------- mrs.goal_completion 1.0.0
// (input)

/// @brief `mrs.goal_completion` 1.0.0 top level (always exactly one event).
struct BoundaryGoalCompletion
{
    std::string schema = "mrs.goal_completion";
    std::string schema_version = "1.0.0";
    std::string instance_id;
    std::uint64_t seq = 0;
    std::string robot;
    std::string goal_id;
    std::uint64_t revision = 0;
};

// ---------------------------------------------------------------- mrs.operator_command 1.0.0
// (input)

/// @brief One element of `operator_command.commands[]` (all three verbs —
/// this module only consumes `goal_override`, `336b`§2-1).
struct BoundaryOperatorCommand
{
    std::uint64_t seq = 0;
    std::string robot;
    std::string verb;  ///< "hold" | "speed_cap" | "goal_override".
    bool active = false;
    bool has_speed_cap_mps = false;
    double speed_cap_mps = 0.0;
    /// Absence sentinel: "" — the .msg wire form has no optional flag for this
    /// field (unlike speed_cap_mps/has_speed_cap_mps). The schema's own
    /// convention maps absence to the empty string ("§7-2, corridor:null 관례"
    /// — endpoints are "L##"-shaped and never collide with ""), so this
    /// struct mirrors the wire representation exactly rather than adding an
    /// internal flag the wire form does not have (F46-1 fix).
    std::string goal_location;
};

/// @brief `mrs.operator_command` 1.0.0 top level.
struct BoundaryOperatorCommands
{
    std::string schema = "mrs.operator_command";
    std::string schema_version = "1.0.0";
    std::string instance_id;
    std::vector<BoundaryOperatorCommand> commands;
};

// ---------------------------------------------------------------- mrs.roadmap (input, minimal
// subset)

/// @brief Only the fields this module reads (`12a_arch_mrta.md` §4:
/// "endpoints_ 비트셋 + 공유표 핸들"). Full roadmap parsing (edges, corridors,
/// provenance) is `mrs_core`'s job, not this module's.
struct BoundaryRoadmap
{
    std::string schema = "mrs.roadmap";
    std::string schema_version = "3.0.0";
    std::string instance_id;
    std::vector<std::string> node_ids;
    std::vector<std::string> endpoints;
};

}  // namespace mrs_mrta::adapter

#endif  // MRS_MRTA_ADAPTER_BOUNDARY_TYPES_HPP
