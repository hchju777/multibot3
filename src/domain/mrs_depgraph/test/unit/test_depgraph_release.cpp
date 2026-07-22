// test_depgraph_release.cpp — 창 릴리스·진행 이벤트·Type-2 통행순서의 벽 (D-05b).
//
// ## 이 파일이 지키는 것
// - `release_next_window`: 의존성 충족 세그먼트를 릴리스하고, 미충족 진입 지점에서 정지하며
//   predecessor_constraint 를 싣는다. W1(seq 단조)·W2(세그먼트 연결)·W3(첫 세그먼트=프론티어).
// - `on_progress_event`: 선행 로봇이 공유 노드를 클리어하면 후속 로봇의 재릴리스가 진행된다.
// - Type-2 통행순서(D-05a build 의 산물)가 실제로 선행/후속을 가른다 — 릴리스가 그것을 소비한다.
//
// ## 이 파일이 검증하지 **못하는** 것 (정직성)
// - W4~W7(제동여유·만료·커밋포·TRUNCATE): L4 CommitState·동역학 상수(v_max) 입력 게이트 — 미배선.
// - QP feasibility·무livelock·F1∧F5: **isaac 필수**(pending-isaac).
//
// R-17: 막힘(후속 대기)과 해소(선행 클리어 후 진행)를 대조로 두어 통행순서 검사가 실제로 잡음을
// 확인.

#include <cstdint>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "mrs_depgraph/dependency_graph.hpp"

namespace
{

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

/**
 * @brief 공유 노드 20 에서 로봇 0(t=1)이 로봇 1(t=1.5)보다 먼저 통과하는 2로봇 계획.
 *        로봇 0: 10→20→30, 로봇 1: 40→20→50. ⇒ 노드 20 에서 로봇 0 이 선행.
 */
std::vector<mrs::RobotPath> crossing_plan()
{
  return {
    make_path(0, {{10, 0.0}, {20, 1.0}, {30, 2.0}}),
    make_path(1, {{40, 0.0}, {20, 1.5}, {50, 2.5}})};
}

} // namespace

// 단일 로봇(충돌 없음)은 전 경로를 한 창으로 릴리스하고, 이후 릴리스는 없다.
TEST(DepgraphRelease, SingleRobotReleasesFullPath)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths({make_path(0, {{10, 0.0}, {11, 1.0}, {12, 2.0}})}, 7U, 1U));

  mrs::ExecutionWindow window;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{0}, window));
  EXPECT_EQ(window.robot_id, 0);
  EXPECT_EQ(window.window_seq, 1U);
  EXPECT_EQ(window.revision_kind, mrs::RevisionKind::NEW);
  ASSERT_EQ(window.segments.size(), 2U);
  EXPECT_TRUE(window.predecessor_constraints.empty());
  EXPECT_EQ(window.roadmap_version, 7U);
  EXPECT_EQ(window.view_id, 1U);
  // W3: 첫 세그먼트 node_from = 프론티어(시작 노드 10).
  EXPECT_EQ(window.segments[0].node_from, mrs::UniformNodeId{10});
  // W2: 세그먼트 연결.
  EXPECT_EQ(window.segments[0].node_to, window.segments[1].node_from);
  EXPECT_EQ(window.segments[1].node_to, mrs::UniformNodeId{12});

  mrs::ExecutionWindow again;
  EXPECT_FALSE(graph.release_next_window(mrs::RobotId{0}, again)); // 목표 도달 — 더 없음
}

// 미구축·미지 로봇 릴리스는 false (안전).
TEST(DepgraphRelease, UnbuiltOrUnknownRobotReturnsFalse)
{
  mrs::DependencyGraph graph;
  mrs::ExecutionWindow window;
  EXPECT_FALSE(graph.release_next_window(mrs::RobotId{0}, window)); // 미구축

  ASSERT_TRUE(graph.build_from_paths({make_path(0, {{10, 0.0}, {11, 1.0}})}, 7U, 1U));
  EXPECT_FALSE(graph.release_next_window(mrs::RobotId{9}, window)); // 미지 로봇
}

// 선행 로봇(0)은 공유 노드에서 막히지 않고 전 경로를 릴리스한다(대조군).
TEST(DepgraphRelease, PredecessorRobotReleasesUnblocked)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(crossing_plan(), 7U, 1U));

  mrs::ExecutionWindow window;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{0}, window));
  EXPECT_EQ(window.segments.size(), 2U);               // 10→20, 20→30
  EXPECT_TRUE(window.predecessor_constraints.empty()); // 노드 20 에서 첫 통과자 — 제약 없음
}

// 후속 로봇(1)은 공유 노드 진입 지점에서 막혀 predecessor_constraint 를 받는다.
TEST(DepgraphRelease, SuccessorRobotBlockedAtSharedNode)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(crossing_plan(), 7U, 1U));

  mrs::ExecutionWindow window;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{1}, window));
  ASSERT_EQ(window.segments.size(), 1U); // 40→20 까지만(20 진입 전 대기)
  EXPECT_EQ(window.segments[0].node_from, mrs::UniformNodeId{40});
  EXPECT_EQ(window.segments[0].node_to, mrs::UniformNodeId{20});
  ASSERT_EQ(window.predecessor_constraints.size(), 1U);
  EXPECT_EQ(window.predecessor_constraints[0].predecessor_robot_id, 0); // 로봇 0 이 선행
  EXPECT_EQ(window.predecessor_constraints[0].node_id, mrs::UniformNodeId{20});
}

// 막힌 후속 로봇은 선행이 클리어하기 전엔 재릴리스되지 않는다(대기).
TEST(DepgraphRelease, BlockedSuccessorWaitsOnReRelease)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(crossing_plan(), 7U, 1U));

  mrs::ExecutionWindow window;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{1}, window)); // 첫 창(40→20 + 제약)
  mrs::ExecutionWindow again;
  EXPECT_FALSE(graph.release_next_window(mrs::RobotId{1}, again)); // 선행 미클리어 — 재릴리스 없음
}

// 선행이 공유 노드를 클리어하면(on_progress_event) 후속의 재릴리스가 진행되고 seq 가 단조 증가한다.
TEST(DepgraphRelease, ProgressUnblocksSuccessorAndSeqIsMonotone)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(crossing_plan(), 7U, 1U));

  mrs::ExecutionWindow first;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{1}, first));
  EXPECT_EQ(first.window_seq, 1U);

  // 로봇 0 이 시작(10)·공유노드(20)를 차례로 클리어 → cleared_count_[0] = 2 (노드 20 통과).
  graph.on_progress_event(mrs::RobotId{0}, mrs::UniformNodeId{10}, 0.5);
  graph.on_progress_event(mrs::RobotId{0}, mrs::UniformNodeId{20}, 1.2);

  mrs::ExecutionWindow second;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{1}, second)); // 이제 진행
  EXPECT_EQ(second.window_seq, 2U);                                // W1 단조
  ASSERT_EQ(second.segments.size(), 1U);                           // 20→50
  EXPECT_EQ(second.segments[0].node_from, mrs::UniformNodeId{20}); // W3: 프론티어=20
  EXPECT_EQ(second.segments[0].node_to, mrs::UniformNodeId{50});
  EXPECT_TRUE(second.predecessor_constraints.empty());
}

// 순서 외 클리어는 진행 상태를 앞당기지 않는다 — 후속은 여전히 막힌다.
TEST(DepgraphRelease, OutOfOrderClearDoesNotUnblock)
{
  mrs::DependencyGraph graph;
  ASSERT_TRUE(graph.build_from_paths(crossing_plan(), 7U, 1U));
  mrs::ExecutionWindow window;
  ASSERT_TRUE(graph.release_next_window(mrs::RobotId{1}, window)); // 막힘

  // 로봇 0 이 공유 노드(20)를 **먼저** 보고 — 그러나 다음 예상 클리어는 시작(10)이라 무시된다.
  graph.on_progress_event(mrs::RobotId{0}, mrs::UniformNodeId{20}, 1.2);
  mrs::ExecutionWindow again;
  EXPECT_FALSE(graph.release_next_window(mrs::RobotId{1}, again)); // 여전히 대기
}
