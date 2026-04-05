#include "sadg_builder.hpp"
#include "sadg_scheduler.hpp"
#include "sadg_validator.hpp"

#include "test_harness.hpp"

#include <vector>

namespace {

using sadg::ActionExecState;
using sadg::ExecutionFeedback;
using sadg::ExecutionFeedbackType;
using sadg::RobotExecState;
using sadg::SadgBuildRequest;
using sadg::SadgBuilder;
using sadg::SadgGraph;
using sadg::SadgPlanHealth;
using sadg::SadgPlanInput;
using sadg::SadgScheduler;
using sadg::SadgTimingPolicy;
using sadg::SadgValidator;

SadgPlanInput make_plan_input()
{
    SadgPlanInput input;
    input.snapshot.current_time = 0;
    input.snapshot.robots = {
        RobotRuntimeState{"r0", 0, std::nullopt, 2, std::nullopt, false},
        RobotRuntimeState{"r1", 3, std::nullopt, 4, std::nullopt, false},
    };
    input.assignments = {
        GoalAssignment{"g0", "r0", 2},
        GoalAssignment{"g1", "r1", 4},
    };
    input.solution = {
        AgentPlan{{0, 0}, {1, 1}, {2, 2}},
        AgentPlan{{3, 0}, {4, 1}},
    };
    return input;
}

}  // namespace

int main()
{
    std::vector<mapf_test::TestCase> tests;

    tests.push_back({"sadg_builder_creates_actions_and_intra_edges",
                     [](mapf_test::TestContext& ctx) {
                         SadgBuilder builder;
                         SadgBuildRequest request;
                         request.plan_input = make_plan_input();

                         const auto result = builder.build(request);
                         MAPF_EXPECT_EQ(ctx, result.graph.action_count(), std::size_t(3));
                         MAPF_EXPECT_EQ(ctx, result.graph.edge_count(), std::size_t(1));
                         MAPF_EXPECT_EQ(ctx,
                                        result.graph.robot_actions(0).size(),
                                        std::size_t(2));
                         MAPF_EXPECT_EQ(ctx,
                                        result.graph.robot_actions(1).size(),
                                        std::size_t(1));

                         const auto& action0 = result.graph.action(0);
                         MAPF_EXPECT_EQ(ctx, action0.from_vertex, 0);
                         MAPF_EXPECT_EQ(ctx, action0.to_vertex, 1);
                         MAPF_EXPECT_FALSE(ctx, action0.is_wait);

                         const auto& edge0 = result.graph.edge(0);
                         MAPF_EXPECT_EQ(ctx,
                                        edge0.type,
                                        sadg::SadgEdgeType::INTRA_ROBOT_ORDER);
                     }});

    tests.push_back({"sadg_scheduler_initializes_ready_actions",
                     [](mapf_test::TestContext& ctx) {
                         SadgBuilder builder;
                         SadgBuildRequest request;
                         request.plan_input = make_plan_input();
                         const auto build = builder.build(request);

                         SadgScheduler scheduler;
                         const auto init =
                             scheduler.initialize(build.graph, request.plan_input.snapshot);

                         MAPF_EXPECT_EQ(ctx,
                                        init.state.action_states[0],
                                        ActionExecState::READY);
                         MAPF_EXPECT_EQ(ctx,
                                        init.state.action_states[1],
                                        ActionExecState::PENDING);
                         MAPF_EXPECT_EQ(ctx,
                                        init.robots[0].state,
                                        RobotExecState::READY);
                     }});

    tests.push_back({"sadg_scheduler_dispatch_and_complete_action",
                     [](mapf_test::TestContext& ctx) {
                         SadgBuilder builder;
                         SadgBuildRequest request;
                         request.plan_input = make_plan_input();
                         const auto build = builder.build(request);

                         SadgScheduler scheduler;
                         auto init =
                             scheduler.initialize(build.graph, request.plan_input.snapshot);

                         const auto dispatch = scheduler.dispatch_ready_actions(
                             build.graph, init.state, init.robots, 1000);
                         MAPF_EXPECT_EQ(ctx, dispatch.dispatched_actions.size(), std::size_t(2));
                         MAPF_EXPECT_EQ(ctx,
                                        init.state.action_states[0],
                                        ActionExecState::DISPATCHED);
                         MAPF_EXPECT_EQ(ctx,
                                        init.robots[0].state,
                                        RobotExecState::EXECUTING);

                         ExecutionFeedback feedback;
                         feedback.robot_id = "r0";
                         feedback.timestamp_ms = 2000;
                         feedback.type = ExecutionFeedbackType::ACTION_DONE;
                         feedback.action_id = "r0:a0";
                         feedback.actual_vertex = 1;

                         scheduler.apply_feedback(build.graph, init.state, init.robots, feedback);
                         scheduler.recompute_ready(build.graph, init.state, init.robots);

                         MAPF_EXPECT_EQ(ctx,
                                        init.state.action_states[0],
                                        ActionExecState::DONE);
                         MAPF_EXPECT_EQ(ctx,
                                        init.state.action_states[1],
                                        ActionExecState::READY);
                         MAPF_EXPECT_EQ(ctx,
                                        init.robots[0].state,
                                        RobotExecState::READY);
                     }});

    tests.push_back({"sadg_validator_detects_timing_deviation",
                     [](mapf_test::TestContext& ctx) {
                         SadgBuilder builder;
                         SadgBuildRequest request;
                         request.plan_input = make_plan_input();
                         const auto build = builder.build(request);

                         SadgScheduler scheduler;
                         auto init =
                             scheduler.initialize(build.graph, request.plan_input.snapshot);
                         scheduler.dispatch_ready_actions(
                             build.graph, init.state, init.robots, 0);

                         ExecutionFeedback completed;
                         completed.robot_id = "r1";
                         completed.timestamp_ms = 1000;
                         completed.type = ExecutionFeedbackType::ACTION_DONE;
                         completed.action_id = "r1:a0";
                         completed.actual_vertex = 4;
                         scheduler.apply_feedback(
                             build.graph, init.state, init.robots, completed);

                         SadgValidator validator;
                         SadgTimingPolicy timing_policy;
                         timing_policy.dt_ms = 1000;

                         const auto report = validator.evaluate(build.graph,
                                                                init.state,
                                                                init.robots,
                                                                1500,
                                                                timing_policy,
                                                                {});
                         MAPF_EXPECT_EQ(ctx,
                                        report.health,
                                        SadgPlanHealth::TIMING_DEVIATION);
                         MAPF_EXPECT_EQ(ctx, report.seed_robot_indices.size(), std::size_t(1));
                         MAPF_EXPECT_EQ(ctx, report.seed_robot_indices[0], 0);
                     }});

    tests.push_back({"sadg_validator_detects_stale_and_invalid_position",
                     [](mapf_test::TestContext& ctx) {
                         SadgBuilder builder;
                         SadgBuildRequest request;
                         request.plan_input = make_plan_input();
                         const auto build = builder.build(request);

                         SadgScheduler scheduler;
                         auto init =
                             scheduler.initialize(build.graph, request.plan_input.snapshot);
                         scheduler.dispatch_ready_actions(
                             build.graph, init.state, init.robots, 0);

                         ExecutionFeedback completed;
                         completed.robot_id = "r1";
                         completed.timestamp_ms = 1000;
                         completed.type = ExecutionFeedbackType::ACTION_DONE;
                         completed.action_id = "r1:a0";
                         completed.actual_vertex = 4;
                         scheduler.apply_feedback(
                             build.graph, init.state, init.robots, completed);

                         SadgValidator validator;
                         SadgTimingPolicy timing_policy;
                         timing_policy.dt_ms = 1000;
                         timing_policy.stale_observation_window_ms = 500;
                         sadg::SadgReplanTriggerPolicy trigger_policy;
                         trigger_policy.dependency_block_escalation_ms = 1500;

                         const auto stale_report = validator.evaluate(build.graph,
                                                                      init.state,
                                                                      init.robots,
                                                                      2600,
                                                                      timing_policy,
                                                                      trigger_policy);
                         MAPF_EXPECT_EQ(ctx, stale_report.health, SadgPlanHealth::STALE);

                         init.robots[0].current_vertex = 99;
                         const auto invalid_report = validator.evaluate(build.graph,
                                                                        init.state,
                                                                        init.robots,
                                                                        2600,
                                                                        timing_policy,
                                                                        trigger_policy);
                         MAPF_EXPECT_EQ(ctx, invalid_report.health, SadgPlanHealth::INVALID);
                         MAPF_EXPECT_EQ(ctx,
                                        invalid_report.seed_robot_indices.size(),
                                        std::size_t(1));
                         MAPF_EXPECT_EQ(ctx, invalid_report.seed_robot_indices[0], 0);
                     }});

    return mapf_test::run_all(tests);
}
