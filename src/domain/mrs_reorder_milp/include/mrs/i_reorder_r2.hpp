#pragma once

/**
 * @file i_reorder_r2.hpp
 * @brief R2(SADG MILP) 통행순서 재조정 포트 — 2차 온디맨드. 계약 L-22 v1.0.0.
 *
 * @note **배치 이력**: 원 잠금본 `i_reorder.hpp` 는 IReorderR1·IReorderR2 를 한 파일에 담았다.
 * `reorder_types.hpp`(공유 값 타입)는 형제 관계 보존을 위해 `mrs_depgraph` 로 분리했다
 * (03_scaffold/scaffold.md 참조). 이 파일의 내용(IReorderR2 본문)은 원문과 동일하다.
 *
 * @note 내부적으로 IMilpSolver 를 워커 스레드에서 사용한다. submit() 은 블로킹하지 않는다
 * (Q-3: MILP 를 릴리스 경로에서 격리).
 */

#include <cstdint>
#include <functional>
#include <string>

#include "mrs/reorder_types.hpp"

namespace mrs
{

/**
 * @brief R2 SADG MILP 재조정 포트 (2차 온디맨드).
 */
class IReorderR2
{
public:
  virtual ~IReorderR2() = default;

  /**
   * @brief 재조정을 접수한다. **비블로킹** (Q-3: MILP 를 릴리스 경로에서 격리).
   * @param[in] request 사건·범위·시도횟수·예산 (ReorderRequest).
   * @param[out] job_id 접수 식별자. ACCEPTED 일 때만 0 이 아닌 값이 설정된다.
   * @return ReorderAck 접수/거부 사유.
   */
  virtual ReorderAck submit(const ReorderRequest & request, std::uint64_t & job_id) noexcept = 0;

  /**
   * @brief 완료 콜백을 등록한다.
   * @param[in] callback 완료 통지 수신 함수 (ReorderCompletion 값 전달).
   * @return void
   */
  virtual void set_completion_callback(std::function<void(const ReorderCompletion &)> callback) = 0;

  /**
   * @brief 진행 중 작업을 취소 요청한다.
   * @param[in] job_id 취소할 접수 식별자.
   * @return bool 취소 요청 접수 여부(실제 중단은 비동기).
   */
  virtual bool cancel(std::uint64_t job_id) noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "sadg_milp" | "stpg_accel".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
