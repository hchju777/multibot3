#pragma once

/**
 * @file i_milp_solver.hpp
 * @brief R2 SADG 스위칭 MILP 백엔드 포트 (seam d, D-04). 계약 v1.0.0.
 *
 * 구현체: CBC(C++ coinor-libcbc-dev, 1순위 — impl-feasibility §2 확정) / HiGHS(대안) /
 *         mip(Python, 원저 대조용 프로세스 어댑터) / Gurobi(성능 부족 실측 시, 비컨테이너 조건).
 *
 * 스레드 규율(Q-3): 이 solve() 는 **수백 ms~초 단위로 블로킹**될 수 있다. 반드시 릴리스
 * 콜백과 분리된 워커 스레드에서 호출한다. 릴리스 경로가 여기 막히면 D-05 창 소비율 지표가
 * 오염된다. 취소는 budget_s 예산과 cancel() 로만 한다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 스위칭 가능 의존성 엣지 1건 (SADG Type-2 엣지의 MILP 결정변수).
 */
struct SwitchableEdge
{
  std::uint32_t edge_index{0};                 ///< 의존성 그래프 내 엣지 인덱스
  RobotId first_robot_id{ROBOT_ID_NONE};       ///< 현재 순서에서 먼저 가는 로봇
  RobotId second_robot_id{ROBOT_ID_NONE};      ///< 현재 순서에서 나중 가는 로봇
  bool is_frozen{false};                       ///< true = 이미 릴리스되어 뒤집을 수 없음 ((C3))
};

/**
 * @brief MILP 모델 입력 (스위칭 변수 모델).
 * @note 그래프 자체는 소유하지 않는 관측 포인터로 넘긴다.
 */
struct MilpModel
{
  const void * dependency_graph{nullptr};   ///< mrs_depgraph 의 의존성 그래프 관측 포인터
  std::vector<SwitchableEdge> switchables;  ///< 스위칭 후보 엣지
  std::vector<double> node_duration_s;      ///< 각 행동 정점의 예상 소요 시간 [s]
  double budget_s{1.0};                     ///< 해결 예산 [s]. 초과 시 현재 최선해 또는 구 순서 유지
};

/**
 * @brief MILP 종료 상태.
 */
enum class MilpStatus : std::uint8_t
{
  OPTIMAL = 0,          ///< 최적해
  FEASIBLE_SUBOPTIMAL = 1, ///< 예산 내 실행가능해 (gap > 0)
  INFEASIBLE = 2,       ///< 실행가능해 없음 — 호출자는 구 순서 유지 후 상위 상향
  BUDGET_EXCEEDED = 3,  ///< 예산 초과, 해 없음
  CANCELLED = 4,        ///< 외부 취소
  SOLVER_ERROR = 5      ///< 백엔드 실패
};

/**
 * @brief MILP 결과.
 */
struct MilpResult
{
  MilpStatus status{MilpStatus::SOLVER_ERROR}; ///< 종료 상태
  std::vector<bool> switch_decision;           ///< switchables 와 동일 길이. true = 순서 반전
  double objective_value{0.0};                 ///< 목적함수 값 (예상 총 완료시간 [s])
  double optimality_gap{1.0};                  ///< 상대 gap [0,1]. OPTIMAL 이면 0
  double solve_time_s{0.0};                    ///< 소요 시간 [s]
};

/**
 * @brief MILP 백엔드 포트.
 */
class IMilpSolver
{
public:
  virtual ~IMilpSolver() = default;

  /**
   * @brief 스위칭 MILP 를 푼다. **블로킹 호출** — 워커 스레드에서만 호출할 것.
   * @param[in] model 의존성 그래프·스위칭 후보·예산 (MilpModel).
   * @return MilpResult 상태·스위칭 결정·gap·소요시간. 예외를 던지지 않는다.
   */
  virtual MilpResult solve(const MilpModel & model) noexcept = 0;

  /**
   * @brief 진행 중인 solve() 를 취소 요청한다(스레드 안전해야 한다).
   * @return void
   */
  virtual void cancel() noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "cbc" | "highs" | "mip_py" | "gurobi".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
