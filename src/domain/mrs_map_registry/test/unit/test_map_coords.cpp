// test_map_coords.cpp — 노드 ↔ 좌표 변환의 벽 (TransformNode MODE_NODE_TO_POSE/MODE_POSE_TO_NODE).
//
// ## 이 파일이 지키는 것
// - node→pose: 뷰 노드 id 의 좌표를 정확히 돌려준다(물리·균일 가상 노드·골격).
// - pose→node: 최근접 노드 + 잔차. **node→pose→node 방향은 잔차 0**(TransformNode.srv (i)).
// - 오류 경로 구별: 없는 뷰 VIEW_NOT_FOUND, 없는 노드 NODE_NOT_FOUND, 비유한 질의 거부.
//
// ## 이 파일이 검증하지 **못하는** 것
// - 좌표계 정렬·물리 실측: unit 티어 밖(합성 좌표). pose→node 는 다대일이라 임의 좌표 잔차 0 불가.
//
// R-17: 성공(정확 조회·잔차 0)과 실패(없는 노드/뷰·비유한)를 대조로 둔다.

#include <cmath>
#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "map_registry_test_fixtures.hpp"
#include "mrs_map_registry/map_registry.hpp"

using namespace mrs_map_registry_test;

namespace
{

/**
 * @brief 경로 5-17-42 (좌표 (0,0)-(5,0)-(10,0)), 두 엣지 각 길이 5.0. 비조밀 물리 id.
 * @return `std::string` — 물리 roadmap YAML.
 */
std::string coord_path_yaml()
{
  std::string y = "nodes:\n";
  y += node_line(5U, 0.0, 0.0, 2.0, true, false);
  y += node_line(17U, 5.0, 0.0, 2.0, false, false);
  y += node_line(42U, 10.0, 0.0, 2.0, true, false);
  y += "edges:\n";
  y += edge_line(0U, 5U, 17U, 2.30);  // 길이 5.0 → 세분화 시 가상 노드 1
  y += edge_line(1U, 17U, 42U, 2.30); // 길이 5.0 → 가상 노드 1
  return y;
}

/**
 * @brief 물리 roadmap 을 로드한 레지스트리를 만든다.
 * @param[in] yaml 물리 roadmap YAML. 자료형 `const std::string &`.
 * @return `mrs::MapRegistry` — 로드된 레지스트리(이동 반환).
 */
mrs::MapRegistry make_loaded(const std::string & yaml)
{
  TempYamlFile file(yaml);
  mrs::MapRegistry reg;
  (void)reg.load_physical_roadmap(file.path());
  return reg;
}

} // namespace

// node→pose: 물리 노드 id 의 좌표를 정확히 돌려준다(theta = 0).
TEST(MapCoords, NodeToPosePhysical)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  const auto p5 = reg.node_to_pose(0U, 5U);
  ASSERT_TRUE(p5.is_ok());
  EXPECT_DOUBLE_EQ(p5.value.x_m, 0.0);
  EXPECT_DOUBLE_EQ(p5.value.y_m, 0.0);
  EXPECT_DOUBLE_EQ(p5.value.theta_rad, 0.0);

  const auto p42 = reg.node_to_pose(0U, 42U);
  ASSERT_TRUE(p42.is_ok());
  EXPECT_DOUBLE_EQ(p42.value.x_m, 10.0);
  EXPECT_DOUBLE_EQ(p42.value.y_m, 0.0);
}

// node→pose: 균일 뷰 가상 노드도 조회된다 — 좌표는 get_view 의 저장값과 일치해야 한다.
TEST(MapCoords, NodeToPoseUniformVirtual)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const auto view = reg.get_view(uni.value);
  ASSERT_TRUE(view.is_ok());

  bool checked_virtual = false;
  for (const mrs::RoadmapNodeView & n : view.value.nodes)
  {
    if (n.is_virtual)
    {
      const auto pose = reg.node_to_pose(uni.value, n.node_id);
      ASSERT_TRUE(pose.is_ok()) << "virtual node " << n.node_id;
      EXPECT_DOUBLE_EQ(pose.value.x_m, n.x_m);
      EXPECT_DOUBLE_EQ(pose.value.y_m, n.y_m);
      checked_virtual = true;
    }
  }
  EXPECT_TRUE(checked_virtual); // 가상 노드가 실제로 존재해 검사가 vacuous 하지 않음
}

// node→pose 실패 구별 — 없는 노드 NODE_NOT_FOUND, 없는 뷰 VIEW_NOT_FOUND.
TEST(MapCoords, NodeToPoseErrorsDistinguished)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  EXPECT_EQ(reg.node_to_pose(0U, 999U).status, mrs::MapStatus::NODE_NOT_FOUND);
  EXPECT_EQ(reg.node_to_pose(999U, 5U).status, mrs::MapStatus::VIEW_NOT_FOUND);
}

// pose→node: 최근접 노드 + 잔차. (5.1, 0.0)은 노드 17(5,0)에 가장 가깝고 잔차 ≈ 0.1.
TEST(MapCoords, PoseToNodeNearestWithResidual)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  const auto near = reg.pose_to_node(0U, 5.1, 0.0);
  ASSERT_TRUE(near.is_ok());
  EXPECT_EQ(near.value.node_id, 17U);
  EXPECT_NEAR(near.value.residual_m, 0.1, 1e-9);
}

// **무손실 방향** — node→pose 로 얻은 좌표를 되돌리면 같은 노드·잔차 0 (TransformNode.srv (i)).
TEST(MapCoords, NodePoseNodeRoundTripHasZeroResidual)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  for (const std::uint32_t node : {5U, 17U, 42U})
  {
    const auto pose = reg.node_to_pose(0U, node);
    ASSERT_TRUE(pose.is_ok());
    const auto back = reg.pose_to_node(0U, pose.value.x_m, pose.value.y_m);
    ASSERT_TRUE(back.is_ok());
    EXPECT_EQ(back.value.node_id, node);
    EXPECT_DOUBLE_EQ(back.value.residual_m, 0.0); // node→pose→node 는 잔차 0
  }
}

// pose→node 비유한 질의는 최근접이 성립하지 않는다 — 거부(대조군: 유한 질의는 통과).
TEST(MapCoords, PoseToNodeRejectsNonFiniteQuery)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  ASSERT_TRUE(reg.pose_to_node(0U, 0.0, 0.0).is_ok()); // 대조군
  EXPECT_EQ(reg.pose_to_node(0U, std::nan(""), 0.0).status, mrs::MapStatus::NODE_NOT_FOUND);
  EXPECT_EQ(reg.pose_to_node(0U, 0.0, std::nan("")).status, mrs::MapStatus::NODE_NOT_FOUND);
}

// pose→node 없는 뷰 VIEW_NOT_FOUND.
TEST(MapCoords, PoseToNodeViewNotFound)
{
  mrs::MapRegistry reg = make_loaded(coord_path_yaml());
  EXPECT_EQ(reg.pose_to_node(999U, 0.0, 0.0).status, mrs::MapStatus::VIEW_NOT_FOUND);
}

// 미로드 레지스트리는 좌표 연산 전 MAP_NOT_LOADED (안전 폴백).
TEST(MapCoords, CoordsFailBeforeLoad)
{
  mrs::MapRegistry reg;
  EXPECT_EQ(reg.node_to_pose(0U, 5U).status, mrs::MapStatus::MAP_NOT_LOADED);
  EXPECT_EQ(reg.pose_to_node(0U, 0.0, 0.0).status, mrs::MapStatus::MAP_NOT_LOADED);
}
