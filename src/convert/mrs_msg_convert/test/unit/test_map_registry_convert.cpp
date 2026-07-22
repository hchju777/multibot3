// test_map_registry_convert.cpp — [1] D-11 map 3뷰·검증 응답 변환의 벽.
//
// 여기서 지키는 것: mappings_to_msg 의 용례 3종(subdivision/collapse_node/collapse_chain) 왕복,
// fill_response(GetUniformView/GetDependencySkeleton) 골든, fill_response(ValidateMap) 의 PASS/FAIL
// 판정·뷰별 강타입 5배열 이관·v6 의 -1(미산출) 센티넬 통과.
//
// ## R-17 규율 — 검사 장치가 실제로 잡는지
// 모든 거부 케이스는 **정상 대조군이 통과함**을 함께 확인한다(control + mutation). 그렇지 않으면
// 하네스가 고장나도 초록불이 켜진다.
//
// ## 이 파일이 검증하지 **못하는** 것
// - 세분화·collapse 알고리즘 자체(왕복 무손실): MapRegistry(C4) unit 테스트의 몫.
// - DDS 왕복·물리: pysim 이상. QP feasibility·livelock: isaac 필수.

#include <cmath>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "convert_test_fixtures.hpp"

using mrs::convert::ConvertResult;
using mrs::convert::ConvertStatus;

namespace
{

/** @brief 뷰 컨테이너 bare id 센티넬(= 강타입 none 값). */
constexpr std::uint32_t NONE = mrs::ROADMAP_ID_NONE;

/**
 * @brief 세분화 균일 뷰를 만든다 — 물리 엣지 1개를 2분할해 가상 노드 1개·SUBDIVISION 대응 1건.
 * @return `mrs::RoadmapViewData` — `view_kind = UNIFORM`, `view_id = 1`.
 */
mrs::RoadmapViewData make_uniform_view()
{
  mrs::RoadmapViewData view;
  view.view_kind = mrs::ViewKind::UNIFORM;
  view.roadmap_version = 42;
  view.view_id = 1;
  view.unit_length_m = 2.0;
  view.unit_length_lower_bound_m = 2.211518; // T1-R6 iw.hub
  view.effective_unit_min_m = 2.0;
  view.effective_unit_max_m = 2.0;
  view.subdivided_edge_ratio = 1.0;

  // 균일 노드 3개: 물리 두 끝(0, 1) + 가상 중간(2).
  mrs::RoadmapNodeView n0;
  n0.node_id = 0;
  n0.x_m = 0.0;
  n0.claim_radius_m = 1.0;
  n0.is_endpoint = true;
  mrs::RoadmapNodeView n1 = n0;
  n1.node_id = 1;
  n1.x_m = 4.0;
  mrs::RoadmapNodeView v2 = n0;
  v2.node_id = 2;
  v2.x_m = 2.0;
  v2.is_endpoint = false;
  v2.is_virtual = true;
  view.nodes = {n0, n1, v2};

  mrs::RoadmapEdgeView e0;
  e0.edge_id = 0;
  e0.node_a = 0;
  e0.node_b = 2;
  e0.length_m = 2.0;
  e0.corridor_width_m = 1.6;
  e0.speed_limit_mps = 0.5;
  mrs::RoadmapEdgeView e1 = e0;
  e1.edge_id = 1;
  e1.node_a = 2;
  e1.node_b = 1;
  view.edges = {e0, e1};

  // 용례 1: 가상 노드 2 → 물리 엣지 100 위 s = 0.5 (k = 1, n = 2).
  mrs::NodeMappingView m;
  m.kind = mrs::MappingKind::SUBDIVISION;
  m.from_node_id = mrs::UniformNodeId{2};
  m.to_physical_edge_id = mrs::PhysicalEdgeId{100};
  m.s = 0.5;
  m.index = 1;
  m.denominator = 2;
  view.mappings = {m};
  return view;
}

/**
 * @brief collapse 골격 뷰를 만든다 — COLLAPSE_NODE 1건 + COLLAPSE_CHAIN 1건.
 * @return `mrs::RoadmapViewData` — `view_kind = SKELETON`, `view_id = 1`.
 */
mrs::RoadmapViewData make_skeleton_view()
{
  mrs::RoadmapViewData view;
  view.view_kind = mrs::ViewKind::SKELETON;
  view.roadmap_version = 42;
  view.view_id = 1;

  mrs::RoadmapNodeView s0;
  s0.node_id = 0;
  s0.x_m = 0.0;
  s0.claim_radius_m = 1.0;
  s0.is_endpoint = true;
  mrs::RoadmapNodeView s1 = s0;
  s1.node_id = 1;
  s1.x_m = 4.0;
  view.nodes = {s0, s1};

  mrs::RoadmapEdgeView e0;
  e0.edge_id = 0;
  e0.node_a = 0;
  e0.node_b = 1;
  e0.length_m = 4.0;
  e0.corridor_width_m = 1.6;
  e0.speed_limit_mps = 0.5;
  view.edges = {e0};

  // 용례 2: 균일 노드 0 → 골격 노드 0 (서수 미사용).
  mrs::NodeMappingView cn;
  cn.kind = mrs::MappingKind::COLLAPSE_NODE;
  cn.from_node_id = mrs::UniformNodeId{0};
  cn.to_skeleton_node_id = mrs::SkeletonNodeId{0};
  // 용례 3: 삼켜진 균일 가상 노드 2 → 골격 엣지 0, 체인 서수 1/2.
  mrs::NodeMappingView cc;
  cc.kind = mrs::MappingKind::COLLAPSE_CHAIN;
  cc.from_node_id = mrs::UniformNodeId{2};
  cc.to_skeleton_edge_id = mrs::SkeletonEdgeId{0};
  cc.index = 1;
  cc.denominator = 2;
  view.mappings = {cn, cc};
  return view;
}

/**
 * @brief 전 검사기 통과 검증 결과를 만든다 (임계 에코는 iw.hub, v_max 닫힌형은 -1 미산출).
 * @return `mrs::RoadmapValidationResult` — `result_pass() == true`.
 */
mrs::RoadmapValidationResult make_passing_result()
{
  mrs::RoadmapValidationResult r;
  r.well_formed = true;
  r.biconnected = false; // U-23 미평가 — result_pass 에 넣지 않는다
  r.pass_width_ok = true;
  r.unit_granularity_ok = true;
  r.r_clear_ok = true;
  r.pass_width_min_m = 1.575808;
  r.turn90_width_min_m = 1.893663;
  r.uturn_width_min_m = 2.211518;
  r.meet_pass_width_min_m = 3.151616;
  r.required_r_clear_m = 1.084724;
  r.required_min_width_m = -1.0; // pending-[0b] 미산출
  r.derated_v_max_mps = -1.0;    // pending-[0b] 미산출
  return r;
}

} // namespace

// ── mappings_to_msg — 3 용례 왕복 ────────────────────────────────────────────

// 용례 1: SUBDIVISION 은 to_edge_id(물리 엣지)·s·서수를 싣고 to_node_id 는 센티넬로 남는다.
TEST(MapRegistryConvert, MappingsSubdivisionRoundtrip)
{
  const mrs::RoadmapViewData view = make_uniform_view();
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  ASSERT_TRUE(mrs::convert::mappings_to_msg(view, out).ok);
  ASSERT_EQ(out.size(), 1U);
  EXPECT_EQ(out[0].from_node_id, 2U);
  EXPECT_EQ(out[0].to_edge_id, 100U);
  EXPECT_EQ(out[0].to_node_id, NONE); // 배타성 — 골격 노드 대상은 해당 없음
  EXPECT_DOUBLE_EQ(out[0].s, 0.5);
  EXPECT_EQ(out[0].index, 1U);
  EXPECT_EQ(out[0].denominator, 2U);
}

// 용례 2·3: 골격 뷰의 두 대응이 각각 to_node_id / to_edge_id 로 갈리고 방향이 유지된다.
TEST(MapRegistryConvert, MappingsCollapseRoundtrip)
{
  const mrs::RoadmapViewData view = make_skeleton_view();
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  ASSERT_TRUE(mrs::convert::mappings_to_msg(view, out).ok);
  ASSERT_EQ(out.size(), 2U);

  // 용례 2 COLLAPSE_NODE: to_node_id 유효, to_edge_id 센티넬, 서수 미사용(denominator = 0).
  EXPECT_EQ(out[0].from_node_id, 0U);
  EXPECT_EQ(out[0].to_node_id, 0U);
  EXPECT_EQ(out[0].to_edge_id, NONE);
  EXPECT_EQ(out[0].denominator, 0U);

  // 용례 3 COLLAPSE_CHAIN: to_edge_id 유효, to_node_id 센티넬, 서수 1/2, s 미사용(0).
  EXPECT_EQ(out[1].from_node_id, 2U);
  EXPECT_EQ(out[1].to_edge_id, 0U);
  EXPECT_EQ(out[1].to_node_id, NONE);
  EXPECT_EQ(out[1].index, 1U);
  EXPECT_EQ(out[1].denominator, 2U);
  EXPECT_DOUBLE_EQ(out[1].s, 0.0);
}

// 방향 역전 방어 — SUBDIVISION 인데 골격 대상이 섞여 있으면 거부한다(control: 정상은 통과).
TEST(MapRegistryConvert, MappingsRejectsDirectionReversal)
{
  mrs::RoadmapViewData view = make_uniform_view();
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  ASSERT_TRUE(mrs::convert::mappings_to_msg(view, out).ok); // 대조군

  view.mappings[0].to_skeleton_node_id = mrs::SkeletonNodeId{7}; // 배타성 위반
  EXPECT_EQ(mrs::convert::mappings_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// from_node_id 중복은 대응이 함수가 아니게 만든다 — 왕복 무손실의 전제를 깬다.
TEST(MapRegistryConvert, MappingsRejectsDuplicateFromId)
{
  mrs::RoadmapViewData view = make_skeleton_view();
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  ASSERT_TRUE(mrs::convert::mappings_to_msg(view, out).ok); // 대조군

  view.mappings[1].from_node_id = view.mappings[0].from_node_id; // 중복
  EXPECT_EQ(mrs::convert::mappings_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 서수 범위 — index > denominator 는 엣지 위 위치가 될 수 없다.
TEST(MapRegistryConvert, MappingsRejectsOrdinalOutOfRange)
{
  mrs::RoadmapViewData view = make_uniform_view();
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  ASSERT_TRUE(mrs::convert::mappings_to_msg(view, out).ok); // 대조군

  view.mappings[0].index = 3; // denominator = 2 인데 index = 3
  EXPECT_EQ(mrs::convert::mappings_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 지도 버전 0 은 런타임 금지값 — 스코프 자기 유효성 실패.
TEST(MapRegistryConvert, MappingsRejectsVersionZero)
{
  mrs::RoadmapViewData view = make_uniform_view();
  view.roadmap_version = mrs::ROADMAP_VERSION_UNSPECIFIED;
  std::vector<mrs_interfaces::msg::NodeMapping> out;
  EXPECT_EQ(mrs::convert::mappings_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── fill_response(GetUniformView) ────────────────────────────────────────────

// 성공 골든 — 봉투 스칼라·배열 3종이 전부 채워지고 RESULT_OK.
TEST(MapRegistryConvert, UniformViewFillSuccess)
{
  const mrs::RoadmapViewData view = make_uniform_view();
  mrs_interfaces::srv::GetUniformView::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(view, resp).ok);
  EXPECT_EQ(resp.result, mrs_interfaces::srv::GetUniformView::Response::RESULT_OK);
  EXPECT_EQ(resp.roadmap_version, 42U);
  EXPECT_EQ(resp.view_id, 1U);
  EXPECT_DOUBLE_EQ(resp.unit_length_m, 2.0);
  EXPECT_DOUBLE_EQ(resp.unit_length_lower_bound_m, 2.211518);
  EXPECT_DOUBLE_EQ(resp.subdivided_edge_ratio, 1.0);
  EXPECT_EQ(resp.nodes.size(), 3U);
  EXPECT_EQ(resp.edges.size(), 2U);
  EXPECT_EQ(resp.subdivision_map.size(), 1U);
}

// V1 종류 대조 — 물리 뷰를 균일 응답에 넣으면 VIEW_KIND_MISMATCH (control: 균일은 통과).
TEST(MapRegistryConvert, UniformViewRejectsWrongKind)
{
  mrs::RoadmapViewData view = make_uniform_view();
  mrs_interfaces::srv::GetUniformView::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(view, resp).ok); // 대조군

  view.view_kind = mrs::ViewKind::PHYSICAL;
  EXPECT_EQ(mrs::convert::fill_response(view, resp).reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

TEST(MapRegistryConvert, UniformViewRejectsVersionZero)
{
  mrs::RoadmapViewData view = make_uniform_view();
  view.roadmap_version = mrs::ROADMAP_VERSION_UNSPECIFIED;
  mrs_interfaces::srv::GetUniformView::Response resp;
  EXPECT_EQ(mrs::convert::fill_response(view, resp).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── fill_response(GetDependencySkeleton) ─────────────────────────────────────

TEST(MapRegistryConvert, SkeletonFillSuccess)
{
  const mrs::RoadmapViewData view = make_skeleton_view();
  mrs_interfaces::srv::GetDependencySkeleton::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(view, resp).ok);
  EXPECT_EQ(resp.result, mrs_interfaces::srv::GetDependencySkeleton::Response::RESULT_OK);
  EXPECT_EQ(resp.roadmap_version, 42U);
  EXPECT_EQ(resp.view_id, 1U);
  EXPECT_EQ(resp.nodes.size(), 2U);
  EXPECT_EQ(resp.collapse_map.size(), 2U);
}

// V1 종류 대조 — 균일 뷰를 골격 응답에 넣으면 VIEW_KIND_MISMATCH.
TEST(MapRegistryConvert, SkeletonRejectsWrongKind)
{
  mrs::RoadmapViewData view = make_skeleton_view();
  mrs_interfaces::srv::GetDependencySkeleton::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(view, resp).ok); // 대조군

  view.view_kind = mrs::ViewKind::UNIFORM;
  EXPECT_EQ(mrs::convert::fill_response(view, resp).reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// ── fill_response(ValidateMap) ───────────────────────────────────────────────

// 전 하드 검사기 통과 → RESULT_PASS, bool·임계 에코·-1 미산출이 그대로 실린다.
TEST(MapRegistryConvert, ValidateMapPassAndUnmeasuredMinusOne)
{
  const mrs::RoadmapValidationResult result = make_passing_result();
  mrs_interfaces::srv::ValidateMap::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(result, 42, 1, resp).ok);
  EXPECT_EQ(resp.result, mrs_interfaces::srv::ValidateMap::Response::RESULT_PASS);
  EXPECT_TRUE(resp.well_formed);
  EXPECT_TRUE(resp.pass_width_ok);
  EXPECT_FALSE(resp.biconnected); // 미평가 — PASS 를 막지 않는다
  EXPECT_EQ(resp.roadmap_version, 42U);
  EXPECT_EQ(resp.view_id, 1U);
  EXPECT_DOUBLE_EQ(resp.meet_pass_width_min_m, 3.151616);
  // v6 핵심 — -1(미산출)이 비음 검사에 걸려 거부되지 않고 그대로 나간다.
  EXPECT_DOUBLE_EQ(resp.required_min_width_m, -1.0);
  EXPECT_DOUBLE_EQ(resp.derated_v_max_mps, -1.0);
}

// 하드 bool 하나라도 false 면 RESULT_FAIL (control: 전부 true → PASS 는 위 테스트).
TEST(MapRegistryConvert, ValidateMapFailWhenOneHardCheckFails)
{
  mrs::RoadmapValidationResult result = make_passing_result();
  result.pass_width_ok = false;
  mrs_interfaces::srv::ValidateMap::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(result, 42, 1, resp).ok);
  EXPECT_EQ(resp.result, mrs_interfaces::srv::ValidateMap::Response::RESULT_FAIL);
}

// 뷰별 강타입 5배열이 원시 값으로 정확히 이관된다(엔티티·뷰 라벨은 배열 위치로 보존).
TEST(MapRegistryConvert, ValidateMapViolationArraysCarryRawValues)
{
  mrs::RoadmapValidationResult result = make_passing_result();
  result.pass_width_ok = false;
  result.narrow_pass_edge_ids = {mrs::PhysicalEdgeId{100}, mrs::PhysicalEdgeId{101}};
  result.mutex_candidate_edge_ids = {mrs::PhysicalEdgeId{100}};
  mrs_interfaces::srv::ValidateMap::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(result, 42, 1, resp).ok);
  ASSERT_EQ(resp.narrow_pass_edge_ids.size(), 2U);
  EXPECT_EQ(resp.narrow_pass_edge_ids[0], 100U);
  EXPECT_EQ(resp.narrow_pass_edge_ids[1], 101U);
  ASSERT_EQ(resp.mutex_candidate_edge_ids.size(), 1U);
  EXPECT_EQ(resp.mutex_candidate_edge_ids[0], 100U);
}

// view_id = 0(물리만 검사)인데 균일 뷰 위반이 실려 있으면 검사 범위와 모순 → 거부.
TEST(MapRegistryConvert, ValidateMapRejectsUniformViolationWhenPhysicalOnly)
{
  mrs::RoadmapValidationResult result = make_passing_result();
  mrs_interfaces::srv::ValidateMap::Response resp;
  ASSERT_TRUE(mrs::convert::fill_response(result, 42, 0, resp).ok); // 대조군 (균일 배열 비어 있음)

  result.violating_uniform_node_ids = {mrs::UniformNodeId{5}};
  EXPECT_EQ(
    mrs::convert::fill_response(result, 42, 0, resp).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 임계 에코 비유한은 도메인 산술 오류 — 거부.
TEST(MapRegistryConvert, ValidateMapRejectsNonFiniteThreshold)
{
  mrs::RoadmapValidationResult result = make_passing_result();
  result.pass_width_min_m = std::nan("");
  mrs_interfaces::srv::ValidateMap::Response resp;
  EXPECT_EQ(
    mrs::convert::fill_response(result, 42, 1, resp).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// -1 이외의 음수 닫힌형은 도메인 오류 — 거부(-1 만 유효 미산출 센티넬).
TEST(MapRegistryConvert, ValidateMapRejectsBadNegativeClosedForm)
{
  mrs::RoadmapValidationResult result = make_passing_result();
  result.derated_v_max_mps = -5.0; // 미산출(-1)이 아닌 음수
  mrs_interfaces::srv::ValidateMap::Response resp;
  EXPECT_EQ(
    mrs::convert::fill_response(result, 42, 1, resp).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 지도 버전 0 에코는 런타임 금지값 — 거부.
TEST(MapRegistryConvert, ValidateMapRejectsVersionZero)
{
  const mrs::RoadmapValidationResult result = make_passing_result();
  mrs_interfaces::srv::ValidateMap::Response resp;
  EXPECT_EQ(
    mrs::convert::fill_response(result, 0, 1, resp).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}
