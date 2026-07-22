/**
 * @file map_registry.cpp
 * @brief MapRegistry 파사드 구현 — 파일 3 단계에서는 **안전 폴백 스텁**이다.
 *
 * 예외를 던지지 않는다(CLAUDE.md 규율 2, 계약 §0.1 V3). 각 함수는 @ref
 * mrs::MapStatus::NOT_IMPLEMENTED 실패를 반환하며 크래시하지 않는다 —
 * 로더·검사기·세분화·collapse·변환의 알고리즘 본문은 파일 4 (코어)가 채운다. 그때 NOT_IMPLEMENTED
 * 를 반환하는 경로가 0 이 된다.
 */

#include "mrs_map_registry/map_registry.hpp"

namespace mrs
{

MapResult<std::uint64_t> MapRegistry::load_physical_roadmap(const std::string & map_file_path)
{
  (void)map_file_path;
  // TODO(파일 4): 로더 YAML + 직선 전제 검증 + 버전 발급. 지금은 안전 폴백.
  return MapResult<std::uint64_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<std::uint32_t> MapRegistry::build_uniform_view(double unit_length_m)
{
  (void)unit_length_m;
  // TODO(파일 4): 입도 하한 강제 + 엣지 세분화 + 균일 뷰 발급. 지금은 안전 폴백.
  return MapResult<std::uint32_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<std::uint32_t> MapRegistry::build_dependency_skeleton(std::uint32_t uniform_view_id)
{
  (void)uniform_view_id;
  // TODO(파일 4): 차수 2 체인 collapse + 골격 뷰 발급. 지금은 안전 폴백.
  return MapResult<std::uint32_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<RoadmapViewData> MapRegistry::get_view(std::uint32_t view_id) const
{
  (void)view_id;
  // TODO(파일 4): graph_index 조회 + 뷰 데이터 조립. 지금은 안전 폴백.
  return MapResult<RoadmapViewData>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<std::uint32_t> MapRegistry::transform_node(
  std::uint32_t from_view_id, std::uint32_t from_node, std::uint32_t to_view_id) const
{
  (void)from_view_id;
  (void)from_node;
  (void)to_view_id;
  // TODO(파일 4): 뷰 간 대응표 기반 노드 변환. 지금은 안전 폴백.
  return MapResult<std::uint32_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<RoadmapValidationResult> MapRegistry::validate_map(
  std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
  double inflation_radius_m, double nid_offset_l_m, double v_max_mps, double omega_max_rps) const
{
  (void)view_id;
  (void)robot_count;
  (void)robot_radius_m;
  (void)inflation_radius_m;
  (void)nid_offset_l_m;
  (void)v_max_mps;
  (void)omega_max_rps;
  // TODO(파일 4): 순수기하 3단 검사기 + 세분화 입도 + r_clear + 검사기 4 advisory. 지금은 안전
  // 폴백.
  return MapResult<RoadmapValidationResult>::fail(MapStatus::NOT_IMPLEMENTED);
}

} // namespace mrs
