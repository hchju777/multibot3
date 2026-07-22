// test_map_transform.cpp — 뷰 간 노드 변환의 왕복 무손실을 지키는 벽 (D-11, 통과 기준 오차 0).
//
// ## 이 파일이 지키는 것 (R-14 — 왕복 항등 테스트 전락 방지)
// phys→uniform→phys · phys→skeleton→phys 왕복이 원본 물리 id 를 정확히 복원함을 **픽스처 손선언
// 절대 대조**로 검증한다. 물리 노드 id 를 비조밀(5, 17, 42)로 두어 균일/골격의 조밀 id(0,1,2)와
// 어긋나게 했다 — 그래야 왕복이 항등이 아니라 실제 대응임이 드러나고, f∘g 를 같은 표에서 파생하지
// 않는다. 정방향 사상은 손으로 적은 절대 대응표(5→0, 17→1, 42→2)에 직접 못박는다.
//
// ## 이 파일이 검증하지 **못하는** 것 (충실도 티어 정직성)
// - 좌표계 정렬·물리 실측: unit 티어 밖. 여기 좌표는 위상 관계를 만들기 위한 합성값이다.
//
// R-17: 무손실(성공)과 대응 부재(NO_MAPPING)·없는 뷰/노드(VIEW/NODE_NOT_FOUND)를 대조로 둔다.

#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "map_registry_test_fixtures.hpp"
#include "mrs_map_registry/map_registry.hpp"

using namespace mrs_map_registry_test;

namespace
{

/**
 * @brief 비조밀 물리 노드 id(5, 17, 42)로 된 경로 5-17-42, 두 엣지 각 길이 5.0.
 *        비조밀 id 는 균일/골격의 조밀 id 와 어긋나 왕복이 항등이 아님을 드러낸다(R-14).
 * @return `std::string` — 물리 roadmap YAML.
 */
std::string sparse_id_path_yaml()
{
  std::string y = "nodes:\n";
  y += node_line(5U, 0.0, 0.0, 2.0, true, false);   // 물리 노드 5
  y += node_line(17U, 5.0, 0.0, 2.0, false, false); // 물리 노드 17
  y += node_line(42U, 10.0, 0.0, 2.0, true, false); // 물리 노드 42
  y += "edges:\n";
  y += edge_line(0U, 5U, 17U, 2.30);  // 길이 5.0 → 세분화 시 가상 노드 1
  y += edge_line(1U, 17U, 42U, 2.30); // 길이 5.0 → 가상 노드 1
  return y;
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

// R-14 절대 대조 — phys→uniform 정방향이 손선언 표(5→0, 17→1, 42→2)와 일치, 왕복이 원본 복원.
TEST(MapTransform, PhysicalUniformRoundTripIsLosslessAgainstDeclaredTable)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());
  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const std::uint32_t uid = uni.value;

  // ── 정방향 절대 대응 (손선언 — 구현 파생 아님). 물리는 조밀 순서로 균일에 사상된다. ──────
  const auto f5 = reg.transform_node(0U, 5U, uid);
  const auto f17 = reg.transform_node(0U, 17U, uid);
  const auto f42 = reg.transform_node(0U, 42U, uid);
  ASSERT_TRUE(f5.is_ok() && f17.is_ok() && f42.is_ok());
  EXPECT_EQ(f5.value, 0U);
  EXPECT_EQ(f17.value, 1U);
  EXPECT_EQ(f42.value, 2U);
  // 항등이 아님을 명시 — 비조밀 물리 id 가 조밀 균일 id 와 다르다(왕복 전락 방지의 핵심).
  EXPECT_NE(f5.value, 5U);
  EXPECT_NE(f17.value, 17U);
  EXPECT_NE(f42.value, 42U);

  // ── 왕복 무손실 (오차 0, 정확 일치) ─────────────────────────────────────────────────
  for (const std::uint32_t phys : {5U, 17U, 42U})
  {
    const auto fwd = reg.transform_node(0U, phys, uid);
    ASSERT_TRUE(fwd.is_ok());
    const auto back = reg.transform_node(uid, fwd.value, 0U);
    ASSERT_TRUE(back.is_ok());
    EXPECT_EQ(back.value, phys); // phys → uniform → phys == 원본
  }
}

// R-14 절대 대조 — phys→skeleton 정방향 손선언 표 + 왕복 복원.
TEST(MapTransform, PhysicalSkeletonRoundTripIsLosslessAgainstDeclaredTable)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());
  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const auto skel = reg.build_dependency_skeleton(uni.value);
  ASSERT_TRUE(skel.is_ok());
  const std::uint32_t sid = skel.value;

  // 골격 노드 = 물리 토폴로지 → 물리 조밀 순서 사상(5→0, 17→1, 42→2).
  EXPECT_EQ(reg.transform_node(0U, 5U, sid).value, 0U);
  EXPECT_EQ(reg.transform_node(0U, 17U, sid).value, 1U);
  EXPECT_EQ(reg.transform_node(0U, 42U, sid).value, 2U);

  for (const std::uint32_t phys : {5U, 17U, 42U})
  {
    const auto fwd = reg.transform_node(0U, phys, sid);
    ASSERT_TRUE(fwd.is_ok());
    const auto back = reg.transform_node(sid, fwd.value, 0U);
    ASSERT_TRUE(back.is_ok());
    EXPECT_EQ(back.value, phys); // phys → skeleton → phys == 원본
  }
}

// 가상 노드 → 물리 = NO_MAPPING (subdivision 이 만든 가상 노드는 대응 물리 노드가 없다).
TEST(MapTransform, VirtualUniformNodeHasNoPhysicalMapping)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());
  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const std::uint32_t uid = uni.value;

  // 물리 3 노드 → 균일 0,1,2. 가상 노드는 그 다음 id(3,4). 하나라도 NO_MAPPING 이어야 한다.
  const auto view = reg.get_view(uid);
  ASSERT_TRUE(view.is_ok());
  bool checked_virtual = false;
  for (const mrs::RoadmapNodeView & n : view.value.nodes)
  {
    if (n.is_virtual)
    {
      const auto r = reg.transform_node(uid, n.node_id, 0U);
      EXPECT_EQ(r.status, mrs::MapStatus::NO_MAPPING) << "virtual node " << n.node_id;
      checked_virtual = true;
    }
  }
  EXPECT_TRUE(checked_virtual); // 가상 노드가 실제로 존재해 검사가 vacuous 하지 않음
}

// 오류 경로 — 없는 뷰 VIEW_NOT_FOUND, 없는 노드 NODE_NOT_FOUND.
TEST(MapTransform, UnknownViewAndNodeAreDistinguished)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());
  const auto uni = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni.is_ok());
  const std::uint32_t uid = uni.value;

  // 없는 뷰 id.
  const auto bad_view = reg.transform_node(999U, 5U, uid);
  EXPECT_EQ(bad_view.status, mrs::MapStatus::VIEW_NOT_FOUND);

  // 존재하는 물리 뷰지만 없는 노드 id.
  const auto bad_node = reg.transform_node(0U, 999U, uid);
  EXPECT_EQ(bad_node.status, mrs::MapStatus::NODE_NOT_FOUND);
}

// build_uniform 전 비물리 뷰로의 변환 = VIEW_NOT_FOUND (파생 뷰가 아직 없다).
TEST(MapTransform, TransformToNonPhysicalViewBeforeBuildFails)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());
  // 뷰 id 1 은 아직 발급되지 않았다(균일 뷰 미생성).
  const auto r = reg.transform_node(0U, 5U, 1U);
  EXPECT_EQ(r.status, mrs::MapStatus::VIEW_NOT_FOUND);
}

// build_uniform 재호출은 골격 뷰를 무효화하고 새 균일 뷰 id 를 발급한다.
TEST(MapTransform, RebuildingUniformInvalidatesSkeleton)
{
  mrs::MapRegistry reg = make_loaded(sparse_id_path_yaml());

  const auto uni1 = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni1.is_ok());
  EXPECT_EQ(uni1.value, 1U); // 손선언: 첫 균일 뷰 id

  const auto skel = reg.build_dependency_skeleton(uni1.value);
  ASSERT_TRUE(skel.is_ok());
  EXPECT_EQ(skel.value, 2U); // 손선언: 골격 뷰 id
  ASSERT_TRUE(reg.get_view(skel.value).is_ok());

  // 재세분화 — 이전 골격이 무효화되고 균일 뷰 id 가 새로 발급된다.
  const auto uni2 = reg.build_uniform_view(2.30, LOWER_BOUND);
  ASSERT_TRUE(uni2.is_ok());
  EXPECT_EQ(uni2.value, 3U); // 손선언: 다음 뷰 id (단조 증가)

  // 옛 골격 뷰는 이제 존재하지 않는다.
  EXPECT_EQ(reg.get_view(2U).status, mrs::MapStatus::VIEW_NOT_FOUND);
  EXPECT_EQ(reg.transform_node(2U, 0U, 0U).status, mrs::MapStatus::VIEW_NOT_FOUND);
  // 새 균일 뷰는 조회·변환 가능.
  EXPECT_TRUE(reg.get_view(3U).is_ok());
  EXPECT_TRUE(reg.transform_node(0U, 5U, 3U).is_ok());
}
