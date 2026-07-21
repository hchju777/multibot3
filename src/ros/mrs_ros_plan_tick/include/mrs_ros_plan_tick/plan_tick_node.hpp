#pragma once

/**
 * @file plan_tick_node.hpp
 * @brief plan_tick_node — 동기 재계획 틱 `/plan_tick` 의 유일한 발행자 (R-05, architecture §1.7).
 *
 * 판정 근거(architecture §1.7-A): sadg_service(순수 이벤트 구동 자기모순)·sim_bridge(sim 누수
 * 금지)·ladder_orchestrator(D-08 재시작이 틱을 끊음) 전부 기각되고 전용 노드로 확정됐다.
 * **도메인 패키지에 의존하지 않는다** — 이 패키지의 CI 통과 기준(architecture §2.2, §2.4).
 *
 * 틱의 값 자체(R-A1 tick_seq = floor((t-t0)/Δt_h))는 이론상 순수 산술이지만, 오케스트레이터
 * 지시(mrs-implement Phase 4)의 "알맹이는 쓰지 마라" 범위에 이 노드의 틱 계산 로직도 포함된다
 * (예외 3건에 미포함) — Phase 5(ros-builder)가 채운다.
 */

#include <cstdint>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/plan_tick.hpp"

namespace mrs
{

/**
 * @brief `/plan_tick` 발행 전용 노드.
 */
class PlanTickNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·발행자·타이머를 구성한다.
   */
  PlanTickNode();
  ~PlanTickNode() override = default;

private:
  /**
   * @brief 타이머 콜백 — 매 replan_period_s 마다 PlanTick 을 발행한다.
   * @return void
   * @note 스텁 단계: try/catch 로 감싸 예외가 노드를 죽이지 않게 하되(CLAUDE.md 규율 2),
   *       R-A1 틱 산식은 Phase 5(ros-builder) 대상이다.
   */
  void on_timer();

  rclcpp::Publisher<mrs_interfaces::msg::PlanTick>::SharedPtr tick_pub_; ///< `/plan_tick` 발행자
  rclcpp::TimerBase::SharedPtr timer_;                                  ///< 주기 타이머
  double replan_period_s_{0.1};                                        ///< Δt_h [s] (T1 §6.1 기본값)
};

} // namespace mrs
