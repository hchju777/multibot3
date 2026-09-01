// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/standby_resolver.hpp"

#include <algorithm>
#include <functional>

namespace mrs_sadg::core
{

std::set<std::string> StandbyResolver::articulation_points(const RoadmapAdjacency& adj)
{
    // Iterative Hopcroft-Tarjan (low-link) articulation points. Read-only.
    std::set<std::string> cut;
    std::map<std::string, int> disc;
    std::map<std::string, int> low;
    std::map<std::string, std::string> parent;
    int timer = 0;

    for (const auto& [start, _] : adj)
    {
        if (disc.count(start))
        {
            continue;
        }
        // Explicit stack: (node, neighbour-iterator index, children-count).
        std::vector<std::string> stack;
        std::map<std::string, std::size_t> next_child;
        std::map<std::string, int> children;
        stack.push_back(start);
        disc[start] = low[start] = ++timer;
        next_child[start] = 0;
        children[start] = 0;
        parent[start] = "";

        while (!stack.empty())
        {
            const std::string u = stack.back();
            auto it = adj.find(u);
            const auto& nbrs = it != adj.end() ? it->second : std::vector<std::string>{};
            if (next_child[u] < nbrs.size())
            {
                const std::string v = nbrs[next_child[u]++];
                if (!disc.count(v))
                {
                    parent[v] = u;
                    ++children[u];
                    disc[v] = low[v] = ++timer;
                    next_child[v] = 0;
                    children[v] = 0;
                    stack.push_back(v);
                }
                else if (v != parent[u])
                {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
            else
            {
                stack.pop_back();
                if (!stack.empty())
                {
                    const std::string p = stack.back();
                    low[p] = std::min(low[p], low[u]);
                    if (!parent[p].empty() && low[u] >= disc[p])
                    {
                        cut.insert(p);
                    }
                }
            }
        }
        if (children[start] > 1)
        {
            cut.insert(start);
        }
    }
    return cut;
}

bool StandbyResolver::standby_recoverable(const RoadmapAdjacency& adj,
                                          const std::string& junction,
                                          int standby_count,
                                          int d3)
{
    return articulation_points(adj).count(junction) != 0 && standby_count >= d3;
}

}  // namespace mrs_sadg::core
