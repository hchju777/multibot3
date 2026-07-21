#pragma once

/**
 * @file ladder_orchestrator_node.hpp
 * @brief ladder_orchestrator — 사다리 상태기계 노드 (D-08). R5 알림(대시보드)은 U-08 유보.
 *
 * ⛔ 이 노드는 `mrs_judge`·`mrs_reorder_btpg` 를 include 하지 않는다 — 판정 결과는
 * JudgeVerdict/RungEvent **값**으로만 받는다(architecture §2.3, C3).
 */

#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ladder/ladder_state_machine.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/judge_verdict.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/srv/partial_replan.hpp"
#include "mrs_interfaces/srv/reassign_request.hpp"

namespace mrs
{

/**
 * @brief 사다리 오케스트레이션 노드.
 */
class LadderOrchestratorNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 로봇 수만큼 escalation_report 구독 + 클라이언트 2종을 구성한다.
   */
  LadderOrchestratorNode();
  ~LadderOrchestratorNode() override = default;

private:
  /**
   * @brief 로봇 i 의 `/robot_i/escalation_report` 수신 콜백.
   * @param[in] robot_index 0-base 로봇 인덱스.
   * @param[in] msg 에스컬레이션 보고.
   * @return void
   */
  void on_escalation_report(
    std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg);

  /** @brief `/ladder/judge_verdict` 수신 콜백 (D-06 경계, 값으로만 수신). @return void */
  void on_judge_verdict(const mrs_interfaces::msg::JudgeVerdict::SharedPtr msg);

  /** @brief `/ladder/rung_event` 수신 콜백 (계측 관측). @return void */
  void on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg);

  mrs::LadderStateMachine ladder_; ///< R0->R5 상태기계

  std::vector<rclcpp::Subscription<mrs_interfaces::msg::EscalationReport>::SharedPtr>
    escalation_report_subs_;
  rclcpp::Subscription<mrs_interfaces::msg::JudgeVerdict>::SharedPtr judge_verdict_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_sub_;

  rclcpp::Client<mrs_interfaces::srv::PartialReplan>::SharedPtr partial_replan_client_;
  rclcpp::Client<mrs_interfaces::srv::ReassignRequest>::SharedPtr reassign_request_client_;

  int robot_count_{2}; ///< 파라미터 robot_count 기본값 (tracer bullet [0] = 2대)
};

} // namespace mrs
