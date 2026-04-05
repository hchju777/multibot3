#include "collision.hpp"
#include "dist_table.hpp"
#include "graph.hpp"
#include "lifelong_planner.hpp"
#include "params.hpp"
#include "planner.hpp"
#include "reservation_table.hpp"
#include "prepared_map.hpp"
#include "solution.hpp"
#include "solution_validation.hpp"
#include "stop_condition.hpp"
#include "test_harness.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <memory>
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
        {"prepared_map_rejects_non_biconnected_graph", [](mapf_test::TestContext& ctx) {
             bool threw = false;
             std::string error;
             try {
                 (void)PreparedMap::from_graph(make_line_graph(4), PreprocessingParams{false});
             } catch (const std::exception& e) {
                 threw = true;
                 error = e.what();
             }

             MAPF_EXPECT_TRUE(ctx, threw);
             MAPF_EXPECT_CONTAINS(ctx, error, "biconnected");
         }},
        {"prepared_map_accepts_biconnected_graph", [](mapf_test::TestContext& ctx) {
             PreparedMap prepared =
                 PreparedMap::from_graph(make_cycle_graph(4), PreprocessingParams{true});

             MAPF_EXPECT_TRUE(ctx, prepared.planning_graph().is_biconnected());
             MAPF_EXPECT_FALSE(ctx, prepared.uses_virtual_lock());
             MAPF_EXPECT_EQ(ctx, prepared.remap_vertex(0), 0);
         }},
        {"reservation_table_blocks_frozen_paths_and_holds", [](mapf_test::TestContext& ctx) {
             FrozenPlanSet frozen;
             frozen.plan_suffixes.push_back(
                 FrozenRobotPlan{"r0", AgentPlan{{0, 0}, {1, 1}, {2, 2}}, true});
             frozen.holds.push_back(HoldReservation{"r1", 4, 0, 3});

             ReservationTable reservations;
             reservations.reserve_all(frozen);

             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(1, 1));
             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(2, 7));
             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(4, 3));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(1, 0, 1));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(1, 0, 2));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(4, 3, 1));
         }},
        {"reservation_table_validates_conflicting_solution", [](mapf_test::TestContext& ctx) {
             ReservationTable reservations;
             reservations.reserve_plan(
                 FrozenRobotPlan{"r0", AgentPlan{{0, 0}, {1, 1}}, true});

             std::string error;
             MAPF_EXPECT_FALSE(ctx,
                               reservations.validate_plan(AgentPlan{{1, 0}, {0, 1}},
                                                          &error));
             MAPF_EXPECT_CONTAINS(ctx, error, "move");
         }},
        {"reservation_table_applies_base_timestep_offset", [](mapf_test::TestContext& ctx) {
             ReservationTable reservations;
             reservations.reserve_plan(
                 FrozenRobotPlan{"r0", AgentPlan{{3, 8}, {4, 9}, {5, 10}}, true},
                 9);
             reservations.reserve_hold(HoldReservation{"r1", 7, 9, 6}, 9);

             MAPF_EXPECT_FALSE(ctx, reservations.is_vertex_reserved(3, 0));
             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(4, 0));
             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(5, 4));
             MAPF_EXPECT_TRUE(ctx, reservations.is_vertex_reserved(7, 0));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(5, 4, 0));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(5, 4, 1));
             MAPF_EXPECT_FALSE(ctx, reservations.is_move_allowed(7, 6, 1));
         }},
        {"params_loads_core_fields", [](mapf_test::TestContext& ctx) {
             const auto path = write_temp_yaml(
                 "mapf_params_core_fields",
                 "planner:\n"
                 "  objective: makespan\n"
                 "  initial_quality_threshold: 1.5\n"
                 "  initial_timeout_ms: 777\n"
                 "  replan_timeout_ms: 88\n"
                 "  parallel_search_workers: 3\n"
                 "  seed: 9\n"
                 "preprocessing:\n"
                 "  virtual_lock: false\n");

             const Params params = Params::load(path.string());
             MAPF_EXPECT_EQ(ctx, static_cast<int>(params.planner.objective),
                            static_cast<int>(ObjectiveType::MAKESPAN));
             MAPF_EXPECT_EQ(ctx, params.planner.initial_quality_threshold, 1.5);
             MAPF_EXPECT_EQ(ctx, params.planner.initial_timeout_ms, 777);
             MAPF_EXPECT_EQ(ctx, params.planner.replan_timeout_ms, 88);
             MAPF_EXPECT_EQ(ctx, params.planner.parallel_search_workers, 3);
             MAPF_EXPECT_EQ(ctx, params.planner.seed, 9);
             MAPF_EXPECT_FALSE(ctx, params.preprocessing.virtual_lock);
         }},
        {"lifelong_planner_plan_initial_api_solves_snapshot_assignments",
         [](mapf_test::TestContext& ctx) {
             Params params = Params::defaults();
             params.preprocessing.virtual_lock = false;
             params.planner.parallel_search_workers = 1;
             params.planner.initial_timeout_ms = 100;
             params.planner.seed = 0;

             MapfPlanner planner(
                 PreparedMap::from_graph(make_cycle_graph(4), params.preprocessing),
                 params);

             InitialPlanRequest req;
             req.snapshot.current_time = 0;
             req.snapshot.robots = {
                 RobotRuntimeState{"r0", 0, std::nullopt, std::nullopt, std::nullopt, false},
                 RobotRuntimeState{"r1", 2, std::nullopt, std::nullopt, std::nullopt, false},
             };
             req.assignments = {
                 GoalAssignment{"g0", "r0", 1},
                 GoalAssignment{"g1", "r1", 3},
             };

             const PlanResult result = planner.plan_initial(req);
             MAPF_EXPECT_TRUE(ctx, result.solved);
             MAPF_EXPECT_TRUE(ctx, result.valid);
             MAPF_EXPECT_EQ(ctx, result.planning_solution.size(), static_cast<std::size_t>(2));
             MAPF_EXPECT_EQ(ctx, result.planning_solution[0].front().first, 0);
             MAPF_EXPECT_EQ(ctx, result.planning_solution[0].back().first, 1);
             MAPF_EXPECT_EQ(ctx, result.planning_solution[1].front().first, 2);
             MAPF_EXPECT_EQ(ctx, result.planning_solution[1].back().first, 3);
         }},
        {"lifelong_planner_replan_affected_api_returns_partial_solution",
         [](mapf_test::TestContext& ctx) {
             Params params = Params::defaults();
             params.preprocessing.virtual_lock = false;
             params.planner.parallel_search_workers = 1;
             params.planner.replan_timeout_ms = 100;
             params.planner.seed = 0;

             MapfPlanner planner(
                 PreparedMap::from_graph(make_cycle_graph(4), params.preprocessing),
                 params);

             AffectedReplanRequest req;
             req.snapshot.current_time = 5;
             req.snapshot.robots = {
                 RobotRuntimeState{"r0", 0, std::nullopt, std::optional<int>(3), std::nullopt, false},
                 RobotRuntimeState{"r1", 2, std::nullopt, std::nullopt, std::nullopt, false},
             };
             req.assignments = {
                 GoalAssignment{"g0", "r0", 1},
             };
             req.affected_robot_indices = {0};
             req.frozen.holds.push_back(
                 HoldReservation{"r1", 2, 5, std::nullopt});

             const PlanResult result = planner.replan_affected(req);
             MAPF_EXPECT_TRUE(ctx, result.solved);
             MAPF_EXPECT_TRUE(ctx, result.valid);
             MAPF_EXPECT_EQ(ctx, result.planning_solution.size(), static_cast<std::size_t>(1));
             MAPF_EXPECT_EQ(ctx, result.planning_solution[0].front().first, 0);
             MAPF_EXPECT_EQ(ctx, result.planning_solution[0].back().first, 1);
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

             auto abort_flag = std::make_shared<std::atomic_bool>(true);
             StopCondition aborted(PlanMode::REPLAN);
             aborted.bind_abort_flag(abort_flag);
             MAPF_EXPECT_TRUE(ctx, aborted.should_abort());
             MAPF_EXPECT_FALSE(ctx, aborted.is_timeout());
         }},
        {"lifelong_runtime_escalates_failed_affected_replan_to_global",
         [](mapf_test::TestContext& ctx) {
             Params params = Params::defaults();
             params.preprocessing.virtual_lock = false;
             params.planner.parallel_search_workers = 1;
             params.planner.initial_timeout_ms = 100;
             params.planner.replan_timeout_ms = 0;
             params.planner.seed = 0;

             LifelongScenario scenario;
             scenario.robots = {
                 RobotSpec{"r0", 0, std::nullopt},
                 RobotSpec{"r1", 2, std::nullopt},
             };
             scenario.events = {
                 ScenarioEvent{
                     "e0",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r0",
                     1,
                     ScenarioTrigger{ScenarioTriggerType::AT_START, std::nullopt},
                     {},
                 },
                 ScenarioEvent{
                     "e1",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r1",
                     3,
                     ScenarioTrigger{ScenarioTriggerType::AFTER_INITIAL, std::nullopt},
                     {},
                 },
             };

             MapfPlanner planner(
                 PreparedMap::from_graph(make_cycle_graph(4), params.preprocessing),
                 params);
             const ScenarioRunResult result = planner.run(scenario);

             MAPF_EXPECT_FALSE(ctx, result.safe_stop_required);
             MAPF_EXPECT_EQ(ctx, result.episodes.size(), static_cast<std::size_t>(2));
             MAPF_EXPECT_FALSE(ctx, result.episodes.front().used_global_replan);
             MAPF_EXPECT_TRUE(ctx, result.episodes.back().used_global_replan);
             MAPF_EXPECT_TRUE(ctx, result.episodes.back().result.solved);
             MAPF_EXPECT_TRUE(ctx, result.episodes.back().result.valid);
             MAPF_EXPECT_EQ(ctx,
                            result.episodes.back().request.robot_ids.size(),
                            scenario.robots.size());
         }},
        {"lifelong_runtime_preserves_shifted_frozen_suffix_positions",
         [](mapf_test::TestContext& ctx) {
             Params params = Params::defaults();
             params.preprocessing.virtual_lock = false;
             params.planner.parallel_search_workers = 1;
             params.planner.initial_timeout_ms = 100;
             params.planner.replan_timeout_ms = 100;
             params.planner.seed = 0;

             LifelongScenario scenario;
             scenario.robots = {
                 RobotSpec{"r0", 0, std::nullopt},
                 RobotSpec{"r1", 2, std::nullopt},
             };
             scenario.events = {
                 ScenarioEvent{
                     "e0",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r0",
                     1,
                     ScenarioTrigger{ScenarioTriggerType::AT_START, std::nullopt},
                     {},
                 },
                 ScenarioEvent{
                     "e1",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r1",
                     3,
                     ScenarioTrigger{ScenarioTriggerType::AFTER_INITIAL, std::nullopt},
                     {},
                 },
                 ScenarioEvent{
                     "e2",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r1",
                     2,
                     ScenarioTrigger{ScenarioTriggerType::ON_ROBOT_IDLE, std::nullopt},
                     {"e1"},
                 },
             };

             MapfPlanner planner(
                 PreparedMap::from_graph(make_cycle_graph(4), params.preprocessing),
                 params);
             const ScenarioRunResult result = planner.run(scenario);

             MAPF_EXPECT_FALSE(ctx, result.safe_stop_required);
             MAPF_EXPECT_EQ(ctx, result.episodes.size(), static_cast<std::size_t>(3));
             MAPF_EXPECT_EQ(ctx, result.final_robots.size(), static_cast<std::size_t>(2));
             MAPF_EXPECT_EQ(ctx, result.final_robots[0].current_vertex, 1);
             MAPF_EXPECT_EQ(ctx, result.final_robots[1].current_vertex, 2);
         }},
        {"lifelong_runtime_requests_safe_stop_when_global_replan_also_fails",
         [](mapf_test::TestContext& ctx) {
             Params params = Params::defaults();
             params.preprocessing.virtual_lock = false;
             params.planner.parallel_search_workers = 1;
             params.planner.initial_timeout_ms = 100;
             params.planner.replan_timeout_ms = 0;
             params.planner.seed = 0;

             LifelongScenario scenario;
             scenario.robots = {
                 RobotSpec{"r0", 0, std::nullopt},
                 RobotSpec{"r1", 2, std::nullopt},
             };
             scenario.events = {
                 ScenarioEvent{
                     "e0",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r0",
                     0,
                     ScenarioTrigger{ScenarioTriggerType::AT_START, std::nullopt},
                     {},
                 },
                 ScenarioEvent{
                     "e1",
                     ScenarioEventType::ASSIGN_GOAL,
                     "r1",
                     3,
                     ScenarioTrigger{ScenarioTriggerType::AFTER_INITIAL, std::nullopt},
                     {},
                 },
             };

             MapfPlanner planner(
                 PreparedMap::from_graph(make_cycle_graph(4), params.preprocessing),
                 params);
             const ScenarioRunResult result = planner.run(scenario);

             MAPF_EXPECT_TRUE(ctx, result.safe_stop_required);
             MAPF_EXPECT_CONTAINS(ctx, result.stop_reason, "global replan failed");
             MAPF_EXPECT_FALSE(ctx, result.episodes.empty());
             MAPF_EXPECT_TRUE(ctx, result.episodes.back().used_global_replan);
             MAPF_EXPECT_EQ(ctx,
                            static_cast<int>(result.episodes.back().result.status),
                            static_cast<int>(PlanningStatus::SAFE_STOP_REQUIRED));
             MAPF_EXPECT_FALSE(ctx, result.episodes.back().result.solved);
             MAPF_EXPECT_TRUE(ctx, result.final_robots[0].held);
             MAPF_EXPECT_TRUE(ctx, result.final_robots[1].held);
         }},
    };

    return mapf_test::run_all(tests);
}
