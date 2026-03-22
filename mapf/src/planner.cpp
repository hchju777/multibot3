#include "planner.hpp"
#include "logger.hpp"
#include "solution_validation.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <future>
#include <numeric>
#include <unordered_set>
#include <spdlog/spdlog.h>

namespace {

using DirectedEdgeKey = std::uint64_t;

DirectedEdgeKey make_edge_key(int from_id, int to_id)
{
    return (static_cast<DirectedEdgeKey>(static_cast<std::uint32_t>(from_id)) << 32) |
           static_cast<std::uint32_t>(to_id);
}

struct AStarState {
    int f;
    int g;
    int vertex_id;
    int time;

    bool operator>(const AStarState& other) const
    {
        if (f != other.f) return f > other.f;
        if (g != other.g) return g > other.g;
        if (vertex_id != other.vertex_id) return vertex_id > other.vertex_id;
        return time > other.time;
    }
};

struct FallbackResult {
    Solution solution;
    int cost = std::numeric_limits<int>::max();
    int expanded_nodes = 0;
    int planned_agents = 0;
    double elapsed_ms = 0.0;
};

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

FallbackResult run_prioritized_fallback(const Instance& ins,
                                        const DistTable& D,
                                        std::atomic<bool>* cancel_flag)
{
    const auto fallback_begin = std::chrono::steady_clock::now();
    FallbackResult result;

    auto should_cancel = [&]() {
        return cancel_flag != nullptr &&
               cancel_flag->load(std::memory_order_relaxed);
    };

    const int n = ins.num_agents();
    const int m = ins.G.num_vertices();
    const int horizon = std::max(32, 4 * m + 4 * n);
    constexpr int kBranchLimit = 6;
    constexpr int kNodeLimit   = 100000;

    using VertexReservations = std::vector<std::vector<bool>>;
    using EdgeReservations   = std::vector<std::unordered_set<DirectedEdgeKey>>;
    struct ReservationDelta {
        std::vector<std::pair<int, int>> vertices;
        std::vector<std::pair<int, DirectedEdgeKey>> same_time_edges;
        std::vector<std::pair<int, DirectedEdgeKey>> next_time_edges;
    };

    auto plan_agent = [&](int agent,
                          const VertexReservations& vertex_reserved,
                          const EdgeReservations& same_time_reverse,
                          const EdgeReservations& next_time_reverse,
                          RawPath& out) -> bool {
        const int total_states = (horizon + 1) * m;
        const int start_id = ins.starts[agent]->id;
        const int goal_id  = ins.goals[agent]->id;

        auto idx = [&](int vertex_id, int time) {
            return time * m + vertex_id;
        };

        std::vector<int> best(total_states, DIST_INF);
        std::vector<int> parent(total_states, -1);
        std::priority_queue<AStarState,
                            std::vector<AStarState>,
                            std::greater<AStarState>> open_states;

        const int start_state = idx(start_id, 0);
        best[start_state] = 0;
        open_states.push(AStarState{D.get(agent, start_id), 0, start_id, 0});

        while (!open_states.empty()) {
            if (should_cancel()) return false;

            const AStarState cur = open_states.top();
            open_states.pop();

            const int cur_index = idx(cur.vertex_id, cur.time);
            if (cur.g != best[cur_index]) continue;

            if (cur.vertex_id == goal_id) {
                bool goal_can_remain_occupied = true;
                for (int future_t = cur.time + 1; future_t <= horizon; ++future_t) {
                    if (vertex_reserved[future_t][goal_id]) {
                        goal_can_remain_occupied = false;
                        break;
                    }
                }
                if (!goal_can_remain_occupied) continue;

                out.clear();
                int state = cur_index;
                while (state != -1) {
                    const int vid = state % m;
                    out.push_back(ins.G.V[vid]);
                    state = parent[state];
                }
                std::reverse(out.begin(), out.end());
                ++result.planned_agents;
                return true;
            }

            if (cur.time >= horizon) continue;

            auto try_push = [&](int next_vid) {
                const int next_time = cur.time + 1;
                if (vertex_reserved[next_time][next_vid]) return;

                const DirectedEdgeKey move_key =
                    make_edge_key(cur.vertex_id, next_vid);
                if (same_time_reverse[next_time].count(move_key)) return;
                if (next_time_reverse[next_time].count(move_key)) return;

                const int next_g = cur.g + 1;
                const int next_index = idx(next_vid, next_time);
                if (next_g >= best[next_index]) return;

                best[next_index] = next_g;
                parent[next_index] = cur_index;
                open_states.push(AStarState{
                    next_g + D.get(agent, next_vid),
                    next_g,
                    next_vid,
                    next_time,
                });
            };

            for (Vertex* nb : ins.G.V[cur.vertex_id]->neighbors) {
                try_push(nb->id);
            }
            try_push(cur.vertex_id);  // wait
        }

        return false;
    };

    auto reserve_path = [&](const RawPath& path,
                            VertexReservations& vertex_reserved,
                            EdgeReservations& same_time_reverse,
                            EdgeReservations& next_time_reverse,
                            ReservationDelta& delta) {
        delta.vertices.clear();
        delta.same_time_edges.clear();
        delta.next_time_edges.clear();
        for (int t = 0; t < static_cast<int>(path.size()); ++t) {
            const int vid = path[t]->id;
            vertex_reserved[t][vid] = true;
            delta.vertices.emplace_back(t, vid);

            if (t == 0) continue;
            const int from_id = path[t - 1]->id;
            const int to_id   = path[t]->id;
            if (from_id == to_id) continue;

            const DirectedEdgeKey reverse_key = make_edge_key(to_id, from_id);
            if (same_time_reverse[t].insert(reverse_key).second) {
                delta.same_time_edges.emplace_back(t, reverse_key);
            }
            if (next_time_reverse[t + 1].insert(reverse_key).second) {
                delta.next_time_edges.emplace_back(t + 1, reverse_key);
            }
        }

        for (int t = static_cast<int>(path.size()); t <= horizon; ++t) {
            vertex_reserved[t][path.back()->id] = true;
            delta.vertices.emplace_back(t, path.back()->id);
        }
    };

    auto unreserve_path = [&](const ReservationDelta& delta,
                              VertexReservations& vertex_reserved,
                              EdgeReservations& same_time_reverse,
                              EdgeReservations& next_time_reverse) {
        for (auto it = delta.vertices.rbegin(); it != delta.vertices.rend(); ++it) {
            vertex_reserved[it->first][it->second] = false;
        }
        for (auto it = delta.same_time_edges.rbegin();
             it != delta.same_time_edges.rend(); ++it) {
            same_time_reverse[it->first].erase(it->second);
        }
        for (auto it = delta.next_time_edges.rbegin();
             it != delta.next_time_edges.rend(); ++it) {
            next_time_reverse[it->first].erase(it->second);
        }
    };

    RawPaths raw(n);
    const std::uint64_t full_mask =
        (n >= 64) ? std::numeric_limits<std::uint64_t>::max()
                  : ((std::uint64_t{1} << n) - 1);

    auto dfs = [&](auto&& self,
                   std::uint64_t assigned_mask,
                   VertexReservations& vertex_reserved,
                   EdgeReservations& same_time_reverse,
                   EdgeReservations& next_time_reverse) -> bool {
        if (should_cancel()) return false;
        if (++result.expanded_nodes > kNodeLimit) return false;
        if (assigned_mask == full_mask) return true;

        std::vector<int> remaining;
        for (int i = 0; i < n; ++i) {
            if ((assigned_mask & (std::uint64_t{1} << i)) == 0) {
                remaining.push_back(i);
            }
        }

        std::sort(remaining.begin(), remaining.end(),
                  [&](int a, int b) {
                      const int da = D.get(a, ins.starts[a]->id);
                      const int db = D.get(b, ins.starts[b]->id);
                      if (da != db) return da > db;
                      return a < b;
                  });
        if (static_cast<int>(remaining.size()) > kBranchLimit) {
            remaining.resize(kBranchLimit);
        }

        for (int agent : remaining) {
            if (should_cancel()) return false;

            RawPath path;
            if (!plan_agent(agent, vertex_reserved,
                            same_time_reverse, next_time_reverse, path)) {
                continue;
            }

            raw[agent] = path;
            ReservationDelta delta;
            reserve_path(path, vertex_reserved,
                         same_time_reverse, next_time_reverse, delta);
            if (self(self,
                     assigned_mask | (std::uint64_t{1} << agent),
                     vertex_reserved,
                     same_time_reverse,
                     next_time_reverse)) {
                return true;
            }
            unreserve_path(delta, vertex_reserved, same_time_reverse, next_time_reverse);
            raw[agent].clear();
        }

        return false;
    };

    VertexReservations vertex_reserved(
        horizon + 1, std::vector<bool>(m, false));
    EdgeReservations same_time_reverse(horizon + 1);
    EdgeReservations next_time_reverse(horizon + 2);

    if (dfs(dfs, 0, vertex_reserved, same_time_reverse, next_time_reverse)) {
        Solution sol = to_solution(raw);
        if (validate_physical_solution(ins, sol)) {
            result.solution = std::move(sol);
            result.cost = compute_sum_of_costs(ins, result.solution);
        }
    }

    result.elapsed_ms = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - fallback_begin).count();
    return result;
}

}  // namespace

// ============================================================
//  생성자 / 소멸자
// ============================================================
Planner::Planner(const Instance& ins,
                 const DistTable& D,
                 StopCondition& stop,
                 std::mt19937& rng,
                 PlannerParams params)
    : ins(ins), D(D), stop(stop), rng(rng), params(params),
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

// ============================================================
//  solve
//
//  흐름:
//    1. 시작 HNode 생성 → OPEN push
//    2. OPEN 이 빌 때까지 또는 stop 조건 만족까지 반복
//       a. HNode pop
//       b. goal 도달 검사
//       c. expand(h) → 새 Config 생성 → 새 HNode push
//    3. best_solution 반환
// ============================================================
Solution Planner::solve()
{
    const int n = ins.num_agents();
    stats = PlannerStats{};
    const int start_h = heuristic(ins.starts);
    std::atomic<bool> cancel_async_fallback{false};
    std::future<FallbackResult> async_fallback;
    bool async_fallback_started = false;
    bool async_fallback_ready_for_initial = false;
    int async_fallback_candidate_cost = std::numeric_limits<int>::max();
    double initial_grace_deadline_ms = std::numeric_limits<double>::infinity();
    const bool async_fallback_enabled =
        (stop.mode == PlanMode::INITIAL)
            ? params.initial_async_fallback
            : params.replan_async_fallback;
    const int async_fallback_delay_ms =
        (stop.mode == PlanMode::INITIAL)
            ? params.initial_async_fallback_delay_ms
            : params.replan_async_fallback_delay_ms;
    auto log_stats = [&]() {
        spdlog::debug(
            "planner stats: hl_expansions={}, pibt_calls={}, pibt_ms={:.3f}, "
            "goal_validations={}, fallback_expanded_nodes={}, fallback_planned_agents={}, "
            "fallback_ms={:.3f}",
            stats.hl_expansions,
            stats.pibt_calls,
            stats.pibt_ms,
            stats.goal_validations,
            stats.fallback_expanded_nodes,
            stats.fallback_planned_agents,
            stats.fallback_ms);
    };
    auto take_fallback_result = [&](FallbackResult result,
                                    const char* source,
                                    bool return_immediately,
                                    bool mark_initial_grace) {
        if (result.solution.empty()) return false;
        stats.fallback_expanded_nodes = result.expanded_nodes;
        stats.fallback_planned_agents = result.planned_agents;
        stats.fallback_ms = result.elapsed_ms;

        if (best_solution.empty() || result.cost < best_cost) {
            best_solution = std::move(result.solution);
            best_cost = result.cost;
            stop.notify_solution(best_cost);
        }

        spdlog::info(
            "{} prioritized fallback produced valid solution: cost={}, expanded_nodes={}, "
            "planned_agents={}, elapsed_ms={:.3f}",
            source,
            best_cost,
            stats.fallback_expanded_nodes,
            stats.fallback_planned_agents,
            stats.fallback_ms);

        if (mark_initial_grace) {
            async_fallback_ready_for_initial = true;
            async_fallback_candidate_cost = result.cost;
            initial_grace_deadline_ms =
                stop.elapsed_ms() + static_cast<double>(params.initial_primary_grace_ms);
            spdlog::debug(
                "initial-mode fallback candidate accepted as incumbent; grace deadline at {:.3f} ms",
                initial_grace_deadline_ms);
        }
        return return_immediately && !best_solution.empty();
    };
    auto maybe_start_async_fallback = [&]() {
        if (!async_fallback_enabled || async_fallback_started || !best_solution.empty()) return;
        if (stop.elapsed_ms() < static_cast<double>(async_fallback_delay_ms)) return;
        async_fallback_started = true;
        async_fallback = std::async(std::launch::async, [&]() {
            return run_prioritized_fallback(ins, D, &cancel_async_fallback);
        });
        spdlog::debug("started async prioritized fallback after {:.3f} ms",
                      stop.elapsed_ms());
    };
    auto maybe_consume_async_fallback = [&]() -> bool {
        if (!async_fallback_started || !async_fallback.valid()) return false;
        if (async_fallback.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            return false;
        }
        FallbackResult result = async_fallback.get();
        if (stop.mode == PlanMode::REPLAN) {
            return take_fallback_result(std::move(result), "async", best_solution.empty(), false);
        }
        return take_fallback_result(std::move(result), "async", false, true);
    };
    auto finalize_async_fallback = [&]() {
        if (!async_fallback_started || !async_fallback.valid()) return;
        cancel_async_fallback.store(true, std::memory_order_relaxed);
        (void)async_fallback.get();
    };
    auto should_return_initial_after_grace = [&]() {
        if (!async_fallback_ready_for_initial) return false;
        if (best_cost < std::numeric_limits<int>::max() && stop.should_stop(best_cost)) {
            spdlog::debug("initial-mode incumbent satisfied stop condition during fallback grace");
            return true;
        }
        if (stop.elapsed_ms() >= initial_grace_deadline_ms) {
            spdlog::debug(
                "initial-mode primary grace expired; returning incumbent cost={} (fallback candidate cost={})",
                best_cost,
                async_fallback_candidate_cost);
            return true;
        }
        return false;
    };

    // 시작 configuration
    HNode* start = new HNode(ins.starts,
                             0,
                             start_h,
                             nullptr);
    all_hnodes.push_back(start);
    visited[ins.starts] = start;

    // low-level 탐색의 루트 LNode 초기화 (who=-1: 제약 없음)
    auto* ll_root = new LNode(-1, nullptr, 0, nullptr);
    all_lnodes.push_back(ll_root);
    start->ll_queue.push(ll_root);

    open.push(start);

    // 시작부터 goal 이면 즉시 반환
    if (start_h == 0) {
        stop.notify_solution(0);
        Solution sol = to_solution({ins.starts});
        log_stats();
        return sol;
    }

    // ③ quality threshold 비교 기준: heuristic(starts) = sum of BFS distances
    //    이론적 SOC 하한을 StopCondition 에 전달한다.
    {
        int lb = start_h;
        if (lb < std::numeric_limits<int>::max()) stop.set_lower_bound(lb);
        spdlog::debug("planner start: agents={}, lower_bound={}", n, lb);
    }

    while (!open.empty() && !stop.is_timeout()) {
        HNode* h = open.top();
        open.pop();

        // goal 도달 검사: 모든 에이전트가 goal 에 있는지
        bool at_goal = true;
        for (int i = 0; i < n; ++i) {
            if (h->C[i] != ins.goals[i]) { at_goal = false; break; }
        }

        if (at_goal) {
            ++stats.goal_validations;
            Solution sol = extract_solution(h);
            if (!validate_physical_solution(ins, sol)) {
                spdlog::warn("discarding invalid goal-state solution at depth={}", h->g_val);
                continue;
            }
            int cost = sum_of_costs(sol);
            if (cost < best_cost) {
                best_cost     = cost;
                best_solution = sol;
                stop.notify_solution(cost);
                spdlog::info("valid solution candidate found: cost={}, makespan={}",
                             cost, h->g_val);
            }
            if (stop.should_stop(cost)) break;
            continue;
        }

        // f-value pruning: best 보다 나쁘면 스킵
        if (best_cost < std::numeric_limits<int>::max() &&
            h->f_val() >= best_cost) {
            continue;
        }

        // 확장: low-level 큐에서 하나의 LNode 를 소비한다.
        // ll_queue 에 남은 항목이 있으면 h 를 재push 해 다른 제약 조합도 탐색.
        expand(h);
        if (!h->ll_queue.empty()) open.push(h);
        ++stats.hl_expansions;
        maybe_start_async_fallback();
        if (maybe_consume_async_fallback()) {
            spdlog::debug("async fallback won before primary search found a solution");
            log_stats();
            return best_solution;
        }
        if (stop.mode == PlanMode::INITIAL && should_return_initial_after_grace()) {
            log_stats();
            return best_solution;
        }
    }

    if (!best_solution.empty()) {
        if (async_fallback_started && async_fallback.valid()) {
            finalize_async_fallback();
        }
        spdlog::debug("planner finished with valid solution after {} high-level expansions",
                      stats.hl_expansions);
        log_stats();
        return best_solution;
    }

    if (async_fallback_started && async_fallback.valid()) {
        FallbackResult result = async_fallback.get();
        if (take_fallback_result(std::move(result), "async", true, false)) {
            log_stats();
            return best_solution;
        }
    }

    spdlog::warn("primary LaCAM search produced no valid solution after {} high-level expansions; trying prioritized fallback",
                 stats.hl_expansions);
    log_stats();
    return solve_prioritized_fallback();
}

// ============================================================
//  heuristic
//  sum of BFS distances (admissible)
// ============================================================
int Planner::heuristic(const Config& C) const
{
    int h = 0;
    for (int i = 0; i < static_cast<int>(C.size()); ++i) {
        int d = D.get(i, C[i]->id);
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
        // 이미 방문: g_val 이 더 작으면 갱신하고 재push
        HNode* existing = it->second;
        if (g < existing->g_val) {
            existing->g_val  = g;
            existing->parent = parent;
            open.push(existing);
        }
        return existing;
    }

    // 새 노드 생성
    int h = heuristic(C);
    auto* node = new HNode(C, g, h, parent);
    all_hnodes.push_back(node);
    visited[C] = node;
    open.push(node);

    // low-level root LNode 초기화 (dummy: who=-1, where=nullptr)
    auto* ll_root = new LNode(-1, nullptr, 0, nullptr);
    all_lnodes.push_back(ll_root);
    node->ll_queue.push(ll_root);

    return node;
}

// ============================================================
//  expand
//
//  HNode h 의 low-level 큐에서 LNode 를 꺼내
//  constraint 목록을 만들고 PIBT 를 호출한다.
//
//  Low-level tree 구조:
//    - 각 LNode 는 (who, where) 제약 하나를 표현
//    - parent 를 따라 올라가며 현재까지의 제약 집합 수집
//    - 새 Config 생성 성공 시 자식 LNode 들을 생성해 큐에 추가
//      (어떤 에이전트의 어떤 이동이 충돌을 유발하는지 탐색)
// ============================================================
bool Planner::expand(HNode* h)
{
    if (h->ll_queue.empty()) return false;

    LNode* ll = h->ll_queue.front();
    h->ll_queue.pop();

    // ll 에서 root 까지 constraint 수집
    {
        std::fill(forced_scratch.begin(), forced_scratch.end(), nullptr);
        constraints_scratch.clear();
        LNode* cur = ll;
        while (cur != nullptr && cur->who != -1) {
            // leaf 쪽 제약이 현재 분기에서 가장 최신 결정이므로 우선한다.
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

    // ③ non-passing collision 검사를 위해
    //    직전 timestep (h->parent → h) 의 확정 edge 를 edge_history 에 로드
    collision.clear();
    if (h->parent != nullptr) {
        const int n = ins.num_agents();
        for (int i = 0; i < n; ++i) {
            collision.record_edge(h->parent->C[i]->id,
                                  h->C[i]->id,
                                  h->g_val);
        }
    }

    // PIBT 로 새 Config 생성 시도
    Config C_next(ins.num_agents(), nullptr);
    const int t = h->g_val + 1;

    const auto pibt_begin = std::chrono::steady_clock::now();
    const bool config_ok = pibt.set_new_config(h->C, C_next, constraints_scratch, t);
    stats.pibt_ms += std::chrono::duration<double, std::milli>(
                         std::chrono::steady_clock::now() - pibt_begin).count();
    ++stats.pibt_calls;

    // ── 1. nullptr 검사: constraint 충돌 또는 funcPIBT 도중 실패 ─────────────
    for (int i = 0; i < ins.num_agents(); ++i) {
        if (C_next[i] == nullptr) return false;
    }

    // ── 2. vertex/swap collision 잔존 검사 ───────────────────────────────────
    //   PIBT 가 ①② 를 모두 방지했으면 config_ok == true.
    //   만약 false 라면 (드물지만, constraint 강제 배치 상황 등에서 발생 가능)
    //   해당 충돌 쌍을 h 의 ll_queue 에 되돌려 다른 constraint 조합을 재시도.
    if (!config_ok) {
        const int n = ins.num_agents();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                const bool vc = (C_next[i] == C_next[j]);
                const bool sc = (h->C[i] == C_next[j] && h->C[j] == C_next[i]);
                if (vc || sc) {
                    auto* ci = new LNode(i, C_next[i], ll->depth + 1, ll);
                    auto* cj = new LNode(j, C_next[j], ll->depth + 1, ll);
                    all_lnodes.push_back(ci);
                    all_lnodes.push_back(cj);
                    h->ll_queue.push(ci);
                    h->ll_queue.push(cj);
                }
            }
        }
        return false;  // solve() 에서 h->ll_queue 가 비어있지 않으면 h 를 재push
    }

    // 새 Config 로 HNode 생성 (또는 기존 노드 갱신)
    HNode* next = get_or_create_hnode(C_next, t, h);

    // ── 3. C_next 의 잔존 충돌 → next 의 ll_queue 에 LNode 추가 ────────────
    // config_ok == true なので vertex/swap は解消済み.
    // ① vertex / ② swap: PIBT 이후에도 드물게 잔존 가능 → next 에 constraint 추가.
    // ③ non-passing: PIBT 내부에서 방지되지만 constraint tree 대안 탐색용 안전망.
    const int n = ins.num_agents();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // ① vertex collision
            if (C_next[i] == C_next[j]) {
                // i 또는 j 를 다른 곳으로 보내는 제약 추가
                auto* child_i = new LNode(i, C_next[i], ll->depth + 1, ll);
                auto* child_j = new LNode(j, C_next[j], ll->depth + 1, ll);
                all_lnodes.push_back(child_i);
                all_lnodes.push_back(child_j);
                next->ll_queue.push(child_i);
                next->ll_queue.push(child_j);
            }
            // ② swap collision
            if (h->C[i] == C_next[j] && h->C[j] == C_next[i]) {
                auto* child_i = new LNode(i, C_next[i], ll->depth + 1, ll);
                auto* child_j = new LNode(j, C_next[j], ll->depth + 1, ll);
                all_lnodes.push_back(child_i);
                all_lnodes.push_back(child_j);
                next->ll_queue.push(child_i);
                next->ll_queue.push(child_j);
            }
        }
    }

    // ③ non-passing collision 안전망 + constraint tree 탐색 지원
    //
    //  PIBT 는 funcPIBT() 내부에서 non-passing 이동을 거부하므로
    //  C_next 에 non-passing 위반이 잔존하는 경우는 드물다.
    //  그러나 PIBT 가 non-passing 으로 인해 에이전트를 대기(wait)로 처리했을 때
    //  LaCAM★ constraint tree 는 이를 인식하지 못해 대안 경로를 탐색하지 않는다.
    //  위반이 감지되면 LNode 를 추가해 해당 에이전트를 다른 vertex 로 유도한다.
    for (int i = 0; i < n; ++i) {
        if (h->C[i] == C_next[i]) continue;  // 대기: edge 이동 없음, 검사 불필요
        if (!collision.is_non_passing_free(h->C[i]->id, C_next[i]->id, t)) {
            auto* child = new LNode(i, C_next[i], ll->depth + 1, ll);
            all_lnodes.push_back(child);
            next->ll_queue.push(child);
        }
    }

    return true;
}

// ============================================================
//  extract_solution
//  goal HNode 에서 root 까지 parent 포인터를 역추적해
//  RawPaths 를 만들고 Solution 으로 변환한다.
// ============================================================
Solution Planner::extract_solution(HNode* h) const
{
    const int n = ins.num_agents();

    // HNode 체인을 root → goal 순으로 수집
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
//  각 에이전트가 goal 에 처음 도달한 timestep 의 합 (true SOC).
//
//  plan.back().second 는 makespan 이므로 n*makespan 이 되는 버그를 수정.
//  ins.goals[i] 를 참조해 각 에이전트의 첫 goal 도달 시각을 구한다.
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

// ============================================================
//  solve_prioritized_fallback
//
//  LaCAM★ 탐색이 해를 찾지 못했을 때 사용하는 pragmatic fallback.
//  랜덤 priority order 를 여러 번 시도하면서 reservation table 기반의
//  space-time A* 로 각 에이전트를 순차 계획한다.
// ============================================================
Solution Planner::solve_prioritized_fallback()
{
    FallbackResult result = run_prioritized_fallback(ins, D, nullptr);
    stats.fallback_expanded_nodes = result.expanded_nodes;
    stats.fallback_planned_agents = result.planned_agents;
    stats.fallback_ms = result.elapsed_ms;

    if (result.solution.empty()) {
        spdlog::warn("prioritized fallback failed to find a valid solution");
        return {};
    }

    best_solution = std::move(result.solution);
    best_cost = result.cost;
    stop.notify_solution(best_cost);
    spdlog::info(
        "sync prioritized fallback produced valid solution: cost={}, expanded_nodes={}, "
        "planned_agents={}, elapsed_ms={:.3f}",
        best_cost,
        stats.fallback_expanded_nodes,
        stats.fallback_planned_agents,
        stats.fallback_ms);
    return best_solution;
}
