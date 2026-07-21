#pragma once

/**
 * @file metrics_collector.hpp
 * @brief §4.2 자체 계측기 — RungEvent(M1·M2·M5·M6·M8·M9)·QP 결과·백엔드 metrics tap 수집.
 *
 * F2 의 신규성 판정(R2 발동률 하한)과 F6 의 반례 실증(M6 순환 증인)이 이 수집기의 정확성에
 * 걸려 있다(RungEvent.msg 는 계약으로 잠김 — L-13). isaac 미부착 지표는 `pending_isaac` 로
 * 태깅되어 연구 검증 승격이 자동 차단된다(architecture §5.3, R-01 조건 3).
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 사다리 가로대 발동·전이 이벤트 1건 — RungEvent.msg 도메인 표현.
 */
struct RungEventRecord
{
  EventId event_id{0};              ///< 교란 상관 키
  Rung rung{Rung::R0_LOCAL_ABSORB}; ///< 발동 가로대
  std::uint8_t transition{0};       ///< ENTER=0/ABSORBED=1/ESCALATE=2/GUARD_REJECT=3/ESCAPE_HATCH=4
  std::uint8_t trigger_kind{0};     ///< HARD=0/SOFT=1
  std::vector<RobotId> affected_robots;  ///< 영향 집합 A (M6 삼중쌍 요소)
  std::uint64_t edge_set_fingerprint{0}; ///< 사용 엣지 집합 해시 (M6 삼중쌍 요소)
  double phi_hat_before_s{0.0};          ///< Phi_hat, 판정 직전 (M2)
  double phi_hat_after_s{0.0};           ///< Phi_hat, 판정/재계획 직후. 미산출 시 NaN
  std::uint32_t attempt_count{0}; ///< 이 event_id 의 이 가로대 시도 횟수 (D-04 게이팅 카운터)
  double duration_s{0.0}; ///< 처리 소요 시간 [s]
};

/**
 * @brief 계측 수집기 — 서버 전 노드가 발행하는 RungEvent/QP 결과/metrics tap 을 집계한다.
 */
class MetricsCollector
{
public:
  MetricsCollector() = default;
  ~MetricsCollector() = default;

  /**
   * @brief 가로대 이벤트를 기록한다 (M1·M5·M6·M8·M9 원천).
   * @param[in] record 이벤트 상세.
   * @return void
   */
  void on_rung_event(const RungEventRecord & record);

  /**
   * @brief L4 QP 해결 결과를 기록한다 (T1-R9 폴백 발동률, QP feasibility rate).
   * @param[in] robot_id 대상 로봇.
   * @param[in] solve_time_s QP 소요 시간 [s].
   * @param[in] used_fallback true = 인증 폴백 채택.
   * @return void
   */
  void on_qp_result(RobotId robot_id, double solve_time_s, bool used_fallback);

  /**
   * @brief 시뮬 백엔드 metrics tap 표본을 기록한다 (백엔드 고유 진실값).
   * @param[in] key 지표 키 (예: "collision_count", "min_separation_m").
   * @param[in] value 값.
   * @param[in] robot_id 로봇별 지표면 로봇 id, 전역이면 ROBOT_ID_NONE.
   * @param[in] pending_isaac true = 충실도 부족, 연구 검증 승격 금지 (§5.3).
   * @return void
   */
  void on_metric_sample(
    const std::string & key, double value, RobotId robot_id, bool pending_isaac);
};

} // namespace mrs
