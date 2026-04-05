#include "lifelong_planner.hpp"

#include "mapf_runtime.hpp"
#include "planner.hpp"
#include "planning_request.hpp"
#include "reservation_table.hpp"
#include "solution_validation.hpp"

#include <algorithm>
#include <atomic>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <thread>
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

int objective_value(const PlanMetrics& metrics, ObjectiveType objective)
{
    return objective == ObjectiveType::MAKESPAN ? metrics.makespan : metrics.soc;
}

bool better_metrics(const PlanMetrics& lhs,
                    const PlanMetrics& rhs,
                    ObjectiveType objective)
{
    const int lhs_primary = objective_value(lhs, objective);
    const int rhs_primary = objective_value(rhs, objective);
    if (lhs_primary != rhs_primary) return lhs_primary < rhs_primary;

    if (objective == ObjectiveType::MAKESPAN) return lhs.soc < rhs.soc;
    return lhs.makespan < rhs.makespan;
}

struct WorkerAttempt {
    Solution solution;
    PlanMetrics metrics;
    bool solved = false;
    bool valid = false;
    bool timed_out = false;
    bool aborted = false;
    std::string validation_error;
};

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
            std::ostringstream oss;
            oss << "duplicate " << what << " detected: " << value;
            throw std::runtime_error(oss.str());
        }
    }
}

std::vector<int> all_robot_indices(const FleetSnapshot& snapshot)
{
    std::vector<int> indices;
    indices.reserve(snapshot.robots.size());
    for (int i = 0; i < static_cast<int>(snapshot.robots.size()); ++i) {
        indices.push_back(i);
    }
    return indices;
}

PlanningRequest build_request_from_snapshot(const FleetSnapshot& snapshot,
                                           const std::vector<GoalAssignment>& assignments,
                                           const std::vector<int>& affected_robot_indices,
                                           const FrozenPlanSet& frozen,
                                           const PlannerParams& default_params,
                                           const std::optional<PlannerParams>& params_override,
                                           PlanMode mode)
{
    PlanningRequest req;
    req.base_timestep = snapshot.current_time;
    req.mode = mode;
    req.params = params_override.value_or(default_params);
    req.snapshot = snapshot;
    req.affected_robot_indices = affected_robot_indices;
    req.frozen = frozen;

    std::unordered_map<std::string, int> robot_index;
    robot_index.reserve(snapshot.robots.size());
    std::vector<std::optional<int>> goals;
    goals.reserve(snapshot.robots.size());

    for (int i = 0; i < static_cast<int>(snapshot.robots.size()); ++i) {
        const auto& robot = snapshot.robots[i];
        if (!robot_index.emplace(robot.id, i).second) {
            throw std::runtime_error("duplicate robot id in fleet snapshot: " + robot.id);
        }
        goals.push_back(robot.active_goal);
    }

    std::unordered_set<std::string> assigned_robot_ids;
    for (const auto& assignment : assignments) {
        const auto index_it = robot_index.find(assignment.robot_id);
        if (index_it == robot_index.end()) {
            throw std::runtime_error("assignment references unknown robot: " +
                                     assignment.robot_id);
        }
        if (!assigned_robot_ids.insert(assignment.robot_id).second) {
            throw std::runtime_error("duplicate assignment for robot: " +
                                     assignment.robot_id);
        }
        goals[index_it->second] = assignment.goal;
    }

    req.robot_ids.reserve(affected_robot_indices.size());
    req.starts.reserve(affected_robot_indices.size());
    req.goals.reserve(affected_robot_indices.size());
    for (int idx : affected_robot_indices) {
        if (idx < 0 || idx >= static_cast<int>(snapshot.robots.size())) {
            throw std::runtime_error("affected robot index out of range");
        }
        const auto& robot = snapshot.robots[idx];
        req.robot_ids.push_back(robot.id);
        req.starts.push_back(robot.current_vertex);
        req.goals.push_back(goals[idx].value_or(robot.current_vertex));
    }

    return req;
}

}  // namespace

MapfPlanner::MapfPlanner(PreparedMap map, Params params)
    : map_(std::move(map)),
      params_(std::move(params)),
      goal_cache_(map_.planning_graph())
{}

PlanResult MapfPlanner::plan(const PlanningRequest& req)
{
    if (req.robot_ids.size() != req.starts.size() ||
        req.starts.size() != req.goals.size()) {
        throw std::runtime_error("planning request size mismatch");
    }

    PlanResult result;
    result.status = PlanningStatus::INVALID_REQUEST;
    if (req.robot_ids.empty()) {
        result.status = PlanningStatus::SOLVED;
        result.solved = true;
        result.valid = true;
        return result;
    }

    expect_unique(req.starts, "planning start vertex");
    expect_unique(req.goals, "planning goal vertex");
    expect_unique(req.affected_robot_indices, "affected robot index");
    const int affected_index_bound = req.snapshot.has_value()
                                         ? static_cast<int>(req.snapshot->robots.size())
                                         : static_cast<int>(req.affected_robot_indices.size());
    for (int idx : req.affected_robot_indices) {
        if (idx < 0 || idx >= affected_index_bound) {
            throw std::runtime_error("affected robot index out of range");
        }
    }
    if (req.snapshot.has_value() && req.snapshot->current_time != req.base_timestep) {
        throw std::runtime_error("planning request snapshot time does not match base_timestep");
    }

    ReservationTable reservations;
    reservations.reserve_all(req.frozen, req.base_timestep);

    if (!reservations.empty()) {
        Solution start_solution(req.starts.size());
        for (int i = 0; i < static_cast<int>(req.starts.size()); ++i) {
            start_solution[i].push_back({req.starts[i], 0});
        }

        if (!reservations.validate_solution(start_solution, &result.validation_error)) {
            result.reason = "planning request conflicts with frozen reservations";
            return result;
        }
    }

    Instance ins = map_.make_instance_from_planning(req.starts, req.goals);
    DistTable D = goal_cache_.build_dist_table(ins.goals);
    int worker_count = std::max(1, req.params.parallel_search_workers);
    const unsigned hw = std::thread::hardware_concurrency();
    if (hw > 0) {
        worker_count = std::min(worker_count, static_cast<int>(hw));
    }

    const auto shared_abort = std::make_shared<std::atomic_bool>(false);
    std::vector<WorkerAttempt> attempts(static_cast<std::size_t>(worker_count));
    std::vector<std::thread> workers;
    workers.reserve(worker_count);

    for (int worker_index = 0; worker_index < worker_count; ++worker_index) {
        workers.emplace_back([&, worker_index]() {
            WorkerAttempt attempt;

            StopCondition stop(req.mode);
            apply_stop_params(stop, req.params);
            stop.bind_abort_flag(shared_abort);

            PlannerParams worker_params = req.params;
            worker_params.seed = req.params.seed + worker_index * 7919;

            std::mt19937 rng(static_cast<unsigned>(worker_params.seed));
            Planner planner(ins,
                            D,
                            stop,
                            rng,
                            worker_params,
                            reservations.empty() ? nullptr : &reservations);

            attempt.solution = planner.solve();
            attempt.timed_out = stop.is_timeout();
            attempt.aborted = stop.is_aborted() && !attempt.timed_out;
            attempt.solved = !attempt.solution.empty();

            if (!attempt.solved) {
                attempts[static_cast<std::size_t>(worker_index)] = std::move(attempt);
                return;
            }

            attempt.valid = validate_physical_solution(ins,
                                                       attempt.solution,
                                                       &attempt.validation_error);
            if (!attempt.valid) {
                attempts[static_cast<std::size_t>(worker_index)] = std::move(attempt);
                return;
            }

            if (!reservations.empty() &&
                !reservations.validate_solution(attempt.solution,
                                               &attempt.validation_error)) {
                attempt.valid = false;
                attempts[static_cast<std::size_t>(worker_index)] = std::move(attempt);
                return;
            }

            attempt.metrics.soc = compute_sum_of_costs(ins, attempt.solution);
            attempt.metrics.makespan = compute_makespan(attempt.solution);
            attempts[static_cast<std::size_t>(worker_index)] = attempt;

            if (!attempt.timed_out) {
                shared_abort->store(true);
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }

    const WorkerAttempt* best_attempt = nullptr;
    bool any_timed_out = false;
    bool any_aborted = false;
    for (const auto& attempt : attempts) {
        any_timed_out = any_timed_out || attempt.timed_out;
        any_aborted = any_aborted || attempt.aborted;
        if (!attempt.solved || !attempt.valid) continue;

        if (best_attempt == nullptr ||
            better_metrics(attempt.metrics, best_attempt->metrics, req.params.objective)) {
            best_attempt = &attempt;
        }
    }

    if (best_attempt == nullptr) {
        result.timed_out = any_timed_out;
        result.status = any_timed_out
                            ? PlanningStatus::TIMEOUT_NO_SOLUTION
                            : PlanningStatus::UNSAT;
        result.validation_error = "planner returned no solution";
        for (const auto& attempt : attempts) {
            if (!attempt.validation_error.empty()) {
                result.validation_error = attempt.validation_error;
                break;
            }
        }
        result.reason = any_timed_out
                            ? "timeout without valid solution"
                            : (any_aborted
                                   ? "parallel search aborted without valid solution"
                                   : "search exhausted without valid solution");
        return result;
    }

    result.planning_solution = best_attempt->solution;
    result.solved = true;
    result.valid = true;
    result.timed_out = best_attempt->timed_out;
    result.metrics = best_attempt->metrics;
    result.cost = result.metrics.soc;
    result.makespan = result.metrics.makespan;
    result.status = result.timed_out
                        ? PlanningStatus::TIMEOUT_WITH_SOLUTION
                        : PlanningStatus::SOLVED;
    result.reason = result.timed_out
                        ? "timeout with incumbent solution"
                        : "valid solution";
    return result;
}

PlanResult MapfPlanner::plan_initial(const InitialPlanRequest& req)
{
    return plan(build_request_from_snapshot(req.snapshot,
                                           req.assignments,
                                           all_robot_indices(req.snapshot),
                                           FrozenPlanSet{},
                                           params_.planner,
                                           req.params_override,
                                           PlanMode::INITIAL));
}

PlanResult MapfPlanner::replan_affected(const AffectedReplanRequest& req)
{
    return plan(build_request_from_snapshot(req.snapshot,
                                           req.assignments,
                                           req.affected_robot_indices,
                                           req.frozen,
                                           params_.planner,
                                           req.params_override,
                                           PlanMode::REPLAN));
}

ScenarioRunResult MapfPlanner::run(const LifelongScenario& scenario)
{
    return ScenarioRuntime(*this).run(scenario);
}
