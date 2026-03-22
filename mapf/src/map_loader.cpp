#include "map_loader.hpp"

#include <unordered_map>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

MapLoader::MapLoader(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to load map YAML: " + std::string(e.what()));
    }

    const YAML::Node& g_node = root["graph"];
    if (!g_node) throw std::runtime_error("YAML: 'graph' key missing");

    std::unordered_map<int, Vertex*> id_to_vertex;

    const YAML::Node& verts = g_node["vertices"];
    if (!verts || !verts.IsSequence()) {
        throw std::runtime_error("YAML: 'graph.vertices' must be a sequence");
    }

    for (const auto& v : verts) {
        const int id = v["id"].as<int>();
        Vertex* vp = graph_.add_vertex();
        if (vp->id != id) {
            throw std::runtime_error(
                "YAML: vertex ids must be contiguous starting from 0 "
                "(got id=" + std::to_string(id) +
                " but expected " + std::to_string(vp->id) + ")");
        }
        id_to_vertex[id] = vp;
    }

    const YAML::Node& edges = g_node["edges"];
    if (!edges || !edges.IsSequence()) {
        throw std::runtime_error("YAML: 'graph.edges' must be a sequence");
    }

    for (const auto& e : edges) {
        if (!e.IsSequence() || e.size() != 2) {
            throw std::runtime_error("YAML: each edge must be [id_a, id_b]");
        }

        const int a = e[0].as<int>();
        const int b = e[1].as<int>();
        if (!id_to_vertex.count(a) || !id_to_vertex.count(b)) {
            throw std::runtime_error("YAML: edge references unknown vertex id");
        }
        graph_.add_edge(id_to_vertex[a], id_to_vertex[b]);
    }
}
