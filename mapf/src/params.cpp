#include "params.hpp"

#include <yaml-cpp/yaml.h>
#include <stdexcept>

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
        read_opt(pl, "initial_quality_threshold", p.planner.initial_quality_threshold);
        read_opt(pl, "initial_timeout_ms",        p.planner.initial_timeout_ms);
        read_opt(pl, "replan_timeout_ms",         p.planner.replan_timeout_ms);
        read_opt(pl, "seed",                      p.planner.seed);

        // backward-compatible aliases
        bool async_fallback_alias = p.planner.initial_async_fallback;
        int async_fallback_delay_alias = p.planner.initial_async_fallback_delay_ms;
        read_opt(pl, "async_fallback",          async_fallback_alias);
        read_opt(pl, "async_fallback_delay_ms", async_fallback_delay_alias);

        p.planner.initial_async_fallback = async_fallback_alias;
        p.planner.replan_async_fallback = async_fallback_alias;
        p.planner.initial_async_fallback_delay_ms = async_fallback_delay_alias;
        p.planner.replan_async_fallback_delay_ms = async_fallback_delay_alias;

        read_opt(pl, "initial_async_fallback",          p.planner.initial_async_fallback);
        read_opt(pl, "initial_async_fallback_delay_ms", p.planner.initial_async_fallback_delay_ms);
        read_opt(pl, "initial_primary_grace_ms",        p.planner.initial_primary_grace_ms);
        read_opt(pl, "replan_async_fallback",           p.planner.replan_async_fallback);
        read_opt(pl, "replan_async_fallback_delay_ms",  p.planner.replan_async_fallback_delay_ms);
    }

    // ── preprocessing ─────────────────────────────────────────
    if (const YAML::Node& pp = root["preprocessing"]) {
        read_opt(pp, "virtual_lock", p.preprocessing.virtual_lock);
    }

    return p;
}
