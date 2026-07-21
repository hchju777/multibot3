#pragma once

/**
 * @file local_traj_node.hpp
 * @brief l4_traj_node — 로봇 1대의 온보드 분산 궤적최적화 노드 (D-07, D-05, R0).
 *
 * per-robot 프로세스: `robot_id` 파라미터로 자기 정체성을, `neighbor_robot_ids` 로 **전 구독**
 * 대상을 받는다(architecture §1.8-B — "구독 층에서 이웃을 고르지 않는다", 안전 구멍 방지).
 *
 * **R-05 기동 거부 검사(실제 구현, design-decision v5 지시)**: `/plan_tick` 발행자가 2개
 * 이상이면 기동을 거부한다. discovery 지연을 감안해 기동 후 1회성 타이머로 검사한다
 * (생성자 시점 동기 조회는 DDS 아직 미발견 상태일 수 있어 신뢬성이 없다).
 */

#include <cstdint>
#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include "mrs_local_traj/shift_operator.hpp"
#include "mrs_local_traj/neighbor_freshness_tracker.hpp"
#include "mrs/i_local_planner.hpp"

#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/plan_tick.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/msg/local_plan_share.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/commit_status.hpp"

namespace mrs
{

/**
 * @brief L4 온보드 궤적최적화 노드 (로봇 1대분).
 */
class LocalTrajNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·전 토픽을 구성한다. R-05 검사는 1회성 타이머로 지연 실행된다.
   */
  LocalTrajNode();
  ~LocalTrajNode() override = default;

private:
  /** @brief `/robot_{id}/execution_window` 수신 콜백. @return void */
  void on_execution_window(const mrs_interfaces::msg::ExecutionWindow::SharedPtr msg);

  /** @brief `/plan_tick` 수신 콜백 (T1-R1, 재계획 틱 구동). @return void */
  void on_plan_tick(const mrs_interfaces::msg::PlanTick::SharedPtr msg);

  /** @brief `/robot_{id}/robot_state` 수신 콜백 (자기 상태). @return void */
  void on_robot_state(const mrs_interfaces::msg::RobotState::SharedPtr msg);

  /**
   * @brief 이웃 `/robot_j/local_plan_share` 수신 콜백 (T4-R3, fresh_j 판정 입력).
   * @param[in] neighbor_robot_id 발신 이웃 로봇 id.
   * @param[in] msg 이웃의 채택 계획.
   * @return void
   */
  void on_neighbor_plan_share(std::uint16_t neighbor_robot_id, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg);

  /**
   * @brief R-05 기동 거부 검사 — `/plan_tick` 발행자 수를 조회해 2개 이상이면 기동을 거부한다.
   * @return void
   * @note 실제 구현(design-decision v5 예외 2건 중 1건). 위반 시 RCLCPP_FATAL 로그 후
   *       `rclcpp::shutdown()` 을 호출한다 — 노드를 죽이지 않고 프로세스를 정상 종료시킨다.
   */
  void check_plan_tick_single_publisher();

  int robot_id_{0};                                 ///< 파라미터 robot_id (기본 0)
  std::vector<std::int64_t> neighbor_robot_ids_;     ///< 파라미터 neighbor_robot_ids (전 구독 대상)

  mrs::ShiftOperator shift_operator_;                ///< 시프트 연산자 S (T1-R3)
  mrs::NeighborFreshnessTracker freshness_tracker_;  ///< fresh_j(h-1) 추적 (T4 §6.1)
  std::shared_ptr<ILocalPlanner> planner_;           ///< ILocalPlanner (seam b 확장). 스캐폴딩은 nullptr

  rclcpp::Subscription<mrs_interfaces::msg::ExecutionWindow>::SharedPtr execution_window_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::PlanTick>::SharedPtr plan_tick_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::RobotState>::SharedPtr robot_state_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::LocalPlanShare>::SharedPtr> neighbor_plan_subs_;

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::EscalationReport>::SharedPtr escalation_report_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::CommitStatus>::SharedPtr commit_status_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::LocalPlanShare>::SharedPtr local_plan_share_pub_;

  rclcpp::TimerBase::SharedPtr startup_check_timer_; ///< R-05 지연 검사용 1회성 타이머
};

} // namespace mrs
