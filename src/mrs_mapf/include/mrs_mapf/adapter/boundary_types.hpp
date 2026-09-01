// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_BOUNDARY_TYPES_HPP
#define MRS_MAPF_ADAPTER_BOUNDARY_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file boundary_types.hpp
/// @brief Plain structs mirroring the frozen boundary schemas this module
/// touches. This is the ONLY place internal types meet the boundary shape
/// (module-developer skill "어댑터 규칙"). No solver cost, timestep,
/// k-robustness, suboptimality, tick, horizon, or seed field lives here.
///
/// 🔴 함정 ③(발주서): `BoundaryDiscretePlan::kSchemaVersion` below is the single
/// named constant carrying "4.0.0" — no other file in this package repeats the
/// literal. `test_check_envelope_version.cpp` diffs this constant against the
/// schema file itself so a future contract bump cannot silently strand this
/// module on a stale envelope (the 1.4.2 lesson — `check_mapf_solution.py`'s own
/// header comment).

namespace mrs_mapf::adapter
{

/// @brief mrs.roadmap 3.0.0 node.
struct BoundaryNode
{
    std::string id;
    double x = 0.0;
    double y = 0.0;
    double clearance_m = 0.0;
};

/// @brief mrs.roadmap 3.0.0 edge (directed traversal vocabulary, closed 3-value enum as string).
struct BoundaryEdge
{
    std::string from;
    std::string to;
    std::string traversal;  ///< "bidirectional" | "forward_only" | "backward_only".
    double length_m = 0.0;
    double width_m = 0.0;
    int capacity_robots = 1;
    std::string corridor;  ///< "" = none.
};

/// @brief mrs.roadmap 3.0.0 document (this module reads `nodes`/`edges`/`endpoints`
/// only — `corridors`/`provenance` are parsed but not algorithmically consumed
/// this round; see known limitations).
struct BoundaryRoadmap
{
    std::string instance_id;
    std::vector<BoundaryNode> nodes;
    std::vector<BoundaryEdge> edges;
    std::vector<std::string> endpoints;
};

/// @brief mrs.assignment 2.0.0 one robot's goal.
struct BoundaryGoal
{
    std::string goal_id;
    std::string task;
    std::string location;
};

/// @brief mrs.assignment 2.0.0 one robot's assignment.
struct BoundaryAssignmentEntry
{
    std::string robot;
    std::string start;
    std::vector<BoundaryGoal> goals;
};

/// @brief mrs.assignment 2.0.0 document.
struct BoundaryAssignment
{
    std::string instance_id;
    std::uint64_t revision = 0;
    std::vector<BoundaryAssignmentEntry> assignments;
    std::vector<std::string> unassigned_tasks;
};

/// @brief mrs.replan_request 7.0.0 blocked directed edge.
struct BoundaryBlockedEdge
{
    std::string from;
    std::string to;
};

/// @brief mrs.replan_request 7.0.0 ordering_constraints[].sequence[] item.
struct BoundaryOrderingItem
{
    std::string robot;
    std::uint32_t index = 0;
};

/// @brief mrs.replan_request 7.0.0 ordering_constraints[] entry.
struct BoundaryOrderingConstraint
{
    std::string location;
    std::vector<BoundaryOrderingItem> sequence;
};

/// @brief mrs.replan_request 7.0.0 baseline[] entry.
struct BoundaryBaselineEntry
{
    std::string robot;
    std::uint32_t committed_segment_count = 0;
};

/// @brief mrs.replan_request 7.0.0 document (the six required fields this
/// module consumes; `reason`/`blocked_hash` are read but not branched on this
/// round — this module does not implement the reason-classification consumers
/// listed as `LL03`'s job in `273`/`286`, a documented known limitation).
struct BoundaryReplanRequest
{
    std::string instance_id;
    std::string reason;
    std::vector<std::string> affected_robots;
    std::vector<BoundaryBlockedEdge> blocked;
    std::string blocked_hash;
    std::vector<BoundaryOrderingConstraint> ordering_constraints;
    std::vector<BoundaryBaselineEntry> baseline;
    std::uint64_t plan_revision = 0;
};

/// @brief mrs.robot_specs 1.1.0 one robot's spec — only the two fields A33
/// permits this module to consume (`robot`, `avg_traversal_speed_mps`); the
/// other five are parsed (additionalProperties:false requires reading them to
/// round-trip the document faithfully in tests) but never read by `core/`.
struct BoundaryRobotSpec
{
    std::string robot;
    double avg_traversal_speed_mps = 0.0;
    double v_max = 0.0;                   ///< A33-forbidden for this module — parsed, unused.
    double brake_decel_min_mps2 = 0.0;    ///< A33-forbidden — parsed, unused.
    double curvature_max_invm = 0.0;      ///< A33-forbidden — parsed, unused.
    bool reverse_motion_allowed = false;  ///< A33-forbidden — parsed, unused.
    double circumradius_m = 0.0;          ///< A33-forbidden — parsed, unused.
};

/// @brief mrs.robot_specs 1.1.0 document.
struct BoundaryRobotSpecs
{
    std::string instance_id;
    std::vector<BoundaryRobotSpec> robots;
};

/// @brief mrs.discrete_plan 4.0.0 one step.
struct BoundaryStep
{
    std::uint32_t index = 0;
    std::string location;
};

/// @brief mrs.discrete_plan 4.0.0 closed terminal vocabulary, as string
/// ("goal_reached" | "plan_truncated" | "no_progress").
struct BoundaryRobotPlan
{
    std::string robot;
    std::string terminal;
    std::vector<BoundaryStep> steps;
};

/// @brief mrs.discrete_plan 4.0.0 visit_order[].sequence[] item.
struct BoundaryVisitItem
{
    std::string robot;
    std::uint32_t index = 0;
};

/// @brief mrs.discrete_plan 4.0.0 visit_order[] entry.
struct BoundaryVisitOrderEntry
{
    std::string location;
    std::vector<BoundaryVisitItem> sequence;
};

/// @brief mrs.discrete_plan 4.0.0 document.
struct BoundaryDiscretePlan
{
    /// 🔴 함정 ③ — the single named constant for the contract envelope version
    /// this module produces AND consumes (its own previous publish). Diffed
    /// against the schema file by `test_check_envelope_version.cpp`.
    static constexpr const char* kSchema = "mrs.discrete_plan";
    static constexpr const char* kSchemaVersion = "4.0.0";

    std::string schema;  ///< as parsed from JSON — checked against kSchema by from_boundary.
    std::string
        schema_version;  ///< as parsed from JSON — checked against kSchemaVersion by from_boundary.
    std::string instance_id;
    std::uint64_t plan_revision = 0;
    std::vector<BoundaryRobotPlan> plans;
    std::vector<BoundaryVisitOrderEntry> visit_order;
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_BOUNDARY_TYPES_HPP
