#include "config_loader.hpp"
#include "dist_table.hpp"
#include "lifelong_planner.hpp"
#include "map_loader.hpp"
#include "params.hpp"
#include "planner.hpp"
#include "prepared_map.hpp"
#include "scenario_loader.hpp"
#include "solution_validation.hpp"
#include "stop_condition.hpp"
#include "test_harness.hpp"

#include <filesystem>
#include <random>
#include <string>
#include <vector>

namespace {

std::filesystem::path source_path(const std::string& relative)
{
    return std::filesystem::path(MAPF_SOURCE_DIR) / relative;
}

Solution solve_config(const std::string& map_name,
                      PlanMode mode,
                      std::string* validation_error)
{
    ConfigLoader loader(source_path("config/scenarios/single_shot/" + map_name).string());
    const std::string params_path = loader.params_path().value_or(
        source_path("config/params/default.yaml").string());
    const Params params = Params::load(params_path);

    if (params.preprocessing.virtual_lock) {
        loader.preprocess();
    }

    if (!loader.graph().is_biconnected()) {
        throw std::runtime_error("graph is not biconnected after preprocessing for " + map_name);
    }

    Instance ins = loader.make_instance();
    DistTable D(loader.graph(), ins.goals);

    StopCondition stop(mode);
    stop.quality_threshold = params.planner.initial_quality_threshold;
    stop.initial_timeout_ms = params.planner.initial_timeout_ms;
    stop.replan_timeout_ms = params.planner.replan_timeout_ms;

    std::mt19937 rng(static_cast<unsigned>(params.planner.seed));
    Planner planner(ins, D, stop, rng, params.planner);
    Solution sol = planner.solve();
    if (sol.empty()) {
        throw std::runtime_error("planner returned no solution for " + map_name);
    }

    if (!validate_physical_solution(ins, sol, validation_error)) {
        throw std::runtime_error("invalid solution for " + map_name + ": " + *validation_error);
    }

    return sol;
}

void expect_valid_solution(mapf_test::TestContext& ctx,
                           const std::string& map_name,
                           PlanMode mode)
{
    std::string error;
    const Solution sol = solve_config(map_name, mode, &error);
    MAPF_EXPECT_FALSE(ctx, sol.empty());
    MAPF_EXPECT_TRUE(ctx, error.empty());
}

void expect_valid_lifelong_scenario(mapf_test::TestContext& ctx,
                                    const std::string& scenario_name)
{
    ScenarioLoader loader(source_path("config/scenarios/lifelong/" + scenario_name).string());
    const LifelongScenario& scenario = loader.scenario();
    const Params params = Params::load(scenario.params_path);

    MapLoader map_loader(scenario.map_path);
    PreparedMap prepared =
        PreparedMap::from_graph(map_loader.release_graph(), params.preprocessing);

    LifelongPlanner planner(std::move(prepared), params);
    const ScenarioRunResult result = planner.run(scenario);

    MAPF_EXPECT_EQ(ctx, result.consumed_events, scenario.events.size());
    MAPF_EXPECT_EQ(ctx, result.final_robots.size(), scenario.robots.size());
    MAPF_EXPECT_FALSE(ctx, result.frames.empty());
    MAPF_EXPECT_EQ(ctx, result.frames.front().t, 0);
    MAPF_EXPECT_EQ(ctx, result.frames.front().positions.size(), scenario.robots.size());
    MAPF_EXPECT_EQ(ctx, result.frames.back().positions.size(), scenario.robots.size());
    if (!scenario.events.empty()) {
        MAPF_EXPECT_FALSE(ctx, result.episodes.empty());
    }

    std::size_t activated_event_count = 0;
    for (const auto& episode : result.episodes) {
        activated_event_count += episode.activated_event_ids.size();
        MAPF_EXPECT_FALSE(ctx, episode.activated_event_ids.empty());
        MAPF_EXPECT_TRUE(ctx, episode.result.solved);
        MAPF_EXPECT_TRUE(ctx, episode.result.valid);
        MAPF_EXPECT_FALSE(ctx, episode.result.planning_solution.empty());
        MAPF_EXPECT_EQ(ctx, episode.request.robot_ids.size(), scenario.robots.size());
    }
    MAPF_EXPECT_EQ(ctx, activated_event_count, scenario.events.size());
}

}  // namespace

int main()
{
    using mapf_test::TestCase;

    const std::vector<std::string> initial_maps = {
        "square.yaml",
        "corridor.yaml",
        "corridor_n4.yaml",
        "corridor_n6.yaml",
        "corridor_n8.yaml",
        "grid_4x4.yaml",
        "ring_24.yaml",
        "warehouse_5x4.yaml",
    };
    const std::vector<std::string> lifelong_scenarios = {
        "square.yaml",
        "corridor.yaml",
        "corridor_n4.yaml",
        "corridor_n6.yaml",
        "corridor_n8.yaml",
        "grid_4x4.yaml",
        "ring_24.yaml",
        "warehouse_5x4.yaml",
    };

    std::vector<TestCase> tests;
    tests.reserve(initial_maps.size() + 3 + lifelong_scenarios.size());

    for (const auto& map_name : initial_maps) {
        tests.push_back(
            {"initial_planner_valid_" + map_name,
             [map_name](mapf_test::TestContext& ctx) {
                 expect_valid_solution(ctx, map_name, PlanMode::INITIAL);
             }});
    }

    for (const auto& map_name : {"corridor_n8.yaml", "grid_4x4.yaml", "warehouse_5x4.yaml"}) {
        tests.push_back(
            {std::string("replan_planner_valid_") + map_name,
             [map_name](mapf_test::TestContext& ctx) {
                 expect_valid_solution(ctx, map_name, PlanMode::REPLAN);
             }});
    }

    for (const auto& scenario_name : lifelong_scenarios) {
        tests.push_back(
            {std::string("lifelong_event_stream_") + scenario_name,
             [scenario_name](mapf_test::TestContext& ctx) {
                 expect_valid_lifelong_scenario(ctx, scenario_name);
             }});
    }

    return mapf_test::run_all(tests);
}
