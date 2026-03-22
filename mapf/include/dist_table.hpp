#pragma once

#include "graph.hpp"
#include <vector>
#include <limits>

constexpr int DIST_INF = std::numeric_limits<int>::max();

// ============================================================
//  DistTable
//  각 로봇의 goal vertex에서 역방향 BFS를 수행해
//  "goal → 모든 vertex" 최단 거리를 사전 계산한다.
//
//  사용법:
//    DistTable D(G, goals);
//    int d = D.get(agent_id, vertex_id);
//
//  시간복잡도: O(|A| × |E|)
//  도달 불가 vertex: DIST_INF 반환
// ============================================================
class DistTable {
public:
    DistTable(const Graph& G,
              const std::vector<Vertex*>& goals);
    explicit DistTable(std::vector<std::vector<int>> table_data);

    // 로봇 i의 goal 까지 vertex v 에서의 최단 거리
    int get(int agent_id, int vertex_id) const;

    int num_agents()   const { return static_cast<int>(table.size()); }
    int num_vertices() const {
        return table.empty() ? 0 : static_cast<int>(table[0].size());
    }

private:
    // table[agent_id][vertex_id] = 거리
    std::vector<std::vector<int>> table;

    void bfs(int agent_id,
             Vertex* goal,
             const Graph& G);
};
