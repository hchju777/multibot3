#pragma once

#include "graph.hpp"

#include <string>

class MapLoader {
public:
    explicit MapLoader(const std::string& path);

    const Graph& graph() const { return graph_; }
    Graph release_graph() { return std::move(graph_); }

private:
    Graph graph_;
};
