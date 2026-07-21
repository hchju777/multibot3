#pragma once

/**
 * @file canned_solver.hpp
 * @brief [0a] tracer bullet 용 **고정 경로** 솔버 — `IPathSolver`(seam a, 계약 L-20 v2.0.0) 더미.
 *
 * architecture §7-2: "`pp_service` 더미 — 고정 경로 반환(솔버 미링크). `IPathSolver` 의
 * `CannedSolver` 구현". **경로계획 알고리즘이 아니다** — 탐색·충돌회피·최적화를 하지 않고,
 * 주입된 왕복 라우트를 시각만 붙여 그대로 돌려준다. 알맹이(LLLG/RHCR)는 로드맵 [1]~[2] 의 일이다.
 *
 * ## 왜 `mrs_planning` 이 아니라 이 패키지에 두는가
 * 실제 솔버 구현체(LLLG/RHCR/PIBT)의 자리는 `mrs_planning`(architecture §2.2)이지만, 이 클래스는
 * **[0a] 배관 실측 전용 더미**이고 [1] 착수 시 폐기·대체된다. 도메인 패키지에 두면 다른 빌더가
 * 같은 파일을 동시에 건드리게 되고, 더미가 도메인 자산으로 오인될 위험도 있다.
 * ⇒ **[1] 에서 실제 솔버가 들어오면 이 파일은 삭제 대상**이다.
 *
 * ## 뷰 규약 (계약 §0.1 V1)
 * 입출력 노드 id 는 전부 **`UniformNodeId`(균일 뷰)** 다. [0a] 는 세분화(subdivision)가 없어
 * 물리 roadmap 과 균일 뷰가 1:1 이지만, **그 사실이 두 뷰를 같은 것으로 취급할 근거는 아니다** —
 * 라우트는 MapRegistry 의 `GetUniformView` 응답(= 균일 뷰 id 공간)에서만 만들어지며,
 * 물리 뷰 id 를 균일 뷰로 재해석하는 변환은 이 클래스도 호출 노드도 하지 않는다(C4).
 *
 * 예외 규율(CLAUDE.md 규율 2, `i_path_solver.hpp`): 이 구현체는 예외를 밖으로 던지지 않는다.
 * 내부 실패는 잡아서 `SolverStatus` 로 보고한다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/i_path_solver.hpp"
#include "mrs/view_ids.hpp"

namespace mrs::ros_pp
{

/**
 * @brief 로봇 1대분 고정 왕복 라우트 — 균일 뷰에서 **인접한** 노드 2점.
 *
 * 두 점이 인접해야 하는 이유: L3 가 이 경로를 세그먼트로 잘라 `ExecutionWindow` 를 만들고,
 * 계약 불변식 W2 가 "각 세그먼트 쌍은 균일 뷰에서 인접"을 요구한다. 그래서 라우트는 임의의
 * 노드 쌍이 아니라 **MapRegistry 가 응답한 엣지**에서만 만들어진다.
 */
struct CannedRoute
{
  RobotId robot_id{ROBOT_ID_NONE}; ///< 이 라우트를 쓰는 로봇 id
  UniformNodeId node_a{};          ///< 왕복 끝점 A. 뷰 **UNIFORM**. 기본값은 센티넬
  UniformNodeId node_b{};          ///< 왕복 끝점 B. 뷰 **UNIFORM**. A 와 균일 뷰에서 인접해야 한다
};

/**
 * @brief 주입된 고정 라우트를 시각만 붙여 반환하는 `IPathSolver` 더미 구현.
 */
class CannedSolver final : public IPathSolver
{
public:
  /**
   * @brief 시각 간격과 왕복 횟수를 지정해 생성한다. 라우트는 @ref set_routes 로 나중에 주입한다.
   * @param[in] segment_duration_s 인접 방문 사이의 계획 시각 간격 [s]. 자료형 `double`.
   *            양수·유한이어야 하며, 위반 시 @ref solve 가 `INTERNAL_ERROR` 를 반환한다.
   * @param[in] lap_count 왕복 횟수. 자료형 `std::uint32_t`. 방문열 길이는 `2 * lap_count + 1` 이다.
   *            0 이면 방문이 1개(제자리)뿐인 경로가 되어 세그먼트가 생기지 않는다.
   */
  CannedSolver(double segment_duration_s, std::uint32_t lap_count);

  ~CannedSolver() override = default;

  /**
   * @brief 고정 라우트 집합과 그 라우트가 속한 뷰 스코프를 주입한다.
   *
   * 스코프를 함께 받는 이유: `IPathSolver` 계약이 "구현체는 입력 그래프가 요청 스코프의 것인지
   * 확인하고 아니면 `STALE_VERSION` 을 반환한다"고 규정하는데, 더미는 그래프를 들지 않으므로
   * **라우트를 만든 스코프**가 그 확인의 유일한 근거다.
   *
   * @param[in] routes 로봇별 고정 왕복 라우트. 자료형 `std::vector<CannedRoute>`.
   *            빈 벡터도 유효하며, 이 경우 @ref solve 는 `INFEASIBLE` 을 반환한다.
   * @param[in] routes_scope 라우트 노드 id 가 유효한 뷰 스코프. 자료형 `mrs::ViewScope`.
   *            `view_kind` 는 `UNIFORM` 이어야 한다.
   * @return void
   */
  void set_routes(std::vector<CannedRoute> routes, const ViewScope & routes_scope);

  /**
   * @brief 계획 방문열의 기준 시각(첫 방문의 도착 시각)을 설정한다.
   *
   * `PathSolverInput` 에 시각 필드가 없어(계약 L-20) 더미가 시각을 만들 근거가 없다 —
   * 호출 노드가 시뮬 시계 절대시각을 넣어 준다.
   *
   * @param[in] plan_start_time_s 첫 방문의 도착 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void set_plan_start_time_s(double plan_start_time_s) noexcept;

  /**
   * @brief 주입된 고정 라우트로 경로를 만들어 반환한다. 탐색·최적화를 하지 않는다.
   *
   * 판정 순서: 스코프 대조 → 라우트 보유 → 요청 항목별 라우트 조회 → 방문열 생성.
   * 요청 항목 중 하나라도 대응 라우트가 없으면 **전체를 `INFEASIBLE` 로 실패**시킨다 —
   * 부분해를 `SUCCESS` 로 내보내면 L3 가 일부 로봇의 창을 영원히 기다린다.
   *
   * @param[in] input 계획 요청. 자료형 `mrs::PathSolverInput`. `entries`·`view_scope` 만 쓰며
   *            `uniform_view`·`frozen_orders`·`horizon_w`·`budget_s`·`allow_warm_start` 는 무시한다
   *            (더미이므로 예산을 소비하지 않는다).
   * @return `mrs::PathSolverOutput` — `status` 는 SUCCESS / STALE_VERSION(스코프 불일치) /
   *         INFEASIBLE(라우트 없음·빈 요청) / INTERNAL_ERROR(파라미터 위반·예외 포착).
   *         `paths` 는 SUCCESS 일 때만 유효하고, `planning_time_s` 는 계산이 없어 항상 0 이다.
   */
  PathSolverOutput solve(const PathSolverInput & input) noexcept override;

  /**
   * @brief 구현체 식별 이름.
   * @return `std::string` — 항상 `"canned"`. 로그·계측·`PlanPaths.srv` 응답의 `solver_name` 에 실린다.
   */
  std::string name() const noexcept override;

  /**
   * @brief 웜스타트 지원 여부.
   * @return `bool` — 항상 false. 고정 경로이므로 이전 해를 재사용할 여지가 없다.
   */
  bool supports_warm_start() const noexcept override;

  /**
   * @brief 현재 주입된 라우트 수를 조회한다 (로그·기동 점검용).
   * @return `std::size_t` — 라우트 개수. 0 이면 아직 MapRegistry 응답을 받지 못한 상태다.
   */
  [[nodiscard]] std::size_t route_count() const noexcept;

private:
  /**
   * @brief @ref solve 의 전제조건(파라미터·스코프·라우트/요청 존재)을 검사한다.
   * @param[in] input 계획 요청. 자료형 `mrs::PathSolverInput`.
   * @param[out] out_message 실패 사유 진단 문자열. 자료형 `const char *`. 정적 리터럴을 가리키며
   *             성공 시 빈 문자열이 들어간다. 소유권을 넘기지 않는다.
   * @return `mrs::SolverStatus` — 전제조건을 만족하면 `SUCCESS`, 아니면 그대로 반환할 실패 상태.
   */
  [[nodiscard]] SolverStatus check_preconditions(
    const PathSolverInput & input, const char *& out_message) const noexcept;

  std::vector<CannedRoute> routes_;  ///< 로봇별 고정 왕복 라우트
  ViewScope routes_scope_{};         ///< 라우트가 속한 뷰 스코프 (STALE_VERSION 판정 근거)
  double segment_duration_s_{0.0};   ///< 인접 방문 간 시각 간격 [s]
  std::uint32_t lap_count_{0};       ///< 왕복 횟수
  double plan_start_time_s_{0.0};    ///< 첫 방문 도착 시각 [s], 시뮬 시계 절대시각
};

} // namespace mrs::ros_pp
