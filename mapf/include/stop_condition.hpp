#pragma once

#include <chrono>
#include <limits>

// ============================================================
//  PlanMode
//  최초 계획과 재계획의 중단 조건이 다르므로 모드로 구분한다.
// ============================================================
enum class PlanMode {
    INITIAL,  // 최초 계획: 경로 품질 우선
    REPLAN    // 재계획:   속도 우선
};

// ============================================================
//  StopCondition
//
//  INITIAL 모드:
//    primary   — quality_threshold 이내에 도달하면 중단
//    secondary — initial_timeout_ms 초과 시 강제 중단
//
//  REPLAN 모드:
//    primary   — 첫 solution 발견 즉시 중단
//    secondary — replan_timeout_ms 초과 시 강제 중단
//
//  사용:
//    stop.notify_solution(cost)  // solution 발견 시 호출
//    stop.should_stop(cost)      // 중단 여부 판단
//    stop.is_timeout()           // 타임아웃 여부
// ============================================================
class StopCondition {
public:
    // ----- 파라미터 (튜닝 가능) -----
    PlanMode mode = PlanMode::INITIAL;

    double quality_threshold  = 1.2;    // optimal 대비 120% 이내면 중단
    int    initial_timeout_ms = 5000;   // 최초 계획 hard limit (ms)
    int    replan_timeout_ms  = 500;    // 재계획 hard limit (ms)

    // --------------------------------
    explicit StopCondition(PlanMode mode = PlanMode::INITIAL)
        : mode(mode),
          start_time(std::chrono::steady_clock::now()),
          lower_bound(0),
          best_known_cost(std::numeric_limits<int>::max()),
          first_solution_found(false)
    {}

    // 탐색 시작 전 호출. heuristic(starts) = sum of BFS distances 를 전달한다.
    // INITIAL 모드의 quality threshold 비교 기준이 된다.
    // 기본값 0 이면 threshold 조건이 항상 만족되어 첫 solution 에서 중단된다.
    void set_lower_bound(int lb);

    // 새 solution 을 발견했을 때 호출 (best_known_cost 갱신)
    void notify_solution(int cost);

    // 현재 cost 에서 탐색을 중단해야 하는지 판단
    bool should_stop(int current_cost) const;

    // 타임아웃 여부
    bool is_timeout() const;

    // 경과 시간 (ms)
    double elapsed_ms() const;

    bool has_solution() const { return first_solution_found; }
    int  best_cost()    const { return best_known_cost; }

private:
    std::chrono::steady_clock::time_point start_time;
    int  lower_bound;       // sum of individual BFS distances (이론적 하한)
    int  best_known_cost;   // 지금까지 발견된 최선 solution cost
    bool first_solution_found;

    int timeout_ms() const {
        return (mode == PlanMode::INITIAL)
               ? initial_timeout_ms
               : replan_timeout_ms;
    }
};
