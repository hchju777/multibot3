#include "prepared_map.hpp"

#include "graph_preprocessor.hpp"

#include <cassert>
#include <stdexcept>

namespace {

Graph clone_graph(const Graph& src)
{
    Graph dst;
    for (int i = 0; i < src.num_vertices(); ++i) {
        dst.add_vertex();
    }
    for (int u = 0; u < src.num_vertices(); ++u) {
        for (Vertex* nb : src.V[u]->neighbors) {
            if (u < nb->id) {
                dst.add_edge(u, nb->id);
            }
        }
    }
    return dst;
}

}  // namespace

PreparedMap::PreparedMap(Graph original_graph,
                         Graph planning_graph,
                         std::vector<int> original_to_planning_vertex,
                         std::vector<std::vector<int>> planning_to_original_vertices,
                         bool uses_virtual_lock)
    : original_graph_(std::move(original_graph)),
      planning_graph_(std::move(planning_graph)),
      original_to_planning_vertex_(std::move(original_to_planning_vertex)),
      planning_to_original_vertices_(std::move(planning_to_original_vertices)),
      uses_virtual_lock_(uses_virtual_lock)
{}

PreparedMap PreparedMap::from_graph(Graph graph,
                                    const PreprocessingParams& params)
{
    const int n = graph.num_vertices();
    if (n == 0) {
        throw std::runtime_error("PreparedMap requires a non-empty graph");
    }

    auto validate_biconnected = [](const Graph& planning_graph) {
        if (!planning_graph.is_biconnected()) {
            throw std::runtime_error("planning graph is not biconnected after preprocessing");
        }
    };

    if (!params.virtual_lock) {
        std::vector<int> original_to_planning(n);
        std::vector<std::vector<int>> planning_to_original(n);
        for (int i = 0; i < n; ++i) {
            original_to_planning[i] = i;
            planning_to_original[i].push_back(i);
        }
        Graph planning_graph = clone_graph(graph);
        validate_biconnected(planning_graph);
        return PreparedMap(std::move(graph),
                           std::move(planning_graph),
                           std::move(original_to_planning),
                           std::move(planning_to_original),
                           false);
    }

    auto result = GraphPreprocessor::apply_virtual_lock(graph, {}, {});
    if (result.original_to_planning_vertex.size() != static_cast<std::size_t>(n)) {
        throw std::runtime_error("graph preprocessor did not return complete vertex mapping");
    }
    validate_biconnected(result.graph);

    return PreparedMap(std::move(graph),
                       std::move(result.graph),
                       std::move(result.original_to_planning_vertex),
                       std::move(result.planning_to_original_vertices),
                       result.num_virtual > 0);
}

int PreparedMap::remap_vertex(int original_vid) const
{
    if (original_vid < 0 ||
        original_vid >= static_cast<int>(original_to_planning_vertex_.size())) {
        throw std::out_of_range("original vertex id out of range");
    }
    return original_to_planning_vertex_[original_vid];
}

const std::vector<int>& PreparedMap::expand_vertex(int planning_vid) const
{
    if (planning_vid < 0 ||
        planning_vid >= static_cast<int>(planning_to_original_vertices_.size())) {
        throw std::out_of_range("planning vertex id out of range");
    }
    return planning_to_original_vertices_[planning_vid];
}

Instance PreparedMap::make_instance(const std::vector<int>& starts_orig,
                                    const std::vector<int>& goals_orig) const
{
    if (starts_orig.size() != goals_orig.size()) {
        throw std::runtime_error("starts/goals size mismatch");
    }

    std::vector<Vertex*> starts;
    std::vector<Vertex*> goals;
    starts.reserve(starts_orig.size());
    goals.reserve(goals_orig.size());

    for (int v : starts_orig) {
        starts.push_back(planning_graph_.V[remap_vertex(v)]);
    }
    for (int v : goals_orig) {
        goals.push_back(planning_graph_.V[remap_vertex(v)]);
    }

    return Instance{planning_graph_, std::move(starts), std::move(goals)};
}

Instance PreparedMap::make_instance_from_planning(const std::vector<int>& starts,
                                                  const std::vector<int>& goals) const
{
    if (starts.size() != goals.size()) {
        throw std::runtime_error("starts/goals size mismatch");
    }

    std::vector<Vertex*> start_vertices;
    std::vector<Vertex*> goal_vertices;
    start_vertices.reserve(starts.size());
    goal_vertices.reserve(goals.size());

    const int n = planning_graph_.num_vertices();
    for (int v : starts) {
        if (v < 0 || v >= n) {
            throw std::out_of_range("planning start vertex id out of range");
        }
        start_vertices.push_back(planning_graph_.V[v]);
    }
    for (int v : goals) {
        if (v < 0 || v >= n) {
            throw std::out_of_range("planning goal vertex id out of range");
        }
        goal_vertices.push_back(planning_graph_.V[v]);
    }

    return Instance{planning_graph_, std::move(start_vertices), std::move(goal_vertices)};
}
