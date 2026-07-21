#include <stdexcept>

#include "mrs_ros_comms_emul/comms_emul_node.hpp"

namespace mrs
{

CommsEmulNode::CommsEmulNode() : rclcpp::Node("comms_emul")
{
  robot_count_ = this->declare_parameter<int>("robot_count", 2);

  rclcpp::QoS qos(1);
  qos.best_effort();
  qos.durability_volatile();
  qos.deadline(rclcpp::Duration::from_seconds(0.2));

  for (int i = 0; i < robot_count_; ++i)
  {
    const std::string ns = "/robot_" + std::to_string(i);
    const std::size_t robot_index = static_cast<std::size_t>(i);

    relay_pubs_.push_back(
      this->create_publisher<mrs_interfaces::msg::LocalPlanShare>(ns + "/local_plan_share", qos));
    source_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::LocalPlanShare>(
        ns + "/local_plan_share_src", qos,
        [this, robot_index](const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
        {
          this->on_local_plan_share_src(robot_index, msg);
        }));
  }

  RCLCPP_INFO(this->get_logger(), "comms_emul started (stub) — robot_count=%d", robot_count_);
}

void CommsEmulNode::on_local_plan_share_src(
  std::size_t robot_index, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
{
  (void)msg;
  try
  {
    // 결함 적용(지연·손실) 없이 그대로 중계하는 것조차 스텁 단계에서는 하지 않는다 —
    // "알맹이 없음" 원칙(Phase 4). 실제 중계+결함주입은 Phase 5(ros-builder) 대상.
    throw std::logic_error(
      "not implemented: CommsEmulNode::on_local_plan_share_src(robot=" +
      std::to_string(robot_index) + ") — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "relay failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
