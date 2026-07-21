#include <stdexcept>

#include "mrs/startup_checks.hpp"
#include "mrs_ros_l4/local_traj_node.hpp"

namespace mrs
{

LocalTrajNode::LocalTrajNode() : rclcpp::Node("l4_traj_node"), planner_(nullptr)
{
  robot_id_ = this->declare_parameter<int>("robot_id", 0);
  neighbor_robot_ids_ = this->declare_parameter<std::vector<std::int64_t>>(
    "neighbor_robot_ids", std::vector<std::int64_t>{});

  const std::string ns = "/robot_" + std::to_string(robot_id_);

  rclcpp::QoS window_qos(5);
  window_qos.reliable();
  window_qos.durability_volatile();
  execution_window_sub_ = this->create_subscription<mrs_interfaces::msg::ExecutionWindow>(
    ns + "/execution_window", window_qos,
    std::bind(&LocalTrajNode::on_execution_window, this, std::placeholders::_1));

  rclcpp::QoS tick_qos(1);
  tick_qos.reliable();
  tick_qos.durability_volatile();
  tick_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  tick_qos.liveliness(rclcpp::LivelinessPolicy::Automatic);
  tick_qos.liveliness_lease_duration(rclcpp::Duration::from_seconds(0.3));
  plan_tick_sub_ = this->create_subscription<mrs_interfaces::msg::PlanTick>(
    "/plan_tick", tick_qos, std::bind(&LocalTrajNode::on_plan_tick, this, std::placeholders::_1));

  rclcpp::QoS state_qos(1);
  state_qos.best_effort();
  state_qos.durability_volatile();
  robot_state_sub_ = this->create_subscription<mrs_interfaces::msg::RobotState>(
    ns + "/robot_state", state_qos, std::bind(&LocalTrajNode::on_robot_state, this, std::placeholders::_1));

  rclcpp::QoS neighbor_qos(1);
  neighbor_qos.best_effort();
  neighbor_qos.durability_volatile();
  neighbor_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  for (const auto & raw_id : neighbor_robot_ids_)
  {
    const std::uint16_t neighbor_id = static_cast<std::uint16_t>(raw_id);
    const std::string neighbor_topic = "/robot_" + std::to_string(raw_id) + "/local_plan_share";
    neighbor_plan_subs_.push_back(
      this->create_subscription<mrs_interfaces::msg::LocalPlanShare>(
        neighbor_topic, neighbor_qos,
        [this, neighbor_id](const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
        {
          this->on_neighbor_plan_share(neighbor_id, msg);
        }));
  }

  rclcpp::QoS cmd_qos(1);
  cmd_qos.best_effort();
  cmd_qos.durability_volatile();
  cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(ns + "/cmd_vel", cmd_qos);

  rclcpp::QoS escalation_qos(20);
  escalation_qos.reliable();
  escalation_qos.durability_volatile();
  escalation_report_pub_ =
    this->create_publisher<mrs_interfaces::msg::EscalationReport>(ns + "/escalation_report", escalation_qos);

  rclcpp::QoS commit_qos(5);
  commit_qos.reliable();
  commit_qos.durability_volatile();
  commit_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  commit_status_pub_ =
    this->create_publisher<mrs_interfaces::msg::CommitStatus>(ns + "/commit_status", commit_qos);

  rclcpp::QoS share_qos(1);
  share_qos.best_effort();
  share_qos.durability_volatile();
  share_qos.deadline(rclcpp::Duration::from_seconds(0.2));
  local_plan_share_pub_ =
    this->create_publisher<mrs_interfaces::msg::LocalPlanShare>(ns + "/local_plan_share", share_qos);

  startup_check_timer_ = this->create_wall_timer(
    std::chrono::milliseconds(1000),
    std::bind(&LocalTrajNode::check_plan_tick_single_publisher, this));

  RCLCPP_INFO(
    this->get_logger(), "l4_traj_node started (stub) — robot_id=%d, neighbors=%zu",
    robot_id_, neighbor_robot_ids_.size());
}

void LocalTrajNode::check_plan_tick_single_publisher()
{
  startup_check_timer_->cancel(); // 1회성

  const std::size_t publisher_count = this->count_publishers("/plan_tick");
  if (mrs::exceeds_single_publisher_count(publisher_count))
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "R-05 위반: /plan_tick 발행자 %zu개 감지(2개 이상) — 기동을 거부합니다. "
      "(계약 L-15: 두 개의 틱 소스는 (A1) 지수 합의를 조용히 깨뜨려 F5 결과를 무효화합니다.)",
      publisher_count);
    rclcpp::shutdown();
    return;
  }
  RCLCPP_INFO(
    this->get_logger(), "R-05 검사 통과 — /plan_tick 발행자 %zu개", publisher_count);
}

void LocalTrajNode::on_execution_window(const mrs_interfaces::msg::ExecutionWindow::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: LocalTrajNode::on_execution_window — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_execution_window failed (stub, safe no-op): %s", e.what());
  }
}

void LocalTrajNode::on_plan_tick(const mrs_interfaces::msg::PlanTick::SharedPtr msg)
{
  (void)msg;
  try
  {
    // T4 S0~S5 (solve_ok(h) 술어, 인증 폴백)는 Phase 5 대상. 시프트 연산자·이웃 신선도
    // 추적기는 이미 배선돼 있으며(shift_operator_, freshness_tracker_) 이 콜백이 호출한다.
    throw std::logic_error("not implemented: LocalTrajNode::on_plan_tick — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_plan_tick failed (stub, safe no-op): %s", e.what());
  }
}

void LocalTrajNode::on_robot_state(const mrs_interfaces::msg::RobotState::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error("not implemented: LocalTrajNode::on_robot_state — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_robot_state failed (stub, safe no-op): %s", e.what());
  }
}

void LocalTrajNode::on_neighbor_plan_share(
  std::uint16_t neighbor_robot_id, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: LocalTrajNode::on_neighbor_plan_share(neighbor=" +
      std::to_string(neighbor_robot_id) + ") — Phase 5 (ros-builder) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_neighbor_plan_share failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
