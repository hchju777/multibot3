#pragma once

/**
 * @file btpg_attempt_tracker.hpp
 * @brief R1 시도횟수/개선폭 게이팅 카운터 (D-04 v2 확정 척도) — btpg_module 내부 상태.
 *
 * ⛔ 이 클래스는 slack·judge 를 참조하지 않는다(D-04 v2 독립성, C3). CI 검사(§2.4-1)가
 * `mrs_judge` include 를 금지한다. IGatingPolicy 는 이 카운터가 산출한 GatingObservation 만
 * 입력으로 받는다.
 */

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "mrs/contract_types.hpp"
#include "mrs/i_gating_policy.hpp"

namespace mrs
{

/**
 * @brief BTPG(R1) 시도 이력을 사건(event_id)별로 추적하고 게이팅 판정을 위임한다.
 */
class BtpgAttemptTracker
{
public:
  /**
   * @brief 게이팅 정책을 주입하며 생성한다.
   * @param[in] policy 사용할 IGatingPolicy 구현체 (AttemptCount 기본 / AlwaysR2 / NeverR2).
   */
  explicit BtpgAttemptTracker(std::shared_ptr<IGatingPolicy> policy) : policy_(std::move(policy))
  {
  }
  ~BtpgAttemptTracker() = default;

  /**
   * @brief R1 시도 1건의 결과(개선폭)를 기록한다.
   * @param[in] event_id 교란 상관 키.
   * @param[in] improvement_s 예상 실행시간 개선폭 [s]. 양수 = 개선(C-018 관측 척도).
   * @return void
   */
  void record_attempt(EventId event_id, double improvement_s);

  /**
   * @brief 사건에 대해 R2 를 호출할지 판정한다.
   * @param[in] event_id 교란 상관 키.
   * @return GatingDecision R2 호출 여부와 적용 임계값(threshold_n).
   */
  GatingDecision evaluate_gating(EventId event_id);

private:
  std::shared_ptr<IGatingPolicy> policy_;                       ///< 게이팅 정책 (seam e)
  std::unordered_map<EventId, GatingObservation> observations_; ///< 사건별 누적 관측
};

} // namespace mrs
