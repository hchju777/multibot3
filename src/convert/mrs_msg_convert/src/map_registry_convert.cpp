#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

std::vector<mrs_interfaces::msg::RoadmapNode> nodes_to_msg(const mrs::RoadmapViewData & view)
{
  (void)view;
  throw std::logic_error("not implemented: mrs::convert::nodes_to_msg — Phase 5 (ros-builder) 대상");
}

std::vector<mrs_interfaces::msg::RoadmapEdge> edges_to_msg(const mrs::RoadmapViewData & view)
{
  (void)view;
  throw std::logic_error("not implemented: mrs::convert::edges_to_msg — Phase 5 (ros-builder) 대상");
}

std::vector<mrs_interfaces::msg::NodeMapping> mappings_to_msg(const mrs::RoadmapViewData & view)
{
  (void)view;
  throw std::logic_error("not implemented: mrs::convert::mappings_to_msg — Phase 5 (ros-builder) 대상");
}

void fill_response(
  const mrs::RoadmapValidationResult & result, mrs_interfaces::srv::ValidateMap::Response & resp)
{
  (void)result;
  (void)resp;
  throw std::logic_error("not implemented: mrs::convert::fill_response(RoadmapValidationResult) — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
