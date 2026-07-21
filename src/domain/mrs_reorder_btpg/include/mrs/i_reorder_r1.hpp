#pragma once

/**
 * @file i_reorder_r1.hpp
 * @brief R1(BTPG) 저비용 통행순서 유연화 포트 — 1차 방어선. 계약 L-22 v1.0.0.
 *
 * @note **배치 이력**: 원 잠금본 `i_reorder.hpp` 는 IReorderR1·IReorderR2 를 한 파일에 담았다.
 * `reorder_types.hpp`(공유 값 타입)는 형제 관계 보존을 위해 `mrs_depgraph` 로 분리했다
 * (03_scaffold/scaffold.md 참조). 이 파일의 내용(IReorderR1 본문)은 원문과 동일하다.
 *
 * 근거: D-04(2단 게이팅), 오케스트레이터 R-02 — "BTPG 라이선스 미결은 계약을 막지 않는다.
 * 포트는 원저 포팅이든 재구현이든 동일하다."
 *
 * Q-2/Q-3 바인딩: `ReorderPassing.srv` 와 **동일한 필드·의미론**을 갖는 프로세스 내 기본 경로다.
 * submit() 은 **비블로킹 접수**이며 결과를 반환하지 않는다 — R2 MILP 가 릴리스 콜백을 막지
 * 않게 하기 위함이다. 완료는 completion 콜백과 RungEvent 로 관측한다.
 */

#include <cstdint>
#include <functional>
#include <string>

#include "mrs/reorder_types.hpp"

namespace mrs
{

/**
 * @brief R1 BTPG 저비용 순서 유연화 포트 (1차 방어선).
 */
class IReorderR1
{
public:
  virtual ~IReorderR1() = default;

  /**
   * @brief 재조정을 접수한다. **비블로킹**.
   * @param[in] request 사건·범위·시도횟수·예산 (ReorderRequest).
   * @param[out] job_id 접수 식별자. ACCEPTED 일 때만 0 이 아닌 값이 설정된다.
   * @return ReorderAck 접수/거부 사유.
   */
  virtual ReorderAck submit(const ReorderRequest & request, std::uint64_t & job_id) noexcept = 0;

  /**
   * @brief 완료 콜백을 등록한다. 콜백은 워커 스레드에서 호출될 수 있다.
   * @param[in] callback 완료 통지 수신 함수 (ReorderCompletion 값 전달).
   * @return void
   */
  virtual void set_completion_callback(std::function<void(const ReorderCompletion &)> callback) = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "btpg_port" | "btpg_reimpl" | "btpg_max".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
