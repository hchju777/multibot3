// test_map_subdivision.cpp — 세분화(물리→균일)·collapse(균일→골격) 구조 골든을 지키는 벽 (D-11
// v2/v6).
//
// ## 이 파일이 지키는 것
// build_uniform 의 입도 하한 강제(unit < 2(ρ+L*) 및 unit 보다 짧은 물리 엣지 거부),
// floor 균등분할 결과(가상 노드 수·실효 입도 [unit,2unit)·subdivided_edge_ratio·
// claim_radius = 최소 인접 균일 엣지/2), 그리고 collapse 골격이 물리 토폴로지(노드·엣지 수 동일,
// view_kind=SKELETON)이며 collapse_map 에 용례 2(COLLAPSE_NODE)·용례 3(COLLAPSE_CHAIN)이 존재함.
//
// ## 이 파일이 검증하지 **못하는** 것 (충실도 티어 정직성)
// - 세분화가 물리 회피 성능·경로 품질에 미치는 영향: 시스템 레벨(pysim/isaac) 관측 대상.
// - 왕복 무손실 좌표 변환의 수치 항등: transform 전담 파일(test_map_transform.cpp)이 맡는다.
//
// R-17: 하한 강제·짧은 엣지 거부는 통과 픽스처(정상 세분화)와 위반 픽스처를 대조로 둔다.

#include <cstddef>
#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "map_registry_test_fixtures.hpp"
#include "mrs_map_registry/map_registry.hpp"

using namespace mrs_map_registry_test;

namespace
{

/**
 * @brief 길이 5.0 물리 엣지 1개(노드 0-1)만 있는 최소 세분화 대상 맵.
 * @return `std::string` — 물리 roadmap YAML.
 */
std::string single_edge_len5_yaml()
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, true, false);
  y += node_line(1U, 5.0, 0.0, 2.0, true, false); // 길이 5.0
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  return y;
}

/**
 * @brief 물리 노드 수만큼 가상 노드(is_virtual)를 센다.
 * @param[in] view 균일 뷰 데이터. 자료형 `const mrs::RoadmapViewData &`.
 * @return `std::size_t` — 가상 노드 개수.
 */
std::size_t count_virtual(const mrs::RoadmapViewData & view)
{
  std::size_t n = 0;
  for (const mrs::RoadmapNodeView & node : view.nodes)
  {
    if (node.is_virtual)
    {
      ++n;
    }
  }
  return n;
}

/**
 * @brief 물리 roadmap 을 로드한 레지스트리를 만든다(로드 성공 전제).
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

// 정상 세분화 — 길이 5.0 엣지를 unit 2.30 으로 floor 분할: seg=2, 가상 1, eff=2.5, ratio=1.0.
TEST(MapSubdivision, UniformViewGoldenStructure)
{
  mrs::MapRegistry reg = make_loaded(single_edge_len5_yaml());
  const auto built = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(built.is_ok()) << mrs::to_string(built.status);
  const std::uint32_t uid = built.value;
  EXPECT_EQ(uid, 1U); // 손선언: 물리=0 뒤 첫 파생 뷰 id

  const auto view = reg.get_view(uid);
  ASSERT_TRUE(view.is_ok());
  const mrs::RoadmapViewData & u = view.value;

  EXPECT_EQ(u.view_kind, mrs::ViewKind::UNIFORM);
  EXPECT_EQ(u.nodes.size(), 3U); // 물리 2 + 가상 1
  EXPECT_EQ(u.edges.size(), 2U);
  EXPECT_EQ(count_virtual(u), 1U);

  EXPECT_NEAR(u.unit_length_m, 2.30, 1e-9);
  EXPECT_NEAR(u.unit_length_lower_bound_m, LOWER_BOUND, 1e-9);

  // 실효 입도 = 5.0/2 = 2.5, 그리고 [unit, 2unit) = [2.30, 4.60) 안.
  EXPECT_NEAR(u.effective_unit_min_m, 2.5, 1e-9);
  EXPECT_NEAR(u.effective_unit_max_m, 2.5, 1e-9);
  EXPECT_GE(u.effective_unit_min_m, u.unit_length_m);
  EXPECT_LT(u.effective_unit_max_m, 2.0 * u.unit_length_m);

  EXPECT_NEAR(u.subdivided_edge_ratio, 1.0, 1e-9); // 물리 엣지 1개 중 1개 분할

  // claim_radius = 최소 인접 균일 엣지 길이 / 2 = 2.5/2 = 1.25 (뷰별 규약 (j)).
  for (const mrs::RoadmapNodeView & n : u.nodes)
  {
    EXPECT_NEAR(n.claim_radius_m, 1.25, 1e-9);
  }
}

// build_uniform 전제 — 로드 전 호출은 MAP_NOT_LOADED.
TEST(MapSubdivision, BuildUniformBeforeLoadReportsMapNotLoaded)
{
  mrs::MapRegistry reg;
  const auto r = reg.build_uniform_view(2.30, LOWER_BOUND);
  EXPECT_EQ(r.status, mrs::MapStatus::MAP_NOT_LOADED);
}

// 위반 — unit 이 이론 하한 미만이면 UNIT_LENGTH_BELOW_BOUND (엣지를 보기도 전에 거부).
TEST(MapSubdivision, UnitBelowLowerBoundRejected)
{
  mrs::MapRegistry reg = make_loaded(single_edge_len5_yaml());
  const auto r = reg.build_uniform_view(2.0, LOWER_BOUND); // 2.0 < 2.211518
  EXPECT_EQ(r.status, mrs::MapStatus::UNIT_LENGTH_BELOW_BOUND);
}

// 위반 — 물리 엣지가 unit 보다 짧으면(seg<1) UNIT_LENGTH_BELOW_BOUND (D-11 risk_note).
TEST(MapSubdivision, PhysicalEdgeShorterThanUnitRejected)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, true, false);
  y += node_line(1U, 2.0, 0.0, 2.0, true, false); // 길이 2.0 < unit 2.30
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.build_uniform_view(2.30, LOWER_BOUND); // unit ≥ 하한이지만 엣지가 짧다
  EXPECT_EQ(r.status, mrs::MapStatus::UNIT_LENGTH_BELOW_BOUND);
}

// collapse 골격 = 물리 토폴로지 + collapse_map 용례 2·3 존재.
TEST(MapSubdivision, SkeletonIsPhysicalTopologyWithCollapseMappings)
{
  // 경로 0-1-2, 두 엣지 각 길이 5.0 → 각 엣지 1 가상 노드(용례 3 발생 보장).
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, true, false);
  y += node_line(1U, 5.0, 0.0, 2.0, false, false);
  y += node_line(2U, 10.0, 0.0, 2.0, true, false);
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  y += edge_line(1U, 1U, 2U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const auto skel = reg.build_dependency_skeleton(uni.value);
  ASSERT_TRUE(skel.is_ok()) << mrs::to_string(skel.status);

  const auto view = reg.get_view(skel.value);
  ASSERT_TRUE(view.is_ok());
  const mrs::RoadmapViewData & s = view.value;

  EXPECT_EQ(s.view_kind, mrs::ViewKind::SKELETON);
  EXPECT_EQ(s.nodes.size(), 3U); // 물리 토폴로지 — 가상 노드 collapse 됨
  EXPECT_EQ(s.edges.size(), 2U);

  // collapse_map: 용례 2(균일 노드→골격 노드) 와 용례 3(삼킨 가상 노드→골격 엣지) 둘 다 존재.
  std::size_t collapse_node = 0;
  std::size_t collapse_chain = 0;
  for (const mrs::NodeMappingView & m : s.mappings)
  {
    if (m.kind == mrs::MappingKind::COLLAPSE_NODE)
    {
      ++collapse_node;
    }
    else if (m.kind == mrs::MappingKind::COLLAPSE_CHAIN)
    {
      ++collapse_chain;
    }
  }
  EXPECT_EQ(collapse_node, 3U);  // 물리 노드마다 1개
  EXPECT_EQ(collapse_chain, 2U); // 가상 노드 2개(엣지당 1)
}

// build_dependency_skeleton 전제 — build_uniform 전 호출은 VIEW_NOT_FOUND.
TEST(MapSubdivision, SkeletonBeforeUniformReportsViewNotFound)
{
  mrs::MapRegistry reg = make_loaded(single_edge_len5_yaml());
  const auto r = reg.build_dependency_skeleton(1U); // 아직 균일 뷰 없음
  EXPECT_EQ(r.status, mrs::MapStatus::VIEW_NOT_FOUND);
}
