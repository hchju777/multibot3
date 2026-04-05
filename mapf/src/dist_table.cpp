#include "dist_table.hpp"

#include <cassert>
#include <future>
#include <queue>
#include <stdexcept>

// ============================================================
//  생성자: 모든 에이전트에 대해 BFS 수행
// ============================================================
DistTable::DistTable(const Graph& G,
                     const std::vector<Vertex*>& goals)
{
    assert(G.num_vertices() > 0);
    assert(!goals.empty());

    const int n_agents = static_cast<int>(goals.size());
    table.resize(n_agents,
                 std::vector<int>(G.num_vertices(), DIST_INF));

    if (n_agents == 1) {
        assert(goals[0] != nullptr);
        bfs(0, goals[0], G);
        return;
    }

    std::vector<std::future<void>> jobs;
    jobs.reserve(n_agents);
    for (int i = 0; i < n_agents; ++i) {
        assert(goals[i] != nullptr);
        jobs.push_back(std::async(std::launch::async,
                                  [this, &G, goal = goals[i], i]() {
                                      bfs(i, goal, G);
                                  }));
    }

    for (auto& job : jobs) {
        job.get();
    }
}

DistTable::DistTable(std::vector<std::vector<int>> table_data)
    : table(std::move(table_data))
{
    if (table.empty()) {
        throw std::runtime_error("DistTable requires at least one agent row");
    }
    const std::size_t width = table.front().size();
    if (width == 0) {
        throw std::runtime_error("DistTable rows must not be empty");
    }
    for (const auto& row : table) {
        if (row.size() != width) {
            throw std::runtime_error("DistTable rows must all have equal width");
        }
    }
}

// ============================================================
//  get
// ============================================================
int DistTable::get(int agent_id, int vertex_id) const
{
    assert(agent_id  >= 0 && agent_id  < num_agents());
    assert(vertex_id >= 0 && vertex_id < num_vertices());
    return table[agent_id][vertex_id];
}

// ============================================================
//  bfs
//  goal vertex 에서 출발하는 역방향 BFS.
//  undirected graph이므로 역방향 = 정방향과 동일.
//
//  결과: table[agent_id][v] = v → goal 최단 거리
// ============================================================
void DistTable::bfs(int agent_id,
                    Vertex* goal,
                    const Graph& /*G*/)
{
    std::queue<Vertex*> q;
    table[agent_id][goal->id] = 0;
    q.push(goal);

    while (!q.empty()) {
        Vertex* curr = q.front();
        q.pop();
        const int d = table[agent_id][curr->id];

        for (Vertex* nb : curr->neighbors) {
            if (table[agent_id][nb->id] == DIST_INF) {
                table[agent_id][nb->id] = d + 1;
                q.push(nb);
            }
        }
    }
}
