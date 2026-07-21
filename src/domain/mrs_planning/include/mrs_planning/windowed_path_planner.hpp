#pragma once

/**
 * @file windowed_path_planner.hpp
 * @brief L2 windowed-lifelong 경로계획 운용 로직 (D-03). seam a(IPathSolver) 소비자.
 *
 * 호출 단위 완전성(해가 존재하면 찾음)은 IPathSolver 구현체(LLLG 1순위)의 책임이며, 이 클래스는
 * 운용 구조(예산·웜스타트·부분 재계획 접합 요청)만 담당한다 — "운용 구조와 솔버를 분리"(D-03)의
 * 코드 표현. 알고리즘 본문은 Phase 5(coordination-builder)가 채운다.
 */

#include <memory>

#include "mrs/i_path_solver.hpp"

namespace mrs
{

/**
 * @brief L2 경로계획 운용자 — IPathSolver 를 교체 가능 컴포넌트로 소비한다.
 */
class WindowedPathPlanner
{
public:
  /**
   * @brief 솔버 플러그인을 주입하며 생성한다.
   * @param[in] solver 사용할 IPathSolver 구현체. nullptr 이면 plan() 호출 시 실패 보고.
   */
  explicit WindowedPathPlanner(std::shared_ptr<IPathSolver> solver) : solver_(std::move(solver)) {}
  ~WindowedPathPlanner() = default;

  /**
   * @brief 런타임에 솔버 구현체를 교체한다 (seam a 교체 지점 — baseline·ablation 스위칭).
   * @param[in] solver 새 IPathSolver 구현체.
   * @return void
   */
  void set_solver(std::shared_ptr<IPathSolver> solver) noexcept { solver_ = std::move(solver); }

  /**
   * @brief 현재 주입된 솔버를 조회한다.
   * @return IPathSolver* 관측 포인터. 미주입이면 nullptr.
   */
  IPathSolver * solver() const noexcept { return solver_.get(); }

  /**
   * @brief 전역/신규 경로계획을 수행한다 (PlanPaths.srv 대응).
   * @param[in] input 계획 요청(그래프·시작/목표·예산·고정 순서).
   * @return PathSolverOutput 상태·경로·소요시간.
   */
  PathSolverOutput plan(const PathSolverInput & input);

  /**
   * @brief R3 부분 재계획을 수행한다 (PartialReplan.srv 대응, D-03 v3 동적 커밋 경계).
   * @param[in] input 계획 요청(영향 로봇 부분집합만 entries 로 담아 전달).
   * @param[in] commit_boundary 정의 T2.1 의 커밋 시작 상태 (beta_i, t_hat, theta_hat).
   * @param[in] frozen_orders 릴리스되어 뒤집을 수 없는 교차 순서 (T2 조건 (C3) 강제).
   * @return PathSolverOutput 상태·경로(첫 visit == commit_boundary.boundary_node)·소요시간.
   */
  PathSolverOutput plan_partial_replan(
    const PathSolverInput & input, const CommitState & commit_boundary,
    const std::vector<FrozenOrder> & frozen_orders);

  /**
   * @brief 계획 예산을 조회한다 (확정서 realtime_requirement <= 1 s/스텝).
   * @return double 현재 설정된 예산 [s].
   */
  double budget_s() const noexcept { return budget_s_; }

private:
  std::shared_ptr<IPathSolver> solver_; ///< 교체 가능 솔버 플러그인 (seam a)
  double budget_s_{1.0};                ///< 계획 예산 [s]. [0a] 실측 전 문헌치
};

} // namespace mrs
