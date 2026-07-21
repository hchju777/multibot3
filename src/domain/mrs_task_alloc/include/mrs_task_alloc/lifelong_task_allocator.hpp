#pragma once

/**
 * @file lifelong_task_allocator.hpp
 * @brief 분리형(decoupled) lifelong 작업할당 — 사다리 최상단 R4 담당 (D-02).
 *
 * core_algorithm: flow-based-ta-2025 컨셉의 min-cost-flow 재구현. **OR-tools 링크는
 * 로드맵 [4] 로 이연**(design-decision v5 build_policy, 사용자 결정) — 이 클래스는 지금
 * 시그니처만 고정하고 OR-tools 를 find_package 하지 않는다.
 *
 * 진동 억제 3종(D-02, 2026-07-20 사용자 결정): (i) 재할당 쿨다운, (ii) 이력 페널티,
 * (iii) 상한 초과 시 R5 승격. aging(기아 방지)은 쿨다운 중에도 누적된다(완주 조건 ④).
 */

#include <cstdint>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief ReassignRequest.srv 의 trigger_type 도메인 표현 (D-02 v2 확정 — 혼합 HARD+SOFT).
 */
enum class ReassignTrigger : std::uint8_t
{
  HARD = 0,                  ///< 즉시 회수·재배정 (EscalationReport 주행불가·고장·도달불가)
  SOFT_REPEATED_FAILURE = 1  ///< aging 큐 재배치 (R3 부분 재계획 N 회 연속 무해)
};

/**
 * @brief 재할당 처리 결과 — ReassignRequest.srv 응답 도메인 표현.
 */
enum class ReassignResult : std::uint8_t
{
  REASSIGNED = 0,      ///< 다른 로봇에 재배정 완료
  QUEUED_AGING = 1,    ///< aging 큐 재배치 (기아 방지는 유지)
  COOLDOWN_ACTIVE = 2, ///< 재할당 쿨다운 중
  ESCALATED_R5 = 3,    ///< 재할당 횟수 상한 초과 → 운영자 알림
  REJECTED = 4         ///< 알 수 없는 task_id 등 요청 오류
};

/**
 * @brief 분리형 lifelong 작업할당자.
 */
class LifelongTaskAllocator
{
public:
  LifelongTaskAllocator() = default;
  ~LifelongTaskAllocator() = default;

  /**
   * @brief 신규/미배정 작업을 로봇에 배정한다 (min-cost-flow 1회 해).
   * @param[in] task_id 작업 id.
   * @param[in] pickup_node 픽업 노드 id (물리 roadmap 기준).
   * @param[in] delivery_node 딜리버리 노드 id (물리 roadmap 기준).
   * @param[out] out_robot_id 배정된 로봇 id. 배정 불가면 ROBOT_ID_NONE.
   * @return bool 배정 성공 여부.
   */
  bool assign_task(
    std::uint64_t task_id, NodeId pickup_node, NodeId delivery_node, RobotId & out_robot_id);

  /**
   * @brief 상향 재할당 요청을 처리한다 (R4, D-02 v2 혼합 트리거 + 진동 억제 3종).
   * @param[in] robot_id 현재 이 작업을 맡은 로봇 id.
   * @param[in] task_id 대상 작업 id.
   * @param[in] trigger 트리거 종류 (HARD | SOFT_REPEATED_FAILURE).
   * @param[in] attempt_count 이 작업의 누적 재할당 시도 횟수.
   * @return ReassignResult 처리 결과.
   */
  ReassignResult handle_reassign_request(
    RobotId robot_id, std::uint64_t task_id, ReassignTrigger trigger, std::uint32_t attempt_count);

  /**
   * @brief aging 타이머를 갱신한다 (무기아 요건 — 완주 분해 조건 ④).
   * @param[in] now_s 현재 시각 [s].
   * @return void
   */
  void tick_aging(double now_s);
};

} // namespace mrs
