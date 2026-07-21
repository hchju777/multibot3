/**
 * @file map_registry_convert.cpp
 * @brief MapRegistry 3뷰·응답 변환 — [0a] tracer bullet 경로 3함수 구현 + 나머지 스텁.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 미구현 함수는 기본 생성
 * @ref mrs::convert::ConvertResult (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼
 * 보이는 값으로 채우지 않는다.
 *
 * ## 구현된 함수 (본문 있음)
 * `nodes_to_msg` · `edges_to_msg` · `fill_response(GetRoadmap)`.
 *
 * ## 유예 (계약 U-18)
 * `mrs::RoadmapViewData` 에 뷰 종류 판별자가 없어 세 뷰가 같은 시그니처를 쓴다. 따라서
 * `nodes_to_msg`/`edges_to_msg` 는 **V1 종류 대조를 수행하지 못한다**. 도메인 헤더를 고치지
 * 않고 [0a] 는 현행 형태로 두며, "어느 뷰인가"는 호출 노드가 어느 서비스 핸들러에서
 * 부르는가로만 보장된다([1] D-11 해소 예정 — 헤더 `@note` 참조).
 * 같은 이유로 노드·엣지 id 는 아직 강타입이 아니어서 `node_id_*` 헬퍼를 쓸 수 없고,
 * 센티넬 판정을 `mrs::NODE_ID_NONE`·`mrs::EDGE_ID_NONE` 과의 직접 대조로 한다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cmath>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mrs::convert
{

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

  std::unordered_set<mrs::NodeId> seen_node_ids;
  seen_node_ids.reserve(view.nodes.size());

  for (const mrs::RoadmapNodeView & node : view.nodes)
  {
    // ② 필드 범위 — 센티넬·중복.
    if (node.node_id == mrs::NODE_ID_NONE)
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
    if (edge.edge_id == mrs::EDGE_ID_NONE)
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    if (edge.node_a == mrs::NODE_ID_NONE || edge.node_b == mrs::NODE_ID_NONE)
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
  (void)view;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult fill_response(
  const mrs::RoadmapViewData & view, std::uint64_t known_version,
  mrs_interfaces::srv::GetRoadmap::Response & resp)
{
  // ① 스코프 자기 유효성 + 물리 뷰 규약(`view_id` 는 항상 0).
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
  const mrs::RoadmapValidationResult & result, std::uint64_t roadmap_version,
  std::uint32_t view_id, mrs_interfaces::srv::ValidateMap::Response & resp)
{
  (void)result;
  (void)roadmap_version;
  (void)view_id;
  (void)resp;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
