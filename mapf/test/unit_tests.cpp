#include "collision.hpp"
#include "config_loader.hpp"
#include "dist_table.hpp"
#include "graph.hpp"
#include "params.hpp"
#include "planner.hpp"
#include "solution.hpp"
#include "solution_validation.hpp"
#include "stop_condition.hpp"
#include "test_harness.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

Graph make_line_graph(int n)
{
    Graph g;
    for (int i = 0; i < n; ++i) g.add_vertex();
    for (int i = 0; i + 1 < n; ++i) g.add_edge(i, i + 1);
    return g;
}

Graph make_cycle_graph(int n)
{
    Graph g = make_line_graph(n);
    if (n > 2) g.add_edge(0, n - 1);
    return g;
}

Instance make_instance(const Graph& g,
                       const std::vector<int>& starts,
                       const std::vector<int>& goals)
{
    std::vector<Vertex*> start_vertices;
    std::vector<Vertex*> goal_vertices;
    start_vertices.reserve(starts.size());
    goal_vertices.reserve(goals.size());

    for (int id : starts) start_vertices.push_back(g.V[id]);
    for (int id : goals) goal_vertices.push_back(g.V[id]);

    return Instance{g, std::move(start_vertices), std::move(goal_vertices)};
}

Solution make_solution(const std::vector<std::vector<int>>& paths)
{
    Solution sol(paths.size());
    for (std::size_t agent = 0; agent < paths.size(); ++agent) {
        for (std::size_t t = 0; t < paths[agent].size(); ++t) {
            sol[agent].push_back({paths[agent][t], static_cast<int>(t)});
        }
    }
    return sol;
}

std::filesystem::path write_temp_yaml(const std::string& stem,
                                      const std::string& contents)
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / (stem + ".yaml");
    std::ofstream out(path);
    out << contents;
    out.close();
    return path;
}

std::filesystem::path write_temp_yaml_in_dir(const std::filesystem::path& dir,
                                             const std::string& name,
                                             const std::string& contents)
{
    const std::filesystem::path path = dir / name;
    std::ofstream out(path);
    out << contents;
    out.close();
    return path;
}

}  // namespace

int main()
{
    using mapf_test::TestCase;

    const std::vector<TestCase> tests = {
        {"graph_biconnected_detection", [](mapf_test::TestContext& ctx) {
             Graph cycle = make_cycle_graph(4);
             Graph line = make_line_graph(4);
             MAPF_EXPECT_TRUE(ctx, cycle.is_biconnected());
             MAPF_EXPECT_FALSE(ctx, line.is_biconnected());
         }},
        {"dist_table_shortest_path_lengths", [](mapf_test::TestContext& ctx) {
             Graph g = make_cycle_graph(4);
             DistTable D(g, {g.V[2], g.V[0]});
             MAPF_EXPECT_EQ(ctx, D.get(0, 2), 0);
             MAPF_EXPECT_EQ(ctx, D.get(0, 1), 1);
             MAPF_EXPECT_EQ(ctx, D.get(0, 0), 2);
             MAPF_EXPECT_EQ(ctx, D.get(1, 3), 1);
             MAPF_EXPECT_EQ(ctx, D.get(1, 0), 0);
         }},
        {"collision_checker_vertex_and_swap", [](mapf_test::TestContext& ctx) {
             Graph g = make_line_graph(2);
             const Config current{g.V[0], g.V[1]};
             const Config vertex_collision{g.V[0], g.V[0]};
             const Config swap_collision{g.V[1], g.V[0]};
             const Config no_collision{g.V[0], g.V[1]};

             MAPF_EXPECT_TRUE(ctx, CollisionChecker::has_vertex_collision(vertex_collision));
             MAPF_EXPECT_FALSE(ctx, CollisionChecker::has_vertex_collision(current));
             MAPF_EXPECT_TRUE(ctx, CollisionChecker::has_swap_collision(current, swap_collision));
             MAPF_EXPECT_FALSE(ctx, CollisionChecker::has_swap_collision(current, no_collision));
         }},
        {"collision_checker_non_passing_history", [](mapf_test::TestContext& ctx) {
             CollisionChecker checker;
             checker.record_edge(0, 1, 1);

             MAPF_EXPECT_FALSE(ctx, checker.is_non_passing_free(1, 0, 2));
             MAPF_EXPECT_TRUE(ctx, checker.is_non_passing_free(1, 0, 3));
             MAPF_EXPECT_TRUE(ctx, checker.is_non_passing_free(1, 1, 2));
         }},
        {"solution_validation_accepts_valid_plan", [](mapf_test::TestContext& ctx) {
             Graph g = make_cycle_graph(4);
             Instance ins = make_instance(g, {0, 2}, {2, 0});
             Solution sol = make_solution({{0, 1, 2}, {2, 3, 0}});
             std::string error;

             MAPF_EXPECT_TRUE(ctx, validate_physical_solution(ins, sol, &error));
             MAPF_EXPECT_TRUE(ctx, error.empty());
         }},
        {"solution_validation_rejects_swap_collision", [](mapf_test::TestContext& ctx) {
             Graph g = make_line_graph(2);
             Instance ins = make_instance(g, {0, 1}, {1, 0});
             Solution sol = make_solution({{0, 1}, {1, 0}});
             std::string error;

             MAPF_EXPECT_FALSE(ctx, validate_physical_solution(ins, sol, &error));
             MAPF_EXPECT_CONTAINS(ctx, error, "swap collision");
         }},
        {"solution_validation_rejects_invalid_edge", [](mapf_test::TestContext& ctx) {
             Graph g = make_line_graph(3);
             Instance ins = make_instance(g, {0}, {2});
             Solution sol = make_solution({{0, 2}});
             std::string error;

             MAPF_EXPECT_FALSE(ctx, validate_physical_solution(ins, sol, &error));
             MAPF_EXPECT_CONTAINS(ctx, error, "invalid edge");
         }},
        {"params_legacy_async_alias_populates_both_modes", [](mapf_test::TestContext& ctx) {
             const auto path = write_temp_yaml(
                 "mapf_params_legacy_alias",
                 "planner:\n"
                 "  async_fallback: false\n"
                 "  async_fallback_delay_ms: 75\n");

             const Params params = Params::load(path.string());
             MAPF_EXPECT_FALSE(ctx, params.planner.initial_async_fallback);
             MAPF_EXPECT_FALSE(ctx, params.planner.replan_async_fallback);
             MAPF_EXPECT_EQ(ctx, params.planner.initial_async_fallback_delay_ms, 75);
             MAPF_EXPECT_EQ(ctx, params.planner.replan_async_fallback_delay_ms, 75);
         }},
        {"params_mode_specific_keys_override_alias", [](mapf_test::TestContext& ctx) {
             const auto path = write_temp_yaml(
                 "mapf_params_mode_override",
                 "planner:\n"
                 "  async_fallback: false\n"
                 "  async_fallback_delay_ms: 75\n"
                 "  initial_async_fallback: true\n"
                 "  initial_async_fallback_delay_ms: 40\n"
                 "  initial_primary_grace_ms: 90\n"
                 "  replan_async_fallback_delay_ms: 5\n");

             const Params params = Params::load(path.string());
             MAPF_EXPECT_TRUE(ctx, params.planner.initial_async_fallback);
             MAPF_EXPECT_FALSE(ctx, params.planner.replan_async_fallback);
             MAPF_EXPECT_EQ(ctx, params.planner.initial_async_fallback_delay_ms, 40);
             MAPF_EXPECT_EQ(ctx, params.planner.replan_async_fallback_delay_ms, 5);
             MAPF_EXPECT_EQ(ctx, params.planner.initial_primary_grace_ms, 90);
         }},
        {"config_loader_supports_single_shot_scenario_relative_paths",
         [](mapf_test::TestContext& ctx) {
             const std::filesystem::path dir =
                 std::filesystem::temp_directory_path() / "mapf_single_shot_loader_test";
             std::filesystem::create_directories(dir / "maps");
             std::filesystem::create_directories(dir / "params");
             std::filesystem::create_directories(dir / "scenarios");

             write_temp_yaml_in_dir(
                 dir / "maps",
                 "square.yaml",
                 "graph:\n"
                 "  vertices:\n"
                 "    - {id: 0, x: 0.0, y: 1.0}\n"
                 "    - {id: 1, x: 1.0, y: 1.0}\n"
                 "    - {id: 2, x: 0.0, y: 0.0}\n"
                 "    - {id: 3, x: 1.0, y: 0.0}\n"
                 "  edges:\n"
                 "    - [0, 1]\n"
                 "    - [1, 3]\n"
                 "    - [3, 2]\n"
                 "    - [2, 0]\n");
             write_temp_yaml_in_dir(
                 dir / "params",
                 "default.yaml",
                 "planner:\n"
                 "  seed: 7\n");
             const std::filesystem::path scenario_path = write_temp_yaml_in_dir(
                 dir / "scenarios",
                 "square.yaml",
                 "version: 1\n"
                 "map: ../maps/square.yaml\n"
                 "params: ../params/default.yaml\n"
                 "robots:\n"
                 "  - {id: 0, start: 0, goal: 3}\n"
                 "  - {id: 1, start: 3, goal: 0}\n");

             ConfigLoader loader(scenario_path.string());
             Instance ins = loader.make_instance();

             MAPF_EXPECT_EQ(ctx, loader.graph().num_vertices(), 4);
             MAPF_EXPECT_EQ(ctx, ins.num_agents(), 2);
             MAPF_EXPECT_CONTAINS(ctx, loader.map_path(), "maps/square.yaml");
             MAPF_EXPECT_TRUE(ctx, loader.params_path().has_value());
             MAPF_EXPECT_CONTAINS(ctx, *loader.params_path(), "params/default.yaml");
         }},
        {"stop_condition_respects_mode_semantics", [](mapf_test::TestContext& ctx) {
             StopCondition initial(PlanMode::INITIAL);
             initial.quality_threshold = 1.2;
             initial.set_lower_bound(10);
             initial.notify_solution(13);
             MAPF_EXPECT_FALSE(ctx, initial.should_stop(13));
             initial.notify_solution(12);
             MAPF_EXPECT_TRUE(ctx, initial.should_stop(12));

             StopCondition replan(PlanMode::REPLAN);
             replan.notify_solution(99);
             MAPF_EXPECT_TRUE(ctx, replan.should_stop(99));
         }},
    };

    return mapf_test::run_all(tests);
}
