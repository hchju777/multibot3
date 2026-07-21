#pragma once

/**
 * @file bench_runner_node.hpp
 * @brief bench_runner — D-09 시나리오 러너. `/sim/step`·`/sim/inject` 클라이언트 +
 * `/sim/metrics_tap`·`/ladder/rung_event` 구독자로 §4.2·§4.3 계측·결함주입을 구동한다.
 *
 * baseline 3종(D-09)은 이 노드가 아니라 `IGatingPolicy`/`IJudgePolicy` 전략 교체 +
 * launch 구성으로 실현된다(architecture §3 seam e/f) — 이 노드는 시나리오 스크립트
 * 실행자일 뿐 baseline 로직을 자체 구현하지 않는다.
 */

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/msg/sim_metric_sample.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"
#include "mrs_interfaces/srv/sim_query_capabilities.hpp"
#include "mrs_interfaces/srv/sim_step.hpp"
#include "mrs_metrics/metrics_collector.hpp"

namespace mrs
{

/**
 * @brief D-09 벤치마크 시나리오 러너 노드.
 */
class BenchRunnerNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 sim 서비스 클라이언트 3종 + 계측 구독자 2종을 구성한다.
   */
  BenchRunnerNode();
  ~BenchRunnerNode() override = default;

  /**
   * @brief 시나리오 스크립트 1건을 실행한다 (D-09).
   * @param[in] scenario_name 시나리오 식별자 (sim-scenarios 규격의 이름 공간).
   * @return bool 실행 성공 여부.
   */
  bool run_scenario(const std::string & scenario_name);

private:
  /** @brief `/sim/metrics_tap` 수신 콜백. @return void */
  void on_metric_sample(const mrs_interfaces::msg::SimMetricSample::SharedPtr msg);

  /** @brief `/ladder/rung_event` 수신 콜백. @return void */
  void on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg);

  mrs::MetricsCollector collector_; ///< §4.2 계측 수집

  rclcpp::Client<mrs_interfaces::srv::SimStep>::SharedPtr step_client_;
  rclcpp::Client<mrs_interfaces::srv::SimInject>::SharedPtr inject_client_;
  rclcpp::Client<mrs_interfaces::srv::SimQueryCapabilities>::SharedPtr capabilities_client_;

  rclcpp::Subscription<mrs_interfaces::msg::SimMetricSample>::SharedPtr metric_sample_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_sub_;
};

} // namespace mrs
