#include "stop_condition.hpp"

#include <algorithm>

// ============================================================
//  set_lower_bound
//  Planner::solve() 가 탐색 시작 전에 heuristic(starts) 를 전달한다.
//  lower_bound = sum of individual BFS distances = SOC 의 이론적 하한.
// ============================================================
void StopCondition::set_lower_bound(int lb)
{
    lower_bound = lb;
}

// ============================================================
//  notify_solution
//  새 solution 발견 시 호출. best_known_cost 갱신.
// ============================================================
void StopCondition::notify_solution(int cost)
{
    best_known_cost      = std::min(best_known_cost, cost);
    first_solution_found = true;
}

// ============================================================
//  should_stop
//
//  INITIAL 모드:
//    solution 이 있고, current_cost <= lower_bound * threshold 이면 중단.
//    lower_bound = heuristic(starts) = 이론적 optimal 하한.
//    이 비교가 의미 있으려면 solve() 시작 전에 set_lower_bound() 를
//    반드시 호출해야 한다.
//
//  REPLAN 모드:
//    첫 solution 발견 즉시 중단
// ============================================================
bool StopCondition::should_stop(int current_cost) const
{
    if (!first_solution_found) return false;
    if (is_timeout())          return true;

    if (mode == PlanMode::REPLAN) {
        return true;  // 첫 solution 발견 즉시
    }

    // INITIAL: lower_bound 기반 quality threshold 검사
    // lower_bound 가 0 이면 (set_lower_bound 미호출) 항상 true → 첫 solution 에서 중단
    return static_cast<double>(current_cost) <=
           static_cast<double>(lower_bound) * quality_threshold;
}

// ============================================================
//  is_timeout
// ============================================================
bool StopCondition::is_timeout() const
{
    return elapsed_ms() >= static_cast<double>(timeout_ms());
}

// ============================================================
//  elapsed_ms
// ============================================================
double StopCondition::elapsed_ms() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(
               now - start_time).count();
}
