#pragma once

/**
 * @file startup_check_node.hpp
 * @brief R-A2 기동 거부 검사 노드 — Δt_h(재계획 주기)가 시뮬 스텝 dt 의 정수배인지 확인한다.
 *
 * design-decision v5 지시로 **실제로 구현**하는 2건 중 1건(다른 1건은 mrs_ros_l4 의 R-05 —
 * `/plan_tick` 발행자 수 검사). 판정 로직 자체는 `mrs/startup_checks.hpp`(mrs_core_types,
 * ROS 무의존)의 `is_integer_multiple()` 를 그대로 쓴다 — 이 노드는 파라미터 조회 + 로깅 +
 * 종료코드 결정만 담당한다.
 *
 * 근거(architecture §1.7-B R-A2): 어긋나면 `/clock` 점프가 틱 경계와 맞지 않아 병합·중복이
 * 발생하고 "전 에이전트가 같은 틱에 재계획"이 시뮬에서 성립하지 않는다 — F5 가 정리 T1 을
 * 검증하지 못한다(통로 폭 검사기와 같은 취급, 전제 미성립 상태로 실험을 돌리지 않는다).
 */

#include <rclcpp/rclcpp.hpp>

namespace mrs
{

/**
 * @brief 1회성 기동 검사 노드 — spin 하지 않는다. `passed()` 로 결과를 조회한 뒤 종료한다.
 */
class StartupCheckNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 replan_period_s/sim_step_s 파라미터로 R-A2 검사를 즉시 수행한다.
   */
  StartupCheckNode();
  ~StartupCheckNode() override = default;

  /**
   * @brief 검사 통과 여부를 반환한다.
   * @return bool true = Δt_h 가 dt 의 정수배(허용오차 내). false = 위반, 기동 거부 대상.
   */
  bool passed() const noexcept { return passed_; }

private:
  bool passed_{false}; ///< R-A2 검사 결과
};

} // namespace mrs
