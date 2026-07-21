/**
 * @file canned_solver.cpp
 * @brief @ref mrs::ros_pp::CannedSolver 본문 — 고정 왕복 경로 생성 (탐색·최적화 없음).
 *
 * 판정·생성 단계를 파일 지역 자유함수로 쪼갠다. `ros/` 패키지의 메서드 길이 상한(경계 검사
 * `tools/check_boundaries.sh` 항목 3)을 지키기 위한 것이며, 동시에 "무엇을 검사하는가"와
 * "무엇을 만드는가"를 눈으로 분리하기 위한 것이다.
 */

#include "mrs_ros_pp/canned_solver.hpp"

#include <cmath>
#include <utility>

namespace mrs::ros_pp
{
namespace
{

/**
 * @brief 요청 항목에 대응하는 고정 라우트를 찾는다.
 * @param[in] routes 라우트 목록. 자료형 `std::vector<CannedRoute>`.
 * @param[in] robot_id 찾을 로봇 id. 자료형 `mrs::RobotId`.
 * @return `const CannedRoute *` — 찾으면 그 원소의 관측 포인터(소유권 없음), 없으면 nullptr.
 */
[[nodiscard]] const CannedRoute * find_route(
  const std::vector<CannedRoute> & routes, RobotId robot_id) noexcept
{
  for (const CannedRoute & route : routes)
  {
    if (route.robot_id == robot_id)
    {
      return &route;
    }
  }
  return nullptr;
}

/**
 * @brief 실패 출력을 만든다 (경로 없음).
 * @param[in] status 종료 상태. 자료형 `mrs::SolverStatus`. `SUCCESS` 를 넘기지 말 것.
 * @param[in] message 진단 문자열. 자료형 `const char *`. 기계 판정에 쓰지 않는다.
 * @return `mrs::PathSolverOutput` — 상태·메시지만 채운 출력. `paths` 는 비어 있다.
 */
[[nodiscard]] PathSolverOutput make_failure(SolverStatus status, const char * message)
{
  PathSolverOutput output;
  output.status = status;
  output.planning_time_s = 0.0;
  output.message = message;
  return output;
}

/**
 * @brief 라우트 하나로 왕복 방문열을 만든다.
 *
 * 시작점 선택: 요청 시작 노드가 라우트 끝점 중 하나면 그쪽에서 출발하고, 어느 쪽도 아니면
 * 결정론적으로 A 에서 출발한다 — [0a] 는 로봇 초기 위치를 `sim_bridge` 가 정하고 이 더미는 그것을
 * 모르므로, 불일치를 실패로 만들지 않는다.
 *
 * @param[in] route 고정 왕복 라우트. 자료형 `CannedRoute`. 끝점 2개는 센티넬이 아니어야 한다.
 * @param[in] entry 요청 항목. 자료형 `mrs::PlanRequestEntry`. `start_node` 만 참조한다.
 * @param[in] lap_count 왕복 횟수. 자료형 `std::uint32_t`. 방문 수는 `2 * lap_count + 1`.
 * @param[in] start_time_s 첫 방문 도착 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
 * @param[in] step_s 인접 방문 간 시각 간격 [s]. 자료형 `double`. 양수여야 단조증가가 성립한다.
 * @param[out] out 채울 경로. 자료형 `mrs::RobotPath`. 기존 내용은 덮어쓴다.
 * @return void
 */
void build_round_trip_path(
  const CannedRoute & route, const PlanRequestEntry & entry, std::uint32_t lap_count,
  double start_time_s, double step_s, RobotPath & out)
{
  const bool start_at_b = (entry.start_node == route.node_b);
  const UniformNodeId first = start_at_b ? route.node_b : route.node_a;
  const UniformNodeId second = start_at_b ? route.node_a : route.node_b;

  out.robot_id = entry.robot_id;
  out.visits.clear();

  const std::size_t visit_count = static_cast<std::size_t>(lap_count) * 2U + 1U;
  out.visits.reserve(visit_count);
  for (std::size_t k = 0; k < visit_count; ++k)
  {
    TimedNodeVisit visit;
    visit.node_id = (k % 2U == 0U) ? first : second;
    visit.arrival_time_s = start_time_s + static_cast<double>(k) * step_s; // 단조증가 보장
    out.visits.push_back(visit);
  }
}

} // namespace

CannedSolver::CannedSolver(double segment_duration_s, std::uint32_t lap_count)
: segment_duration_s_(segment_duration_s), lap_count_(lap_count)
{
}

void CannedSolver::set_routes(std::vector<CannedRoute> routes, const ViewScope & routes_scope)
{
  routes_ = std::move(routes);
  routes_scope_ = routes_scope;
}

void CannedSolver::set_plan_start_time_s(double plan_start_time_s) noexcept
{
  plan_start_time_s_ = plan_start_time_s;
}

SolverStatus CannedSolver::check_preconditions(
  const PathSolverInput & input, const char *& out_message) const noexcept
{
  // ① 파라미터 자기 검사 — 값이 비정상이면 시각 단조증가(계약 L-09)를 만들 수 없다.
  if (!std::isfinite(segment_duration_s_) || segment_duration_s_ <= 0.0 ||
      !std::isfinite(plan_start_time_s_) || plan_start_time_s_ < 0.0)
  {
    out_message = "canned: invalid duration or start time";
    return SolverStatus::INTERNAL_ERROR;
  }

  // ② 스코프 대조 — 더미는 그래프를 들지 않으므로 "라우트를 만든 스코프"가 유일한 근거다.
  if (input.view_scope.view_kind != ViewKind::UNIFORM || input.view_scope != routes_scope_)
  {
    out_message = "canned: request scope differs from routes";
    return SolverStatus::STALE_VERSION;
  }

  // ③ 빈 요청을 SUCCESS 로 돌려주면 L3 가 빈 계획을 정상 계획으로 본다.
  if (routes_.empty() || input.entries.empty())
  {
    out_message = "canned: no routes injected or no entries";
    return SolverStatus::INFEASIBLE;
  }

  out_message = "";
  return SolverStatus::SUCCESS;
}

// 예외 규율: 포트가 `noexcept` 이므로 어떤 예외도 밖으로 나갈 수 없다(CLAUDE.md 규율 2).
// ⚠ 최종 catch 에서 @ref make_failure 를 부르지 않는 이유 — 진단 문자열 할당이 다시 던지면
// `noexcept` 위반으로 프로세스가 즉시 죽는다. 할당 없는 기본 출력만 돌려준다
// (`PathSolverOutput` 의 기본 상태가 이미 `INTERNAL_ERROR` 다).
PathSolverOutput CannedSolver::solve(const PathSolverInput & input) noexcept
{
  try
  {
    const char * precondition_message = "";
    const SolverStatus precondition = check_preconditions(input, precondition_message);
    if (precondition != SolverStatus::SUCCESS)
    {
      return make_failure(precondition, precondition_message);
    }

    PathSolverOutput output;
    output.planning_time_s = 0.0; // 더미는 계산을 하지 않는다 — 0 은 측정값이 아니라 "계산 없음"이다
    output.paths.reserve(input.entries.size());

    for (const PlanRequestEntry & entry : input.entries)
    {
      const CannedRoute * route = find_route(routes_, entry.robot_id);
      if (route == nullptr || route->node_a.is_none() || route->node_b.is_none())
      {
        // 부분해를 SUCCESS 로 내보내지 않는다 — 빠진 로봇의 창을 L3 가 영원히 기다리게 된다.
        return make_failure(SolverStatus::INFEASIBLE, "canned: no route for a requested robot");
      }

      RobotPath path;
      build_round_trip_path(
        *route, entry, lap_count_, plan_start_time_s_, segment_duration_s_, path);
      output.paths.push_back(std::move(path));
    }

    output.status = SolverStatus::SUCCESS;
    output.message = "canned: fixed round-trip route ([0a] tracer bullet dummy)";
    return output;
  }
  catch (...)
  {
    PathSolverOutput output; // 위 주석 참조 — 여기서는 할당을 시도하지 않는다

    output.planning_time_s = 0.0;
    return output;
  }
}

std::string CannedSolver::name() const noexcept
{
  // 짧은 리터럴이라 SSO 로 할당이 일어나지 않는다 — noexcept 포트에서 안전하다.
  return std::string("canned");
}

bool CannedSolver::supports_warm_start() const noexcept
{
  return false;
}

std::size_t CannedSolver::route_count() const noexcept
{
  return routes_.size();
}

} // namespace mrs::ros_pp
