#pragma once

/**
 * @file milp_job_tracker.hpp
 * @brief R2 SADG MILP 재조정 작업 관리 — sadg_milp_module 책임 (D-04, seam d 소비자).
 *
 * Q-3 스레드 규율: solve() 는 블로킹이므로 이 클래스는 워커 스레드 실행을 전제하고, 릴리스
 * 콜백을 절대 막지 않는다. 알고리즘 본문(스레드 풀·취소 전파)은 Phase 5 대상.
 */

#include <cstdint>
#include <memory>

#include "mrs/i_milp_solver.hpp"

namespace mrs
{

/**
 * @brief IMilpSolver(seam d) 를 워커 스레드에서 구동하는 R2 작업 관리자.
 */
class MilpJobTracker
{
public:
  /**
   * @brief MILP 백엔드를 주입하며 생성한다.
   * @param[in] solver 사용할 IMilpSolver 구현체 (CBC 1순위).
   */
  explicit MilpJobTracker(std::shared_ptr<IMilpSolver> solver) : solver_(std::move(solver))
  {
  }
  ~MilpJobTracker() = default;

  /**
   * @brief 스위칭 MILP 작업을 제출한다. 실제 solve() 는 워커 스레드에서 실행된다.
   * @param[in] model 의존성 그래프·스위칭 후보·예산.
   * @return MilpResult 상태·스위칭 결정·gap·소요시간.
   */
  MilpResult solve_switching(const MilpModel & model);

  /**
   * @brief 진행 중인 모든 MILP 작업을 취소한다.
   * @return void
   */
  void cancel_all();

private:
  std::shared_ptr<IMilpSolver> solver_; ///< MILP 백엔드 (seam d)
};

} // namespace mrs
