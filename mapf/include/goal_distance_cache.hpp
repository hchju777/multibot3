#pragma once

#include "dist_table.hpp"

#include <unordered_map>
#include <vector>

class GoalDistanceCache {
public:
    explicit GoalDistanceCache(const Graph& G) : G(G) {}

    const std::vector<int>& distances_to_goal(int planning_goal_vid);
    DistTable build_dist_table(const std::vector<Vertex*>& goals);

private:
    const Graph& G;
    std::unordered_map<int, std::vector<int>> cache_;
};
