// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_BOUNDARY_TYPES_HPP
#define MRS_SADG_ADAPTER_BOUNDARY_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file boundary_types.hpp
/// @brief Plain-old-data mirrors of the boundary schemas SADG consumes/produces.
/// These carry exactly the frozen schema fields — no internal representation, no
/// algorithm parameters, no timesteps, no absolute time (CLAUDE.md invariant 2/3;
/// check_schema_purity.py enforces it on the emitted JSON). `adapter/` converts
/// between these and `core/` types; `io/` serializes these to/from JSON text.

namespace mrs_sadg::adapter
{

// ----------------------------------------------------------- mrs.discrete_plan 4.0.0 (consume)

/// @brief One plan step (order index + location; no timestep).
struct BoundaryStep
{
    std::uint32_t index = 0;  ///< 0-based order index.
    std::string location;     ///< roadmap node id.
};

/// @brief One robot's plan.
struct BoundaryRobotPlan
{
    std::string robot;                ///< robot id.
    std::string terminal;             ///< terminal (goal) label.
    std::vector<BoundaryStep> steps;  ///< ordered steps.
};

/// @brief One robot's occupancy of a shared location, in visit order.
struct BoundaryVisitItem
{
    std::string robot;        ///< robot id.
    std::uint32_t index = 0;  ///< its step index at the location.
};

/// @brief The visit order of one shared location.
struct BoundaryVisitOrderEntry
{
    std::string location;                     ///< shared location.
    std::vector<BoundaryVisitItem> sequence;  ///< robots in visit order.
};

/// @brief An mrs.discrete_plan 4.0.0 document.
struct BoundaryDiscretePlan
{
    static constexpr const char* kSchema = "mrs.discrete_plan";
    static constexpr const char* kSchemaVersion = "4.0.0";
    std::string schema = kSchema;
    std::string schema_version = kSchemaVersion;
    std::string instance_id;
    std::uint64_t plan_revision = 0;
    std::vector<BoundaryRobotPlan> plans;
    std::vector<BoundaryVisitOrderEntry> visit_order;
};

// ----------------------------------------------------- mrs.execution_constraints 3.0.0 (produce)

/// @brief One compiled segment.
struct BoundarySegment
{
    std::string id;     ///< "{robot}#{k}".
    std::string robot;  ///< owning robot.
    std::string from;   ///< from location.
    std::string to;     ///< to location.
};

/// @brief One dependency (sequential | ordering).
struct BoundaryDependency
{
    std::string from;  ///< predecessor segment id.
    std::string to;    ///< successor segment id.
    std::string type;  ///< "sequential" | "ordering".
};

/// @brief One switch-group alternative.
struct BoundaryAlternative
{
    std::string id;                                ///< unique in the group.
    std::vector<BoundaryDependency> dependencies;  ///< ordering edges.
};

/// @brief One switch group at a shared location.
struct BoundarySwitchGroup
{
    std::string id;                                 ///< unique.
    std::string location;                           ///< shared location.
    std::vector<BoundaryAlternative> alternatives;  ///< >= 2 mutually exclusive.
};

/// @brief An mrs.execution_constraints 3.0.0 document.
struct BoundaryExecutionConstraints
{
    static constexpr const char* kSchema = "mrs.execution_constraints";
    static constexpr const char* kSchemaVersion = "3.0.0";
    std::string schema = kSchema;
    std::string schema_version = kSchemaVersion;
    std::string instance_id;
    std::uint64_t commit_seq = 0;
    std::vector<BoundarySegment> segments;
    std::vector<BoundaryDependency> dependencies;
    std::vector<BoundarySwitchGroup> switch_groups;
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_BOUNDARY_TYPES_HPP
