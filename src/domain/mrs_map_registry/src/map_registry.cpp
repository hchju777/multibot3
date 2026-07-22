/**
 * @file map_registry.cpp
 * @brief MapRegistry 파사드 — 공개 API 를 PIMPL(@ref mrs::MapRegistry::Impl)로 포워드한다.
 *
 * 예외를 던지지 않는다(CLAUDE.md 규율 2, 계약 §0.1 V3). 아직 구현되지 않은 연산은
 * @ref mrs::MapStatus::NOT_IMPLEMENTED 를 반환한다(하위 B·C 에서 채운다).
 */

#include "mrs_map_registry/map_registry.hpp"

#include <memory>
#include <utility>

#include "map_registry_impl.hpp"

namespace mrs
{

MapRegistry::MapRegistry() : impl_{std::make_unique<Impl>()}
{
}

MapRegistry::~MapRegistry() = default;

MapRegistry::MapRegistry(MapRegistry &&) noexcept = default;

MapRegistry & MapRegistry::operator=(MapRegistry &&) noexcept = default;

MapResult<std::uint64_t> MapRegistry::load_physical_roadmap(const std::string & map_file_path)
{
  return impl_->load(map_file_path);
}

MapResult<std::uint32_t> MapRegistry::build_uniform_view(double unit_length_m)
{
  (void)unit_length_m;
  // TODO(하위 C): 입도 하한 강제 + 엣지 세분화 + 균일 뷰 발급.
  return MapResult<std::uint32_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<std::uint32_t> MapRegistry::build_dependency_skeleton(std::uint32_t uniform_view_id)
{
  (void)uniform_view_id;
  // TODO(하위 C): 차수 2 체인 collapse + 골격 뷰 발급.
  return MapResult<std::uint32_t>::fail(MapStatus::NOT_IMPLEMENTED);
}

MapResult<RoadmapViewData> MapRegistry::get_view(std::uint32_t view_id) const
{
  return impl_->get_view(view_id);
}

MapResult<std::uint32_t> MapRegistry::transform_node(
  std::uint32_t from_view_id, std::uint32_t from_node, std::uint32_t to_view_id) const
{
  (void)from_view_id;
  (void)from_node;
  (void)to_view_id;
  // TODO(하위 C): 뷰 간 대응표 기반 노드 변환.
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
  // TODO(하위 B): 순수기하 3단 검사기 + 세분화 입도 + r_clear + 검사기 4 advisory.
  return MapResult<RoadmapValidationResult>::fail(MapStatus::NOT_IMPLEMENTED);
}

std::uint64_t MapRegistry::roadmap_version() const noexcept
{
  return impl_->roadmap_version;
}

} // namespace mrs
