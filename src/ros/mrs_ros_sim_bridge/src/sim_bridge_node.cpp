#include <stdexcept>

#include "mrs_ros_sim_bridge/sim_bridge_node.hpp"

namespace mrs
{

SimBridgeNode::SimBridgeNode() : rclcpp::Node("sim_bridge")
{
  robot_count_ = this->declare_parameter<int>("robot_count", 2);
  this->declare_parameter<std::string>("sim_backend", "pysim"); // config 한 줄 교체 지점 (R-01)

  rclcpp::QoS state_qos(1);
  state_qos.best_effort();
  state_qos.durability_volatile();

  rclcpp::QoS cmd_qos(1);
  cmd_qos.best_effort();
  cmd_qos.durability_volatile();

  for (int i = 0; i < robot_count_; ++i)
  {
    const std::string ns = "/robot_" + std::to_string(i);
    robot_state_pubs_.push_back(
      this->create_publisher<mrs_interfaces::msg::RobotState>(ns + "/robot_state", state_qos));

    const std::size_t robot_index = static_cast<std::size_t>(i);
    cmd_vel_subs_.push_back(
      this->create_subscription<geometry_msgs::msg::Twist>(
        ns + "/cmd_vel", cmd_qos,
        [this, robot_index](const geometry_msgs::msg::Twist::SharedPtr msg)
        {
          this->on_cmd_vel(robot_index, msg);
        }));
  }

  step_srv_ = this->create_service<mrs_interfaces::srv::SimStep>(
    "/sim/step",
    std::bind(&SimBridgeNode::on_step, this, std::placeholders::_1, std::placeholders::_2));
  inject_srv_ = this->create_service<mrs_interfaces::srv::SimInject>(
    "/sim/inject",
    std::bind(&SimBridgeNode::on_inject, this, std::placeholders::_1, std::placeholders::_2));
  capabilities_srv_ = this->create_service<mrs_interfaces::srv::SimQueryCapabilities>(
    "/sim/query_capabilities",
    std::bind(
      &SimBridgeNode::on_query_capabilities, this, std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(
    this->get_logger(), "sim_bridge started (stub) — robot_count=%d, backend=nullptr", robot_count_);
}

void SimBridgeNode::on_step(
  const std::shared_ptr<mrs_interfaces::srv::SimStep::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimStep::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: SimBridgeNode::on_step — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_step failed (stub, safe no-op): %s", e.what());
  }
}

void SimBridgeNode::on_inject(
  const std::shared_ptr<mrs_interfaces::srv::SimInject::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimInject::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error("not implemented: SimBridgeNode::on_inject — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_inject failed (stub, safe no-op): %s", e.what());
  }
}

void SimBridgeNode::on_query_capabilities(
  const std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Request> request,
  std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error(
      "not implemented: SimBridgeNode::on_query_capabilities — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_query_capabilities failed (stub, safe no-op): %s", e.what());
  }
}

void SimBridgeNode::on_cmd_vel(std::size_t robot_index, const geometry_msgs::msg::Twist::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: SimBridgeNode::on_cmd_vel(robot=" + std::to_string(robot_index) +
      ") — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_cmd_vel failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
