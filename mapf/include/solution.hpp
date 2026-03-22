#pragma once

#include "graph.hpp"
#include <vector>
#include <utility>

// ============================================================
//  Solution 타입 정의
//
//  raw 경로 (planner 내부 표현):
//    RawPath  = 로봇 1대의 vertex 포인터 시퀀스
//    RawPaths = 전체 로봇의 raw 경로
//
//  SADG 입력 형식:
//    PlanTuple  = (vertex_id, timestep)
//    AgentPlan  = 로봇 1대의 (vertex_id, timestep) 시퀀스
//    Solution   = 전체 로봇의 AgentPlan
//
//  변환: to_solution(RawPaths) → Solution
// ============================================================

using RawPath  = std::vector<Vertex*>;
using RawPaths = std::vector<RawPath>;

using PlanTuple = std::pair<int, int>;  // (vertex_id, timestep)
using AgentPlan = std::vector<PlanTuple>;
using Solution  = std::vector<AgentPlan>;

// ============================================================
//  RawPaths → Solution  (SADG 입력 형식으로 변환)
// ============================================================
Solution to_solution(const RawPaths& raw);

// ============================================================
//  SolveStats: 탐색 결과 통계
// ============================================================
struct SolveStats {
    int    makespan          = 0;
    int    sum_of_costs      = 0;
    double elapsed_ms        = 0.0;
    int    hl_expansions     = 0;  // high-level node 확장 횟수
    bool   solution_found    = false;
};
