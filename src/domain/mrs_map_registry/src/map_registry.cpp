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

MapResult<std::uint32_t> MapRegistry::build_uniform_view(
  double unit_length_m, double unit_length_lower_bound_m)
{
  return impl_->build_uniform(unit_length_m, unit_length_lower_bound_m);
}

MapResult<std::uint32_t> MapRegistry::build_dependency_skeleton(std::uint32_t uniform_view_id)
{
  return impl_->build_skeleton(uniform_view_id);
}

MapResult<RoadmapViewData> MapRegistry::get_view(std::uint32_t view_id) const
{
  return impl_->get_view(view_id);
}

MapResult<std::uint32_t> MapRegistry::transform_node(
  std::uint32_t from_view_id, std::uint32_t from_node, std::uint32_t to_view_id) const
{
  return impl_->transform(from_view_id, from_node, to_view_id);
}

MapResult<RoadmapValidationResult> MapRegistry::validate_map(
  std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
  double inflation_radius_m, double nid_offset_l_m, double v_max_mps, double omega_max_rps) const
{
  return impl_->validate(
    view_id, robot_count, robot_radius_m, inflation_radius_m, nid_offset_l_m, v_max_mps,
    omega_max_rps);
}

std::uint64_t MapRegistry::roadmap_version() const noexcept
{
  return impl_->roadmap_version;
}

} // namespace mrs
