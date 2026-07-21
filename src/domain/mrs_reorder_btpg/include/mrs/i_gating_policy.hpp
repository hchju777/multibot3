#pragma once

/**
 * @file i_gating_policy.hpp
 * @brief R1 → R2 게이팅 정책 포트 (seam e, D-04 v2). 계약 v1.0.0.
 *
 * 확정서 v2 판정식: "BTPG 후처리의 예상 실행시간 개선폭이 0 이하인 시도가 N회 연속이면 R2 발동."
 * N 은 ROS2 param 이며 F2 스윕 대상이다.
 *
 * ⛔ **금지 — judge 참조**: 이 포트의 관측 구조체에 slack 추정치를 넣지 마라. btpg_module 과
 * judge_module 의 완전 독립이 D-04 v2 의 핵심 결정이며(F2·F3 교락 방지, D-06 의 'R2↔R3 단일
 * 경계 한정' 보존), 패키지 의존 그래프(mrs_reorder_btpg ↛ mrs_judge)로도 강제된다
 * (architecture §2.4 검사 1). slack 2차 확인 확장은 F2 결과 후 **계약 변경 게이트**를 거친다.
 *
 * 구현체: AttemptCount(기본) / AlwaysR2(vanilla SADG baseline) / NeverR2(BTPG-only baseline).
 * baseline 2종이 새 코드가 아니라 전략 교체가 되도록 하는 것이 이 포트의 목적이다(D-09).
 */

#include <cstdint>
#include <string>

namespace mrs
{

/**
 * @brief 게이팅 관측 — **btpg_module 내부 카운터만** 담는다.
 */
struct GatingObservation
{
  std::uint32_t attempt_count{0};           ///< 이 사건에 대한 R1 시도 누적 횟수
  std::uint32_t consecutive_nonpositive{0}; ///< 개선폭 <= 0 이 연속된 횟수
  double last_improvement_s{0.0}; ///< 직전 R1 시도의 예상 실행시간 개선폭 [s].
                                  ///< 양수 = 개선. C-018 이 관측된 척도 그 자체다
};

/**
 * @brief 게이팅 판정.
 */
struct GatingDecision
{
  bool invoke_r2{false};        ///< true = R2 SADG MILP 온디맨드 호출
  std::uint32_t threshold_n{0}; ///< 판정에 적용된 N (계측·재현 기록용)
};

/**
 * @brief R1 → R2 게이팅 정책 포트.
 */
class IGatingPolicy
{
public:
  virtual ~IGatingPolicy() = default;

  /**
   * @brief R1 흡수 실패 여부를 판정해 R2 호출 여부를 결정한다.
   * @param[in] obs R1 시도 횟수·개선폭 관측 (GatingObservation). slack 은 포함하지 않는다.
   * @return GatingDecision R2 호출 여부와 적용 임계.
   */
  virtual GatingDecision evaluate(const GatingObservation & obs) noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "attempt_count" | "always_r2" | "never_r2".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
