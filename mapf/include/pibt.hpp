#pragma once

#include "graph.hpp"
#include "dist_table.hpp"
#include "collision.hpp"
#include <vector>
#include <random>

// ============================================================
//  Constraint
//  LaCAM★ low-level search 에서 생성되는 제약.
//  "agent who는 timestep t에 where에 있어야 한다."
// ============================================================
struct Constraint {
    int     who;       // 대상 에이전트 id
    Vertex* where;     // 강제 위치
};

// ============================================================
//  PIBT  (Priority Inheritance with Backtracking)
//
//  LaCAM★ 의 configuration generator 역할.
//  현재 Config C_now 에서 다음 Config C_next 를 생성한다.
//
//  알고리즘:
//    1. 에이전트를 우선순위 순으로 처리
//    2. funcPIBT() 로 각 에이전트의 다음 위치 결정
//    3. 충돌이 있으면 priority inheritance 로 해결
//
//  multibot3 추가:
//    - funcPIBT 내부에서 non-passing collision (③) 검사
//    - timestep을 인자로 받아 edge_history 참조
// ============================================================
class PIBT {
public:
    PIBT(const Graph& G,
         const DistTable& D,
         CollisionChecker& collision,
         std::mt19937& rng);

    // 새 configuration 생성 시도
    // constraints: LaCAM★ LNode 에서 내려온 강제 배치 목록
    // timestep:    현재 이동이 완료되는 시각 (non-passing 검사용)
    // 반환값: 충돌 없는 C_next 생성 성공 여부
    bool set_new_config(const Config& C_now,
                        Config& C_next,
                        const std::vector<Constraint>& constraints,
                        int timestep);

private:
    const Graph&      G;
    const DistTable&  D;
    CollisionChecker& collision;
    std::mt19937&     rng;

    // occupied_now[vertex_id]  = 현재 timestep에 해당 vertex를 점유한 에이전트 id (-1이면 없음)
    // occupied_next[vertex_id] = 다음 timestep에 해당 vertex로 이동 예정인 에이전트 id (-1이면 없음)
    std::vector<int> occupied_now;
    std::vector<int> occupied_next;

    // in_stack[agent_id] = true ⟺ 해당 에이전트가 현재 funcPIBT 재귀 호출 스택 위에 있음.
    // priority inheritance 연쇄에서 A→B→C→A 같은 순환을 탐지해 무한 재귀를 방지한다.
    // set_new_config 호출마다 false 로 초기화.
    std::vector<bool> in_stack;

    // candidate_order[agent_id][vertex_id] = goal distance 기준으로 미리 정렬된
    // 다음 위치 후보(neighbors + wait). 동점 구간은 생성 시 한 번 섞어 둔다.
    std::vector<std::vector<std::vector<Vertex*>>> candidate_order;

    // -------------------------------------------------------
    //  funcPIBT
    //  에이전트 agent_id 의 다음 위치를 결정한다.
    //  목표에 가까운 neighbor를 우선 시도하고,
    //  충돌 시 해당 vertex를 점유한 에이전트를 재귀적으로 처리.
    //
    //  반환값: 이동 결정 성공 여부
    // -------------------------------------------------------
    bool funcPIBT(int agent_id,
                  const Config& C_now,
                  Config& C_next,
                  int timestep,
                  int caller_id = -1);  // 재귀 호출 시 호출자 id
};
