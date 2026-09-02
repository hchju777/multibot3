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

// ------------------------------------------------------------------ mrs.entry_events 2.0.0
// (consume)

/// @brief One entry/departure event (entry_events.schema.json §6, §17-2-2).
/// `entered:true` <=> `location == segment.to` (arrival); `entered:false` <=>
/// `location == segment.from` (departure) — the schema's own field-level
/// constraint (`entry_events_adapter.hpp` derives the internal classification
/// from exactly this linkage).
struct BoundaryEntryEvent
{
    std::uint64_t seq = 0;  ///< per-robot 0-based monotonic (skip = loss, B-19).
    std::string robot;      ///< owning robot.
    std::string segment;    ///< "{robot}#{k}" this event concerns.
    std::string location;   ///< the vertex entered/departed.
    bool entered = false;   ///< true = arrival (segment.to), false = departure (segment.from).
};

/// @brief An mrs.entry_events 2.0.0 document (upstream, trajopt -> sadg_t0).
struct BoundaryEntryEvents
{
    static constexpr const char* kSchema = "mrs.entry_events";
    static constexpr const char* kSchemaVersion = "2.0.0";
    std::string schema = kSchema;
    std::string schema_version = kSchemaVersion;
    std::string instance_id;
    std::vector<BoundaryEntryEvent> events;
};

// -------------------------------------------------------------- mrs.stop_declaration 5.0.1
// (consume)

/// @brief One stop declaration (stop_declaration.schema.json §6, legal `reason`
/// subset {exogenous_block, infeasible_subgoal, unresolvable_locally}).
struct BoundaryStopDeclaration
{
    std::uint64_t seq = 0;     ///< per-robot 0-based monotonic (skip = loss).
    std::string robot;         ///< owning robot.
    std::string blocked_from;  ///< blocked_edge.from_id.
    std::string blocked_to;    ///< blocked_edge.to_id.
    bool declared = false;     ///< open (true) / release (false).
    std::string reason;        ///< the vocabulary string (unvalidated here; the
                               ///< node validates against the closed vocabulary
                               ///< before constructing this struct).
};

/// @brief An mrs.stop_declaration 5.0.1 document (upstream, trajopt -> sadg_t0).
struct BoundaryStopDeclarations
{
    static constexpr const char* kSchema = "mrs.stop_declaration";
    static constexpr const char* kSchemaVersion = "5.0.1";
    std::string schema = kSchema;
    std::string schema_version = kSchemaVersion;
    std::string instance_id;
    std::vector<BoundaryStopDeclaration> declarations;
};

// ----------------------------------------------------------------- mrs.segment_release 2.0.0
// (produce)

/// @brief One segment's arrival-vertex entry permit bit (segment_release.schema.json).
struct BoundarySegmentReleaseItem
{
    std::string id;         ///< "{robot}#{k}".
    bool released = false;  ///< true = arrival-vertex entry now permitted.
};

/// @brief An mrs.segment_release 2.0.0 document. One artifact = one robot.
struct BoundarySegmentRelease
{
    static constexpr const char* kSchema = "mrs.segment_release";
    static constexpr const char* kSchemaVersion = "2.0.0";
    std::string schema = kSchema;
    std::string schema_version = kSchemaVersion;
    std::string instance_id;
    std::string robot;
    std::uint64_t release_seq = 0;    ///< node-owned: increments every publish.
    std::uint64_t commit_seq = 0;     ///< echoes mrs.execution_constraints.commit_seq (node-owned).
    std::uint64_t entry_seq_ack = 0;  ///< node-owned: 1 + max(entry_events.seq) reflected so far.
    std::vector<BoundarySegmentReleaseItem> segments;
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_BOUNDARY_TYPES_HPP
