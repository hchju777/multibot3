// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_TYPES_HPP
#define MRS_SADG_CORE_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

/// @file types.hpp
/// @brief Internal (method-owned) data structures for the execution layer.
///
/// These types are entirely free (CLAUDE.md invariant 6): they never cross the
/// boundary. The boundary artifacts (mrs.execution_constraints 3.0.0,
/// mrs.discrete_plan 4.0.0, mrs.segment_release 2.0.0) are produced/consumed
/// only in `adapter/` (`from_boundary`/`to_boundary`) and serialized only in
/// `io/`. Absolute time, timesteps, k_robustness, solver options and budgets
/// never appear here — those live in `config/sadg.yaml` (CN-6).
///
/// A segment id has the form "{robot}#{k}" (k >= 0), the k-th step->step+1 hop of
/// a robot's discrete plan. This is the same id shape `check_sadg_acyclic.py`
/// and `discrete_plan` 4.0.0 use, so the adapter is an identity on ids.

namespace mrs_sadg::core
{

/// @brief A segment identifier "{robot}#{k}" (internal == boundary spelling).
using SegmentId = std::string;

/// @brief Dependency kind carried by the internal graph (contract §2 ［D］).
enum class DepType
{
    kSequential,  ///< Type-1: same robot, consecutive segments (E0 chain).
    kOrdering     ///< Type-2: cross-robot ordering at a shared location (［D5］~［D8］).
};

/// @brief A compiled segment: the k-th hop steps[k] -> steps[k+1] of a robot.
struct Segment
{
    SegmentId id;       ///< "{robot}#{k}".
    std::string robot;  ///< owning robot.
    std::string from;   ///< steps[k].location.
    std::string to;     ///< steps[k+1].location.
};

/// @brief A dependency edge from -> to of a given kind.
struct Dependency
{
    SegmentId from;                     ///< predecessor segment id.
    SegmentId to;                       ///< successor segment id.
    DepType type = DepType::kOrdering;  ///< sequential | ordering.
};

/// @brief One robot's occupancy of a shared location in visit order.
struct VisitItem
{
    std::string robot;        ///< the robot.
    std::uint32_t index = 0;  ///< its step index at that location.
};

/// @brief The ordered occupancy sequence of one shared location.
struct VisitEntry
{
    std::string location;             ///< the shared location.
    std::vector<VisitItem> sequence;  ///< robots that visit it, in visit order.
};

/// @brief One robot's discrete plan as an ordered location list.
struct RobotSteps
{
    std::string robot;                   ///< the robot.
    std::string terminal;                ///< its terminal (goal) label (opaque here).
    std::vector<std::string> locations;  ///< steps[i].location, i = 0..n-1.
};

/// @brief The internal view of an mrs.discrete_plan 4.0.0 (H-cons premise held).
struct PlanView
{
    std::string instance_id;              ///< scenario instance.
    std::uint64_t plan_revision = 0;      ///< discrete_plan.plan_revision.
    std::vector<RobotSteps> robots;       ///< per-robot step lists.
    std::vector<VisitEntry> visit_order;  ///< shared-location visit orders.
};

/// @brief One alternative of a switch group: the ordering edges it installs.
struct SwitchAlternative
{
    std::string id;                ///< alternative id (opaque, unique in group).
    std::vector<Dependency> deps;  ///< ordering edges (all kOrdering).
};

/// @brief A switch group at a shared location: mutually-exclusive, complete.
struct SwitchGroup
{
    std::string id;                               ///< group id (opaque, unique).
    std::string location;                         ///< the shared location.
    std::vector<SwitchAlternative> alternatives;  ///< >= 2 mutually-exclusive orders.
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_TYPES_HPP
