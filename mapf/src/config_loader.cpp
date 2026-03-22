#include "config_loader.hpp"
#include "graph_preprocessor.hpp"
#include "map_loader.hpp"

#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>

namespace {

std::string resolve_path_from(const std::filesystem::path& base_dir,
                              const std::string& raw_path)
{
    const std::filesystem::path path(raw_path);
    if (path.is_absolute()) return path.lexically_normal().string();
    return (base_dir / path).lexically_normal().string();
}

std::string require_scalar(const YAML::Node& node, const char* key)
{
    if (!node[key] || !node[key].IsScalar()) {
        throw std::runtime_error(std::string("YAML: '") + key + "' must be a scalar");
    }
    return node[key].as<std::string>();
}

void load_graph_from_embedded_yaml(const YAML::Node& g_node,
                                   Graph* graph,
                                   std::unordered_map<int, Vertex*>* id_to_vertex)
{
    const YAML::Node& verts = g_node["vertices"];
    if (!verts || !verts.IsSequence()) {
        throw std::runtime_error("YAML: 'graph.vertices' must be a sequence");
    }

    for (const auto& v : verts) {
        const int id = v["id"].as<int>();
        Vertex* vp = graph->add_vertex();
        if (vp->id != id) {
            throw std::runtime_error(
                "YAML: vertex ids must be contiguous starting from 0 "
                "(got id=" + std::to_string(id) +
                " but expected " + std::to_string(vp->id) + ")");
        }
        (*id_to_vertex)[id] = vp;
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
        if (!id_to_vertex->count(a) || !id_to_vertex->count(b)) {
            throw std::runtime_error("YAML: edge references unknown vertex id");
        }
        graph->add_edge((*id_to_vertex)[a], (*id_to_vertex)[b]);
    }
}

}  // namespace

ConfigLoader::ConfigLoader(const std::string& path)
    : path_(std::filesystem::absolute(path).lexically_normal().string()),
      graph_(std::make_unique<Graph>())
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path_);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to load YAML: " + std::string(e.what()));
    }

    const std::filesystem::path base_dir = std::filesystem::path(path_).parent_path();

    std::unordered_map<int, Vertex*> id_to_vertex;
    if (root["map"]) {
        map_path_ = resolve_path_from(base_dir, require_scalar(root, "map"));
        if (root["params"]) {
            params_path_ = resolve_path_from(base_dir, require_scalar(root, "params"));
        }

        MapLoader map_loader(map_path_);
        *graph_ = map_loader.release_graph();
        for (Vertex* v : graph_->V) {
            id_to_vertex[v->id] = v;
        }
    } else {
        const YAML::Node& g_node = root["graph"];
        if (!g_node) throw std::runtime_error("YAML: either 'map' or 'graph' key is required");
        map_path_ = path_;
        if (root["params"]) {
            params_path_ = resolve_path_from(base_dir, require_scalar(root, "params"));
        }
        load_graph_from_embedded_yaml(g_node, graph_.get(), &id_to_vertex);
    }

    // ── 로봇 로드 ─────────────────────────────────────────────
    const YAML::Node& robots = root["robots"];
    if (!robots || !robots.IsSequence())
        throw std::runtime_error("YAML: 'robots' must be a sequence");

    struct RobotEntry { int id, start, goal; };
    std::vector<RobotEntry> entries;
    entries.reserve(robots.size());

    for (const auto& r : robots) {
        RobotEntry re;
        re.id    = r["id"].as<int>();
        re.start = r["start"].as<int>();
        re.goal  = r["goal"].as<int>();
        entries.push_back(re);
    }

    std::sort(entries.begin(), entries.end(),
              [](const RobotEntry& a, const RobotEntry& b){ return a.id < b.id; });

    for (const auto& re : entries) {
        if (!id_to_vertex.count(re.start))
            throw std::runtime_error("YAML: robot start vertex id not found");
        if (!id_to_vertex.count(re.goal))
            throw std::runtime_error("YAML: robot goal vertex id not found");
        starts_.push_back(id_to_vertex[re.start]);
        goals_.push_back(id_to_vertex[re.goal]);
    }

    if (starts_.empty())
        throw std::runtime_error("YAML: no robots defined");
}

// ============================================================
//  preprocess  —  Virtual lock vertex 전처리 (§5.3)
// ============================================================
void ConfigLoader::preprocess()
{
    auto result = GraphPreprocessor::apply_virtual_lock(*graph_, starts_, goals_);

    if (result.num_virtual > 0) {
        graph_  = std::make_unique<Graph>(std::move(result.graph));
        starts_ = std::move(result.starts);
        goals_  = std::move(result.goals);
    }
    // num_virtual == 0: bridge 없음 → 변환 불필요, 현재 graph_ 유지
}

Instance ConfigLoader::make_instance() const
{
    return Instance{*graph_, starts_, goals_};
}
