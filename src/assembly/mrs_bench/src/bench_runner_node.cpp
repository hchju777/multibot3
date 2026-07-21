#include <stdexcept>

#include "mrs_bench/bench_runner_node.hpp"

namespace mrs
{

BenchRunnerNode::BenchRunnerNode() : rclcpp::Node("bench_runner")
{
  step_client_ = this->create_client<mrs_interfaces::srv::SimStep>("/sim/step");
  inject_client_ = this->create_client<mrs_interfaces::srv::SimInject>("/sim/inject");
  capabilities_client_ =
    this->create_client<mrs_interfaces::srv::SimQueryCapabilities>("/sim/query_capabilities");

  rclcpp::QoS metric_qos(50);
  metric_qos.reliable();
  metric_qos.durability_volatile();
  metric_sample_sub_ = this->create_subscription<mrs_interfaces::msg::SimMetricSample>(
    "/sim/metrics_tap", metric_qos,
    std::bind(&BenchRunnerNode::on_metric_sample, this, std::placeholders::_1));

  rclcpp::QoS rung_qos(50);
  rung_qos.reliable();
  rung_qos.durability_volatile();
  rung_event_sub_ = this->create_subscription<mrs_interfaces::msg::RungEvent>(
    "/ladder/rung_event", rung_qos,
    std::bind(&BenchRunnerNode::on_rung_event, this, std::placeholders::_1));

  RCLCPP_INFO(this->get_logger(), "bench_runner started (stub)");
}

bool BenchRunnerNode::run_scenario(const std::string & scenario_name)
{
  (void)scenario_name;
  throw std::logic_error(
    "not implemented: BenchRunnerNode::run_scenario — Phase 6 (sim-runner/sim-scenarios) 대상");
}

void BenchRunnerNode::on_metric_sample(const mrs_interfaces::msg::SimMetricSample::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error(
      "not implemented: BenchRunnerNode::on_metric_sample — Phase 6 (sim-runner) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_metric_sample failed (stub, safe no-op): %s", e.what());
  }
}

void BenchRunnerNode::on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg)
{
  (void)msg;
  try
  {
    throw std::logic_error("not implemented: BenchRunnerNode::on_rung_event — Phase 6 (sim-runner) 대상");
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(this->get_logger(), "on_rung_event failed (stub, safe no-op): %s", e.what());
  }
}

} // namespace mrs
