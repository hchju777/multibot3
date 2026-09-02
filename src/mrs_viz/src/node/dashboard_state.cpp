// SPDX-License-Identifier: Apache-2.0
#include "dashboard_state.hpp"

namespace mrs_viz::node
{

nlohmann::json ToJson(const DashboardState& s,
                      std::size_t max_trajectory_points_per_robot,
                      std::size_t max_dependency_edges)
{
    nlohmann::json out;
    out["stamp_sec"] = s.stamp_sec;
    out["frame_id"] = s.frame_id;

    // --- ① 위치·경로·튜브 ---
    nlohmann::json roadmap;
    roadmap["have_data"] = s.have_roadmap;
    auto nodes = nlohmann::json::array();
    for (const auto& n : s.roadmap_nodes)
    {
        nodes.push_back({{"id", n.id}, {"x", n.x}, {"y", n.y}, {"is_endpoint", n.is_endpoint}});
    }
    roadmap["nodes"] = nodes;
    auto edges = nlohmann::json::array();
    for (const auto& e : s.roadmap_edges)
    {
        edges.push_back({{"from_id", e.from_id}, {"to_id", e.to_id}});
    }
    roadmap["edges"] = edges;
    out["roadmap"] = roadmap;

    auto robots = nlohmann::json::object();
    for (const auto& robot : s.robot_roster)
    {
        const auto it = s.robots.find(robot);
        nlohmann::json r;
        if (it != s.robots.end() && it->second.has_odom)
        {
            r["has_odom"] = true;
            r["x"] = it->second.x;
            r["y"] = it->second.y;
            r["yaw"] = it->second.yaw;
        }
        else
        {
            r["has_odom"] = false;  // 값을 지어내지 않는다 — 아직 odom 없으면 좌표 0건.
        }
        const auto rel_it = s.segment_release_seq.find(robot);
        if (rel_it != s.segment_release_seq.end())
        {
            r["last_segment_release_seq"] = rel_it->second;
        }
        robots[robot] = r;
    }
    out["robots"] = robots;

    nlohmann::json traj;
    traj["have_data"] = s.have_trajectories;
    traj["instance_id"] = s.trajectories_instance_id;
    traj["min_separation_m"] = s.min_separation_m;
    auto traj_robots = nlohmann::json::array();
    for (const auto& rt : s.trajectories)
    {
        nlohmann::json rj;
        rj["robot"] = rt.robot;
        rj["n_points_total"] = rt.points.size();
        auto pts = nlohmann::json::array();
        // 표시 상한 — 값을 왜곡하지 않는 균등 간격 서브샘플(끝점은 항상 포함).
        const std::size_t n = rt.points.size();
        const std::size_t cap = max_trajectory_points_per_robot;
        if (n <= cap || cap == 0)
        {
            for (const auto& p : rt.points)
            {
                pts.push_back({{"x", p.x}, {"y", p.y}});
            }
        }
        else if (cap == 1)
        {
            pts.push_back({{"x", rt.points.back().x}, {"y", rt.points.back().y}});
        }
        else
        {
            for (std::size_t k = 0; k < cap; ++k)
            {
                const std::size_t idx = (k * (n - 1)) / (cap - 1);
                pts.push_back({{"x", rt.points[idx].x}, {"y", rt.points[idx].y}});
            }
        }
        rj["points"] = pts;
        traj_robots.push_back(rj);
    }
    traj["robots"] = traj_robots;
    out["trajectories"] = traj;

    // --- ③ SADG 의존 그래프·스위칭 ---
    nlohmann::json exec;
    exec["have_data"] = s.have_exec_constraints;
    exec["commit_seq"] = s.commit_seq;
    exec["n_segments"] = s.n_segments;
    exec["n_dependencies_total"] = s.n_dependencies_total;
    auto deps = nlohmann::json::array();
    std::size_t dep_count = 0;
    for (const auto& d : s.dependencies)
    {
        if (dep_count >= max_dependency_edges)
        {
            break;
        }
        deps.push_back({{"from_id", d.from_id}, {"to_id", d.to_id}, {"type", d.type_name}});
        ++dep_count;
    }
    exec["dependencies"] = deps;
    exec["dependencies_truncated"] = s.n_dependencies_total > deps.size();
    auto groups = nlohmann::json::array();
    for (const auto& g : s.switch_groups)
    {
        groups.push_back(
            {{"id", g.id}, {"location", g.location}, {"n_alternatives", g.n_alternatives}});
    }
    exec["switch_groups"] = groups;
    out["execution_constraints"] = exec;

    // --- ② 재계획 사건(상태 전이만 — 요청·결과 payload는 관측 불가, 374§1) ---
    nlohmann::json replan;
    replan["have_data"] = s.have_replan_status;
    auto goals = nlohmann::json::array();
    std::size_t goal_count = 0;
    for (const auto& g : s.replan_goals)
    {
        if (goal_count >= max_dependency_edges)
        {
            break;
        }
        goals.push_back({{"goal_id_short", g.goal_id_short}, {"status", g.status_name}});
        ++goal_count;
    }
    replan["goals"] = goals;
    out["replan_status"] = replan;

    // --- ② 서브골 열 — 계측(diagnostic)이며 현재 발행자가 없다(374§1). ---
    out["subgoal_stream"] = {{"have_data", false},
                             {"note", "trajopt 내부 T-C 단계 — 진단 토픽 미발행 (333_p2§4-2②)"}};

    // --- ④ 틱 예산·계측 게이지 ---
    out["diagnostics"] = {
        {"have_data", s.have_diagnostics},
        {"note", "현재 파이프라인에 /diagnostics 발행자가 없다 (CN-19 소비자 측만 준비됨)"}};

    nlohmann::json result_summary;
    result_summary["have_data"] = s.have_result_summary;
    if (s.have_result_summary)
    {
        result_summary["content"] = s.result_summary_raw;
    }
    out["result_summary"] = result_summary;

    return out;
}

}  // namespace mrs_viz::node
