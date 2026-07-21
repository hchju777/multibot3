/**
 * @file tracer_roadmap.cpp
 * @brief [0a] 하드코딩 물리 roadmap 픽스처의 구현 (형태·근거는 헤더 주석 참조).
 *
 * ⚠ 알고리즘이 아니다 — 좌표 리터럴을 도메인 값 타입으로 옮기는 것이 전부다.
 *   세분화·collapse·좌표 변환은 이 파일에 **없다**(architecture C4).
 */

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "mrs_ros_map_registry/tracer_roadmap.hpp"

namespace mrs
{

namespace
{

/** @brief 사다리 격자의 열 개수 (하단·상단 통로 각각의 노드 수). */
constexpr std::uint32_t GRID_COLUMNS = 4U;
/** @brief 사다리 격자의 행 개수 (하단 통로 + 상단 통로). */
constexpr std::uint32_t GRID_ROWS = 2U;

/**
 * @brief 격자 노드 1개의 배치·역할 명세 (좌표는 격자 간격 s 와 곱해 산출한다).
 */
struct TracerNodeSpec
{
  std::uint32_t column;  ///< 열 인덱스 (x = column * s)
  std::uint32_t row;     ///< 행 인덱스 (y = row * s)
  bool is_endpoint;      ///< 대기 가능 지점 여부 — 격자의 네 모서리
  bool is_task_endpoint; ///< 픽업/딜리버리 지점 여부 — 대각선 두 모서리
};

/**
 * @brief 격자 엣지 1개의 양단 노드 명세.
 */
struct TracerEdgeSpec
{
  std::uint32_t node_a; ///< 양단 노드 a (배열 인덱스 = node_id)
  std::uint32_t node_b; ///< 양단 노드 b
};

// 노드 id 는 배열 인덱스와 같다: n0..n3 = 하단 통로(행 0), n4..n7 = 상단 통로(행 1).
// 엔드포인트는 네 모서리(n0·n3·n4·n7), 그중 작업 엔드포인트는 대각선(n0·n7).
// ⇒ 비작업 엔드포인트 = {n3, n4} 2개 = [0a] 로봇 수 2 (well-formed 하한, 헤더 근거 3번).
constexpr TracerNodeSpec NODE_SPECS[] = {
  {0U, 0U, true, true},   // n0 — 하단 좌측 모서리 · 작업 엔드포인트
  {1U, 0U, false, false}, // n1
  {2U, 0U, false, false}, // n2
  {3U, 0U, true, false},  // n3 — 하단 우측 모서리 · 비작업 엔드포인트
  {0U, 1U, true, false},  // n4 — 상단 좌측 모서리 · 비작업 엔드포인트
  {1U, 1U, false, false}, // n5
  {2U, 1U, false, false}, // n6
  {3U, 1U, true, true},   // n7 — 상단 우측 모서리 · 작업 엔드포인트
};

// 하단 통로 3 + 상단 통로 3 + 가로대 4 = 10 엣지. 가로대가 4개라 교행 경로가 항상 존재한다.
constexpr TracerEdgeSpec EDGE_SPECS[] = {
  {0U, 1U}, {1U, 2U}, {2U, 3U},           // 하단 통로
  {4U, 5U}, {5U, 6U}, {6U, 7U},           // 상단 통로
  {0U, 4U}, {1U, 5U}, {2U, 6U}, {3U, 7U}, // 가로대(교행 지점)
};

static_assert(
  sizeof(NODE_SPECS) / sizeof(NODE_SPECS[0]) == GRID_COLUMNS * GRID_ROWS,
  "NODE_SPECS 는 GRID_COLUMNS x GRID_ROWS 격자를 빠짐없이 덮어야 한다");

/**
 * @brief 노드 명세 표를 도메인 노드 목록으로 펼친다.
 * @param[in] params 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 * @param[out] out 채울 노드 목록. 자료형 `std::vector<mrs::RoadmapNodeView>`. 뒤에 덧붙인다.
 * @return void
 */
void append_tracer_nodes(const TracerRoadmapParams & params, std::vector<RoadmapNodeView> & out)
{
  // 계약 L-16 미결 U-01: r_clear 산정식은 OPEN(theory T1 O6). 보수적 초기값 unit_length_m/2 를
  // 그대로 쓴다 — 이 픽스처의 unit_length 는 격자 간격 s 다(전 엣지 길이가 s 로 같다).
  const double claim_radius_m = params.grid_spacing_m / 2.0;

  std::uint32_t node_id = 0U;
  for (const TracerNodeSpec & spec : NODE_SPECS)
  {
    RoadmapNodeView node;
    node.node_id = node_id;
    node.x_m = static_cast<double>(spec.column) * params.grid_spacing_m;
    node.y_m = static_cast<double>(spec.row) * params.grid_spacing_m;
    node.claim_radius_m = claim_radius_m;
    node.is_endpoint = spec.is_endpoint;
    node.is_task_endpoint = spec.is_task_endpoint;
    node.is_virtual = false; // 물리 뷰에는 가상 노드가 없다(계약 `RoadmapNode.msg`).
    out.push_back(node);
    ++node_id;
  }
}

/**
 * @brief 엣지 명세 표를 도메인 엣지 목록으로 펼친다.
 * @param[in] params 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 * @param[out] out 채울 엣지 목록. 자료형 `std::vector<mrs::RoadmapEdgeView>`. 뒤에 덧붙인다.
 * @return void
 */
void append_tracer_edges(const TracerRoadmapParams & params, std::vector<RoadmapEdgeView> & out)
{
  std::uint32_t edge_id = 0U;
  for (const TracerEdgeSpec & spec : EDGE_SPECS)
  {
    RoadmapEdgeView edge;
    edge.edge_id = edge_id;
    edge.node_a = spec.node_a;
    edge.node_b = spec.node_b;
    // 격자 이웃만 잇는다 — 통로 방향이든 가로대 방향이든 길이는 격자 간격 s 로 같다.
    edge.length_m = params.grid_spacing_m;
    edge.corridor_width_m = params.corridor_width_m;
    edge.speed_limit_mps = params.speed_limit_mps;
    out.push_back(edge);
    ++edge_id;
  }
}

/**
 * @brief 픽스처 파라미터가 뷰를 만들 수 있는 값인지 검사한다.
 * @param[in] params 검사할 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 * @return `bool` — 지도 버전이 1 이상이고 세 실수값이 전부 유한·비음(간격은 양수)이면 true.
 */
[[nodiscard]] bool params_are_valid(const TracerRoadmapParams & params)
{
  // 0 은 계약이 런타임에 금지한다 — 나가면 수신자가 100% 폐기한다.
  if (params.roadmap_version == ROADMAP_VERSION_UNSPECIFIED)
  {
    return false;
  }
  if (!std::isfinite(params.grid_spacing_m) || params.grid_spacing_m <= 0.0)
  {
    return false;
  }
  if (!std::isfinite(params.corridor_width_m) || params.corridor_width_m < 0.0)
  {
    return false;
  }
  return std::isfinite(params.speed_limit_mps) && params.speed_limit_mps >= 0.0;
}

/**
 * @brief 사다리 격자 노드·엣지를 채운 뷰 데이터를 만든다 (뷰 종류에 무관한 공통 골격).
 *
 * 두 공개 함수가 이것을 공유한다. 격자 형상이 한 곳에만 있어야 물리 뷰와 균일 뷰가
 * **정의상 같은 격자**임이 코드로 보장된다 — 두 벌로 적으면 한쪽만 고쳤을 때 조용히 갈린다.
 *
 * @param[in] params 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 * @param[in] view_id 이 뷰의 인스턴스 id. 자료형 `std::uint32_t`.
 * @param[out] out 채울 뷰 데이터. 자료형 `mrs::RoadmapViewData`. 통째로 대입된다.
 * @return void
 */
void build_grid_view(
  const TracerRoadmapParams & params, std::uint32_t view_id, RoadmapViewData & out)
{
  RoadmapViewData built;
  built.roadmap_version = params.roadmap_version;
  built.view_id = view_id;
  built.unit_length_m = params.grid_spacing_m;
  // 이론 하한 2(r + L) 은 로봇 반경 r·NID 오프셋 L 을 요구하는데 둘 다 [0a] 에 미정이다.
  // 지어내지 않고 0 으로 둔다. 이 값을 강제하는 것은 검사기 3-b(ValidateMap, [1] 대상)이며
  // [0a] 는 그 검사기를 호출하지 않으므로 배관에 영향이 없다 — "0 = 미유도"의 뜻이지
  // "하한이 0 이다"라는 주장이 아니다.
  built.unit_length_lower_bound_m = 0.0;
  built.nodes.reserve(sizeof(NODE_SPECS) / sizeof(NODE_SPECS[0]));
  built.edges.reserve(sizeof(EDGE_SPECS) / sizeof(EDGE_SPECS[0]));
  append_tracer_nodes(params, built.nodes);
  append_tracer_edges(params, built.edges);
  // mappings 는 비운다 — 물리 뷰에는 대응표가 없고, 균일 뷰는 가상 노드가 0 개라 원소도 0 개다.

  out = std::move(built);
}

} // namespace

bool build_tracer_roadmap(const TracerRoadmapParams & params, RoadmapViewData & out)
{
  if (!params_are_valid(params))
  {
    return false;
  }

  build_grid_view(params, VIEW_ID_PHYSICAL, out); // 물리 뷰의 view_id 는 항상 0 (계약 §0.1)
  return true;
}

bool build_tracer_uniform_view(
  const TracerRoadmapParams & params, std::uint32_t uniform_view_id, RoadmapViewData & out)
{
  if (!params_are_valid(params))
  {
    return false;
  }

  // 0 은 물리 뷰의 예약값이다. 균일 뷰가 그 값을 쓰면 두 뷰의 (roadmap_version, view_id) 쌍이
  // 같아져 런타임 스코프 대조가 물리↔균일 혼동을 못 잡는다(헤더 @param 참조).
  if (uniform_view_id == VIEW_ID_PHYSICAL)
  {
    return false;
  }

  // ⚠ 세분화를 수행하지 않는다. 이 픽스처는 전 엣지 길이가 격자 간격 s 로 같게 **만들어졌고**,
  //   이미 균일한 격자에 대한 세분화는 항등이다(헤더 참조). 격자를 그대로 균일 뷰로 발급한다.
  build_grid_view(params, uniform_view_id, out);
  return true;
}

} // namespace mrs
