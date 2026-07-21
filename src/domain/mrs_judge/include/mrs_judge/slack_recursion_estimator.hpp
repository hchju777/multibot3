#pragma once

/**
 * @file slack_recursion_estimator.hpp
 * @brief ADG slack 재귀 추정 + G-eps 수용 가드 소비자 (D-06, theory T3 §4).
 *
 * 경계 규율(D-06): 이 클래스는 **R2↔R3 단일 경계에만** 배치된다. R1->R2 게이팅(D-04)은
 * `mrs_reorder_btpg` 의 시도횟수 카운터가 독립적으로 담당하며 이 패키지를 참조하지 않는다
 * (D-04 v2 독립성 — CI 검사 §2.4-1 이 반대 방향 include 를 금지한다).
 */

#include <memory>

#include "mrs/i_judge_policy.hpp"

namespace mrs
{

/**
 * @brief holistic-adg-monitoring-2025 Algorithm 1 컨셉 재구현 — R2↔R3 판정자.
 */
class SlackRecursionEstimator
{
public:
  /**
   * @brief 판정 정책을 주입하며 생성한다.
   * @param[in] policy 사용할 IJudgePolicy 구현체 (RuleSlack 기본).
   */
  explicit SlackRecursionEstimator(std::shared_ptr<IJudgePolicy> policy) : policy_(std::move(policy)) {}
  ~SlackRecursionEstimator() = default;

  /**
   * @brief 사건의 현재 slack 관측치를 추정한다 (ADG 재귀 슬랙 전파).
   * @param[in] event_id 교란 상관 키.
   * @return JudgeObservation completion_excess_s·phi_hat_s·직접 영향 집합.
   */
  JudgeObservation estimate(EventId event_id) const;

  /**
   * @brief R3 릴리스 여부를 G-eps 가드로 판정한다 (명제 T3.3, F6 on/off 비교축).
   * @param[in] phi_hat_old_s 구계획의 Phi_hat [s].
   * @param[in] phi_hat_new_s 신규 부분계획의 Phi_hat [s].
   * @param[in] consecutive_rejects 이 사건의 연속 거부 횟수.
   * @return GuardDecision 릴리스 수용 여부·탈출구 사용 여부.
   */
  GuardDecision check_guard(
    double phi_hat_old_s, double phi_hat_new_s, std::uint32_t consecutive_rejects) const;

private:
  std::shared_ptr<IJudgePolicy> policy_; ///< 판정 정책 (seam f)
};

} // namespace mrs
