#pragma once

#include "graph.hpp"

#include <vector>

// ============================================================
//  PreprocessResult
//  apply_virtual_lock() 의 반환값.
//  새로 생성된 Graph 와 리매핑된 starts/goals 를 보유한다.
// ============================================================
struct PreprocessResult {
    Graph                graph;           // 변환된 그래프
    std::vector<Vertex*> starts;          // 리매핑된 시작 위치
    std::vector<Vertex*> goals;           // 리매핑된 목표 위치
    std::vector<int>     original_to_planning_vertex;
    std::vector<std::vector<int>> planning_to_original_vertices;
    int                  num_virtual;     // 생성된 virtual lock vertex 수
};

// ============================================================
//  GraphPreprocessor
//  concept/MAPF_LAYER.md §5.3 — Virtual Lock Vertex 변환
//
//  변환 전 (biconnected 위반):
//    [구역A] — a — b — c — d — [구역B]   (bridge chain)
//
//  변환 후:
//    [구역A] — [V_lock] — [구역B]         (virtual lock vertex)
//
//  알고리즘:
//    1. Tarjan iterative DFS 로 bridge edge 탐색
//    2. 모든 edge 가 bridge 인 vertex = "corridor vertex"
//    3. Corridor vertex 의 연결 컴포넌트 → 각각 V_lock 하나로 collapse
//    4. Starts/goals 가 corridor vertex 위에 있으면 V_lock 으로 리매핑
// ============================================================
class GraphPreprocessor {
public:
    // Bridge chain collapse 적용.
    // 입력 Graph/starts/goals 는 수정하지 않고 새 PreprocessResult 반환.
    static PreprocessResult apply_virtual_lock(
        const Graph&                G,
        const std::vector<Vertex*>& starts,
        const std::vector<Vertex*>& goals);

private:
    // Tarjan iterative DFS 로 bridge edge (u,v) 목록 반환 (u < v 보장)
    static std::vector<std::pair<int,int>> find_bridges(const Graph& G);
};
