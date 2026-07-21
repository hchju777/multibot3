#pragma once

/**
 * @file startup_checks.hpp
 * @brief 기동 거부 검사의 순수 판정 로직 (design-decision v5 지시 — 지금 실제로 구현하는 2건 중 2건).
 *
 * ROS 그래프 API(rclcpp::Node::count_publishers 등)에 대한 의존 없이 **순수 함수**로 둔다 —
 * (i) `mrs_bringup`(R-A2: Δt_h 가 시뮬 스텝 dt 의 정수배인지)와 (ii) `mrs_ros_l4`(R-05:
 * `/plan_tick` 발행자 수가 그래프 API 로 조회된 뒤 그 카운트를 판정하는 부분)가 **같은 판정
 * 로직**을 공유하도록 여기 둔다. 두 노드 모두 도메인 계층(§2.3 core_types → {task_alloc,
 * ladder, sim_abstraction, metrics, ...})을 참조할 수 있으므로 순환·역행 의존 없이 재사용된다.
 *
 * ROS 그래프를 실제로 조회하는 부분(`node->count_publishers("/plan_tick")`)은 이 헤더의
 * 소관이 아니다 — rclcpp 를 끌어들이면 C1(ROS 무의존)을 깨기 때문이며, 각 소비 노드
 * (`mrs_ros_l4`)가 조회 후 이 헤더의 순수 함수로 판정만 위임한다.
 */

#include <cmath>
#include <cstddef>

namespace mrs
{

/**
 * @brief 정수배 판정의 부동소수 반올림 허용 상대오차.
 * @note 기본 인자로 두면 호출자가 조용히 다른 값을 넘길 수 있는데, 두 소비처가 **같은 판정
 *       로직**을 공유하는 것이 이 헤더의 존재 이유다. 상수로 고정한다.
 */
constexpr double STEP_RATIO_RELATIVE_TOLERANCE = 1e-6;

/**
 * @brief Δt_h(재계획 주기)가 시뮬 스텝 dt 의 정수배인지 검사한다 (R-A2, architecture §1.7-B).
 *
 * 근거: 어긋나면 `/clock` 점프가 틱 경계와 맞지 않아 병합·중복이 발생하고 "전 에이전트가
 * 같은 틱에 재계획"이 시뮬에서 성립하지 않는다 — F5 가 정리 T1 을 검증하지 못한다.
 * `mrs_bringup` 은 이 함수가 false 를 반환하면 기동을 거부한다(통로 폭 검사기와 같은 취급).
 *
 * @param[in] replan_period_s 재계획 주기 Δt_h [s]. 0 이하이면 항상 false.
 * @param[in] sim_step_s 시뮬 스텝 dt [s]. 0 이하이면 항상 false.
 * @param[in] relative_tolerance 부동소수 반올림 허용 상대오차. 자료형 `double`,
 *            기본값 @ref STEP_RATIO_RELATIVE_TOLERANCE.
 * @return `bool` — **true = 통과**(정수배, 허용오차 내). false = 위반이므로 기동을 거부해야 한다.
 */
inline bool is_integer_multiple(
  double replan_period_s,
  double sim_step_s,
  double relative_tolerance = STEP_RATIO_RELATIVE_TOLERANCE) noexcept
{
  if (replan_period_s <= 0.0 || sim_step_s <= 0.0)
  {
    return false;
  }
  const double ratio = replan_period_s / sim_step_s;
  const double nearest_integer = std::round(ratio);
  if (nearest_integer < 1.0)
  {
    return false;
  }
  const double residual = std::fabs(ratio - nearest_integer);
  return residual <= relative_tolerance * nearest_integer;
}

/**
 * @brief `/plan_tick` 발행자 수가 단일 발행자 강제(계약 L-15)를 만족하는지 판정한다 (R-05).
 *
 * 근거: 발행자가 2 개 이상이면 로봇마다 다른 스텝 지수 h 를 배포받을 수 있어 (A1) 지수 합의가
 * 조용히 깨진다(T4 §3.2) — 조용한 위반은 F5 결과를 무효화한다. `mrs_ros_l4` 는 이 함수가
 * false 를 반환하면 기동을 거부한다.
 *
 * @note 반환 극성은 @ref is_integer_multiple 과 **의도적으로 동일**하다(둘 다 "통과 = true").
 *       같은 헤더의 두 기동 게이트가 서로 반대 극성이면 호출자가 한쪽을 뒤집어 쓰기 쉽고,
 *       그러면 기동 게이트가 조용히 무력화된다 — 두 검사 모두 조용한 위반을 막으려고 만든 것이다.
 *
 * @param[in] publisher_count 그래프 API 로 조회한 `/plan_tick` 발행자 수. 자료형 `std::size_t`.
 * @return `bool` — **true = 통과**(0 또는 1 개). false = 위반(2 개 이상)이므로 기동을 거부해야 한다.
 */
inline bool is_single_publisher_ok(std::size_t publisher_count) noexcept
{
  return publisher_count < 2;
}

} // namespace mrs
