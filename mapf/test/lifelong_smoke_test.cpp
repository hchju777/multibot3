//  lifelong_smoke_test.cpp
//  scripted event-stream scenario 를 replay 하며 lifelong MAPF 를 검증한다.
//
//  사용법:
//    ./mapf_lifelong_test <lifelong_scenario.yaml> [--out run.json] [--log-level info]
//    ./mapf_lifelong_test   (기본: config/scenarios/lifelong/square.yaml/.env)

#include "cli_env.hpp"
#include "lifelong_planner.hpp"
#include "logger.hpp"
#include "map_loader.hpp"
#include "params.hpp"
#include "prepared_map.hpp"
#include "scenario_loader.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

namespace {

void write_lifelong_json(const std::string& path,
                         const LifelongScenario& scenario,
                         const PreparedMap& prepared,
                         const ScenarioRunResult& run)
{
    std::ofstream f(path);
    if (!f) {
        spdlog::warn("cannot open '{}' for lifelong JSON output", path);
        return;
    }

    f << "{\n";
    f << "  \"kind\": \"lifelong_run_v1\",\n";
    f << "  \"scenario_path\": \"" << scenario.path << "\",\n";
    f << "  \"map_path\": \"" << scenario.map_path << "\",\n";
    f << "  \"params_path\": \"" << scenario.params_path << "\",\n";
    f << "  \"uses_virtual_lock\": " << (prepared.uses_virtual_lock() ? "true" : "false") << ",\n";

    f << "  \"planning_graph\": {\n";
    f << "    \"vertices\": [\n";
    for (int vid = 0; vid < prepared.planning_graph().num_vertices(); ++vid) {
        const auto& backing = prepared.expand_vertex(vid);
        f << "      {\"id\": " << vid << ", \"backing_vertices\": [";
        for (std::size_t i = 0; i < backing.size(); ++i) {
            if (i) f << ", ";
            f << backing[i];
        }
        f << "]}";
        if (vid + 1 < prepared.planning_graph().num_vertices()) f << ",";
        f << "\n";
    }
    f << "    ],\n";

    std::vector<std::pair<int, int>> edges;
    for (int u = 0; u < prepared.planning_graph().num_vertices(); ++u) {
        for (Vertex* nb : prepared.planning_graph().V[u]->neighbors) {
            if (u < nb->id) edges.push_back({u, nb->id});
        }
    }
    f << "    \"edges\": [\n";
    for (std::size_t i = 0; i < edges.size(); ++i) {
        f << "      [" << edges[i].first << ", " << edges[i].second << "]";
        if (i + 1 < edges.size()) f << ",";
        f << "\n";
    }
    f << "    ]\n";
    f << "  },\n";

    f << "  \"robots\": [\n";
    for (std::size_t i = 0; i < scenario.robots.size(); ++i) {
        const auto& robot = scenario.robots[i];
        f << "    {\"id\": \"" << robot.id << "\", "
          << "\"start\": " << prepared.remap_vertex(robot.start);
        if (robot.home) {
            f << ", \"home\": " << prepared.remap_vertex(*robot.home);
        } else {
            f << ", \"home\": null";
        }
        f << "}";
        if (i + 1 < scenario.robots.size()) f << ",";
        f << "\n";
    }
    f << "  ],\n";

    f << "  \"episodes\": [\n";
    for (std::size_t i = 0; i < run.episodes.size(); ++i) {
        const auto& episode = run.episodes[i];
        f << "    {\n";
        f << "      \"index\": " << episode.index << ",\n";
        f << "      \"mode\": \"" << (episode.request.mode == PlanMode::INITIAL ? "INITIAL" : "REPLAN") << "\",\n";
        f << "      \"cost\": " << episode.result.cost << ",\n";
        f << "      \"makespan\": " << episode.result.makespan << ",\n";
        f << "      \"executed_until_timestep\": " << episode.executed_until_timestep << ",\n";
        f << "      \"activated_event_ids\": [";
        for (std::size_t j = 0; j < episode.activated_event_ids.size(); ++j) {
            if (j) f << ", ";
            f << "\"" << episode.activated_event_ids[j] << "\"";
        }
        f << "]\n";
        f << "    }";
        if (i + 1 < run.episodes.size()) f << ",";
        f << "\n";
    }
    f << "  ],\n";

    f << "  \"frames\": [\n";
    for (std::size_t i = 0; i < run.frames.size(); ++i) {
        const auto& frame = run.frames[i];
        f << "    {\n";
        f << "      \"t\": " << frame.t << ",\n";
        f << "      \"episode_index\": " << frame.episode_index << ",\n";
        f << "      \"activated_event_ids\": [";
        for (std::size_t j = 0; j < frame.activated_event_ids.size(); ++j) {
            if (j) f << ", ";
            f << "\"" << frame.activated_event_ids[j] << "\"";
        }
        f << "],\n";
        f << "      \"positions\": [";
        for (std::size_t j = 0; j < frame.positions.size(); ++j) {
            if (j) f << ", ";
            f << frame.positions[j];
        }
        f << "],\n";
        f << "      \"goals\": [";
        for (std::size_t j = 0; j < frame.active_goals.size(); ++j) {
            if (j) f << ", ";
            if (frame.active_goals[j].has_value()) {
                f << *frame.active_goals[j];
            } else {
                f << "null";
            }
        }
        f << "]\n";
        f << "    }";
        if (i + 1 < run.frames.size()) f << ",";
        f << "\n";
    }
    f << "  ]\n";
    f << "}\n";
}

std::string format_event_ids(const std::vector<std::string>& ids)
{
    if (ids.empty()) return "(none)";
    std::ostringstream oss;
    for (std::size_t i = 0; i < ids.size(); ++i) {
        if (i) oss << ", ";
        oss << ids[i];
    }
    return oss.str();
}

std::string format_backing_vertices(const PreparedMap& map, int planning_vid)
{
    const auto& originals = map.expand_vertex(planning_vid);
    std::ostringstream oss;
    oss << "v" << planning_vid << " <- [";
    for (std::size_t i = 0; i < originals.size(); ++i) {
        if (i) oss << ", ";
        oss << originals[i];
    }
    oss << "]";
    return oss.str();
}

}  // namespace

int main(int argc, char* argv[])
{
    std::string scenario_path = "../config/scenarios/lifelong/square.yaml";
    std::string log_level_name = "info";
    std::string out_path;
    std::optional<std::filesystem::path> dotenv_path;
    int positional = 0;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--log-level" && i + 1 < argc) {
            log_level_name = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            out_path = argv[++i];
        } else if (!arg.empty() && arg[0] != '-' && positional == 0) {
            scenario_path = arg;
            ++positional;
        }
    }

    dotenv_path = mapf_cli::find_dotenv(std::filesystem::current_path());
    if (positional < 1) {
        if (const char* env_scenario = std::getenv("MAPF_LIFELONG_SCENARIO_PATH")) {
            const std::string env_path = mapf_cli::trim_copy(env_scenario);
            if (!env_path.empty()) scenario_path = env_path;
        } else if (dotenv_path) {
            const auto dotenv_scenario =
                mapf_cli::load_dotenv_value(*dotenv_path, "MAPF_LIFELONG_SCENARIO_PATH");
            if (dotenv_scenario && !mapf_cli::trim_copy(*dotenv_scenario).empty()) {
                scenario_path =
                    mapf_cli::resolve_path_from(dotenv_path->parent_path(), *dotenv_scenario);
            }
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

    try {
        ScenarioLoader scenario_loader(scenario_path);
        const LifelongScenario& scenario = scenario_loader.scenario();
        Params params = Params::load(scenario.params_path);

        MapLoader map_loader(scenario.map_path);
        PreparedMap prepared =
            PreparedMap::from_graph(map_loader.release_graph(), params.preprocessing);

        if (!prepared.planning_graph().is_biconnected()) {
            spdlog::error("planning graph is not biconnected after preprocessing");
            return 1;
        }

        LifelongPlanner planner(std::move(prepared), params);
        const ScenarioRunResult run = planner.run(scenario);

        spdlog::info("Scenario: {}", scenario.path);
        spdlog::info("Map     : {}", scenario.map_path);
        spdlog::info("Params  : {}", scenario.params_path);
        spdlog::info("Robots  : {}", scenario.robots.size());
        spdlog::info("Events consumed: {}/{}",
                     run.consumed_events,
                     scenario.events.size());

        for (const auto& episode : run.episodes) {
            spdlog::info("=== Episode {} ===", episode.index);
            spdlog::info("activated events: {}", format_event_ids(episode.activated_event_ids));
            spdlog::info("mode={}, cost={}, makespan={}, executed_until_t={}",
                         episode.request.mode == PlanMode::INITIAL ? "INITIAL" : "REPLAN",
                         episode.result.cost,
                         episode.result.makespan,
                         episode.executed_until_timestep);
        }

        spdlog::info("=== Final Robot States ===");
        for (const auto& robot : run.final_robots) {
            std::ostringstream oss;
            oss << robot.id << ": " << format_backing_vertices(planner.prepared_map(),
                                                                robot.current_vertex);
            if (robot.active_goal) {
                oss << ", active_goal=" << format_backing_vertices(planner.prepared_map(),
                                                                    *robot.active_goal);
            } else {
                oss << ", active_goal=(idle)";
            }
            spdlog::info("{}", oss.str());
        }

        if (!out_path.empty()) {
            write_lifelong_json(out_path, scenario, planner.prepared_map(), run);
            spdlog::info("[json] written to {}", out_path);
        }
    } catch (const std::exception& e) {
        spdlog::error("{}", e.what());
        return 1;
    }

    return 0;
}
