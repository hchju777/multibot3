// test_depgraph_build.cpp — ADG 구축(build_from_paths)의 검증 벽 (D-05a).
//
// ## 이 파일이 지키는 것
// - L2 산출 경로로부터 ADG 구축이 **손상된 입력을 거부**하고(버전 0·로봇 중복·센티넬 노드·시각
//   역행·비유한), 실패 시 **부분 구축을 만들지 않는다**(직전 상태 불변).
// - 예외를 던지지 않는다 — 실패는 `false` 반환(규율 #2, 순수 도메인).
//
// ## 이 파일이 검증하지 **못하는** 것 (정직성)
// - Type-2 통행순서 정렬·프론티어 초기화의 **내부 정확성**: 공개 API 로 관측 불가하며 창 릴리스가
//   그것을 소비하는 **D-05b**(release_next_window)에서 검증된다.
// - QP feasibility·무livelock·F1∧F5: **isaac 필수**(pending-isaac) — 이 티어 밖.
//
// R-17: 각 거부 케이스는 정상 대조군(통과)과 짝지어 하네스가 실제로 잡는지 확인한다.

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "mrs_depgraph/dependency_graph.hpp"

namespace
{

/**
 * @brief (노드 raw id, 도착시각) 목록으로 로봇 1대분 경로를 만든다.
 * @param[in] robot 로봇 id. 자료형 `std::uint16_t`.
 * @param[in] visits (균일 노드 raw id, arrival_time_s) 목록. 자료형 `std::vector<std::pair<...>>`.
 * @return `mrs::RobotPath` — 균일 뷰 방문열 경로.
 */
mrs::RobotPath make_path(
  std::uint16_t robot, const std::vector<std::pair<std::uint32_t, double>> & visits)
{
  mrs::RobotPath path;
  path.robot_id = mrs::RobotId{robot};
  for (const auto & visit : visits)
  {
    mrs::TimedNodeVisit tnv;
    tnv.node_id = mrs::UniformNodeId{visit.first};
    tnv.arrival_time_s = visit.second;
    path.visits.push_back(tnv);
  }
  return path;
}

/** @brief 로봇 2대·공유 노드가 있는 정상 계획(대조군). 로봇 0: 10→11→12, 로봇 1: 12→11. */
std::vector<mrs::RobotPath> valid_plan()
{
  return {make_path(0, {{10, 0.0}, {11, 1.0}, {12, 2.0}}), make_path(1, {{12, 0.5}, {11, 1.5}})};
}

} // namespace

// 정상 계획은 구축되고 스코프(지도 버전)를 보고한다.
TEST(DepgraphBuild, ValidBuildSucceedsAndReportsVersion)
{
  mrs::DependencyGraph graph;
  EXPECT_TRUE(graph.build_from_paths(valid_plan(), 7U, 1U));
  EXPECT_EQ(graph.roadmap_version(), 7U);
}

// 지도 버전 0 은 런타임 금지값 — 거부(대조군: 버전 7 은 통과).
TEST(DepgraphBuild, RejectsVersionZero)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(valid_plan(), 7U, 1U)); // 대조군
  EXPECT_FALSE(graph.build_from_paths(valid_plan(), 0U, 1U));
}

// 로봇 id 중복은 방문열 귀속을 모호하게 만든다 — 거부.
TEST(DepgraphBuild, RejectsDuplicateRobotId)
{
  mrs::DependencyGraph graph;
  const std::vector<mrs::RobotPath> dup = {
    make_path(0, {{10, 0.0}, {11, 1.0}}), make_path(0, {{12, 0.0}, {13, 1.0}})};
  EXPECT_FALSE(graph.build_from_paths(dup, 7U, 1U));
}

// 센티넬 노드(UNIFORM_NODE_ID_NONE)는 유효 방문이 될 수 없다 — 거부.
TEST(DepgraphBuild, RejectsSentinelNode)
{
  mrs::DependencyGraph graph;
  mrs::RobotPath path = make_path(0, {{10, 0.0}});
  mrs::TimedNodeVisit bad;
  bad.node_id = mrs::UNIFORM_NODE_ID_NONE;
  bad.arrival_time_s = 1.0;
  path.visits.push_back(bad);
  EXPECT_FALSE(graph.build_from_paths({path}, 7U, 1U));
}

// 도착시각 역행은 손상된 계획 — 거부(대조군: 단조 비감소는 통과).
TEST(DepgraphBuild, RejectsNonMonotoneArrival)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths({make_path(0, {{10, 0.0}, {11, 1.0}})}, 7U, 1U)); // 대조군
  EXPECT_FALSE(graph.build_from_paths({make_path(0, {{10, 1.0}, {11, 0.5}})}, 7U, 1U));
}

// 비유한 도착시각은 정렬·간격 계산을 무의미하게 만든다 — 거부.
TEST(DepgraphBuild, RejectsNonFiniteArrival)
{
  mrs::DependencyGraph graph;
  EXPECT_FALSE(graph.build_from_paths({make_path(0, {{10, 0.0}, {11, std::nan("")}})}, 7U, 1U));
}

// 빈 계획(로봇 0대)은 유효한 빈 그래프다 — 통과(버전은 여전히 기록).
TEST(DepgraphBuild, AcceptsEmptyPaths)
{
  mrs::DependencyGraph graph;
  EXPECT_TRUE(graph.build_from_paths({}, 7U, 1U));
  EXPECT_EQ(graph.roadmap_version(), 7U);
}

// 동시각·공유 노드(교행 지점)에서도 결정론적으로 구축된다(tie-break 크래시 없음).
TEST(DepgraphBuild, AcceptsSimultaneousArrivalAtSharedNode)
{
  mrs::DependencyGraph graph;
  // 로봇 0·1 이 노드 11 에 **같은 시각** 도착 — robot_id tie-break 로 결정론.
  const std::vector<mrs::RobotPath> tie = {
    make_path(0, {{10, 0.0}, {11, 1.0}}), make_path(1, {{12, 0.0}, {11, 1.0}})};
  EXPECT_TRUE(graph.build_from_paths(tie, 7U, 1U));
}

// **부분 구축 없음** — 실패한 재빌드는 직전 성공 상태를 건드리지 않는다.
TEST(DepgraphBuild, FailedRebuildLeavesPriorStateUntouched)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(valid_plan(), 7U, 1U));
  ASSERT_EQ(graph.roadmap_version(), 7U);

  // 손상된 재빌드(로봇 중복) — false 이고, 직전 버전이 그대로 남는다.
  const std::vector<mrs::RobotPath> dup = {make_path(0, {{10, 0.0}}), make_path(0, {{11, 0.0}})};
  EXPECT_FALSE(graph.build_from_paths(dup, 9U, 2U));
  EXPECT_EQ(graph.roadmap_version(), 7U); // 9 로 바뀌지 않았다 — 커밋 전 실패
}
