// test_map_loader.cpp — 물리 roadmap YAML 로더의 계약을 지키는 벽 (D-11 로드 경로).
//
// ## 이 파일이 지키는 것
// 정상 로드 시 버전 발급(단조증가)·뷰 노드/엣지 수·좌표에서 계산된 엣지 길이 ‖p_a−p_b‖,
// 실패 사유의 정확한 분류(FILE_NOT_FOUND vs FILE_PARSE_ERROR), 로드 전 조회의 MAP_NOT_LOADED,
// 그리고 파서가 강제하는 불변식(중복 노드 id·매달린 엣지 참조·task⇒endpoint·비유한/음수 claim).
//
// ## 이 파일이 검증하지 **못하는** 것 (충실도 티어 정직성)
// - DDS 경계·서비스 RESULT_* 접기: 노드 계층(mrs_ros_map_registry)의 몫. unit 티어 밖.
// - yaml-cpp 자체의 문법 파싱 정확성: 외부 라이브러리 계약이며 여기서 재검증하지 않는다.
//
// R-17: 각 실패 분류마다 통과 대조군 + 위반 픽스처 둘 다 둔다(검사가 vacuously-pass 하지 않음
// 증명).

#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "map_registry_test_fixtures.hpp"
#include "mrs_map_registry/map_registry.hpp"

using namespace mrs_map_registry_test;

namespace
{

/**
 * @brief 3-4-5 직각 삼각형 한 변으로 길이 5.0 이 자명한 최소 유효 물리 roadmap 을 만든다.
 * @return `std::string` — 노드 2개(0,1)·엣지 1개(길이 5.0)를 담은 YAML.
 */
std::string minimal_valid_yaml()
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, true, false);
  y += node_line(1U, 3.0, 4.0, 2.0, true, false); // (3,4) → ‖p0−p1‖ = 5.0
  y += "edges:\n";
  y += edge_line(0U, 0U, 1U, 2.0);
  return y;
}

} // namespace

// 정상 로드: 첫 버전 = 1, 뷰 노드/엣지 수, 좌표에서 계산된 엣지 길이 = 5.0.
TEST(MapLoader, ValidLoadIssuesVersionOneAndComputesEdgeLength)
{
  TempYamlFile file(minimal_valid_yaml());
  mrs::MapRegistry reg;

  const auto load = reg.load_physical_roadmap(file.path());
  ASSERT_TRUE(load.is_ok()) << mrs::to_string(load.status);
  EXPECT_EQ(load.value, 1U); // 손선언: 첫 발급 버전
  EXPECT_EQ(reg.roadmap_version(), 1U);

  const auto view = reg.get_view(0U);
  ASSERT_TRUE(view.is_ok()) << mrs::to_string(view.status);
  EXPECT_EQ(view.value.view_kind, mrs::ViewKind::PHYSICAL);
  EXPECT_EQ(view.value.nodes.size(), 2U);
  ASSERT_EQ(view.value.edges.size(), 1U);
  // 길이는 파일에 없고 좌표에서 계산된다 — (3,4) 빗변 = 5.0 (직선 전제 구성적 강제).
  EXPECT_NEAR(view.value.edges[0].length_m, 5.0, 1e-9);
}

// 로드 전 조회는 MAP_NOT_LOADED (버전도 0).
TEST(MapLoader, QueryBeforeLoadReportsMapNotLoaded)
{
  mrs::MapRegistry reg;
  EXPECT_EQ(reg.roadmap_version(), 0U);
  const auto view = reg.get_view(0U);
  EXPECT_FALSE(view.is_ok());
  EXPECT_EQ(view.status, mrs::MapStatus::MAP_NOT_LOADED);
}

// 없는 파일 → FILE_NOT_FOUND (FILE_PARSE_ERROR 와 뭉개지지 않음).
TEST(MapLoader, MissingFileReportsFileNotFound)
{
  TempYamlFile file(minimal_valid_yaml());
  const std::string missing = file.remove_and_get_missing_path();
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(missing);
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_NOT_FOUND);
}

// 중복 노드 id → FILE_PARSE_ERROR (인덱스가 노드를 유일하게 지목 못 함).
TEST(MapLoader, DuplicateNodeIdReportsParseError)
{
  std::string y = "nodes:\n";
  y += node_line(5U, 0.0, 0.0, 2.0, false, false);
  y += node_line(5U, 1.0, 0.0, 2.0, false, false); // 중복 id 5
  y += "edges: []\n";
  TempYamlFile file(y);
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(file.path());
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_PARSE_ERROR);
}

// 매달린 엣지 참조(존재하지 않는 노드 b) → FILE_PARSE_ERROR.
TEST(MapLoader, DanglingEdgeReferenceReportsParseError)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, false, false);
  y += "edges:\n";
  y += edge_line(0U, 0U, 9U, 2.0); // 노드 9 없음
  TempYamlFile file(y);
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(file.path());
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_PARSE_ERROR);
}

// 불변식 위반 (is_task_endpoint ∧ ¬is_endpoint) → FILE_PARSE_ERROR.
TEST(MapLoader, TaskEndpointWithoutEndpointReportsParseError)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, 2.0, false, true); // task 인데 endpoint 아님
  y += "edges: []\n";
  TempYamlFile file(y);
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(file.path());
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_PARSE_ERROR);
}

// 음수 claim_radius → FILE_PARSE_ERROR (0 이하 거부).
TEST(MapLoader, NonPositiveClaimRadiusReportsParseError)
{
  std::string y = "nodes:\n";
  y += node_line(0U, 0.0, 0.0, -1.0, false, false); // 음수 claim
  y += "edges: []\n";
  TempYamlFile file(y);
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(file.path());
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_PARSE_ERROR);
}

// 비유한 좌표(.nan) → FILE_PARSE_ERROR.
TEST(MapLoader, NonFiniteCoordinateReportsParseError)
{
  std::string y = "nodes:\n";
  y += "  - {id: 0, x: .nan, y: 0.0, claim_radius: 2.0, is_endpoint: false, is_task_endpoint: "
       "false}\n";
  y += "edges: []\n";
  TempYamlFile file(y);
  mrs::MapRegistry reg;
  const auto load = reg.load_physical_roadmap(file.path());
  EXPECT_FALSE(load.is_ok());
  EXPECT_EQ(load.status, mrs::MapStatus::FILE_PARSE_ERROR);
}

// 재로드 시 버전 단조증가 (1 → 2), 그리고 이전 파생 뷰 무효화(버전만 여기서 확인).
TEST(MapLoader, ReloadMonotonicallyIncrementsVersion)
{
  TempYamlFile file(minimal_valid_yaml());
  mrs::MapRegistry reg;

  const auto first = reg.load_physical_roadmap(file.path());
  ASSERT_TRUE(first.is_ok());
  EXPECT_EQ(first.value, 1U);

  const auto second = reg.load_physical_roadmap(file.path());
  ASSERT_TRUE(second.is_ok());
  EXPECT_EQ(second.value, 2U); // 손선언: 단조증가
  EXPECT_EQ(reg.roadmap_version(), 2U);
}
