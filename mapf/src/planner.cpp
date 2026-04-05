#include "planner.hpp"

#include "solution_validation.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#include <spdlog/spdlog.h>

// ============================================================
//  생성자 / 소멸자
// ============================================================
Planner::Planner(const Instance& ins,
                 const DistTable& D,
                 StopCondition& stop,
                 std::mt19937& rng,
                 PlannerParams params,
                 const ReservationTable* reservations)
    : ins(ins), D(D), stop(stop), rng(rng), params(params),
      reservations(reservations),
      pibt(ins.G, D, collision, rng),
      best_cost(std::numeric_limits<int>::max()),
      forced_scratch(ins.num_agents(), nullptr)
{
    constraints_scratch.reserve(ins.num_agents());
}

Planner::~Planner()
{
    for (auto* n : all_hnodes) delete n;
    for (auto* n : all_lnodes) delete n;
}

namespace {

OpenEntry make_open_entry(HNode* node)
{
    return OpenEntry{node, node->g_val, node->h_val, node->f_val()};
}

}  // namespace

// ============================================================
//  solve
//
//  prioritized fallback 없이 primary LaCAM 탐색만 수행한다.
//  timeout이 발생하더라도 valid incumbent가 있으면 그것을 반환하고,
//  없으면 빈 Solution 을 반환한다.
// ============================================================
Solution Planner::solve()
{
    const int n = ins.num_agents();
    stats = PlannerStats{};
    const int start_h = heuristic(ins.starts);

    auto log_stats = [&]() {
        spdlog::debug(
            "planner stats: hl_expansions={}, pibt_calls={}, pibt_ms={:.3f}, "
            "goal_validations={}",
            stats.hl_expansions,
            stats.pibt_calls,
            stats.pibt_ms,
            stats.goal_validations);
    };

    HNode* start = new HNode(ins.starts, 0, start_h, nullptr);
    all_hnodes.push_back(start);
    visited[ins.starts] = start;

    auto* ll_root = new LNode(-1, nullptr, 0, nullptr);
    all_lnodes.push_back(ll_root);
    start->ll_queue.push(ll_root);

    open.push(make_open_entry(start));

    if (start_h == 0) {
        Solution sol = to_solution({ins.starts});
        if (reservations != nullptr) {
            std::string error;
            if (!reservations->validate_solution(sol, &error)) {
                spdlog::warn("start-state solution conflicts with reservations: {}", error);
                log_stats();
                return {};
            }
        }
        stop.notify_solution(0);
        log_stats();
        return sol;
    }

    if (start_h < std::numeric_limits<int>::max()) {
        stop.set_lower_bound(start_h);
    }
    spdlog::debug("planner start: agents={}, lower_bound={}", n, start_h);

    while (!open.empty() && !stop.should_abort()) {
        const OpenEntry entry = open.top();
        open.pop();
        HNode* h = entry.node;

        if (h == nullptr) continue;
        if (entry.g_snapshot != h->g_val ||
            entry.h_snapshot != h->h_val ||
            entry.f_snapshot != h->f_val()) {
            continue;
        }

        bool at_goal = true;
        for (int i = 0; i < n; ++i) {
            if (h->C[i] != ins.goals[i]) {
                at_goal = false;
                break;
            }
        }

        if (at_goal) {
            ++stats.goal_validations;
            Solution sol = extract_solution(h);
            if (!validate_physical_solution(ins, sol)) {
                spdlog::warn("discarding invalid goal-state solution at depth={}", h->g_val);
                continue;
            }
            if (reservations != nullptr) {
                std::string reservation_error;
                if (!reservations->validate_solution(sol, &reservation_error)) {
                    spdlog::debug("discarding reservation-conflicting solution at depth={}: {}",
                                  h->g_val,
                                  reservation_error);
                    continue;
                }
            }

            const int cost = sum_of_costs(sol);
            if (cost < best_cost) {
                best_cost = cost;
                best_solution = std::move(sol);
                stop.notify_solution(cost);
                spdlog::info("valid solution candidate found: cost={}, makespan={}",
                             cost, h->g_val);
            }

            if (stop.should_stop(cost)) break;
            continue;
        }

        if (best_cost < std::numeric_limits<int>::max() &&
            h->f_val() >= best_cost) {
            continue;
        }

        expand(h);
        if (!h->ll_queue.empty()) open.push(make_open_entry(h));
        ++stats.hl_expansions;
    }

    if (!best_solution.empty()) {
        if (stop.is_timeout()) {
            spdlog::info(
                "planner timed out after {} high-level expansions; returning incumbent cost={}",
                stats.hl_expansions,
                best_cost);
        } else if (stop.is_aborted()) {
            spdlog::debug(
                "planner aborted after {} high-level expansions; returning incumbent cost={}",
                stats.hl_expansions,
                best_cost);
        } else {
            spdlog::debug(
                "planner finished with valid solution after {} high-level expansions",
                stats.hl_expansions);
        }
        log_stats();
        return best_solution;
    }

    if (stop.is_timeout()) {
        spdlog::warn(
            "planner timed out without a valid solution after {} high-level expansions",
            stats.hl_expansions);
    } else if (stop.is_aborted()) {
        spdlog::debug(
            "planner aborted without a valid solution after {} high-level expansions",
            stats.hl_expansions);
    } else {
        spdlog::warn(
            "planner exhausted the search space without a valid solution after {} high-level expansions",
            stats.hl_expansions);
    }
    log_stats();
    return {};
}

// ============================================================
//  heuristic
//  sum of BFS distances (admissible)
// ============================================================
int Planner::heuristic(const Config& C) const
{
    int h = 0;
    for (int i = 0; i < static_cast<int>(C.size()); ++i) {
        const int d = D.get(i, C[i]->id);
        if (d == DIST_INF) return std::numeric_limits<int>::max();
        h += d;
    }
    return h;
}

// ============================================================
//  get_or_create_hnode
// ============================================================
HNode* Planner::get_or_create_hnode(const Config& C,
                                    int g,
                                    HNode* parent)
{
    auto it = visited.find(C);
    if (it != visited.end()) {
        HNode* existing = it->second;
        if (g < existing->g_val) {
            existing->g_val = g;
            existing->parent = parent;
            open.push(make_open_entry(existing));
        }
        return existing;
    }

    const int h = heuristic(C);
    auto* node = new HNode(C, g, h, parent);
    all_hnodes.push_back(node);
    visited[C] = node;
    open.push(make_open_entry(node));

    auto* ll_root = new LNode(-1, nullptr, 0, nullptr);
    all_lnodes.push_back(ll_root);
    node->ll_queue.push(ll_root);

    return node;
}

// ============================================================
//  expand
// ============================================================
bool Planner::expand(HNode* h)
{
    if (h->ll_queue.empty()) return false;

    LNode* ll = h->ll_queue.front();
    h->ll_queue.pop();

    {
        std::fill(forced_scratch.begin(), forced_scratch.end(), nullptr);
        constraints_scratch.clear();
        LNode* cur = ll;
        while (cur != nullptr && cur->who != -1) {
            if (forced_scratch[cur->who] == nullptr) {
                forced_scratch[cur->who] = cur->where;
            }
            cur = cur->parent;
        }

        for (int i = 0; i < ins.num_agents(); ++i) {
            if (forced_scratch[i] != nullptr) {
                constraints_scratch.push_back({i, forced_scratch[i]});
            }
        }
    }

    collision.clear();
    if (h->parent != nullptr) {
        for (int i = 0; i < ins.num_agents(); ++i) {
            collision.record_edge(h->parent->C[i]->id,
                                  h->C[i]->id,
                                  h->g_val);
        }
    }

    Config C_next(ins.num_agents(), nullptr);
    const int t = h->g_val + 1;
    PIBTFailure pibt_failure;

    const auto pibt_begin = std::chrono::steady_clock::now();
    const bool config_ok =
        pibt.set_new_config(h->C,
                            C_next,
                            constraints_scratch,
                            t,
                            reservations,
                            &pibt_failure);
    const auto pibt_end = std::chrono::steady_clock::now();
    stats.pibt_ms += std::chrono::duration<double, std::milli>(
                         pibt_end - pibt_begin).count();
    ++stats.pibt_calls;

    auto enqueue_constraint = [&](std::queue<LNode*>& queue, int agent, Vertex* where) {
        if (agent < 0 || agent >= ins.num_agents() || where == nullptr) return false;
        auto* child = new LNode(agent, where, ll->depth + 1, ll);
        all_lnodes.push_back(child);
        queue.push(child);
        return true;
    };

    for (int i = 0; i < ins.num_agents(); ++i) {
        if (C_next[i] == nullptr) {
            if (pibt_failure.primary_agent >= 0) {
                const int agent = pibt_failure.primary_agent;
                enqueue_constraint(h->ll_queue, agent, h->C[agent]);
            }
            return false;
        }
    }

    if (!config_ok) {
        const int n = ins.num_agents();
        bool branched = false;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                const bool vc = (C_next[i] == C_next[j]);
                const bool sc = (h->C[i] == C_next[j] && h->C[j] == C_next[i]);
                if (vc || sc) {
                    branched |= enqueue_constraint(h->ll_queue, i, C_next[i]);
                    branched |= enqueue_constraint(h->ll_queue, j, C_next[j]);
                }
            }
        }
        if (!branched) {
            switch (pibt_failure.kind) {
            case PIBTFailureKind::CONSTRAINT_CONFLICT:
            case PIBTFailureKind::RESERVATION_VERTEX:
            case PIBTFailureKind::RESERVATION_MOVE:
            case PIBTFailureKind::NON_PASSING:
            case PIBTFailureKind::WAIT_BLOCKED:
                if (pibt_failure.primary_agent >= 0) {
                    const int agent = pibt_failure.primary_agent;
                    branched |= enqueue_constraint(h->ll_queue, agent, h->C[agent]);
                }
                break;
            case PIBTFailureKind::VERTEX_CONFLICT:
            case PIBTFailureKind::SWAP_CONFLICT:
                if (pibt_failure.primary_agent >= 0) {
                    const int agent = pibt_failure.primary_agent;
                    branched |= enqueue_constraint(h->ll_queue, agent, C_next[agent]);
                }
                if (pibt_failure.secondary_agent >= 0) {
                    const int agent = pibt_failure.secondary_agent;
                    branched |= enqueue_constraint(h->ll_queue, agent, C_next[agent]);
                }
                break;
            case PIBTFailureKind::NONE:
                break;
            }
        }
        return false;
    }

    HNode* next = get_or_create_hnode(C_next, t, h);

    const int n = ins.num_agents();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (C_next[i] == C_next[j]) {
                enqueue_constraint(next->ll_queue, i, C_next[i]);
                enqueue_constraint(next->ll_queue, j, C_next[j]);
            }
            if (h->C[i] == C_next[j] && h->C[j] == C_next[i]) {
                enqueue_constraint(next->ll_queue, i, C_next[i]);
                enqueue_constraint(next->ll_queue, j, C_next[j]);
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (h->C[i] == C_next[i]) continue;
        if (!collision.is_non_passing_free(h->C[i]->id, C_next[i]->id, t)) {
            enqueue_constraint(next->ll_queue, i, C_next[i]);
        }
    }

    return true;
}

// ============================================================
//  extract_solution
// ============================================================
Solution Planner::extract_solution(HNode* h) const
{
    const int n = ins.num_agents();

    std::vector<HNode*> chain;
    for (HNode* cur = h; cur != nullptr; cur = cur->parent) {
        chain.push_back(cur);
    }
    std::reverse(chain.begin(), chain.end());

    const int T = static_cast<int>(chain.size());

    RawPaths raw(n);
    for (int i = 0; i < n; ++i) {
        raw[i].reserve(T);
        for (HNode* node : chain) {
            raw[i].push_back(node->C[i]);
        }
    }

    return to_solution(raw);
}

// ============================================================
//  sum_of_costs
// ============================================================
int Planner::sum_of_costs(const Solution& sol) const
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
