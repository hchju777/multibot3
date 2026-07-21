#pragma once

/**
 * @file ladder_state_machine.hpp
 * @brief R0->R1->R2->R3->R4(->R5) 점증 에스컬레이션 상태기계 (D-08).
 *
 * 각 로봇/사건은 자기 층에서 흡수 가능한 교란만 처리하고, 흡수 실패 시 상위로 반환한다.
 * ⛔ 이 클래스는 `mrs_judge`·`mrs_reorder_btpg` 를 include 하지 않는다(architecture §2.3 —
 * "상태기계가 판정 로직을 알면 F2·F3·F6 이 서로 오염된다"). 판정 결과는 JudgeVerdict/RungEvent
 * **값**으로만 받는다. SYNC_LOST 는 R3/R4 로 라우팅하지 않는다(R-06 구속, 안전 정지·동기 복구).
 */

#include <cstdint>
#include <unordered_map>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 사다리 상태기계.
 */
class LadderStateMachine
{
public:
  LadderStateMachine() = default;
  ~LadderStateMachine() = default;

  /**
   * @brief 하향(L4->서버) 에스컬레이션을 수신해 라우팅한다 (이원 트리거 — HARD/SOFT).
   * @param[in] robot_id 보고 로봇.
   * @param[in] reason 에스컬레이션 사유. SYNC_LOST 는 R3/R4 금지, 안전 정지·동기 복구 경로(R-06).
   * @param[in] event_id 교란 상관 키.
   * @return void
   */
  void on_escalation(RobotId robot_id, EscalationReason reason, EventId event_id);

  /**
   * @brief judge_module 의 R2↔R3 승격 판정을 반영한다 (D-06 경계, 값으로만 수신).
   * @param[in] event_id 교란 상관 키.
   * @param[in] promote_r3 true = R3 부분 재계획 승격.
   * @return void
   */
  void on_judge_verdict(EventId event_id, bool promote_r3);

  /**
   * @brief 로봇(사건)의 현재 가로대를 조회한다.
   * @param[in] robot_id 대상 로봇.
   * @return Rung 현재 가로대. 사건 없음이면 R0_LOCAL_ABSORB.
   */
  Rung current_rung(RobotId robot_id) const;

private:
  std::unordered_map<RobotId, Rung> rung_by_robot_; ///< 로봇별 현재 가로대
};

} // namespace mrs
