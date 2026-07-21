#include <stdexcept>

#include "mrs_ros_plan_tick/plan_tick_node.hpp"

namespace mrs
{

PlanTickNode::PlanTickNode() : rclcpp::Node("plan_tick_node")
{
  replan_period_s_ = this->declare_parameter<double>("replan_period_s", 0.1);

  // QoS 정본(contract-registry §3): reliable/volatile/depth1/deadline 2*Δt_h/
  // liveliness AUTOMATIC lease 3*Δt_h — plan_tick_node 의 유일한 실패 모드(정지)를
  // 전 구독자가 즉시 관측하는 수단이다.
  rclcpp::QoS qos(1);
  qos.reliable();
  qos.durability_volatile();
  qos.deadline(rclcpp::Duration::from_seconds(2.0 * replan_period_s_));
  qos.liveliness(rclcpp::LivelinessPolicy::Automatic);
  qos.liveliness_lease_duration(rclcpp::Duration::from_seconds(3.0 * replan_period_s_));

  tick_pub_ = this->create_publisher<mrs_interfaces::msg::PlanTick>("/plan_tick", qos);

  timer_ = this->create_wall_timer(
    std::chrono::duration<double>(replan_period_s_), std::bind(&PlanTickNode::on_timer, this));

  RCLCPP_INFO(this->get_logger(), "plan_tick_node started (stub) — replan_period_s=%f", replan_period_s_);
}

void PlanTickNode::on_timer()
{
  try
  {
    // R-A1(tick_seq = floor((t-t0)/replan_period_s_)) 산식은 Phase 5(ros-builder) 대상.
    throw std::logic_error("not implemented: PlanTickNode::on_timer — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_timer failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
