//  smoke_test.cpp
//  single-shot scenario YAML 에서 맵/로봇/파라미터를 읽어 MAPF 를 푼다.
//
//  사용법:
//    ./mapf_test <single_shot_scenario.yaml> [params.yaml] [--out solution.json]
//    ./mapf_test
//      (기본: single_shot square scenario + scenario params/.env)

#include "config_loader.hpp"
#include "cli_env.hpp"
#include "dist_table.hpp"
#include "logger.hpp"
#include "params.hpp"
#include "planner.hpp"
#include "solution_validation.hpp"
#include "stop_condition.hpp"

#include <cstdlib>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <fstream>
#include <optional>
#include <random>
#include <sstream>
#include <string>

// ── 간단한 JSON 직렬화 ────────────────────────────────────────
static void write_json(const std::string& path,
                       const Instance&    ins,
                       const Solution&    sol,
                       bool               solved)
{
    std::ofstream f(path);
    if (!f) {
        spdlog::warn("cannot open '{}' for JSON output", path);
        return;
    }

    const int n = ins.num_agents();

    f << "{\n";
    f << "  \"solved\": " << (solved ? "true" : "false") << ",\n";
    f << "  \"num_agents\": " << n << ",\n";

    // starts / goals
    f << "  \"robots\": [\n";
    for (int i = 0; i < n; ++i) {
        f << "    {\"id\": " << i
          << ", \"start\": " << ins.starts[i]->id
          << ", \"goal\": "  << ins.goals[i]->id  << "}";
        if (i + 1 < n) f << ",";
        f << "\n";
    }
    f << "  ],\n";

    // paths: paths[i] = [v0, v1, v2, ...]  (vertex id per timestep)
    f << "  \"paths\": [\n";
    for (int i = 0; i < n; ++i) {
        const AgentPlan* plan = (i < static_cast<int>(sol.size())) ? &sol[i] : nullptr;
        f << "    [";
        for (int t = 0; plan != nullptr && t < static_cast<int>(plan->size()); ++t) {
            if (t) f << ", ";
            f << (*plan)[t].first;   // vertex_id
        }
        f << "]";
        if (i + 1 < n) f << ",";
        f << "\n";
    }
    f << "  ]\n";
    f << "}\n";
}

static std::string format_agent_plan(int agent_id, const AgentPlan& plan)
{
    std::ostringstream oss;
    oss << "Robot " << agent_id << ":";
    for (const auto& [vid, t] : plan) {
        oss << ' ' << 'v' << vid << "(t=" << t << ')';
    }
    return oss.str();
}

int main(int argc, char* argv[])
{
    // ── 인수 파싱 ────────────────────────────────────────────
    std::string scenario_path = "../config/scenarios/single_shot/square.yaml";
    std::string params_path;
    std::string out_path;
    std::string log_level_name;
    std::optional<std::filesystem::path> dotenv_path;

    int positional = 0;  // 위치 인수 순서: 0=scenario, 1=params
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--out" && i + 1 < argc) {
            out_path = argv[++i];
        } else if (a == "--log-level" && i + 1 < argc) {
            log_level_name = argv[++i];
        } else if (a[0] != '-') {
            if (positional == 0) { scenario_path = a; ++positional; }
            else if (positional == 1) { params_path = a; ++positional; }
        }
    }

    dotenv_path = mapf_cli::find_dotenv(std::filesystem::current_path());

    if (positional < 1) {
        if (const char* env_scenario = std::getenv("MAPF_SINGLE_SHOT_SCENARIO_PATH")) {
            const std::string env_path = mapf_cli::trim_copy(env_scenario);
            if (!env_path.empty()) scenario_path = env_path;
        } else if (const char* legacy_env_map = std::getenv("MAPF_MAP_PATH")) {
            const std::string env_path = mapf_cli::trim_copy(legacy_env_map);
            if (!env_path.empty()) scenario_path = env_path;
        } else if (dotenv_path) {
            const auto dotenv_scenario =
                mapf_cli::load_dotenv_value(*dotenv_path, "MAPF_SINGLE_SHOT_SCENARIO_PATH");
            if (dotenv_scenario && !mapf_cli::trim_copy(*dotenv_scenario).empty()) {
                scenario_path =
                    mapf_cli::resolve_path_from(dotenv_path->parent_path(), *dotenv_scenario);
            } else {
                const auto legacy_dotenv_map =
                    mapf_cli::load_dotenv_value(*dotenv_path, "MAPF_MAP_PATH");
                if (legacy_dotenv_map && !mapf_cli::trim_copy(*legacy_dotenv_map).empty()) {
                    scenario_path =
                        mapf_cli::resolve_path_from(dotenv_path->parent_path(), *legacy_dotenv_map);
                }
            }
        }
    }

    if (log_level_name.empty()) {
        if (const char* env_level = std::getenv("MAPF_LOG_LEVEL")) {
            log_level_name = env_level;
        } else {
            log_level_name = "info";
        }
    }

    spdlog::level::level_enum log_level = spdlog::level::info;
    const std::string requested_log_level = log_level_name;
    const bool parsed_level = try_parse_log_level(log_level_name, log_level);
    if (!parsed_level) {
        log_level_name = "info";
        log_level = spdlog::level::info;
    }
    init_logging(log_level);
    if (!parsed_level) {
        spdlog::warn("unknown log level '{}', falling back to '{}'",
                     requested_log_level, log_level_name);
    }
    spdlog::debug("logging initialized at level '{}'", log_level_name);

    // ── scenario / map 로드 ─────────────────────────────────
    ConfigLoader loader(scenario_path);

    if (positional < 2 || params_path.empty()) {
        if (const char* env_params = std::getenv("MAPF_PARAMS_PATH")) {
            const std::string env_path = mapf_cli::trim_copy(env_params);
            if (!env_path.empty()) params_path = env_path;
        } else if (dotenv_path) {
            const auto dotenv_params =
                mapf_cli::load_dotenv_value(*dotenv_path, "MAPF_PARAMS_PATH");
            if (dotenv_params && !mapf_cli::trim_copy(*dotenv_params).empty()) {
                params_path =
                    mapf_cli::resolve_path_from(dotenv_path->parent_path(), *dotenv_params);
            }
        }
    }
    if (params_path.empty()) {
        if (loader.params_path()) {
            params_path = *loader.params_path();
        } else {
            params_path = "../config/params/default.yaml";
        }
    }

    spdlog::info("Scenario: {}", scenario_path);
    spdlog::info("Map     : {}", loader.map_path());
    spdlog::info("Params  : {}", params_path);

    // ── 파라미터 로드 ────────────────────────────────────────
    Params params = Params::load(params_path);

    spdlog::info("[params] initial_quality_threshold : {}",
                 params.planner.initial_quality_threshold);
    spdlog::info("[params] initial_timeout_ms        : {}",
                 params.planner.initial_timeout_ms);
    spdlog::info("[params] replan_timeout_ms         : {}",
                 params.planner.replan_timeout_ms);
    spdlog::info("[params] seed                      : {}",
                 params.planner.seed);
    spdlog::info("[params] initial_async_fallback    : {}",
                 params.planner.initial_async_fallback ? "true" : "false");
    spdlog::info("[params] initial_async_fallback_delay_ms : {}",
                 params.planner.initial_async_fallback_delay_ms);
    spdlog::info("[params] initial_primary_grace_ms  : {}",
                 params.planner.initial_primary_grace_ms);
    spdlog::info("[params] replan_async_fallback     : {}",
                 params.planner.replan_async_fallback ? "true" : "false");
    spdlog::info("[params] replan_async_fallback_delay_ms : {}",
                 params.planner.replan_async_fallback_delay_ms);
    spdlog::info("[params] virtual_lock              : {}",
                 params.preprocessing.virtual_lock ? "true" : "false");

    spdlog::info("Vertices (raw): {}", loader.graph().num_vertices());

    // ── 전처리 ───────────────────────────────────────────────
    if (params.preprocessing.virtual_lock) {
        loader.preprocess();
        spdlog::info("Vertices (after preprocess): {}",
                     loader.graph().num_vertices());
    }

    // ── Biconnected 검증 ─────────────────────────────────────
    if (!loader.graph().is_biconnected()) {
        spdlog::error("graph is not biconnected after preprocessing");
        return 1;
    }
    spdlog::info("[OK] biconnected check passed");

    // ── Instance / DistTable ─────────────────────────────────
    Instance ins = loader.make_instance();
    const int n  = ins.num_agents();
    spdlog::info("Agents: {}, Vertices: {}", n, loader.graph().num_vertices());

    DistTable D(loader.graph(), ins.goals);
    spdlog::info("[OK] dist_table built");

    // ── Planner ──────────────────────────────────────────────
    StopCondition stop(PlanMode::INITIAL);
    stop.quality_threshold  = params.planner.initial_quality_threshold;
    stop.initial_timeout_ms = params.planner.initial_timeout_ms;
    stop.replan_timeout_ms  = params.planner.replan_timeout_ms;

    std::mt19937 rng(static_cast<unsigned>(params.planner.seed));
    Planner planner(ins, D, stop, rng, params.planner);

    Solution sol = planner.solve();
    std::string validation_error;
    if (sol.empty()) {
        spdlog::error("planner returned no solution");
        if (!out_path.empty()) write_json(out_path, ins, Solution{}, false);
        return 1;
    }

    // ── 물리적 유효성 검사 ───────────────────────────────────
    if (!validate_physical_solution(ins, sol, &validation_error)) {
        spdlog::error("runner rejected planner output: {}", validation_error);
        if (!out_path.empty()) write_json(out_path, ins, Solution{}, false);
        return 1;
    }
    spdlog::info("[OK] solution found");
    spdlog::info("[OK] solution validation passed");

    // ── Solution 출력 ────────────────────────────────────────
    spdlog::info("=== Solution ===");
    for (int i = 0; i < n; ++i) {
        spdlog::info("{}", format_agent_plan(i, sol[i]));
    }

    // ── JSON 저장 ────────────────────────────────────────────
    if (!out_path.empty()) {
        write_json(out_path, ins, sol, true);
        spdlog::info("[json] written to {}", out_path);
    }

    return 0;
}
