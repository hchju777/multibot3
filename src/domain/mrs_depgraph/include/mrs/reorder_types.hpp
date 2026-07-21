#pragma once

/**
 * @file reorder_types.hpp
 * @brief IReorderR1(mrs_reorder_btpg)·IReorderR2(mrs_reorder_milp) 공유 값 타입.
 *
 * @note **배치 이력**: 계약 레지스트리 잠금본 `i_reorder.hpp` (L-22) 는 IReorderR1 과
 * IReorderR2 를 한 파일에 선언했다. architecture §2.3 은 `mrs_reorder_btpg` 와
 * `mrs_reorder_milp` 를 형제(엣지 없음)로 확정했으므로, 한쪽 포트 헤더가 다른 쪽을 include 하면
 * 그 형제 관계가 깨진다. 이 파일은 두 포트가 **공유하는 값 타입만** 양쪽이 이미 의존하는
 * `mrs_depgraph` 로 분리 배치한 것이다 — 필드·의미론은 원문(L-22)과 100% 동일하며 계약 내용
 * 변경이 아니다(scaffolder 배치 결정, `03_scaffold/scaffold.md` 기록).
 */

#include <cstdint>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 재조정 요청 (ReorderPassing.srv 요청과 1:1).
 */
struct ReorderRequest
{
  EventId event_id{0};                ///< 교란 상관 키
  std::vector<RobotId> scope_robots;  ///< 재조정 범위. 비면 전역
  std::uint32_t attempt_count{0};     ///< 이 tier 시도 횟수 (D-04 v2 게이팅 카운터)
  std::uint32_t base_plan_epoch{0};   ///< 대상 계획 세대
  double budget_s{1.0};               ///< 처리 예산 [s]. 초과 시 구 순서 유지
  // ⛔ slack_estimate 를 추가하지 말 것 — D-04 v2 의 btpg↛judge 독립성 결정을 되돌리는 변경이다.
};

/**
 * @brief 접수 결과 (ReorderPassing.srv 응답과 1:1).
 */
enum class ReorderAck : std::uint8_t
{
  ACCEPTED = 0,        ///< 접수. 결과는 릴리스·RungEvent 로 관측
  REJECTED_BUSY = 1,   ///< 동일 tier 작업 진행 중
  REJECTED_SCOPE = 2,  ///< 범위 로봇이 현재 그래프에 없음
  REJECTED_STALE = 3   ///< base_plan_epoch 불일치
};

/**
 * @brief 재조정 완료 통지 (계측·게이팅 카운터 갱신용).
 */
struct ReorderCompletion
{
  EventId event_id{0};             ///< 상관 키
  std::uint64_t reorder_job_id{0}; ///< 접수 식별자
  bool succeeded{false};           ///< true = 순서가 실제로 갱신됨
  double improvement_s{0.0};       ///< 예상 실행시간 개선폭 [s]. 양수 = 개선.
                                   ///< R1 의 이 값이 IGatingPolicy 의 유일한 입력이다
  double duration_s{0.0};          ///< 처리 소요 시간 [s]
};

} // namespace mrs
