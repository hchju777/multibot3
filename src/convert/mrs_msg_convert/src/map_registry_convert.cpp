/**
 * @file map_registry_convert.cpp
 * @brief MapRegistry 3뷰·응답 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 스텁은 기본 생성 @ref mrs::convert::ConvertResult
 * (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않는다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult nodes_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapNode> & out)
{
  (void)view;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult edges_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapEdge> & out)
{
  (void)view;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
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
  (void)view;
  (void)known_version;
  (void)resp;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
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
