// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_TYPES_HPP
#define MRS_MAPF_CORE_TYPES_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

/// @file types.hpp
/// @brief Internal (non-boundary) data structures for the MAPF module (CN-2/CN-3
/// pure — no ROS, no JSON, no `mrs_msgs`).
///
/// 🔴 These types are free to diverge from the boundary shape (module-developer
/// skill "어댑터 규칙" — 내부는 자유, 경계는 스키마 그대로). The single place that
/// converts between this file's types and the frozen schemas is `adapter/`
/// (CN-8/CN-9). No timestep, solver cost, k-robustness, suboptimality, horizon
/// or seed field lives on the boundary side (`adapter/boundary_types.hpp`) —
/// those may appear HERE only, as internal working state.

namespace mrs_mapf::core
{

/// @brief Directed-traversal vocabulary of a roadmap edge (mirrors mrs.roadmap 3.0.0).
enum class Traversal
{
    kBidirectional,
    kForwardOnly,
    kBackwardOnly
};

/// @brief One directed traversable edge, expanded from `mrs.roadmap.edges[]`
/// (an edge with `bidirectional` traversal expands into two `RoadmapArc`s).
struct RoadmapArc
{
    std::string from;
    std::string to;
    double length_m = 0.0;    ///< geometric traversal length [m].
    double width_m = 0.0;     ///< passable width [m] (unused by the search plugin today).
    int capacity_robots = 1;  ///< lanes abreast (roadmap 3.0.0 semantics — not an in-flight cap).
    std::string corridor;     ///< corridor id, empty = none.
};

/// @brief Internal roadmap graph — the search plugin's only view of `mrs.roadmap`.
struct Roadmap
{
    std::vector<std::string>
        node_ids;  ///< dense-index order (CN §255-8-2 decision 2 analogue for vertices).
    std::map<std::string, std::vector<RoadmapArc>>
        out_arcs;                        ///< node id -> outgoing directed arcs.
    std::vector<std::string> endpoints;  ///< V^ep — waiting/goal-eligible vertices.

    /// @brief Is @p id a declared node?
    bool has_node(const std::string& id) const
    {
        for (const auto& n : node_ids)
        {
            if (n == id)
            {
                return true;
            }
        }
        return false;
    }
};

/// @brief One robot's physical spec relevant to this module (`mrs.robot_specs`
/// 1.1.0 — A33 forbids consuming anything but `robot` and
/// `avg_traversal_speed_mps` here; the other five fields are read by `mrs_trajopt`
/// only).
struct RobotSpec
{
    std::string robot;
    double avg_traversal_speed_mps = 0.0;  ///< nominal average speed [m/s], > 0.
};

/// @brief One robot's assignment (`mrs.assignment` 2.0.0) — start vertex + ordered goal locations.
struct AssignmentEntry
{
    std::string robot;
    std::string start;  ///< most-recent-sample vertex (§5-3 caveat — not "now").
    std::vector<std::string> goal_locations;  ///< remaining goal sequence, in order.
};

/// @brief A directed blocked edge (`mrs.replan_request.blocked[]`).
struct BlockedEdge
{
    std::string from;
    std::string to;
};

/// @brief One already-realized entry in a location's ordering constraint
/// (`mrs.replan_request.ordering_constraints[]` — the *received*, i.e. hard,
/// prefix; time-free, index-only).
struct OrderingPrefixItem
{
    std::string robot;
    std::uint32_t index = 0;  ///< that robot's committed `plan_revision`-space step index.
};

/// @brief One location's realized-prefix ordering constraint.
struct OrderingConstraint
{
    std::string location;
    std::vector<OrderingPrefixItem> sequence;  ///< non-empty, already-realized prefix.
};

/// @brief One robot's baseline commit count (`mrs.replan_request.baseline[]`).
struct BaselineEntry
{
    std::string robot;
    std::uint32_t committed_segment_count = 0;  ///< k — §286-1 #5 / PX09.
};

/// @brief One step of a robot's plan (index is the position in `steps`, not a timestep).
struct StepEntry
{
    std::uint32_t index = 0;
    std::string location;
};

/// @brief Closed vocabulary for `mrs.discrete_plan.plans[].terminal`.
enum class Terminal
{
    kGoalReached,
    kPlanTruncated,
    kNoProgress
};

/// @brief One robot's plan (`mrs.discrete_plan.plans[]`).
struct RobotPlan
{
    std::string robot;
    Terminal terminal = Terminal::kNoProgress;
    std::vector<StepEntry>
        steps;  ///< length >= 1. Consecutive equal locations are illegal (fold rule).
};

/// @brief One resolved visit-order item (`(robot, index)` — a pointer into that robot's `steps`).
struct VisitItem
{
    std::string robot;
    std::uint32_t index = 0;
};

/// @brief One location's total order of visits (`mrs.discrete_plan.visit_order[]`).
struct VisitOrderEntry
{
    std::string location;
    std::vector<VisitItem> sequence;  ///< size >= 2, no duplicate (robot,index) pairs.
};

/// @brief The internal draft of a full `mrs.discrete_plan` document, pre-adapter.
struct DiscretePlanDraft
{
    std::string instance_id;
    std::uint64_t plan_revision = 0;
    std::vector<RobotPlan> plans;
    std::vector<VisitOrderEntry> visit_order;
};

/// @brief Everything `service/` gathers before calling a planning strategy —
/// the internal counterpart of the boundary inputs, already through `adapter/`.
struct PlanningRequest
{
    std::string instance_id;
    Roadmap roadmap;
    std::vector<RobotSpec>
        robot_specs;  ///< full roster (needed for RB-1 on out-of-scope robots — §255-8-3).
    std::vector<AssignmentEntry> assignments;  ///< full roster.
    std::vector<std::string> affected_robots;  ///< A^req — floor, may be widened (§255-3).
    std::vector<BlockedEdge> blocked;
    std::vector<OrderingConstraint>
        ordering_constraints;  ///< received/realized prefix — hard constraint.
    std::vector<BaselineEntry> baseline;
    std::uint64_t request_plan_revision =
        0;  ///< replan_request.plan_revision — which discrete_plan this request's indices were
            ///< computed against.
    std::optional<DiscretePlanDraft>
        previous_plan;  ///< PX09 "retained" — the producer's own last publish, if held.
};

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_TYPES_HPP
