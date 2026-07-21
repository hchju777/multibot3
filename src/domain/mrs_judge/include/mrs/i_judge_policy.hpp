#pragma once

/**
 * @file i_judge_policy.hpp
 * @brief R2 ↔ R3 판정자 포트 + G-eps 수용 가드 (seam f, D-06 · theory T3). 계약 v1.0.0.
 *
 * 근거: D-06 — 판정자는 **R2↔R3 단일 경계에만** 배치한다(근거 카드 2편이 그 경계에서만
 * 검증됐으므로 전 경계 확장은 '검증할 가설'로 강등, C6-2). R1→R2 는 IGatingPolicy 가,
 * R3→R4 는 D-02 트리거가 담당하며 이 포트는 그것들을 알지 못한다.
 *
 * G-eps 가드(theory 명제 T3.3, T3-R1): 무조건 사다리 종료성은 **거짓**이다(명제 T3.2 반례).
 * R3 릴리스를 Phi_hat 단조 감소로 게이팅해야 릴리스 횟수가 유계가 된다. 가드는 on/off 토글
 * 가능해야 하며(F6 비교축, 확정서 D-08 v3), 탈출구가 없으면 영구 폐쇄 통로에서 필요한
 * 재계획까지 막는다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 판정자 관측 (ADG slack 재귀 추정 결과).
 */
struct JudgeObservation
{
  EventId event_id{0};             ///< 교란 상관 키
  double completion_excess_s{0.0}; ///< max(예상 완료시각 − 계획 완료시각) [s]
  double threshold_s{2.0}; ///< 소프트 트리거 임계 [s]. ROS2 param, 초기값 2.0(원저)
  double phi_hat_s{0.0};   ///< Phi_hat = sum_{i in D} t_hat_completion [s]
  std::vector<RobotId> direct_affected; ///< slack 전파가 산출한 직접 영향 집합 D
};

/**
 * @brief 판정 결과.
 */
struct JudgeDecision
{
  bool promote_r3{false};               ///< true = R3 부분 재계획 승격
  std::vector<RobotId> direct_affected; ///< 직접 영향 집합 D (확장 A 는 AffectedSetClosure 소관)
  double phi_hat_s{0.0};                ///< 판정 시점 Phi_hat [s] — 가드 비교 기준값
};

/**
 * @brief G-eps 가드 판정 결과 (theory T3 §4).
 */
struct GuardDecision
{
  bool accept_release{false}; ///< true = 신규 부분계획을 릴리스, false = 폐기하고 R2 에 머문다
  bool escape_hatch_used{false}; ///< true = 탈출구 발동(도달불가 또는 연속 거부 N_reject 초과)
  double phi_hat_new_s{0.0}; ///< 신규 계획의 Phi_hat [s] (계측 M2)
};

/**
 * @brief R2↔R3 판정자 포트.
 */
class IJudgePolicy
{
public:
  virtual ~IJudgePolicy() = default;

  /**
   * @brief R3 승격 여부를 판정한다.
   * @param[in] obs slack 추정 결과와 임계 (JudgeObservation).
   * @return JudgeDecision 승격 여부·직접 영향 집합·Phi_hat.
   */
  virtual JudgeDecision evaluate(const JudgeObservation & obs) noexcept = 0;

  /**
   * @brief G-eps 수용 가드 — 부분 재계획 결과를 릴리스할지 판정한다.
   * @param[in] phi_hat_old_s 구계획의 Phi_hat [s].
   * @param[in] phi_hat_new_s 신규 부분계획의 Phi_hat [s]. 도달 불가면 무한대.
   * @param[in] consecutive_rejects 이 사건에서 연속 거부된 횟수 (탈출구 판정 입력).
   * @return GuardDecision 릴리스 수용 여부와 탈출구 사용 여부.
   *         가드가 off(param) 이면 항상 accept_release = true 를 반환해야 한다.
   */
  virtual GuardDecision check_release_guard(
    double phi_hat_old_s, double phi_hat_new_s, std::uint32_t consecutive_rejects) noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "rule_slack" | "always_replan" | "never_replan".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
