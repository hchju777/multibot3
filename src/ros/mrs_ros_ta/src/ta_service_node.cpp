#include <stdexcept>

#include "mrs_ros_ta/ta_service_node.hpp"

namespace mrs
{

TaServiceNode::TaServiceNode() : rclcpp::Node("ta_service")
{
  rclcpp::QoS qos(5);
  qos.reliable();
  qos.durability_volatile();
  task_assignment_pub_ =
    this->create_publisher<mrs_interfaces::msg::TaskAssignment>("/task_assignment", qos);

  reassign_request_srv_ = this->create_service<mrs_interfaces::srv::ReassignRequest>(
    "/ta_service/reassign_request",
    std::bind(
      &TaServiceNode::on_reassign_request, this, std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(this->get_logger(), "ta_service started (stub)");
}

void TaServiceNode::on_reassign_request(
  const std::shared_ptr<mrs_interfaces::srv::ReassignRequest::Request> request,
  std::shared_ptr<mrs_interfaces::srv::ReassignRequest::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error(
      "not implemented: TaServiceNode::on_reassign_request — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_reassign_request failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
