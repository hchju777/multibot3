#include "params.hpp"

#include <yaml-cpp/yaml.h>
#include <stdexcept>

namespace {

ObjectiveType parse_objective(const std::string& value)
{
    if (value == "soc") return ObjectiveType::SOC;
    if (value == "makespan") return ObjectiveType::MAKESPAN;
    throw std::runtime_error("Invalid planner objective: '" + value + "'");
}

}  // namespace

// 헬퍼: 노드에서 값 읽기. 없으면 기본값 유지.
template<typename T>
static void read_opt(const YAML::Node& node, const char* key, T& out)
{
    if (node[key]) out = node[key].as<T>();
}

Params Params::load(const std::string& path)
{
    Params p;

    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to load params YAML: " +
                                 std::string(e.what()));
    }

    // ── planner ──────────────────────────────────────────────
    if (const YAML::Node& pl = root["planner"]) {
        if (pl["objective"]) {
            p.planner.objective = parse_objective(pl["objective"].as<std::string>());
        }
        read_opt(pl, "initial_quality_threshold", p.planner.initial_quality_threshold);
        read_opt(pl, "initial_timeout_ms",        p.planner.initial_timeout_ms);
        read_opt(pl, "replan_timeout_ms",         p.planner.replan_timeout_ms);
        read_opt(pl, "parallel_search_workers",   p.planner.parallel_search_workers);
        read_opt(pl, "seed",                      p.planner.seed);
    }

    // ── preprocessing ─────────────────────────────────────────
    if (const YAML::Node& pp = root["preprocessing"]) {
        read_opt(pp, "virtual_lock", p.preprocessing.virtual_lock);
    }

    return p;
}
