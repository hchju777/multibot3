/**
 * @file map_registry_convert.cpp
 * @brief MapRegistry 3뷰·응답 변환 — [1] D-11 전 경로 구현 (스텁 없음).
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 실패는 반환값(@ref mrs::convert::ConvertResult,
 * `ok = false` + 사유)으로만 보고한다.
 *
 * ## 구현된 함수 (본문 있음 — 전량)
 * `nodes_to_msg` · `edges_to_msg` · `mappings_to_msg` · `fill_response(GetRoadmap)` ·
 * `fill_response(GetUniformView)` · `fill_response(GetDependencySkeleton)` ·
 * `fill_response(ValidateMap)`.
 *
 * ## U-18 해소 ([1] D-11)
 * `mrs::RoadmapViewData` 에 이제 뷰 종류 판별자 `view_kind` 가 있다(파일 3). **V1 종류 대조는
 * 담은 응답을 채우는 `fill_response(...)` 가 수행**한다 — 각 응답은 계약이 종류를 고정하므로
 * (`GetRoadmap`→PHYSICAL / `GetUniformView`→UNIFORM / `GetDependencySkeleton`→SKELETON) 그
 * 래퍼가 `view.view_kind` 를 기대 종류와 대조하고 어긋나면 @ref ConvertStatus::VIEW_KIND_MISMATCH
 * 로 폐기한다. `nodes_to_msg`/`edges_to_msg`/`mappings_to_msg` 는 종류 불가지론 배열 변환기로
 * 남으며(어느 뷰든 원소를 변환), 종류 판정은 래퍼가 단독으로 진다.
 *
 * ## id 규약
 * 뷰 컨테이너(@ref mrs::RoadmapViewData)의 노드·엣지 id 는 설계상 bare `std::uint32_t` 이며
 * (사용자 결정, 파일 3 — 뷰 종류는 원소가 아니라 컨테이너의 `view_kind` 가 고정한다), 센티넬
 * 판정은 그 bare 공간의 센티넬 `mrs::ROADMAP_ID_NONE` 과의 직접 대조로 한다. 뷰별 강타입 id 는
 * `NodeMappingView`(대응표)·`RoadmapValidationResult`(검사 위반 배열)가 담당하며, 그 강타입에서
 * 원시 값을 꺼낼 때만 `.value()` 를 쓴다(변환 단일 소유, 규칙 V3).
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cmath>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mrs::convert
{

namespace
{

/**
 * @brief 뷰별 강타입 id 배열에서 원시 값을 꺼내 uint32 배열로 채운다 (변환 단일 소유, 규칙 V3).
 * @tparam StrongId 뷰별 강타입 (@ref mrs::PhysicalNodeId / @ref mrs::UniformEdgeId 등).
 * @param[in] src 도메인 강타입 id 목록. 자료형 `const std::vector<StrongId> &`.
 * @param[out] dst 채울 원시 id 배열. 자료형 `std::vector<std::uint32_t> &`. 먼저 비워진다.
 * @return void
 */
template <class StrongId>
void fill_id_array(const std::vector<StrongId> & src, std::vector<std::uint32_t> & dst)
{
  dst.clear();
  dst.reserve(src.size());
  for (const StrongId & id : src)
  {
    dst.push_back(id.value());
  }
}

} // namespace

ConvertResult nodes_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapNode> & out)
{
  // ① 스코프 자기 유효성 — 0 이 나가면 V3 를 지키는 수신자가 100% 폐기한다.
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  std::vector<mrs_interfaces::msg::RoadmapNode> converted;
  converted.reserve(view.nodes.size());

  std::unordered_set<std::uint32_t> seen_node_ids;
  seen_node_ids.reserve(view.nodes.size());

  for (const mrs::RoadmapNodeView & node : view.nodes)
  {
    // ② 필드 범위 — 센티넬·중복.
    if (node.node_id == mrs::ROADMAP_ID_NONE)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (!seen_node_ids.insert(node.node_id).second)
    {
      // 중복 id 는 뷰 안에서 "어느 노드인가"를 결정할 수 없게 만든다.
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    // ③ 불변식 — 작업 엔드포인트는 엔드포인트여야 한다.
    if (node.is_task_endpoint && !node.is_endpoint)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    // ④ 연속량. ⚠ claim_radius_m 의 산정식은 OPEN(theory T1 O6)이므로 0·음수·비유한만 거부하고
    //    상한·하한 판정은 하지 않는다 — 없는 기준으로 거르면 [0a] 실측 자체가 막힌다.
    if (!std::isfinite(node.x_m) || !std::isfinite(node.y_m))
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (!std::isfinite(node.claim_radius_m) || node.claim_radius_m <= 0.0)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    mrs_interfaces::msg::RoadmapNode node_msg;
    node_msg.node_id = node.node_id;
    node_msg.x_m = node.x_m;
    node_msg.y_m = node.y_m;
    node_msg.claim_radius_m = node.claim_radius_m;
    node_msg.is_endpoint = node.is_endpoint;
    node_msg.is_task_endpoint = node.is_task_endpoint;
    node_msg.is_virtual = node.is_virtual;
    converted.push_back(node_msg);
  }

  // 성공 시에만 대입한다 — 부분 변환된 노드 배열이 나가면 뷰가 조용히 작아진다.
  out = std::move(converted);
  return convert_ok();
}

ConvertResult edges_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapEdge> & out)
{
  // ① 스코프 자기 유효성.
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  std::vector<mrs_interfaces::msg::RoadmapEdge> converted;
  converted.reserve(view.edges.size());

  for (const mrs::RoadmapEdgeView & edge : view.edges)
  {
    // ② 필드 범위 — 센티넬 3건.
    if (edge.edge_id == mrs::ROADMAP_ID_NONE)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (edge.node_a == mrs::ROADMAP_ID_NONE || edge.node_b == mrs::ROADMAP_ID_NONE)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    // ③ 연속량. 참조 무결성·퇴화 엣지·자기루프·중복 엣지는 **ValidateMap 검사기 1·2 의 소관**
    //    이며 여기서 재구현하지 않는다.
    if (!std::isfinite(edge.length_m) || edge.length_m <= 0.0)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (!std::isfinite(edge.corridor_width_m) || edge.corridor_width_m < 0.0)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (!std::isfinite(edge.speed_limit_mps) || edge.speed_limit_mps < 0.0)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    mrs_interfaces::msg::RoadmapEdge edge_msg;
    edge_msg.edge_id = edge.edge_id;
    edge_msg.node_a = edge.node_a;
    edge_msg.node_b = edge.node_b;
    edge_msg.length_m = edge.length_m;
    edge_msg.corridor_width_m = edge.corridor_width_m;
    edge_msg.speed_limit_mps = edge.speed_limit_mps;
    converted.push_back(edge_msg);
  }

  out = std::move(converted);
  return convert_ok();
}

ConvertResult mappings_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::NodeMapping> & out)
{
  // ① 스코프 자기 유효성.
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  std::vector<mrs_interfaces::msg::NodeMapping> converted;
  converted.reserve(view.mappings.size());

  // `from_node_id`(항상 균일 뷰) 중복 금지 — 대응이 함수여야 왕복이 성립한다.
  std::unordered_set<std::uint32_t> seen_from_ids;
  seen_from_ids.reserve(view.mappings.size());

  for (const mrs::NodeMappingView & mapping : view.mappings)
  {
    // ② 원본은 항상 균일 뷰 노드이고 센티넬이 아니다(강타입이 종류를 이미 못박는다).
    if (mapping.from_node_id.is_none())
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (!seen_from_ids.insert(mapping.from_node_id.value()).second)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }

    mrs_interfaces::msg::NodeMapping mapping_msg;
    mapping_msg.from_node_id = mapping.from_node_id.value();
    // 기본은 전부 센티넬·0 — 용례가 지목하는 필드만 아래에서 유효값으로 덮는다. 남는 `to_*` 는
    // 센티넬로 남아 "해당 없음"을 정확히 신호한다(수신자가 배타성으로 용례를 되읽는다).
    mapping_msg.to_node_id = mrs::ROADMAP_ID_NONE;
    mapping_msg.to_edge_id = mrs::ROADMAP_ID_NONE;
    mapping_msg.s = 0.0;
    mapping_msg.index = 0U;
    mapping_msg.denominator = 0U;

    // ③ 용례별 채움 — `kind` 가 지목하는 강타입 대상 하나만 유효하고 나머지 두 대상은 센티넬
    //    이어야 한다. `kind` 와 실제 채워진 필드가 어긋나면 §0 규칙 V4(방향 역전 금지) 위반이다.
    //    세 to_* 가 전부 bare `uint32` 로 나가므로(계약), 이 배타성 검사만이 방향을 지킨다.
    switch (mapping.kind)
    {
      case mrs::MappingKind::SUBDIVISION:
      {
        // 용례 1: 균일 가상 노드 → 물리 엣지 위 위치. to_physical_edge_id·s·서수(k/n).
        if (mapping.to_physical_edge_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        if (!mapping.to_skeleton_node_id.is_none() || !mapping.to_skeleton_edge_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        // s 는 [0,1] 유한. 서수는 n>=1, k<=n (k=0..n, 계약 NodeMapping.msg 용례 1).
        if (!std::isfinite(mapping.s) || mapping.s < 0.0 || mapping.s > 1.0)
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        if (mapping.denominator < 1U || mapping.index > mapping.denominator)
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        mapping_msg.to_edge_id = mapping.to_physical_edge_id.value();
        mapping_msg.s = mapping.s;
        mapping_msg.index = mapping.index;
        mapping_msg.denominator = mapping.denominator;
        break;
      }
      case mrs::MappingKind::COLLAPSE_NODE:
      {
        // 용례 2: 균일 노드 → 골격 노드. to_skeleton_node_id, 서수 없음(denominator = 0).
        if (mapping.to_skeleton_node_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        if (!mapping.to_physical_edge_id.is_none() || !mapping.to_skeleton_edge_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        mapping_msg.to_node_id = mapping.to_skeleton_node_id.value();
        // denominator = 0(서수 미사용)은 이미 기본값. s·index 도 이 용례에서는 미사용이다.
        break;
      }
      case mrs::MappingKind::COLLAPSE_CHAIN:
      {
        // 용례 3: 삼켜진 균일 가상 노드 → 골격 엣지 + 체인 서수. to_skeleton_edge_id·서수.
        // s 는 이 용례에서 미사용이다(순서는 정수 서수로만 담는다 — GATE Q4).
        if (mapping.to_skeleton_edge_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        if (!mapping.to_physical_edge_id.is_none() || !mapping.to_skeleton_node_id.is_none())
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        if (mapping.denominator < 1U || mapping.index > mapping.denominator)
        {
          return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
        }
        mapping_msg.to_edge_id = mapping.to_skeleton_edge_id.value();
        mapping_msg.index = mapping.index;
        mapping_msg.denominator = mapping.denominator;
        break;
      }
      default:
        // `kind` 가 이 빌드가 아는 집합 밖 — 도메인 enum 이 uint8 이라 방어한다(뭉갬 금지).
        return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
    }

    converted.push_back(mapping_msg);
  }

  // 성공 시에만 대입한다 — 부분 변환된 배열이 나가면 대응표가 조용히 작아진다.
  out = std::move(converted);
  return convert_ok();
}

ConvertResult fill_response(
  const mrs::RoadmapViewData & view, std::uint64_t known_version,
  mrs_interfaces::srv::GetRoadmap::Response & resp)
{
  // ① 뷰 종류 대조(V1, U-18 해소) — 이 응답은 계약이 PHYSICAL 로 고정한다. 발행 측이 엉뚱한
  //    종류의 뷰를 넘기면 여기서 폐기한다.
  if (view.view_kind != mrs::ViewKind::PHYSICAL)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }
  // ② 스코프 자기 유효성 + 물리 뷰 규약(`view_id` 는 항상 0).
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }
  if (view.view_id != mrs::VIEW_ID_PHYSICAL)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  mrs_interfaces::srv::GetRoadmap::Response filled;
  filled.roadmap_version = view.roadmap_version;
  filled.frame_id = "map"; // 계약 §0 좌표계 규약 + `GetRoadmap.srv` 응답 주석

  if (known_version == view.roadmap_version)
  {
    // 대역폭 규약 — 클라이언트 캐시가 최신이면 본문 없이 UNCHANGED 만 보낸다.
    filled.result = mrs_interfaces::srv::GetRoadmap::Response::RESULT_UNCHANGED;
  }
  else
  {
    filled.result = mrs_interfaces::srv::GetRoadmap::Response::RESULT_OK;

    // 하위 변환의 실패 사유를 **그대로** 전달한다 — 여기서 사유를 갈아치우면 어느 원소가
    // 계약을 어겼는지 사후에 복원할 수 없다.
    const ConvertResult nodes_result = nodes_to_msg(view, filled.nodes);
    if (!nodes_result.ok)
    {
      return nodes_result;
    }

    const ConvertResult edges_result = edges_to_msg(view, filled.edges);
    if (!edges_result.ok)
    {
      return edges_result;
    }
  }

  resp = std::move(filled);
  return convert_ok();
}

ConvertResult fill_response(
  const mrs::RoadmapViewData & view, mrs_interfaces::srv::GetUniformView::Response & resp)
{
  // ① 뷰 종류 대조(V1, U-18 해소) — 이 응답은 계약이 UNIFORM 으로 고정한다.
  if (view.view_kind != mrs::ViewKind::UNIFORM)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }
  // ② 스코프 자기 유효성.
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  mrs_interfaces::srv::GetUniformView::Response filled;
  filled.result = mrs_interfaces::srv::GetUniformView::Response::RESULT_OK;

  // ③ 노드·엣지·대응표 id 는 전부 하위 변환이 채운다(V3). 실패 사유는 갈아치우지 않고 그대로
  //    전달한다 — 어느 원소가 계약을 어겼는지 사후에 복원할 수 있어야 한다.
  const ConvertResult nodes_result = nodes_to_msg(view, filled.nodes);
  if (!nodes_result.ok)
  {
    return nodes_result;
  }
  const ConvertResult edges_result = edges_to_msg(view, filled.edges);
  if (!edges_result.ok)
  {
    return edges_result;
  }
  const ConvertResult map_result = mappings_to_msg(view, filled.subdivision_map);
  if (!map_result.ok)
  {
    return map_result;
  }

  // ④ 봉투 스칼라(노드 id 가 아니다) — 이 함수가 과거 노드가 하던 수동 채움을 흡수한다
  //    (계약 결손 해소: `fill_response(GetUniformView)` 부재를 [1] 에서 메운다).
  filled.roadmap_version = view.roadmap_version;
  filled.view_id = view.view_id;
  filled.unit_length_m = view.unit_length_m;
  filled.unit_length_lower_bound_m = view.unit_length_lower_bound_m;
  filled.effective_unit_min_m = view.effective_unit_min_m;
  filled.effective_unit_max_m = view.effective_unit_max_m;
  filled.subdivided_edge_ratio = view.subdivided_edge_ratio;

  resp = std::move(filled);
  return convert_ok();
}

ConvertResult fill_response(
  const mrs::RoadmapViewData & view, mrs_interfaces::srv::GetDependencySkeleton::Response & resp)
{
  // ① 뷰 종류 대조(V1, U-18 해소) — 이 응답은 계약이 SKELETON 으로 고정한다.
  if (view.view_kind != mrs::ViewKind::SKELETON)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }
  if (view.roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  mrs_interfaces::srv::GetDependencySkeleton::Response filled;
  filled.result = mrs_interfaces::srv::GetDependencySkeleton::Response::RESULT_OK;

  const ConvertResult nodes_result = nodes_to_msg(view, filled.nodes);
  if (!nodes_result.ok)
  {
    return nodes_result;
  }
  const ConvertResult edges_result = edges_to_msg(view, filled.edges);
  if (!edges_result.ok)
  {
    return edges_result;
  }
  const ConvertResult map_result = mappings_to_msg(view, filled.collapse_map);
  if (!map_result.ok)
  {
    return map_result;
  }

  // 골격 뷰의 스코프는 유래 균일 뷰의 (version, view_id)를 그대로 에코한다(종류만 SKELETON).
  filled.roadmap_version = view.roadmap_version;
  filled.view_id = view.view_id;

  resp = std::move(filled);
  return convert_ok();
}

ConvertResult fill_response(
  const mrs::RoadmapValidationResult & result, std::uint64_t roadmap_version, std::uint32_t view_id,
  mrs_interfaces::srv::ValidateMap::Response & resp)
{
  // ① 스코프 자기 유효성 — 응답은 요청 스코프를 에코한다.
  if (roadmap_version == mrs::ROADMAP_VERSION_UNSPECIFIED)
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }
  // ② view_id = 0 은 "균일 뷰 입도 검사를 하지 않았다"는 뜻 — 균일 뷰 위반 목록은 비어야 한다.
  //    물리만 검사했는데 균일 위반이 실리면 검사 범위와 결과가 모순된다.
  if (
    view_id == mrs::VIEW_ID_PHYSICAL &&
    (!result.violating_uniform_node_ids.empty() || !result.violating_uniform_edge_ids.empty()))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }
  // ③ 임계 에코 유한·비음 — ρ·L* 기하에서 계산된 값이라 비유한·음수면 도메인 산술 오류다.
  const double thresholds[] = {
    result.pass_width_min_m, result.turn90_width_min_m, result.uturn_width_min_m,
    result.meet_pass_width_min_m, result.required_r_clear_m};
  for (double t : thresholds)
  {
    if (!std::isfinite(t) || t < 0.0)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
  }
  // ④ v_max 닫힌형 2건 — 유한이고 **-1(미산출, pending-[0b]) 또는 비음**. -1 은 v6 의 유효
  //    센티넬(v_max 미측정 → 미산출)이므로 비음 검사로 거르면 안 된다(ValidateMap.srv v6).
  //    그 밖의 음수는 도메인 오류로 폐기한다.
  for (double v : {result.required_min_width_m, result.derated_v_max_mps})
  {
    if (!std::isfinite(v) || (v < 0.0 && v != -1.0))
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
  }

  mrs_interfaces::srv::ValidateMap::Response filled;

  // ⑤ 하드 판정 — 도메인 result_pass() 가 정본이다(bool 조합을 여기서 재구현하지 않는다).
  //    ⚠ 채우지 않으면 실패가 RESULT_PASS(=0)로 나간다 — 기본값이 곧 오탐이다.
  filled.result = result.result_pass() ? mrs_interfaces::srv::ValidateMap::Response::RESULT_PASS
                                       : mrs_interfaces::srv::ValidateMap::Response::RESULT_FAIL;

  filled.well_formed = result.well_formed;
  filled.biconnected =
    result.biconnected; // [1] 미평가(U-23) — false 는 '미평가'이지 '실패'가 아니다
  filled.pass_width_ok = result.pass_width_ok;
  filled.unit_granularity_ok = result.unit_granularity_ok;
  filled.r_clear_ok = result.r_clear_ok;

  filled.roadmap_version = roadmap_version;
  filled.view_id = view_id;

  // ⑥ 위반 목록 — 뷰별·엔티티별 강타입 배열에서 원시 값을 꺼낸다(V3 단일 소유).
  fill_id_array(result.violating_wellformed_node_ids, filled.violating_wellformed_node_ids);
  fill_id_array(result.violating_biconnected_node_ids, filled.violating_biconnected_node_ids);
  fill_id_array(result.narrow_pass_edge_ids, filled.narrow_pass_edge_ids);
  fill_id_array(result.violating_uniform_node_ids, filled.violating_uniform_node_ids);
  fill_id_array(result.violating_uniform_edge_ids, filled.violating_uniform_edge_ids);
  fill_id_array(result.turn90_incapable_edge_ids, filled.turn90_incapable_edge_ids);
  fill_id_array(result.uturn_incapable_edge_ids, filled.uturn_incapable_edge_ids);
  fill_id_array(result.mutex_candidate_edge_ids, filled.mutex_candidate_edge_ids);

  // ⑦ 계산된 임계 에코 + v_max 닫힌형(-1 미산출 그대로 통과 — 재계산하지 않는다, T1 소유는 검사기
  // 3).
  filled.pass_width_min_m = result.pass_width_min_m;
  filled.turn90_width_min_m = result.turn90_width_min_m;
  filled.uturn_width_min_m = result.uturn_width_min_m;
  filled.meet_pass_width_min_m = result.meet_pass_width_min_m;
  filled.required_r_clear_m = result.required_r_clear_m;
  filled.required_min_width_m = result.required_min_width_m;
  filled.derated_v_max_mps = result.derated_v_max_mps;

  filled.messages = result.messages;

  resp = std::move(filled);
  return convert_ok();
}

} // namespace mrs::convert
