#include <stdexcept>

#include "mrs_ros_pp/pp_service_node.hpp"

namespace mrs
{

PpServiceNode::PpServiceNode() : rclcpp::Node("pp_service"), planner_(nullptr)
{
  rclcpp::QoS sub_qos(5);
  sub_qos.reliable();
  sub_qos.durability_volatile();
  task_assignment_sub_ = this->create_subscription<mrs_interfaces::msg::TaskAssignment>(
    "/task_assignment", sub_qos,
    std::bind(&PpServiceNode::on_task_assignment, this, std::placeholders::_1));

  rclcpp::QoS pub_qos(5);
  pub_qos.reliable();
  pub_qos.durability_volatile();
  planned_paths_pub_ =
    this->create_publisher<mrs_interfaces::msg::PlannedPaths>("/planned_paths", pub_qos);

  plan_paths_srv_ = this->create_service<mrs_interfaces::srv::PlanPaths>(
    "/pp_service/plan_paths",
    std::bind(&PpServiceNode::on_plan_paths, this, std::placeholders::_1, std::placeholders::_2));
  partial_replan_srv_ = this->create_service<mrs_interfaces::srv::PartialReplan>(
    "/pp_service/partial_replan",
    std::bind(&PpServiceNode::on_partial_replan, this, std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(this->get_logger(), "pp_service started (stub) — solver not yet injected");
}

void PpServiceNode::on_task_assignment(const mrs_interfaces::msg::TaskAssignment::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: PpServiceNode::on_task_assignment — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_task_assignment failed (stub, safe no-op): %s", e.what());
  }
}

void PpServiceNode::on_plan_paths(
  const std::shared_ptr<mrs_interfaces::srv::PlanPaths::Request> request,
  std::shared_ptr<mrs_interfaces::srv::PlanPaths::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: PpServiceNode::on_plan_paths — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_plan_paths failed (stub, safe no-op): %s", e.what());
  }
}

void PpServiceNode::on_partial_replan(
  const std::shared_ptr<mrs_interfaces::srv::PartialReplan::Request> request,
  std::shared_ptr<mrs_interfaces::srv::PartialReplan::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error(
      "not implemented: PpServiceNode::on_partial_replan — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_partial_replan failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
