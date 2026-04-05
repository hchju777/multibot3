#include "goal_distance_cache.hpp"

#include <cassert>
#include <queue>

const std::vector<int>& GoalDistanceCache::distances_to_goal(int planning_goal_vid)
{
    const auto it = cache_.find(planning_goal_vid);
    if (it != cache_.end()) {
        return it->second;
    }

    std::vector<int> distances(G.num_vertices(), DIST_INF);
    std::queue<Vertex*> q;
    distances[planning_goal_vid] = 0;
    q.push(G.V[planning_goal_vid]);

    while (!q.empty()) {
        Vertex* curr = q.front();
        q.pop();
        const int d = distances[curr->id];

        for (Vertex* nb : curr->neighbors) {
            if (distances[nb->id] == DIST_INF) {
                distances[nb->id] = d + 1;
                q.push(nb);
            }
        }
    }

    return cache_.emplace(planning_goal_vid, std::move(distances)).first->second;
}

DistTable GoalDistanceCache::build_dist_table(const std::vector<Vertex*>& goals)
{
    assert(!goals.empty());

    std::vector<std::vector<int>> table;
    table.reserve(goals.size());
    for (Vertex* goal : goals) {
        assert(goal != nullptr);
        table.push_back(distances_to_goal(goal->id));
    }
    return DistTable(std::move(table));
}
