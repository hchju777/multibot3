#pragma once

/**
 * @file i_path_solver.hpp
 * @brief L2 경로계획 솔버 플러그인 포트 (seam a, D-03). 계약 v1.0.0.
 *
 * 근거: D-03 — "운용 구조(수신지평 창 + 이벤트 구동 재계획)와 솔버를 분리, 솔버는 교체 가능
 * 컴포넌트". 출력은 **시각 부여 node-visit 시퀀스**로 표준화되어 L3 가 솔버 불가지론이 된다.
 * 구현체: LLLG(1순위) / RHCR(baseline·폴백) / PIBT(폴백 후보, 미확정) / CannedSolver(tracer bullet).
 *
 * 예외 규율(CLAUDE.md 규율 2): 구현체는 예외를 밖으로 던지지 않는다. 내부 실패는 잡아서
 * SolverStatus 로 보고하고, 호출자는 구계획 유지로 폴백한다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 계획 요청의 로봇 1대분 항목.
 */
struct PlanRequestEntry
{
  RobotId robot_id{ROBOT_ID_NONE}; ///< 로봇 id
  NodeId start_node{NODE_ID_NONE}; ///< 시작 노드 id (균일 뷰)
  double start_theta_rad{0.0};     ///< 시작 자세 [rad], map, [-pi, pi] (diff-drive 회전 비용)
  NodeId goal_node{NODE_ID_NONE};  ///< 목표 노드 id (균일 뷰)
};

/**
 * @brief 솔버 종료 상태.
 */
enum class SolverStatus : std::uint8_t
{
  SUCCESS = 0,         ///< 전 엔트리에 대한 해 반환
  INFEASIBLE = 1,      ///< 해 없음
  BUDGET_EXCEEDED = 2, ///< 예산 초과 — paths 는 부분해이거나 비어 있음
  STALE_VERSION = 3,   ///< roadmap_version/view_id 불일치
  INTERNAL_ERROR = 4   ///< 구현체 내부 실패(예외를 잡아 여기로 보고)
};

/**
 * @brief 솔버 입력.
 * @note graph 는 소유하지 않는 관측 포인터다. 호출자가 solve() 반환 시까지 유효를 보장한다.
 */
struct PathSolverInput
{
  const void * uniform_view{nullptr};        ///< 균일 뷰 그래프 (mrs_map_registry 의 GraphView*)
  std::uint64_t roadmap_version{0};          ///< 지도 버전
  std::uint32_t view_id{0};                  ///< 균일 뷰 id (입도 정합 — Q-7)
  std::vector<PlanRequestEntry> entries;     ///< 계획 대상
  std::vector<FrozenOrder> frozen_orders;    ///< 뒤집을 수 없는 교차 순서 (T2 (C3)). 전역 계획이면 빈 벡터
  std::uint32_t horizon_w{0};                ///< 수신지평 창 크기 [스텝]. 0 = 솔버 기본값
  double budget_s{1.0};                      ///< 계획 예산 [s] (확정서 realtime_requirement ≤1 s/스텝)
  bool allow_warm_start{true};               ///< 이전 해 접미부 웜스타트 허용 여부 (LLLG)
};

/**
 * @brief 솔버 출력.
 */
struct PathSolverOutput
{
  SolverStatus status{SolverStatus::INTERNAL_ERROR}; ///< 종료 상태
  std::vector<RobotPath> paths;                      ///< 시각 부여 node-visit 시퀀스 (표준형)
  double planning_time_s{0.0};                       ///< 실제 소요 시간 [s]
  std::string message;                               ///< 진단 문자열 (기계 판정 금지)
};

/**
 * @brief L2 경로계획 솔버 포트.
 */
class IPathSolver
{
public:
  virtual ~IPathSolver() = default;

  /**
   * @brief 다중로봇 경로를 계획한다.
   * @param[in] input 그래프·시작/목표·예산·고정 순서를 담은 요청 (PathSolverInput).
   * @return PathSolverOutput 상태·경로·소요시간. 예외를 던지지 않는다(실패는 status 로 보고).
   */
  virtual PathSolverOutput solve(const PathSolverInput & input) noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "lllg" | "rhcr" | "pibt" | "canned" 등 — 로그·계측에 기록된다.
   */
  virtual std::string name() const noexcept = 0;

  /**
   * @brief 웜스타트(이전 해 재사용) 지원 여부.
   * @return bool true 면 allow_warm_start 를 해석한다. false 면 무시된다.
   */
  virtual bool supports_warm_start() const noexcept = 0;
};

} // namespace mrs
