// test_map_validators.cpp — roadmap 정적 검사기 1·3·4 가 실제로 판정을 가르는지 지키는 벽 (D-11
// v6).
//
// ## 이 파일이 지키는 것
// 검사기 1(well-formed: 타 엔드포인트 비경유 도달성 + 비작업 엔드포인트 수 ≥ 로봇 수),
// 검사기 3(통과폭 2ρ strict / 90°점턴 2ρ+L* / U턴 2(ρ+L*) / r_clear ≥ r),
// 검사기 4(교행 4ρ advisory — result_pass 를 막지 않는 하드-하류-미방출 불변),
// 계산된 임계 에코값(ρ·L* 기하, v_max 무입력), 경계값 거동(strict `>` vs `<`/`>=`).
//
// ## 이 파일이 검증하지 **못하는** 것 (충실도 티어 정직성)
// - 검사기 2(biconnected): [1] 미평가(U-23) — 실행되지 않으며 result_pass 판정에서 제외된다.
// - v_max 의존 닫힌형(required_min_width_m 등): v_max 실측 전(pending-[0b]) 이라 -1 미산출만 검증.
// - QP feasibility·livelock·물리 재현성: unit 티어로 판정 불가. isaac 필수.
//
// ## R-19 — 관측 장치를 관측 대상보다 먼저 검증한다
// 이 검사기들은 F5 실험의 관측 장치(맵 적격성 게이트)다. 실험이 이들을 신뢰하려면 검사기가 알려진
// 입력에서 정확히 통과/위반을 가른다는 것이 선증돼야 한다 — 그것이 아래 통과/위반 대조쌍의 취지다.
//
// ## R-17 — vacuous 통과 금지
// 각 검사기마다 통과 픽스처와 위반 픽스처를 둘 다 두고, 위반이 실제로 배열/false 를 만드는지
// 확인한다.

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
 * @brief YAML 문자열로 물리 roadmap 을 로드한 레지스트리를 만든다(로드 성공 전제).
 * @param[in] yaml 물리 roadmap YAML. 자료형 `const std::string &`.
 * @return `mrs::MapRegistry` — 로드된 레지스트리(이동 반환). 로드 실패 시 미로드 상태로 반환된다.
 */
mrs::MapRegistry make_loaded(const std::string & yaml)
{
  TempYamlFile file(yaml);
  mrs::MapRegistry reg;
  (void)reg.load_physical_roadmap(file.path());
  return reg;
}

/**
 * @brief 폭 검사 전용 스타 그래프 — 중심 노드 0 과 6 개 잎을 잇는 엣지 6 개(각기 다른 폭).
 *        엣지 e4 는 정확히 2ρ, e5 는 정확히 2(ρ+L*) 로 경계 strict 비교를 결정적으로 시험한다.
 * @return `std::string` — 폭 패널 YAML.
 */
std::string width_panel_yaml()
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, false, false); // 중심
  for (std::uint32_t i = 1U; i <= 6U; ++i)
  {
    y += node_line(i, static_cast<double>(i), 0.0, 2.0, false, false);
  }
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 1.60);      // 통과 OK, turn90/uturn/mutex 불가
  y += edge_line(1U, 0U, 2U, 2.00);      // 통과 OK, turn90 OK, uturn 불가, mutex
  y += edge_line(2U, 0U, 3U, 2.30);      // 전부 OK, mutex
  y += edge_line(3U, 0U, 4U, 1.50);      // 통과 FAIL(narrow)
  y += edge_line(4U, 0U, 5U, 2.0 * RHO); // 경계: 정확히 2ρ → strict > 불통과(narrow)
  y += edge_line(5U, 0U, 6U, 2.0 * (RHO + LSTAR)); // 경계: 정확히 2(ρ+L*) → uturn OK(>=)
  return y;
}

} // namespace

// 검사기 3 폭 골든 — 각 폭 엣지가 통과폭/90°점턴/U턴/mutex 배열에 옳게 들어가고 빠진다.
TEST(MapValidatorsWidth, EdgeWidthClassificationMatchesGolden)
{
  mrs::MapRegistry reg = make_loaded(width_panel_yaml());
  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok()) << mrs::to_string(r.status);
  const mrs::RoadmapValidationResult & v = r.value;

  // 통과폭 하드 (w > 2ρ, strict): 위반 = e3(1.50), e4(=2ρ). 나머지는 통과.
  EXPECT_FALSE(v.pass_width_ok); // 위반 존재 → false (vacuous 통과 아님)
  EXPECT_TRUE(contains_edge(v.narrow_pass_edge_ids, 3U));
  EXPECT_TRUE(contains_edge(v.narrow_pass_edge_ids, 4U)); // 경계 2ρ 는 strict > 라 narrow
  EXPECT_FALSE(contains_edge(v.narrow_pass_edge_ids, 0U));
  EXPECT_FALSE(contains_edge(v.narrow_pass_edge_ids, 1U));
  EXPECT_FALSE(contains_edge(v.narrow_pass_edge_ids, 2U));
  EXPECT_FALSE(contains_edge(v.narrow_pass_edge_ids, 5U));

  // 90° 점턴 불가 (w < 2ρ+L* = 1.893663): e0(1.60), e3(1.50), e4(1.5758). e1/e2/e5 는 가능.
  EXPECT_TRUE(contains_edge(v.turn90_incapable_edge_ids, 0U));
  EXPECT_TRUE(contains_edge(v.turn90_incapable_edge_ids, 3U));
  EXPECT_TRUE(contains_edge(v.turn90_incapable_edge_ids, 4U));
  EXPECT_FALSE(contains_edge(v.turn90_incapable_edge_ids, 1U));
  EXPECT_FALSE(contains_edge(v.turn90_incapable_edge_ids, 2U));
  EXPECT_FALSE(contains_edge(v.turn90_incapable_edge_ids, 5U));

  // U턴 불가 (w < 2(ρ+L*) = 2.211518): e0,e1,e3,e4. e2(2.30) 가능, e5(정확히 2.2115) 가능(>=).
  EXPECT_TRUE(contains_edge(v.uturn_incapable_edge_ids, 0U));
  EXPECT_TRUE(contains_edge(v.uturn_incapable_edge_ids, 1U));
  EXPECT_TRUE(contains_edge(v.uturn_incapable_edge_ids, 3U));
  EXPECT_TRUE(contains_edge(v.uturn_incapable_edge_ids, 4U));
  EXPECT_FALSE(contains_edge(v.uturn_incapable_edge_ids, 2U));
  EXPECT_FALSE(contains_edge(v.uturn_incapable_edge_ids, 5U)); // 경계 정확히 2(ρ+L*) → OK

  // mutex 후보 (w < 4ρ = 3.151616): 여섯 엣지 폭 전부 < 3.15 → 전부 후보.
  for (std::uint32_t e = 0U; e <= 5U; ++e)
  {
    EXPECT_TRUE(contains_edge(v.mutex_candidate_edge_ids, e)) << "edge " << e;
  }
}

// 계산된 임계 에코 — ρ·L* 기하에서 계산된 5 임계가 손선언 골든과 일치. v_max 미입력 → 닫힌형 -1.
TEST(MapValidatorsWidth, EchoedThresholdsMatchHandDeclaredGolden)
{
  mrs::MapRegistry reg = make_loaded(width_panel_yaml());
  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  const mrs::RoadmapValidationResult & v = r.value;

  EXPECT_NEAR(v.pass_width_min_m, PASS_MIN, 1e-9);     // 2ρ  = 1.575808
  EXPECT_NEAR(v.turn90_width_min_m, TURN90_MIN, 1e-9); // 2ρ+L* = 1.893663
  EXPECT_NEAR(v.uturn_width_min_m, UTURN_MIN, 1e-9);   // 2(ρ+L*) = 2.211518
  EXPECT_NEAR(v.required_r_clear_m, ROBOT_R, 1e-9);    // r = 1.084724

  // 검사기 4 임계 = 4ρ. 코드는 3.151616, 문서(D-11·HANDOFF)는 3.151615 — ~1e-6 불일치를 명시 고정.
  EXPECT_NEAR(v.meet_pass_width_min_m, MEET_MIN, 1e-9); // 코드값 3.151616
  EXPECT_GT(std::abs(v.meet_pass_width_min_m - 3.151615), 5e-7); // 문서값과 어긋남을 회귀로 못박음

  // v_max 미측정(0) → 닫힌형 미산출 = -1 (pending-[0b]).
  EXPECT_EQ(v.required_min_width_m, -1.0);
  EXPECT_EQ(v.derated_v_max_mps, -1.0);
}

// 검사기 4 advisory 불변 — mutex 후보로 태깅돼도 검사기 1/3 통과 시 result_pass()=true.
TEST(MapValidatorsAdvisory, MutexCandidateDoesNotBlockResultPass)
{
  // 엔드포인트 2개(비작업)를 비-엔드포인트 M 을 경유해 연결, 폭 2.30(통과 OK·mutex 후보), claim ≥
  // r.
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, true, false);  // 엔드포인트
  y += node_line(1U, 1.0, 0.0, 1.20, false, false); // 경유 노드 M
  y += node_line(2U, 2.0, 0.0, 1.20, true, false);  // 엔드포인트
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30); // < 4ρ → mutex 후보, 그러나 > 2ρ → 통과 OK
  y += edge_line(1U, 1U, 2U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 2U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  const mrs::RoadmapValidationResult & v = r.value;

  EXPECT_FALSE(v.mutex_candidate_edge_ids.empty()); // advisory 태깅은 실제로 발생
  EXPECT_TRUE(v.well_formed);
  EXPECT_TRUE(v.pass_width_ok);
  EXPECT_TRUE(v.r_clear_ok);
  EXPECT_TRUE(v.unit_granularity_ok); // view_id=0 → 입도 검사 생략(vacuous true)
  EXPECT_TRUE(v.result_pass()); // 하드 하류 미방출 불변 — advisory 가 막지 않음
}

// 검사기 3 r_clear 통과 — 모든 claim_radius ≥ r.
TEST(MapValidatorsRClear, ClaimRadiusAtOrAboveRobotRadiusPasses)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, ROBOT_R, false, false); // 정확히 r → OK (>=)
  y += node_line(1U, 3.0, 0.0, 1.20, false, false);
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_TRUE(r.value.r_clear_ok);
}

// 검사기 3 r_clear 위반 — 한 노드의 claim_radius < r → r_clear_ok=false (위반 대조군).
TEST(MapValidatorsRClear, ClaimRadiusBelowRobotRadiusFails)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, false, false);
  y += node_line(1U, 3.0, 0.0, 1.00, false, false); // 1.00 < r=1.084724
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_FALSE(r.value.r_clear_ok);
}

// 검사기 1 통과 — 두 엔드포인트가 비-엔드포인트 경유 노드로 연결, 비작업 수 ≥ 로봇 수.
TEST(MapValidatorsWellFormed, ConnectedEndpointsPass)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, true, false);  // 엔드포인트
  y += node_line(1U, 1.0, 0.0, 1.20, false, false); // 경유(비-엔드포인트)
  y += node_line(2U, 2.0, 0.0, 1.20, true, false);  // 엔드포인트
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  y += edge_line(1U, 1U, 2U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 2U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_TRUE(r.value.well_formed);
  EXPECT_TRUE(r.value.violating_wellformed_node_ids.empty());
}

// 검사기 1 위반 — 분리된 두 엔드포인트(엣지 없음)는 서로 도달 불가 → 둘 다 위반.
TEST(MapValidatorsWellFormed, DisconnectedEndpointsBothViolate)
{
  std::string y = "nodes:\n";
  y += node_line(3U, 0.0, 0.0, 1.20, true, false); // 고립 엔드포인트
  y += node_line(8U, 9.0, 0.0, 1.20, true, false); // 고립 엔드포인트
  y += "edges: []\n";
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_FALSE(r.value.well_formed);
  EXPECT_TRUE(contains_node(r.value.violating_wellformed_node_ids, 3U));
  EXPECT_TRUE(contains_node(r.value.violating_wellformed_node_ids, 8U));
}

// 검사기 1 위반 — 비작업 엔드포인트 수 < 로봇 수 → count 게이트로 false (도달성은 통과).
TEST(MapValidatorsWellFormed, InsufficientNonTaskEndpointsFailByCount)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, true, false); // 비작업 엔드포인트 1개뿐
  y += node_line(1U, 1.0, 0.0, 1.20, false, false);
  y += node_line(2U, 2.0, 0.0, 1.20, true, false); // 두 번째 엔드포인트지만...
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  y += edge_line(1U, 1U, 2U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  // 로봇 5 대 요구 — 비작업 엔드포인트 2개 < 5 → count_ok=false.
  const auto r = reg.validate_map(0U, 5U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_FALSE(r.value.well_formed);
  // 도달성 자체는 통과했음을 대조 — 위반 배열이 비어 있어야 count 게이트 단독 실패임이 드러난다.
  EXPECT_TRUE(r.value.violating_wellformed_node_ids.empty());
}

// 검사기 1 위반 — 두 엔드포인트 사이 유일 경로가 제3 엔드포인트를 경유하면 well-connected 아님.
TEST(MapValidatorsWellFormed, PathThroughThirdEndpointIsNotWellConnected)
{
  // 사슬 E0 — E1(엔드포인트) — E2 : E0↔E2 의 유일 경로가 엔드포인트 E1 을 지난다.
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, true, false); // E0
  y += node_line(1U, 1.0, 0.0, 1.20, true, false); // E1 (중간이자 엔드포인트)
  y += node_line(2U, 2.0, 0.0, 1.20, true, false); // E2
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  y += edge_line(1U, 1U, 2U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);

  const auto r = reg.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  ASSERT_TRUE(r.is_ok());
  EXPECT_FALSE(r.value.well_formed);
  // E0·E2 는 서로(E1 비경유) 도달 불가 → 위반. E1 은 양쪽에 인접하므로 위반 아님.
  EXPECT_TRUE(contains_node(r.value.violating_wellformed_node_ids, 0U));
  EXPECT_TRUE(contains_node(r.value.violating_wellformed_node_ids, 2U));
  EXPECT_FALSE(contains_node(r.value.violating_wellformed_node_ids, 1U));
}

// validate_map 오류 경로 — 로드 전 MAP_NOT_LOADED, 없는 뷰 VIEW_NOT_FOUND.
TEST(MapValidatorsErrors, ReportsMapNotLoadedAndViewNotFound)
{
  mrs::MapRegistry empty;
  const auto before = empty.validate_map(0U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  EXPECT_EQ(before.status, mrs::MapStatus::MAP_NOT_LOADED);

  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 1.20, false, false);
  y += node_line(1U, 3.0, 0.0, 1.20, false, false);
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.30);
  mrs::MapRegistry reg = make_loaded(y);
  const auto bad_view = reg.validate_map(999U, 0U, ROBOT_R, RHO, LSTAR, 0.0, 0.0);
  EXPECT_EQ(bad_view.status, mrs::MapStatus::VIEW_NOT_FOUND);
}
