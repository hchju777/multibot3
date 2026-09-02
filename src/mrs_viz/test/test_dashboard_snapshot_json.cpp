// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest, no rclcpp — same precedent as
// test_http_request_parse.cpp). Locks `ToJson` (dashboard_state.hpp/.cpp):
// the ROS-free serializer `dashboard_node.cpp` calls every snapshot tick.

#include <cassert>
#include <cstdio>
#include <string>

#include "../src/node/dashboard_state.hpp"

using namespace mrs_viz::node;

namespace
{

void TestEmptyStateHasNoDataFlagsFalse()
{
    DashboardState s;
    s.robot_roster = {"r0", "r1"};
    const auto j = ToJson(s, 200, 200);
    assert(j["roadmap"]["have_data"] == false);
    assert(j["trajectories"]["have_data"] == false);
    assert(j["execution_constraints"]["have_data"] == false);
    assert(j["replan_status"]["have_data"] == false);
    assert(j["result_summary"]["have_data"] == false);
    // 값을 지어내지 않는다 — roster에 있어도 odom 없으면 has_odom false.
    assert(j["robots"]["r0"]["has_odom"] == false);
    assert(j["robots"]["r1"]["has_odom"] == false);
}

void TestRobotPoseRoundTrips()
{
    DashboardState s;
    s.robot_roster = {"r0"};
    RobotPoseState pose;
    pose.has_odom = true;
    pose.x = 1.5;
    pose.y = -2.25;
    pose.yaw = 0.75;
    s.robots["r0"] = pose;
    const auto j = ToJson(s, 200, 200);
    assert(j["robots"]["r0"]["has_odom"] == true);
    assert(j["robots"]["r0"]["x"] == 1.5);
    assert(j["robots"]["r0"]["y"] == -2.25);
    assert(j["robots"]["r0"]["yaw"] == 0.75);
}

void TestTrajectoryPointCapKeepsEndpoints()
{
    DashboardState s;
    s.have_trajectories = true;
    RobotTrajectoryLite rt;
    rt.robot = "r0";
    for (int i = 0; i < 1000; ++i)
    {
        rt.points.push_back(TrajectoryPointLite{static_cast<double>(i), 0.0});
    }
    s.trajectories.push_back(rt);
    const auto j = ToJson(s, /*max_trajectory_points_per_robot=*/10, 200);
    const auto& pts = j["trajectories"]["robots"][0]["points"];
    assert(pts.size() == 10);
    assert(j["trajectories"]["robots"][0]["n_points_total"] == 1000);
    // 끝점 보존 — 서브샘플이 경로 길이를 눈에 띄게 줄이지 않는다.
    assert(pts.front()["x"] == 0.0);
    assert(pts.back()["x"] == 999.0);
}

void TestTrajectoryCapZeroMeansUnlimited()
{
    DashboardState s;
    s.have_trajectories = true;
    RobotTrajectoryLite rt;
    rt.robot = "r0";
    for (int i = 0; i < 50; ++i)
    {
        rt.points.push_back(TrajectoryPointLite{static_cast<double>(i), 0.0});
    }
    s.trajectories.push_back(rt);
    const auto j = ToJson(s, /*max_trajectory_points_per_robot=*/0, 200);
    assert(j["trajectories"]["robots"][0]["points"].size() == 50);
}

void TestDependencyCapTruncatesAndFlags()
{
    DashboardState s;
    s.have_exec_constraints = true;
    s.n_dependencies_total = 5;
    for (int i = 0; i < 5; ++i)
    {
        s.dependencies.push_back(DependencyLite{"a" + std::to_string(i), "b", "sequential"});
    }
    const auto j = ToJson(s, 200, /*max_dependency_edges=*/2);
    assert(j["execution_constraints"]["dependencies"].size() == 2);
    assert(j["execution_constraints"]["dependencies_truncated"] == true);
    assert(j["execution_constraints"]["n_dependencies_total"] == 5);
}

void TestDependencyNoTruncationWhenUnderCap()
{
    DashboardState s;
    s.have_exec_constraints = true;
    s.n_dependencies_total = 1;
    s.dependencies.push_back(DependencyLite{"a", "b", "ordering"});
    const auto j = ToJson(s, 200, 200);
    assert(j["execution_constraints"]["dependencies_truncated"] == false);
}

void TestSubgoalStreamAndDiagnosticsAlwaysReportNoData()
{
    // 374§1 — 현재 파이프라인에 발행자가 0건임을 실측 확인했다. ToJson이
    // 이 둘을 have_data:true로 지어내면 안 된다(입력으로 켤 스위치가 없다).
    DashboardState s;
    const auto j = ToJson(s, 200, 200);
    assert(j["subgoal_stream"]["have_data"] == false);
    assert(j["diagnostics"]["have_data"] == false);
}

void TestResultSummaryPassesThroughVerbatim()
{
    DashboardState s;
    s.have_result_summary = true;
    s.result_summary_raw = nlohmann::json{{"seed", 7}, {"outcome", "ok"}};
    const auto j = ToJson(s, 200, 200);
    assert(j["result_summary"]["have_data"] == true);
    assert(j["result_summary"]["content"]["seed"] == 7);
    assert(j["result_summary"]["content"]["outcome"] == "ok");
}

}  // namespace

int main()
{
    TestEmptyStateHasNoDataFlagsFalse();
    TestRobotPoseRoundTrips();
    TestTrajectoryPointCapKeepsEndpoints();
    TestTrajectoryCapZeroMeansUnlimited();
    TestDependencyCapTruncatesAndFlags();
    TestDependencyNoTruncationWhenUnderCap();
    TestSubgoalStreamAndDiagnosticsAlwaysReportNoData();
    TestResultSummaryPassesThroughVerbatim();
    std::puts("test_dashboard_snapshot_json: all assertions passed");
    return 0;
}
