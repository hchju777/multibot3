#include <stdexcept>

#include "mrs_ros_sadg/sadg_service_node.hpp"

namespace mrs
{

SadgServiceNode::SadgServiceNode()
: rclcpp::Node("sadg_service"),
  btpg_tracker_(nullptr),
  milp_tracker_(nullptr),
  judge_(nullptr)
{
  robot_count_ = this->declare_parameter<int>("robot_count", 2);
  this->declare_parameter<std::string>("reorder_transport", "inproc"); // Q-2: inproc|service

  rclcpp::QoS planned_paths_qos(5);
  planned_paths_qos.reliable();
  planned_paths_qos.durability_volatile();
  planned_paths_sub_ = this->create_subscription<mrs_interfaces::msg::PlannedPaths>(
    "/planned_paths", planned_paths_qos,
    std::bind(&SadgServiceNode::on_planned_paths, this, std::placeholders::_1));

  rclcpp::QoS commit_status_qos(5);
  commit_status_qos.reliable();
  commit_status_qos.durability_volatile();
  commit_status_qos.deadline(rclcpp::Duration::from_seconds(0.2)); // 2*Δt_h 기본값(0.1s) 문헌치

  rclcpp::QoS window_qos(5);
  window_qos.reliable();
  window_qos.durability_volatile(); // transient_local 금지(architecture §1.5 — 좀비 창 차단)

  for (int i = 0; i < robot_count_; ++i)
  {
    const std::string ns = "/robot_" + std::to_string(i);
    const std::size_t robot_index = static_cast<std::size_t>(i);
    commit_status_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::CommitStatus>(
        ns + "/commit_status", commit_status_qos,
        [this, robot_index](const mrs_interfaces::msg::CommitStatus::SharedPtr msg)
        {
          this->on_commit_status(robot_index, msg);
        }));
    execution_window_pubs_.push_back(
      this->create_publisher<mrs_interfaces::msg::ExecutionWindow>(
        ns + "/execution_window", window_qos));
  }

  rclcpp::QoS verdict_qos(50);
  verdict_qos.reliable();
  verdict_qos.durability_volatile();
  judge_verdict_pub_ =
    this->create_publisher<mrs_interfaces::msg::JudgeVerdict>("/ladder/judge_verdict", verdict_qos);

  rclcpp::QoS rung_qos(50);
  rung_qos.reliable();
  rung_qos.durability_volatile();
  rung_event_pub_ = this->create_publisher<mrs_interfaces::msg::RungEvent>("/ladder/rung_event", rung_qos);

  reorder_passing_srv_ = this->create_service<mrs_interfaces::srv::ReorderPassing>(
    "/sadg_service/reorder_passing",
    std::bind(&SadgServiceNode::on_reorder_passing, this, std::placeholders::_1, std::placeholders::_2));

  RCLCPP_INFO(this->get_logger(), "sadg_service started (stub) — robot_count=%d", robot_count_);
}

void SadgServiceNode::on_planned_paths(const mrs_interfaces::msg::PlannedPaths::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: SadgServiceNode::on_planned_paths — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_planned_paths failed (stub, safe no-op): %s", e.what());
  }
}

void SadgServiceNode::on_commit_status(
  std::size_t robot_index, const mrs_interfaces::msg::CommitStatus::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: SadgServiceNode::on_commit_status(robot=" + std::to_string(robot_index) +
      ") — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_commit_status failed (stub, safe no-op): %s", e.what());
  }
}

void SadgServiceNode::on_reorder_passing(
  const std::shared_ptr<mrs_interfaces::srv::ReorderPassing::Request> request,
  std::shared_ptr<mrs_interfaces::srv::ReorderPassing::Response> response)
{
  (void)request;
  (void)response;
  try
  {
    throw std::logic_error(
      "not implemented: SadgServiceNode::on_reorder_passing — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_reorder_passing failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
