#include "lifelong_planner.hpp"
#include "map_loader.hpp"
#include "params.hpp"
#include "prepared_map.hpp"
#include "scenario_loader.hpp"
#include "test_harness.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace {

std::filesystem::path source_path(const std::string& relative)
{
    return std::filesystem::path(MAPF_SOURCE_DIR) / relative;
}

void expect_valid_prepared_map(mapf_test::TestContext& ctx,
                               const std::string& map_name)
{
    MapLoader map_loader(source_path("config/maps/" + map_name).string());
    Graph raw = map_loader.release_graph();
    const std::string params_path = source_path("config/params/default.yaml").string();
    const Params params = Params::load(params_path);

    PreparedMap prepared = PreparedMap::from_graph(std::move(raw), params.preprocessing);
    MAPF_EXPECT_TRUE(ctx, prepared.planning_graph().is_biconnected());
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

    MapfPlanner planner(std::move(prepared), params);
    const ScenarioRunResult result = planner.run(scenario);

    MAPF_EXPECT_EQ(ctx, result.consumed_events, scenario.events.size());
    MAPF_EXPECT_EQ(ctx, result.final_robots.size(), scenario.robots.size());
    MAPF_EXPECT_FALSE(ctx, result.frames.empty());
    MAPF_EXPECT_FALSE(ctx, result.safe_stop_required);
    MAPF_EXPECT_EQ(ctx, result.frames.front().t, 0);
    MAPF_EXPECT_EQ(ctx, result.frames.front().positions.size(), scenario.robots.size());
    MAPF_EXPECT_EQ(ctx, result.frames.back().positions.size(), scenario.robots.size());
    if (!scenario.events.empty()) {
        MAPF_EXPECT_FALSE(ctx, result.episodes.empty());
    }

    std::size_t activated_event_count = 0;
    for (const auto& episode : result.episodes) {
        activated_event_count += episode.activated_event_ids.size();
        MAPF_EXPECT_TRUE(ctx, episode.result.solved);
        MAPF_EXPECT_TRUE(ctx, episode.result.valid);
        MAPF_EXPECT_FALSE(ctx, episode.result.planning_solution.empty());
        MAPF_EXPECT_EQ(ctx,
                       episode.result.planning_solution.size(),
                       scenario.robots.size());
        MAPF_EXPECT_EQ(ctx,
                       episode.request.robot_ids.size(),
                       episode.request.affected_robot_indices.size());
    }
    MAPF_EXPECT_EQ(ctx, activated_event_count, scenario.events.size());
}

}  // namespace

int main()
{
    using mapf_test::TestCase;

    const std::vector<std::string> maps = {
        "square.yaml",
        "corridor.yaml",
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
    tests.reserve(maps.size() + lifelong_scenarios.size());

    for (const auto& map_name : maps) {
        tests.push_back(
            {"prepared_map_valid_" + map_name,
             [map_name](mapf_test::TestContext& ctx) {
                 expect_valid_prepared_map(ctx, map_name);
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
