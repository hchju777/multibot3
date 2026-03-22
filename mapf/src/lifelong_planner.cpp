#include "lifelong_planner.hpp"

#include "planner.hpp"
#include "solution_validation.hpp"

#include <algorithm>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace {

int compute_makespan(const Solution& sol)
{
    int makespan = 0;
    for (const auto& plan : sol) {
        if (!plan.empty()) {
            makespan = std::max(makespan, plan.back().second);
        }
    }
    return makespan;
}

int compute_sum_of_costs(const Instance& ins, const Solution& sol)
{
    int total = 0;
    for (int i = 0; i < static_cast<int>(sol.size()); ++i) {
        const int goal_id = ins.goals[i]->id;
        for (const auto& [vid, t] : sol[i]) {
            if (vid == goal_id) {
                total += t;
                break;
            }
        }
    }
    return total;
}

int position_at(const AgentPlan& plan, int timestep)
{
    if (plan.empty()) {
        throw std::runtime_error("cannot query position from empty plan");
    }
    const int clamped = std::min(timestep, plan.back().second);
    return plan[clamped].first;
}

int first_arrival_time(const AgentPlan& plan, int goal_vid)
{
    for (const auto& [vid, t] : plan) {
        if (vid == goal_vid) return t;
    }
    return std::numeric_limits<int>::max();
}

void apply_stop_params(StopCondition& stop, const PlannerParams& params)
{
    stop.quality_threshold = params.initial_quality_threshold;
    stop.initial_timeout_ms = params.initial_timeout_ms;
    stop.replan_timeout_ms = params.replan_timeout_ms;
}

template <typename T>
void expect_unique(const std::vector<T>& values, const char* what)
{
    std::unordered_set<T> seen;
    for (const T& value : values) {
        if (!seen.insert(value).second) {
            throw std::runtime_error(std::string("duplicate ") + what + " detected");
        }
    }
}

}  // namespace

LifelongPlanner::LifelongPlanner(PreparedMap map, Params params)
    : map_(std::move(map)),
      params_(std::move(params)),
      goal_cache_(map_.planning_graph())
{}

PlanResult LifelongPlanner::plan(const PlanningRequest& req)
{
    if (req.robot_ids.size() != req.starts.size() ||
        req.starts.size() != req.goals.size()) {
        throw std::runtime_error("planning request size mismatch");
    }

    PlanResult result;
    if (req.robot_ids.empty()) {
        result.solved = true;
        result.valid = true;
        return result;
    }

    expect_unique(req.starts, "planning start vertex");
    expect_unique(req.goals, "planning goal vertex");

    Instance ins = map_.make_instance_from_planning(req.starts, req.goals);
    DistTable D = goal_cache_.build_dist_table(ins.goals);

    StopCondition stop(req.mode);
    apply_stop_params(stop, req.params);

    std::mt19937 rng(static_cast<unsigned>(req.params.seed));
    Planner planner(ins, D, stop, rng, req.params);

    result.planning_solution = planner.solve();
    result.solved = !result.planning_solution.empty();
    if (!result.solved) {
        result.validation_error = "planner returned no solution";
        return result;
    }

    result.valid = validate_physical_solution(ins,
                                              result.planning_solution,
                                              &result.validation_error);
    if (!result.valid) {
        return result;
    }

    result.cost = compute_sum_of_costs(ins, result.planning_solution);
    result.makespan = compute_makespan(result.planning_solution);
    return result;
}

ScenarioRunResult LifelongPlanner::run(const LifelongScenario& scenario)
{
    if (scenario.replan_policy != ReplanPolicy::REPLAN_ON_ASSIGNMENT) {
        throw std::runtime_error("unsupported lifelong replan policy");
    }

    std::unordered_map<std::string, int> robot_index;
    std::vector<RobotRuntimeState> robots;
    robots.reserve(scenario.robots.size());
    for (const auto& spec : scenario.robots) {
        if (!robot_index.emplace(spec.id, static_cast<int>(robots.size())).second) {
            throw std::runtime_error("duplicate robot id in runtime state: " + spec.id);
        }
        RobotRuntimeState state;
        state.id = spec.id;
        state.current_vertex = map_.remap_vertex(spec.start);
        if (spec.home) state.home_vertex = map_.remap_vertex(*spec.home);
        robots.push_back(std::move(state));
    }

    {
        std::vector<int> starts;
        starts.reserve(robots.size());
        for (const auto& robot : robots) starts.push_back(robot.current_vertex);
        expect_unique(starts, "planning start vertex after preprocessing");
    }

    std::vector<bool> consumed(scenario.events.size(), false);

    auto activate_events = [&](ScenarioTriggerType trigger_type,
                               const std::unordered_set<int>& candidate_robots) {
        std::vector<std::string> activated_ids;
        std::unordered_set<int> activated_robot_indices;
        for (int i = 0; i < static_cast<int>(scenario.events.size()); ++i) {
            if (consumed[i]) continue;
            const ScenarioEvent& event = scenario.events[i];
            if (event.when.type != trigger_type) continue;

            const auto it = robot_index.find(event.robot_id);
            if (it == robot_index.end()) {
                throw std::runtime_error("event references unknown robot: " + event.robot_id);
            }
            const int idx = it->second;
            if (candidate_robots.count(idx) == 0) continue;
            if (!activated_robot_indices.insert(idx).second) continue;

            robots[idx].active_goal = map_.remap_vertex(event.goal);
            consumed[i] = true;
            activated_ids.push_back(event.id);
        }
        return activated_ids;
    };

    auto count_consumed = [&]() {
        return static_cast<std::size_t>(
            std::count(consumed.begin(), consumed.end(), true));
    };

    auto collect_idle_robots = [&]() {
        std::unordered_set<int> idle;
        for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
            if (!robots[i].active_goal.has_value()) {
                idle.insert(i);
            }
        }
        return idle;
    };

    auto has_any_active_goal = [&]() {
        for (const auto& robot : robots) {
            if (robot.active_goal.has_value()) return true;
        }
        return false;
    };

    auto first_pending_idle_event_exists = [&](int robot_idx) {
        for (int i = 0; i < static_cast<int>(scenario.events.size()); ++i) {
            if (consumed[i]) continue;
            const ScenarioEvent& event = scenario.events[i];
            if (event.when.type != ScenarioTriggerType::ON_ROBOT_IDLE) continue;
            if (event.robot_id == robots[robot_idx].id) return true;
        }
        return false;
    };

    auto build_request = [&](PlanMode mode) {
        PlanningRequest req;
        req.mode = mode;
        req.params = params_.planner;
        req.robot_ids.reserve(robots.size());
        req.starts.reserve(robots.size());
        req.goals.reserve(robots.size());
        for (const auto& robot : robots) {
            req.robot_ids.push_back(robot.id);
            req.starts.push_back(robot.current_vertex);
            req.goals.push_back(
                robot.active_goal.value_or(robot.home_vertex.value_or(robot.current_vertex)));
        }
        return req;
    };

    std::unordered_set<int> all_robot_indices;
    for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
        all_robot_indices.insert(i);
    }

    std::vector<std::string> activated_event_ids =
        activate_events(ScenarioTriggerType::AT_START, all_robot_indices);
    if (activated_event_ids.empty()) {
        activated_event_ids =
            activate_events(ScenarioTriggerType::ON_ROBOT_IDLE, collect_idle_robots());
    }

    ScenarioRunResult run_result;
    bool first_episode = true;
    int episode_index = 0;
    int current_t = 0;

    auto capture_frame = [&](int frame_episode_index,
                             const std::vector<std::string>& frame_events) {
        ScenarioFrame frame;
        frame.t = current_t;
        frame.episode_index = frame_episode_index;
        frame.activated_event_ids = frame_events;
        frame.positions.reserve(robots.size());
        frame.active_goals.reserve(robots.size());
        for (const auto& robot : robots) {
            frame.positions.push_back(robot.current_vertex);
            frame.active_goals.push_back(robot.active_goal);
        }
        run_result.frames.push_back(std::move(frame));
    };

    capture_frame(episode_index, activated_event_ids);

    while (true) {
        bool has_active_goal = false;
        for (const auto& robot : robots) {
            if (robot.active_goal.has_value()) {
                has_active_goal = true;
                break;
            }
        }

        if (!has_active_goal && activated_event_ids.empty()) {
            activated_event_ids =
                activate_events(ScenarioTriggerType::ON_ROBOT_IDLE, collect_idle_robots());
            if (activated_event_ids.empty()) break;
        }

        const PlanMode mode = first_episode ? PlanMode::INITIAL : PlanMode::REPLAN;
        PlanningRequest req = build_request(mode);
        PlanResult plan_result = plan(req);
        if (!plan_result.solved) {
            throw std::runtime_error("lifelong planning failed: " + plan_result.validation_error);
        }
        if (!plan_result.valid) {
            throw std::runtime_error("lifelong planning produced invalid solution: " +
                                     plan_result.validation_error);
        }

        int execute_until = plan_result.makespan;
        int earliest_idle_trigger = std::numeric_limits<int>::max();
        for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
            if (!robots[i].active_goal.has_value()) continue;
            if (robots[i].current_vertex == *robots[i].active_goal) continue;
            if (!first_pending_idle_event_exists(i)) continue;

            const int arrival_t =
                first_arrival_time(plan_result.planning_solution[i], *robots[i].active_goal);
            earliest_idle_trigger = std::min(earliest_idle_trigger, arrival_t);
        }
        if (earliest_idle_trigger != std::numeric_limits<int>::max()) {
            execute_until = earliest_idle_trigger;
        }

        const std::vector<std::optional<int>> episode_goals = [&]() {
            std::vector<std::optional<int>> goals;
            goals.reserve(robots.size());
            for (const auto& robot : robots) goals.push_back(robot.active_goal);
            return goals;
        }();

        for (int step = 1; step < execute_until; ++step) {
            ScenarioFrame frame;
            frame.t = current_t + step;
            frame.episode_index = episode_index;
            frame.positions.reserve(robots.size());
            frame.active_goals = episode_goals;
            for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
                frame.positions.push_back(position_at(plan_result.planning_solution[i], step));
            }
            run_result.frames.push_back(std::move(frame));
        }

        for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
            robots[i].current_vertex =
                position_at(plan_result.planning_solution[i], execute_until);
        }

        std::unordered_set<int> idle_now;
        for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
            if (robots[i].active_goal.has_value() &&
                robots[i].current_vertex == *robots[i].active_goal) {
                robots[i].active_goal.reset();
                idle_now.insert(i);
            }
        }

        EpisodeResult episode;
        episode.index = episode_index++;
        episode.activated_event_ids = activated_event_ids;
        episode.request = std::move(req);
        episode.result = std::move(plan_result);
        episode.executed_until_timestep = execute_until;
        run_result.episodes.push_back(std::move(episode));

        activated_event_ids =
            activate_events(ScenarioTriggerType::ON_ROBOT_IDLE, idle_now);
        current_t += execute_until;
        first_episode = false;
        const bool will_continue = has_any_active_goal() || !activated_event_ids.empty();
        capture_frame(will_continue ? episode_index : (episode_index - 1), activated_event_ids);
    }

    run_result.final_robots = std::move(robots);
    run_result.consumed_events = count_consumed();
    return run_result;
}
