#include "graph_preprocessor.hpp"

#include <algorithm>
#include <cassert>
#include <queue>
#include <set>
#include <stack>

// ============================================================
//  find_bridges  — Tarjan iterative DFS
//  반환: bridge edge 목록 {u, v}  (u < v)
// ============================================================
std::vector<std::pair<int,int>>
GraphPreprocessor::find_bridges(const Graph& G)
{
    const int n = G.num_vertices();
    std::vector<int> disc(n, -1), low(n, -1), par(n, -1);
    std::vector<std::pair<int,int>> bridges;
    int timer = 0;

    for (int s = 0; s < n; ++s) {
        if (disc[s] != -1) continue;

        // (vertex_id, 현재 neighbor 인덱스)
        std::stack<std::pair<int,int>> stk;
        disc[s] = low[s] = timer++;
        stk.push({s, 0});

        while (!stk.empty()) {
            int  u = stk.top().first;
            int& i = stk.top().second;
            const auto& nbrs = G.V[u]->neighbors;

            bool pushed = false;
            while (i < static_cast<int>(nbrs.size())) {
                int v = nbrs[i]->id;
                ++i;

                if (v == par[u]) continue;          // 부모 edge 스킵

                if (disc[v] == -1) {                // 미방문 → DFS 진행
                    par[v]  = u;
                    disc[v] = low[v] = timer++;
                    stk.push({v, 0});
                    pushed = true;
                    break;
                } else {                            // back edge → low 갱신
                    low[u] = std::min(low[u], disc[v]);
                }
            }

            if (!pushed) {                          // u 의 DFS 완료
                stk.pop();
                if (par[u] != -1) {
                    low[par[u]] = std::min(low[par[u]], low[u]);
                    if (low[u] > disc[par[u]]) {    // bridge 조건
                        int p = par[u];
                        bridges.push_back({std::min(p, u), std::max(p, u)});
                    }
                }
            }
        }
    }

    return bridges;
}

// ============================================================
//  apply_virtual_lock
// ============================================================
PreprocessResult GraphPreprocessor::apply_virtual_lock(
    const Graph&                G,
    const std::vector<Vertex*>& starts,
    const std::vector<Vertex*>& goals)
{
    const int n = G.num_vertices();

    // ── 1. Bridge 탐색 ────────────────────────────────────────
    auto bridges = find_bridges(G);

    // Bridge set (정규화된 pair, O(log n) lookup)
    std::set<std::pair<int,int>> bridge_set;
    for (auto& [u, v] : bridges)
        bridge_set.insert({std::min(u, v), std::max(u, v)});

    auto is_bridge = [&](int u, int v) {
        return bridge_set.count({std::min(u, v), std::max(u, v)}) > 0;
    };

    // ── 2. Corridor vertex 판별 ───────────────────────────────
    //  모든 인접 edge 가 bridge 인 vertex = corridor vertex
    std::vector<bool> is_corridor(n, false);
    for (int u = 0; u < n; ++u) {
        const auto& nbrs = G.V[u]->neighbors;
        if (nbrs.empty()) continue;
        bool all_bridge = true;
        for (Vertex* nb : nbrs) {
            if (!is_bridge(u, nb->id)) { all_bridge = false; break; }
        }
        is_corridor[u] = all_bridge;
    }

    // ── 3. Corridor vertex 의 연결 컴포넌트 (BFS) ────────────
    std::vector<int> comp(n, -1);
    int n_comps = 0;

    for (int u = 0; u < n; ++u) {
        if (!is_corridor[u] || comp[u] != -1) continue;

        std::queue<int> q;
        q.push(u);
        comp[u] = n_comps;

        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (Vertex* nb : G.V[v]->neighbors) {
                int w = nb->id;
                if (is_corridor[w] && comp[w] == -1) {
                    comp[w] = n_comps;
                    q.push(w);
                }
            }
        }
        ++n_comps;
    }

    // ── 4. 새 Graph 구성 ─────────────────────────────────────
    PreprocessResult result;
    result.num_virtual = n_comps;

    std::vector<int> old_to_new(n, -1);
    std::vector<std::vector<int>> comp_members(n_comps);

    // 4a. Non-corridor vertex 추가 (id 순서 보존)
    for (int u = 0; u < n; ++u) {
        if (!is_corridor[u]) {
            Vertex* nv     = result.graph.add_vertex();
            old_to_new[u]  = nv->id;
        }
    }

    // 4b. Virtual lock vertex 추가 (컴포넌트당 1개)
    std::vector<int> vlock_id(n_comps);
    for (int c = 0; c < n_comps; ++c) {
        vlock_id[c] = result.graph.add_vertex()->id;
    }

    for (int u = 0; u < n; ++u) {
        if (is_corridor[u]) {
            comp_members[comp[u]].push_back(u);
        }
    }

    // ── 5. Edge 추가 ─────────────────────────────────────────
    std::set<std::pair<int,int>> added;

    auto add_once = [&](int a, int b) {
        if (a > b) std::swap(a, b);
        if (added.insert({a, b}).second)
            result.graph.add_edge(result.graph.V[a], result.graph.V[b]);
    };

    for (int u = 0; u < n; ++u) {
        for (Vertex* nb : G.V[u]->neighbors) {
            int v = nb->id;
            if (u >= v) continue;   // 각 edge 한 번씩만 처리

            int nu = is_corridor[u] ? vlock_id[comp[u]] : old_to_new[u];
            int nv = is_corridor[v] ? vlock_id[comp[v]] : old_to_new[v];

            if (nu == nv) continue; // 같은 V_lock 내부 edge → 스킵
            add_once(nu, nv);
        }
    }

    // ── 6. Starts / Goals 리매핑 ─────────────────────────────
    auto remap = [&](Vertex* v) -> Vertex* {
        int new_id = is_corridor[v->id]
                     ? vlock_id[comp[v->id]]
                     : old_to_new[v->id];
        assert(new_id >= 0);
        return result.graph.V[new_id];
    };

    result.original_to_planning_vertex.resize(n, -1);
    result.planning_to_original_vertices.resize(result.graph.num_vertices());
    for (int u = 0; u < n; ++u) {
        const int new_id = is_corridor[u]
                         ? vlock_id[comp[u]]
                         : old_to_new[u];
        assert(new_id >= 0);
        result.original_to_planning_vertex[u] = new_id;
    }
    for (int u = 0; u < n; ++u) {
        if (!is_corridor[u]) {
            result.planning_to_original_vertices[old_to_new[u]].push_back(u);
        }
    }
    for (int c = 0; c < n_comps; ++c) {
        result.planning_to_original_vertices[vlock_id[c]] = std::move(comp_members[c]);
    }

    result.starts.reserve(starts.size());
    result.goals.reserve(goals.size());
    for (Vertex* s : starts) result.starts.push_back(remap(s));
    for (Vertex* g : goals)  result.goals.push_back(remap(g));

    return result;
}
