#pragma once

#include "graph.hpp"
#include "params.hpp"
#include "planner.hpp"

#include <vector>

class PreparedMap {
public:
    PreparedMap(Graph original_graph,
                Graph planning_graph,
                std::vector<int> original_to_planning_vertex,
                std::vector<std::vector<int>> planning_to_original_vertices,
                bool uses_virtual_lock);

    static PreparedMap from_graph(Graph graph,
                                  const PreprocessingParams& params);

    const Graph& original_graph() const { return original_graph_; }
    const Graph& planning_graph() const { return planning_graph_; }

    bool uses_virtual_lock() const { return uses_virtual_lock_; }

    int remap_vertex(int original_vid) const;
    const std::vector<int>& expand_vertex(int planning_vid) const;

    Instance make_instance(const std::vector<int>& starts_orig,
                           const std::vector<int>& goals_orig) const;
    Instance make_instance_from_planning(const std::vector<int>& starts,
                                         const std::vector<int>& goals) const;

private:
    Graph original_graph_;
    Graph planning_graph_;
    std::vector<int> original_to_planning_vertex_;
    std::vector<std::vector<int>> planning_to_original_vertices_;
    bool uses_virtual_lock_ = false;
};
