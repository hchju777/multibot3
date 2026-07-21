#pragma once

/**
 * @file sim_bridge_node.hpp
 * @brief sim_bridge — ISimBackend(seam c) 뒤에 실제 백엔드(pysim|isaac|fake)를 숨기는 유일 통로.
 *
 * architecture §5.1: 코어(도메인·노드)는 시뮬레이터를 모른다 — 알고 있는 것은 이 노드가
 * 발행하는 RobotState 와 자신이 발행하는 cmd_vel 뿐이다. 시뮬 티어에서 `/clock` 의 유일
 * 발행자(architecture §1.7-B, R-A2)이며, 백엔드 선택은 `sim_backend` 파라미터 한 줄이다
 * (scaffolding 통과 기준). 구체 백엔드 어댑터(ISimBackend 파생)는 noexcept 포트를 throw 로
 * 스텁화할 수 없어(-Wterminate) Phase 5(coordination-builder) 로 이관한다 —
 * `mrs_sim_abstraction/sim_backend_registry.hpp` 주석 참조.
 */

#include <cstdint>
#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include "mrs/i_sim_backend.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/srv/sim_step.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"
#include "mrs_interfaces/srv/sim_query_capabilities.hpp"

namespace mrs
{

/**
 * @brief 시뮬 백엔드 프로세스 경계를 감싸는 sim_bridge 노드.
 */
class SimBridgeNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 로봇 수만큼 robot_state/cmd_vel 토픽·sim 서비스 3종을 구성한다.
   */
  SimBridgeNode();
  ~SimBridgeNode() override = default;

private:
  /**
   * @brief `/sim/step` 서비스 핸들러 — 시뮬 시간을 명시적으로 전진시킨다.
   * @param[in] request steps 필드를 담은 요청.
   * @param[out] response 상태·전진 후 시뮬 시각.
   * @return void
   */
  void on_step(
    const std::shared_ptr<mrs_interfaces::srv::SimStep::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimStep::Response> response);

  /**
   * @brief `/sim/inject` 서비스 핸들러 — 결함을 주입한다.
   * @param[in] request 결함 종류·대상·강도.
   * @param[out] response 접수 결과·주입 id.
   * @return void
   */
  void on_inject(
    const std::shared_ptr<mrs_interfaces::srv::SimInject::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimInject::Response> response);

  /**
   * @brief `/sim/query_capabilities` 서비스 핸들러 — 백엔드 능력을 신고한다 (Q-8).
   * @param[in] request 빈 요청.
   * @param[out] response SimCapabilities.
   * @return void
   */
  void on_query_capabilities(
    const std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Response> response);

  /**
   * @brief 로봇 i 의 cmd_vel 수신 콜백 — actuate() 로 전달될 지령.
   * @param[in] robot_index 0-base 로봇 인덱스.
   * @param[in] msg 몸체 선속도·각속도 지령.
   * @return void
   */
  void on_cmd_vel(std::size_t robot_index, const geometry_msgs::msg::Twist::SharedPtr msg);

  std::shared_ptr<ISimBackend> backend_; ///< 활성 백엔드 (seam c). 스캐폴딩 단계는 nullptr

  std::vector<rclcpp::Publisher<mrs_interfaces::msg::RobotState>::SharedPtr> robot_state_pubs_;
  std::vector<rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr> cmd_vel_subs_;

  rclcpp::Service<mrs_interfaces::srv::SimStep>::SharedPtr step_srv_;
  rclcpp::Service<mrs_interfaces::srv::SimInject>::SharedPtr inject_srv_;
  rclcpp::Service<mrs_interfaces::srv::SimQueryCapabilities>::SharedPtr capabilities_srv_;

  int robot_count_{2}; ///< 파라미터 robot_count 기본값 (tracer bullet [0] = 2대)
};

} // namespace mrs
