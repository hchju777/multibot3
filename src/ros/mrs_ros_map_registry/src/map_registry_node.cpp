#include <stdexcept>

#include "mrs_ros_map_registry/map_registry_node.hpp"

namespace mrs
{

MapRegistryNode::MapRegistryNode() : rclcpp::Node("map_registry")
{
  get_roadmap_srv_ = this->create_service<mrs_interfaces::srv::GetRoadmap>(
    "/map_registry/get_roadmap",
    std::bind(&MapRegistryNode::on_get_roadmap, this, std::placeholders::_1, std::placeholders::_2));
  get_uniform_view_srv_ = this->create_service<mrs_interfaces::srv::GetUniformView>(
    "/map_registry/get_uniform_view",
    std::bind(
      &MapRegistryNode::on_get_uniform_view, this, std::placeholders::_1, std::placeholders::_2));
  get_dependency_skeleton_srv_ = this->create_service<mrs_interfaces::srv::GetDependencySkeleton>(
    "/map_registry/get_dependency_skeleton",
    std::bind(
      &MapRegistryNode::on_get_dependency_skeleton, this, std::placeholders::_1,
      std::placeholders::_2));
  transform_node_srv_ = this->create_service<mrs_interfaces::srv::TransformNode>(
    "/map_registry/transform_node",
    std::bind(&MapRegistryNode::on_transform_node, this, std::placeholders::_1, std::placeholders::_2));
  validate_map_srv_ = this->create_service<mrs_interfaces::srv::ValidateMap>(
    "/map_registry/validate_map",
    std::bind(&MapRegistryNode::on_validate_map, this, std::placeholders::_1, std::placeholders::_2));

  // QoS 정본(contract-registry §3): reliable/transient_local/depth1 — 늦은 구독자에게도 필요(Q-6).
  rclcpp::QoS version_qos(1);
  version_qos.reliable();
  version_qos.transient_local();
  roadmap_version_pub_ =
    this->create_publisher<std_msgs::msg::UInt64>("/map_registry/roadmap_version", version_qos);

  RCLCPP_INFO(this->get_logger(), "map_registry started (stub)");
}

void MapRegistryNode::on_get_roadmap(
  const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: MapRegistryNode::on_get_roadmap — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_get_roadmap failed (stub, safe no-op): %s", e.what());
  }
}

void MapRegistryNode::on_get_uniform_view(
  const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: MapRegistryNode::on_get_uniform_view — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_get_uniform_view failed (stub, safe no-op): %s", e.what());
  }
}

void MapRegistryNode::on_get_dependency_skeleton(
  const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: MapRegistryNode::on_get_dependency_skeleton — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_get_dependency_skeleton failed (stub, safe no-op): %s", e.what());
  }
}

void MapRegistryNode::on_transform_node(
  const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
  std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: MapRegistryNode::on_transform_node — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_transform_node failed (stub, safe no-op): %s", e.what());
  }
}

void MapRegistryNode::on_validate_map(
  const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: MapRegistryNode::on_validate_map — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_validate_map failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
