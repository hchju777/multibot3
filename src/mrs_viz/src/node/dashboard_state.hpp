// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_VIZ_NODE_DASHBOARD_STATE_HPP
#define MRS_VIZ_NODE_DASHBOARD_STATE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

/// @file dashboard_state.hpp
/// @brief Plain (ROS-free) snapshot of what the browser dashboard renders +
/// its `ToJson` serializer (374_mrs_viz_dashboard.md — 48차 웨이브 3).
///
/// 🔴 Deliberately ROS-free: `dashboard_node.cpp` fills a `DashboardState`
/// from the ROS messages it has cached, then calls `ToJson`. Keeping the
/// struct + serializer here (not in the `.cpp`) makes `ToJson` unit-testable
/// without `rclcpp::init` (`test/test_dashboard_snapshot_json.cpp`) — the same
/// reason `mrs_sim/src/node/roadmap_json.hpp` is a free-standing header
/// (comment there: "공개 include/mrs_viz/를 두지 않는 이유는 ... CN-1 필수
/// 넷을 이 도구 패키지에 강제하지 않기 위함").
///
/// 🔴 D18 ("렌더러가 둘로 갈리지 않게"): this file does NOT reimplement
/// `mrs_sim/viz_markers_node.cpp`'s derived-fact logic (e.g. "the" target
/// segment per robot via `{robot}#{index}` parsing). It carries raw fields
/// (segment / dependency lists) and lets the browser decide what to
/// highlight. Both renderers read the SAME boundary topics — they do not
/// derive from each other — so there is one source of truth even though
/// there are two rendering targets (rviz markers vs. browser canvas).

namespace mrs_viz::node
{

/// @brief One robot's most recent pose (from `/{robot}/odom`).
struct RobotPoseState
{
    bool has_odom = false;
    double x = 0.0;
    double y = 0.0;
    double yaw = 0.0;
};

/// @brief One roadmap vertex (from `/roadmap`).
struct RoadmapNodeLite
{
    std::string id;
    double x = 0.0;
    double y = 0.0;
    bool is_endpoint = false;
};

/// @brief One roadmap edge (from `/roadmap`), coordinates resolved from
/// `RoadmapNodeLite` by id at serialization time.
struct RoadmapEdgeLite
{
    std::string from_id;
    std::string to_id;
};

/// @brief One trajectory sample (from `/trajectories`).
struct TrajectoryPointLite
{
    double x = 0.0;
    double y = 0.0;
};

/// @brief One robot's planned trajectory (from `/trajectories`).
struct RobotTrajectoryLite
{
    std::string robot;
    std::vector<TrajectoryPointLite> points;
};

/// @brief One SADG dependency edge (from `/execution_constraints`).
struct DependencyLite
{
    std::string from_id;
    std::string to_id;
    std::string type_name;  ///< via `mrs_core_msgs::DependencyTypeToString`.
};

/// @brief One SADG switch group (from `/execution_constraints`).
struct SwitchGroupLite
{
    std::string id;
    std::string location;
    std::size_t n_alternatives = 0;
};

/// @brief One observed action goal status (from `/replan/_action/status`).
struct ReplanGoalStatusLite
{
    std::string goal_id_short;  ///< first 4 bytes of the UUID, hex.
    std::string status_name;
};

/// @brief The full browser-facing snapshot. `dashboard_node.cpp` rebuilds
/// one of these on every `snapshot_period_ms` tick from cached subscriber
/// state and serializes it via `ToJson`.
struct DashboardState
{
    double stamp_sec = 0.0;
    std::string frame_id;

    bool have_roadmap = false;
    std::vector<RoadmapNodeLite> roadmap_nodes;
    std::vector<RoadmapEdgeLite> roadmap_edges;

    /// Robot roster order is preserved (not alphabetized) — comes from the
    /// same `robots` roster as `state_integrator`/`viz_markers_node`
    /// (F48-6 single-source pattern), passed in by the launch file.
    std::vector<std::string> robot_roster;
    std::map<std::string, RobotPoseState> robots;

    bool have_trajectories = false;
    std::string trajectories_instance_id;
    double min_separation_m = 0.0;  ///< `mrs.trajectories.limits.min_separation_m` — used to
                                    ///< draw a "tube" of this width around each path; not a
                                    ///< new field, a rendering choice on an existing one.
    std::vector<RobotTrajectoryLite> trajectories;

    bool have_exec_constraints = false;
    std::uint64_t commit_seq = 0;
    std::size_t n_segments = 0;
    std::size_t n_dependencies_total = 0;
    std::vector<DependencyLite> dependencies;  ///< capped at `max_dependency_edges`.
    std::vector<SwitchGroupLite> switch_groups;

    /// Per-robot latest `release_seq` observed on `/{robot}/segment_release`.
    std::map<std::string, std::uint64_t> segment_release_seq;

    bool have_replan_status = false;
    std::vector<ReplanGoalStatusLite> replan_goals;  ///< capped at `max_dependency_edges`.

    /// 🔴 No current producer in the pipeline for `/diagnostics` (CN-19) —
    /// confirmed by source grep, 374§1. Kept as an honest "no data" flag
    /// forward-compatible with a future producer; `dashboard_node` never
    /// fabricates content here.
    bool have_diagnostics = false;

    /// 🔴 No `mrs_eval` in this repo yet — `result_json_path` param is
    /// empty by default and this stays false. If a run's `result.json`
    /// appears at the configured path, its raw parsed content is passed
    /// through verbatim (no reinterpretation).
    bool have_result_summary = false;
    nlohmann::json result_summary_raw;
};

/// @brief Serialize @p s to the JSON body served at `GET /api/state`.
/// @param s Snapshot to serialize.
/// @param max_trajectory_points_per_robot Per-robot trajectory point cap
///        (display-only decimation, not a physical limit — see
///        `config/viz_dashboard.yaml` header table).
/// @param max_dependency_edges Cap shared by the dependency list and the
///        replan-goal list (display-only).
/// @return A JSON object with the keys documented in
///         `374_mrs_viz_dashboard.md` §「경계 토픽 → JSON snapshot 필드」.
nlohmann::json ToJson(const DashboardState& s,
                      std::size_t max_trajectory_points_per_robot,
                      std::size_t max_dependency_edges);

}  // namespace mrs_viz::node

#endif  // MRS_VIZ_NODE_DASHBOARD_STATE_HPP
