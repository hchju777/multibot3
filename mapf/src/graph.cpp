#include "graph.hpp"

#include <algorithm>
#include <cassert>
#include <stack>
#include <stdexcept>

// ============================================================
//  Graph 소멸자: 동적 할당된 Vertex 해제
// ============================================================
Graph::~Graph()
{
    for (auto* v : V) delete v;
}

// ============================================================
//  add_vertex
//  id = 현재 V.size() 로 자동 부여
// ============================================================
Vertex* Graph::add_vertex()
{
    auto* v = new Vertex(static_cast<int>(V.size()));
    V.push_back(v);
    return v;
}

// ============================================================
//  add_edge  (undirected)
// ============================================================
void Graph::add_edge(Vertex* u, Vertex* v)
{
    assert(u != nullptr && v != nullptr);
    assert(u != v);  // self-loop 금지
    u->neighbors.push_back(v);
    v->neighbors.push_back(u);
}

void Graph::add_edge(int uid, int vid)
{
    assert(uid >= 0 && uid < num_vertices());
    assert(vid >= 0 && vid < num_vertices());
    add_edge(V[uid], V[vid]);
}

// ============================================================
//  is_biconnected  (iterative Tarjan DFS)
//
//  조건:
//    1. 그래프가 연결되어 있어야 한다 (connected)
//    2. Articulation point(단절점)가 없어야 한다
//
//  Tarjan 알고리즘:
//    disc[u]: DFS에서 u를 처음 방문한 시각
//    low[u]:  u의 subtree에서 back edge로 도달 가능한 가장 빠른 disc 값
//
//  Articulation point 판정:
//    - root: DFS 트리에서 직계 자식(child_count)이 2개 이상이면 AP
//    - non-root u: 자식 v에 대해 low[v] >= disc[u] 이면 AP
//
//  재귀 DFS 대신 명시적 스택(iterative)을 사용해
//  대형 그래프(수천 vertex 이상)의 스택 오버플로를 방지한다.
// ============================================================
bool Graph::is_biconnected() const
{
    const int n = num_vertices();
    if (n <= 1) return false;

    std::vector<int> disc(n, -1);
    std::vector<int> low(n, -1);
    std::vector<int> par(n, -1);
    std::vector<int> child_count(n, 0);  // DFS 트리에서의 직계 자식 수
    bool has_ap = false;
    int  timer  = 0;

    // 스택 프레임: (vertex_id, neighbors 중 다음에 처리할 인덱스)
    struct Frame { int u; int ni; };
    std::stack<Frame> stk;

    disc[0] = low[0] = timer++;
    stk.push({0, 0});

    while (!stk.empty()) {
        auto& f = stk.top();
        const int u = f.u;
        const auto& nbrs = V[u]->neighbors;

        bool pushed = false;
        while (f.ni < static_cast<int>(nbrs.size())) {
            const int v = nbrs[f.ni]->id;
            ++f.ni;

            if (disc[v] == -1) {
                // 미방문 → DFS 진행
                par[v] = u;
                ++child_count[u];
                disc[v] = low[v] = timer++;
                stk.push({v, 0});
                pushed = true;
                break;
            } else if (v != par[u]) {
                // back edge → low 갱신
                low[u] = std::min(low[u], disc[v]);
            }
        }

        if (!pushed) {
            // u 의 DFS 완료: 부모 p 에 low 전파 및 AP 판정
            stk.pop();
            const int p = par[u];
            if (p != -1) {
                low[p] = std::min(low[p], low[u]);

                if (par[p] == -1) {
                    // p 는 root: 직계 자식 수로 판정 (root 가 pop 될 때 처리)
                } else {
                    // p 는 non-root: low[u] >= disc[p] 이면 p 가 AP
                    if (low[u] >= disc[p]) has_ap = true;
                }
            } else {
                // u 자체가 root: child_count > 1 이면 AP
                if (child_count[u] > 1) has_ap = true;
            }
        }
    }

    if (has_ap) return false;

    // 연결성 확인: 모든 vertex 가 방문됐어야 함
    for (int i = 0; i < n; ++i) {
        if (disc[i] == -1) return false;
    }

    return true;
}
