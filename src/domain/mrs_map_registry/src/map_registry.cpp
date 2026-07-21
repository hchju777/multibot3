#include <stdexcept>

#include "mrs_map_registry/map_registry.hpp"

namespace mrs
{

bool MapRegistry::load_physical_roadmap(const std::string & map_file_path)
{
  (void)map_file_path;
  throw std::logic_error(
    "not implemented: MapRegistry::load_physical_roadmap — Phase 5 (coordination-builder) 대상");
}

bool MapRegistry::build_uniform_view(double unit_length_m, std::uint32_t & out_view_id)
{
  (void)unit_length_m;
  (void)out_view_id;
  throw std::logic_error(
    "not implemented: MapRegistry::build_uniform_view — Phase 5 (coordination-builder) 대상");
}

bool MapRegistry::build_dependency_skeleton(
  std::uint32_t uniform_view_id, std::uint32_t & out_skeleton_view_id)
{
  (void)uniform_view_id;
  (void)out_skeleton_view_id;
  throw std::logic_error(
    "not implemented: MapRegistry::build_dependency_skeleton — Phase 5 (coordination-builder) 대상");
}

bool MapRegistry::get_view(std::uint32_t view_id, RoadmapViewData & out_data) const
{
  (void)view_id;
  (void)out_data;
  throw std::logic_error(
    "not implemented: MapRegistry::get_view — Phase 5 (coordination-builder) 대상");
}

bool MapRegistry::transform_node(
  std::uint32_t from_view_id, NodeId from_node,
  std::uint32_t to_view_id, NodeId & out_node) const
{
  (void)from_view_id;
  (void)from_node;
  (void)to_view_id;
  (void)out_node;
  throw std::logic_error(
    "not implemented: MapRegistry::transform_node — Phase 5 (coordination-builder) 대상");
}

RoadmapValidationResult MapRegistry::validate_map(
  std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
  double v_max_mps, double omega_max_rps, double nid_offset_l_m) const
{
  (void)view_id;
  (void)robot_count;
  (void)robot_radius_m;
  (void)v_max_mps;
  (void)omega_max_rps;
  (void)nid_offset_l_m;
  throw std::logic_error(
    "not implemented: MapRegistry::validate_map — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
