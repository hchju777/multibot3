#include "mapf_runtime.hpp"

#include "lifelong_planner.hpp"
#include "planning_request.hpp"
#include "solution_validation.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
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

    int current_vertex = plan.front().first;
    for (const auto& [vertex, step_timestep] : plan) {
        if (step_timestep > timestep) break;
        current_vertex = vertex;
    }
    return current_vertex;
}

int first_arrival_time(const AgentPlan& plan, int goal_vid)
{
    for (const auto& [vid, t] : plan) {
        if (vid == goal_vid) return t;
    }
    return std::numeric_limits<int>::max();
}

Solution shift_solution(const Solution& solution, int base_timestep)
{
    Solution shifted = solution;
    for (auto& plan : shifted) {
        for (auto& step : plan) {
            step.second += base_timestep;
        }
    }
    return shifted;
}

AgentPlan extract_relative_suffix(const AgentPlan& absolute_plan, int base_timestep)
{
    AgentPlan suffix;
    if (absolute_plan.empty()) return suffix;

    for (const auto& [vertex, timestep] : absolute_plan) {
        if (timestep < base_timestep) continue;
        suffix.push_back({vertex, timestep - base_timestep});
    }

    if (suffix.empty()) {
        suffix.push_back({position_at(absolute_plan, base_timestep), 0});
    } else if (suffix.front().second != 0) {
        suffix.insert(suffix.begin(),
                      {position_at(absolute_plan, base_timestep), 0});
    }

    return suffix;
}

Solution compose_full_solution(const std::vector<RobotRuntimeState>& robots,
                               const std::vector<int>& affected_robot_indices,
                               const Solution& affected_solution,
                               const Solution& previous_absolute_solution,
                               int base_timestep)
{
    Solution full_solution(robots.size());
    std::vector<bool> filled(robots.size(), false);

    for (int i = 0; i < static_cast<int>(affected_robot_indices.size()); ++i) {
        const int robot_idx = affected_robot_indices[i];
        if (robot_idx < 0 || robot_idx >= static_cast<int>(robots.size())) continue;

        if (i < static_cast<int>(affected_solution.size()) &&
            !affected_solution[i].empty()) {
            full_solution[robot_idx] = affected_solution[i];
        } else {
            full_solution[robot_idx].push_back({robots[robot_idx].current_vertex, 0});
        }
        filled[robot_idx] = true;
    }

    for (int robot_idx = 0; robot_idx < static_cast<int>(robots.size()); ++robot_idx) {
        if (filled[robot_idx]) continue;

        if (!previous_absolute_solution.empty() &&
            robot_idx < static_cast<int>(previous_absolute_solution.size())) {
            full_solution[robot_idx] =
                extract_relative_suffix(previous_absolute_solution[robot_idx],
                                        base_timestep);
        }

        if (full_solution[robot_idx].empty()) {
            full_solution[robot_idx].push_back({robots[robot_idx].current_vertex, 0});
        }
    }

    return full_solution;
}

template <typename T>
void expect_unique(const std::vector<T>& values, const char* what)
{
    std::unordered_set<T> seen;
    for (const T& value : values) {
        if (!seen.insert(value).second) {
            std::ostringstream oss;
            oss << "duplicate " << what << " detected: " << value;
            throw std::runtime_error(oss.str());
        }
    }
}

std::vector<GoalAssignment> assignments_from_request(const PlanningRequest& req)
{
    std::vector<GoalAssignment> assignments;
    assignments.reserve(req.robot_ids.size());
    for (int i = 0; i < static_cast<int>(req.robot_ids.size()); ++i) {
        assignments.push_back(
            GoalAssignment{req.robot_ids[i], req.robot_ids[i], req.goals[i]});
    }
    return assignments;
}

InitialPlanRequest make_initial_request(const PlanningRequest& req)
{
    if (!req.snapshot.has_value()) {
        throw std::runtime_error("initial planning request requires snapshot");
    }

    InitialPlanRequest initial_req;
    initial_req.snapshot = *req.snapshot;
    initial_req.assignments = assignments_from_request(req);
    initial_req.params_override = req.params;
    return initial_req;
}

AffectedReplanRequest make_affected_replan_request(const PlanningRequest& req)
{
    if (!req.snapshot.has_value()) {
        throw std::runtime_error("affected replanning request requires snapshot");
    }

    AffectedReplanRequest replan_req;
    replan_req.snapshot = *req.snapshot;
    replan_req.assignments = assignments_from_request(req);
    replan_req.affected_robot_indices = req.affected_robot_indices;
    replan_req.frozen = req.frozen;
    replan_req.params_override = req.params;
    return replan_req;
}

PlanningRequestSummary summarize_request(const PlanningRequest& req)
{
    PlanningRequestSummary summary;
    summary.robot_ids = req.robot_ids;
    summary.affected_robot_indices = req.affected_robot_indices;
    summary.base_timestep = req.base_timestep;
    summary.mode = req.mode;
    summary.frozen_plan_count = static_cast<int>(req.frozen.plan_suffixes.size());
    summary.hold_count = static_cast<int>(req.frozen.holds.size());
    return summary;
}

}  // namespace

ScenarioRuntime::ScenarioRuntime(MapfPlanner& planner)
    : planner_(planner)
{}

ScenarioRunResult ScenarioRuntime::run(const LifelongScenario& scenario)
{
    if (scenario.replan_policy != ReplanPolicy::REPLAN_ON_ASSIGNMENT) {
        throw std::runtime_error("unsupported lifelong replan policy");
    }

    const PreparedMap& map = planner_.prepared_map();
    const Params& params = planner_.params();

    std::unordered_map<std::string, int> robot_index;
    std::vector<RobotRuntimeState> robots;
    robots.reserve(scenario.robots.size());
    for (const auto& spec : scenario.robots) {
        if (!robot_index.emplace(spec.id, static_cast<int>(robots.size())).second) {
            throw std::runtime_error("duplicate robot id in runtime state: " + spec.id);
        }
        RobotRuntimeState state;
        state.id = spec.id;
        state.current_vertex = map.remap_vertex(spec.start);
        if (spec.home) state.home_vertex = map.remap_vertex(*spec.home);
        robots.push_back(std::move(state));
    }

    {
        std::vector<int> starts;
        starts.reserve(robots.size());
        for (const auto& robot : robots) starts.push_back(robot.current_vertex);
        expect_unique(starts, "planning start vertex after preprocessing");
    }

    std::vector<bool> consumed(scenario.events.size(), false);
    std::unordered_set<std::string> completed_event_ids;

    auto dependencies_completed = [&](const ScenarioEvent& event) {
        for (const auto& dep : event.after_event_ids) {
            if (completed_event_ids.count(dep) == 0) return false;
        }
        return true;
    };

    auto activate_events = [&](ScenarioTriggerType trigger_type,
                               const std::unordered_set<int>& candidate_robots) {
        std::vector<std::string> activated_ids;
        std::vector<bool> has_pending_event(robots.size(), false);
        std::unordered_set<int> reserved_goals;

        for (int idx = 0; idx < static_cast<int>(robots.size()); ++idx) {
            if (robots[idx].active_goal.has_value()) {
                reserved_goals.insert(*robots[idx].active_goal);
            }
        }

        for (int i = 0; i < static_cast<int>(scenario.events.size()); ++i) {
            if (consumed[i]) continue;
            const ScenarioEvent& event = scenario.events[i];
            if (event.when.type != trigger_type) continue;
            if (!dependencies_completed(event)) continue;

            const auto it = robot_index.find(event.robot_id);
            if (it == robot_index.end()) {
                throw std::runtime_error("event references unknown robot: " + event.robot_id);
            }
            const int idx = it->second;
            if (candidate_robots.count(idx) == 0) continue;
            if (!has_pending_event[idx]) {
                has_pending_event[idx] = true;
            }
        }

        for (int idx = 0; idx < static_cast<int>(robots.size()); ++idx) {
            if (robots[idx].active_goal.has_value()) continue;
            if (!has_pending_event[idx]) {
                reserved_goals.insert(robots[idx].current_vertex);
            }
        }

        std::unordered_set<int> activated_robot_indices;
        for (int i = 0; i < static_cast<int>(scenario.events.size()); ++i) {
            if (consumed[i]) continue;
            const ScenarioEvent& event = scenario.events[i];
            if (event.when.type != trigger_type) continue;
            if (!dependencies_completed(event)) continue;

            const auto it = robot_index.find(event.robot_id);
            if (it == robot_index.end()) {
                throw std::runtime_error("event references unknown robot: " + event.robot_id);
            }
            const int idx = it->second;
            if (candidate_robots.count(idx) == 0) continue;
            if (!activated_robot_indices.insert(idx).second) continue;

            const int desired_goal = map.remap_vertex(event.goal);
            if (reserved_goals.count(desired_goal) != 0) {
                reserved_goals.insert(robots[idx].current_vertex);
                continue;
            }

            robots[idx].active_goal = desired_goal;
            robots[idx].active_event_id = event.id;
            consumed[i] = true;
            activated_ids.push_back(event.id);
            reserved_goals.insert(desired_goal);
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

    const bool has_after_initial_events = [&]() {
        for (const auto& event : scenario.events) {
            if (event.when.type == ScenarioTriggerType::AFTER_INITIAL) return true;
        }
        return false;
    }();

    auto ready_idle_event_exists = [&](int robot_idx) {
        for (int i = 0; i < static_cast<int>(scenario.events.size()); ++i) {
            if (consumed[i]) continue;
            const ScenarioEvent& event = scenario.events[i];
            if (event.when.type != ScenarioTriggerType::ON_ROBOT_IDLE) continue;
            if (!dependencies_completed(event)) continue;
            if (event.robot_id == robots[robot_idx].id) return true;
        }
        return false;
    };

    ScenarioRunResult run_result;
    bool first_episode = true;
    int episode_index = 0;
    int current_t = 0;
    Solution committed_absolute_solution;

    auto collect_activated_robot_indices =
        [&](const std::vector<std::string>& event_ids) {
            std::unordered_set<std::string> activated(event_ids.begin(), event_ids.end());
            std::vector<int> indices;
            for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
                if (!robots[i].active_event_id.has_value()) continue;
                if (activated.count(*robots[i].active_event_id) == 0) continue;
                indices.push_back(i);
            }
            return indices;
        };

    auto build_request = [&](PlanMode mode,
                             const std::vector<int>& affected_robot_indices) {
        PlanningRequest req;
        req.base_timestep = current_t;
        req.mode = mode;
        req.params = params.planner;
        req.snapshot = FleetSnapshot{};
        req.snapshot->current_time = current_t;
        req.robot_ids.reserve(affected_robot_indices.size());
        req.starts.reserve(affected_robot_indices.size());
        req.goals.reserve(affected_robot_indices.size());
        req.affected_robot_indices = affected_robot_indices;

        std::vector<bool> is_affected(robots.size(), false);
        for (int idx : affected_robot_indices) {
            if (idx < 0 || idx >= static_cast<int>(robots.size())) {
                throw std::runtime_error("affected robot index out of range in build_request");
            }
            is_affected[idx] = true;
        }

        for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
            const auto& robot = robots[i];
            req.snapshot->robots.push_back(robot);

            if (is_affected[i]) {
                req.robot_ids.push_back(robot.id);
                req.starts.push_back(robot.current_vertex);
                req.goals.push_back(robot.active_goal.value_or(robot.current_vertex));
                continue;
            }

            if (!committed_absolute_solution.empty()) {
                req.frozen.plan_suffixes.push_back(
                    FrozenRobotPlan{robot.id, committed_absolute_solution[i], true});
            } else {
                req.frozen.holds.push_back(
                    HoldReservation{robot.id, robot.current_vertex, current_t, std::nullopt});
            }
        }
        return req;
    };

    std::unordered_set<int> all_robot_indices;
    for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
        all_robot_indices.insert(i);
    }
    std::vector<int> all_robot_indices_vec;
    all_robot_indices_vec.reserve(robots.size());
    for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
        all_robot_indices_vec.push_back(i);
    }

    std::vector<std::string> activated_event_ids =
        activate_events(ScenarioTriggerType::AT_START, all_robot_indices);
    bool initial_phase_pending = !activated_event_ids.empty();
    if (activated_event_ids.empty()) {
        activated_event_ids =
            activate_events(ScenarioTriggerType::AFTER_INITIAL, collect_idle_robots());
    }
    if (activated_event_ids.empty()) {
        activated_event_ids =
            activate_events(ScenarioTriggerType::ON_ROBOT_IDLE, collect_idle_robots());
    }

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
        const std::vector<int> affected_robot_indices =
            collect_activated_robot_indices(activated_event_ids);
        PlanningRequest req = build_request(mode, affected_robot_indices);
        PlanResult plan_result;
        const bool can_use_initial_api =
            mode == PlanMode::INITIAL &&
            req.snapshot.has_value();
        if (can_use_initial_api) {
            plan_result = planner_.plan_initial(make_initial_request(req));
        } else {
            plan_result = planner_.replan_affected(make_affected_replan_request(req));
        }
        bool used_global_replan = false;

        const bool should_try_global_replan =
            !plan_result.solved &&
            !affected_robot_indices.empty() &&
            static_cast<int>(affected_robot_indices.size()) <
                static_cast<int>(robots.size());
        if (should_try_global_replan) {
            PlanningRequest global_req =
                build_request(PlanMode::REPLAN, all_robot_indices_vec);
            global_req.params.replan_timeout_ms =
                std::max(global_req.params.replan_timeout_ms,
                         global_req.params.initial_timeout_ms);

            PlanResult global_result =
                planner_.replan_affected(make_affected_replan_request(global_req));
            if (global_result.solved && global_result.valid) {
                req = std::move(global_req);
                plan_result = std::move(global_result);
                used_global_replan = true;
                if (plan_result.reason.empty()) {
                    plan_result.reason = "global replan after affected replan timeout";
                } else {
                    plan_result.reason =
                        "global replan after affected replan timeout; " +
                        plan_result.reason;
                }
            } else {
                req = std::move(global_req);
                plan_result = std::move(global_result);
                plan_result.status = PlanningStatus::SAFE_STOP_REQUIRED;
                plan_result.reason =
                    "affected replan failed and global replan failed";
                if (!plan_result.validation_error.empty()) {
                    plan_result.reason += ": " + plan_result.validation_error;
                }
                for (auto& robot : robots) {
                    robot.held = true;
                }

                EpisodeResult episode;
                episode.index = episode_index++;
                episode.activated_event_ids = activated_event_ids;
                episode.request = summarize_request(req);
                episode.result = std::move(plan_result);
                episode.executed_until_timestep = 0;
                episode.used_global_replan = true;
                run_result.episodes.push_back(std::move(episode));
                run_result.final_robots = robots;
                run_result.consumed_events = count_consumed();
                run_result.safe_stop_required = true;
                run_result.stop_reason = run_result.episodes.back().result.reason;
                return run_result;
            }
        }

        if (!plan_result.solved) {
            plan_result.status = PlanningStatus::SAFE_STOP_REQUIRED;
            plan_result.reason = "planner failed without recoverable global replan";
            if (!plan_result.validation_error.empty()) {
                plan_result.reason += ": " + plan_result.validation_error;
            }
            for (auto& robot : robots) {
                robot.held = true;
            }

            EpisodeResult episode;
            episode.index = episode_index++;
            episode.activated_event_ids = activated_event_ids;
            episode.request = summarize_request(req);
            episode.result = std::move(plan_result);
            episode.executed_until_timestep = 0;
            episode.used_global_replan = used_global_replan;
            run_result.episodes.push_back(std::move(episode));
            run_result.final_robots = robots;
            run_result.consumed_events = count_consumed();
            run_result.safe_stop_required = true;
            run_result.stop_reason = run_result.episodes.back().result.reason;
            return run_result;
        }

        plan_result.planning_solution =
            compose_full_solution(robots,
                                  req.affected_robot_indices,
                                  plan_result.planning_solution,
                                  committed_absolute_solution,
                                  current_t);

        {
            std::vector<int> starts;
            std::vector<int> goals;
            starts.reserve(robots.size());
            goals.reserve(robots.size());
            for (const auto& robot : robots) {
                starts.push_back(robot.current_vertex);
                goals.push_back(robot.active_goal.value_or(robot.current_vertex));
            }

            Instance full_instance = map.make_instance_from_planning(starts, goals);
            plan_result.valid = validate_physical_solution(full_instance,
                                                           plan_result.planning_solution,
                                                           &plan_result.validation_error);
            if (!plan_result.valid) {
                throw std::runtime_error(
                    "lifelong planning produced invalid merged solution: " +
                    plan_result.validation_error);
            }
            plan_result.metrics.soc =
                compute_sum_of_costs(full_instance, plan_result.planning_solution);
            plan_result.metrics.makespan =
                compute_makespan(plan_result.planning_solution);
            plan_result.cost = plan_result.metrics.soc;
            plan_result.makespan = plan_result.metrics.makespan;
        }

        int execute_until = plan_result.makespan;
        if (!(initial_phase_pending && has_after_initial_events)) {
            int earliest_idle_trigger = std::numeric_limits<int>::max();
            for (int i = 0; i < static_cast<int>(robots.size()); ++i) {
                if (!robots[i].active_goal.has_value()) continue;
                if (robots[i].current_vertex == *robots[i].active_goal) continue;
                if (!ready_idle_event_exists(i)) continue;

                const int arrival_t =
                    first_arrival_time(plan_result.planning_solution[i], *robots[i].active_goal);
                earliest_idle_trigger = std::min(earliest_idle_trigger, arrival_t);
            }
            if (earliest_idle_trigger != std::numeric_limits<int>::max()) {
                execute_until = earliest_idle_trigger;
            }
        }

        committed_absolute_solution =
            shift_solution(plan_result.planning_solution, current_t);

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
                if (robots[i].active_event_id.has_value()) {
                    completed_event_ids.insert(*robots[i].active_event_id);
                    robots[i].active_event_id.reset();
                }
                robots[i].active_goal.reset();
                idle_now.insert(i);
            }
        }

        EpisodeResult episode;
        episode.index = episode_index++;
        episode.activated_event_ids = activated_event_ids;
        episode.request = summarize_request(req);
        episode.result = std::move(plan_result);
        episode.executed_until_timestep = execute_until;
        episode.used_global_replan = used_global_replan;
        run_result.episodes.push_back(std::move(episode));

        activated_event_ids.clear();
        if (initial_phase_pending && !has_any_active_goal()) {
            initial_phase_pending = false;
            activated_event_ids =
                activate_events(ScenarioTriggerType::AFTER_INITIAL, collect_idle_robots());
        }
        if (activated_event_ids.empty()) {
            activated_event_ids =
                activate_events(ScenarioTriggerType::ON_ROBOT_IDLE, idle_now);
        }
        current_t += execute_until;
        first_episode = false;
        const bool will_continue = has_any_active_goal() || !activated_event_ids.empty();
        capture_frame(will_continue ? episode_index : (episode_index - 1), activated_event_ids);
    }

    run_result.final_robots = std::move(robots);
    run_result.consumed_events = count_consumed();
    return run_result;
}
