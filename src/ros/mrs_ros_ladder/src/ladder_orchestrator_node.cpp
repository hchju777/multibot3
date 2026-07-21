#include <stdexcept>

#include "mrs_ros_ladder/ladder_orchestrator_node.hpp"

namespace mrs
{

LadderOrchestratorNode::LadderOrchestratorNode() : rclcpp::Node("ladder_orchestrator")
{
  robot_count_ = this->declare_parameter<int>("robot_count", 2);

  rclcpp::QoS escalation_qos(20);
  escalation_qos.reliable();
  escalation_qos.durability_volatile();

  for (int i = 0; i < robot_count_; ++i)
  {
    const std::string ns = "/robot_" + std::to_string(i);
    const std::size_t robot_index = static_cast<std::size_t>(i);
    escalation_report_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::EscalationReport>(
        ns + "/escalation_report", escalation_qos,
        [this, robot_index](const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
        {
          this->on_escalation_report(robot_index, msg);
        }));
  }

  rclcpp::QoS verdict_qos(50);
  verdict_qos.reliable();
  verdict_qos.durability_volatile();
  judge_verdict_sub_ = this->create_subscription<mrs_interfaces::msg::JudgeVerdict>(
    "/ladder/judge_verdict", verdict_qos,
    std::bind(&LadderOrchestratorNode::on_judge_verdict, this, std::placeholders::_1));

  rclcpp::QoS rung_qos(50);
  rung_qos.reliable();
  rung_qos.durability_volatile();
  rung_event_sub_ = this->create_subscription<mrs_interfaces::msg::RungEvent>(
    "/ladder/rung_event", rung_qos,
    std::bind(&LadderOrchestratorNode::on_rung_event, this, std::placeholders::_1));

  partial_replan_client_ =
    this->create_client<mrs_interfaces::srv::PartialReplan>("/pp_service/partial_replan");
  reassign_request_client_ =
    this->create_client<mrs_interfaces::srv::ReassignRequest>("/ta_service/reassign_request");

  RCLCPP_INFO(this->get_logger(), "ladder_orchestrator started (stub) — robot_count=%d", robot_count_);
}

void LadderOrchestratorNode::on_escalation_report(
  std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: LadderOrchestratorNode::on_escalation_report(robot=" +
      std::to_string(robot_index) + ") — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_escalation_report failed (stub, safe no-op): %s", e.what());
  }
}

void LadderOrchestratorNode::on_judge_verdict(const mrs_interfaces::msg::JudgeVerdict::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: LadderOrchestratorNode::on_judge_verdict — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_judge_verdict failed (stub, safe no-op): %s", e.what());
  }
}

void LadderOrchestratorNode::on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: LadderOrchestratorNode::on_rung_event — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_rung_event failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
