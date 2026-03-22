#include "pibt.hpp"

#include <algorithm>
#include <cassert>

// ============================================================
//  생성자
// ============================================================
PIBT::PIBT(const Graph& G,
           const DistTable& D,
           CollisionChecker& collision,
           std::mt19937& rng)
    : G(G), D(D), collision(collision), rng(rng)
{
    const int n = G.num_vertices();
    const int num_agents = D.num_agents();
    occupied_now.resize(n, -1);
    occupied_next.resize(n, -1);
    candidate_order.resize(num_agents, std::vector<std::vector<Vertex*>>(n));

    for (int agent = 0; agent < num_agents; ++agent) {
        for (int vertex_id = 0; vertex_id < n; ++vertex_id) {
            Vertex* curr = G.V[vertex_id];
            auto& candidates = candidate_order[agent][vertex_id];
            candidates = curr->neighbors;
            candidates.push_back(curr);

            std::sort(candidates.begin(), candidates.end(),
                      [&](Vertex* a, Vertex* b) {
                          const int da = D.get(agent, a->id);
                          const int db = D.get(agent, b->id);
                          if (da != db) return da < db;
                          return a->id < b->id;
                      });

            int i = 0;
            while (i < static_cast<int>(candidates.size())) {
                const int dist = D.get(agent, candidates[i]->id);
                int j = i + 1;
                while (j < static_cast<int>(candidates.size()) &&
                       D.get(agent, candidates[j]->id) == dist) {
                    ++j;
                }
                std::shuffle(candidates.begin() + i, candidates.begin() + j, rng);
                i = j;
            }
        }
    }
}

// ============================================================
//  set_new_config
//
//  흐름:
//    1. constraints 로 강제 배치된 에이전트 먼저 처리
//    2. 나머지 에이전트를 랜덤 순서로 처리
//    3. 각 에이전트에 대해 funcPIBT() 호출
//    4. 최종 C_next 에 ①② 충돌이 없으면 성공 반환
//
//  NOTE: collision.edge_history 는 호출 전에 이미
//        직전 timestep 의 확정된 edge 들로 채워져 있어야 한다.
//        (planner::expand() 가 담당)
// ============================================================
bool PIBT::set_new_config(const Config& C_now,
                          Config& C_next,
                          const std::vector<Constraint>& constraints,
                          int timestep)
{
    const int n = static_cast<int>(C_now.size());
    assert(n > 0);

    C_next.assign(n, nullptr);

    // occupied / in_stack 배열 초기화
    std::fill(occupied_now.begin(),  occupied_now.end(),  -1);
    std::fill(occupied_next.begin(), occupied_next.end(), -1);
    in_stack.assign(n, false);

    for (int i = 0; i < n; ++i) {
        assert(C_now[i] != nullptr);
        occupied_now[C_now[i]->id] = i;
    }

    // --- 1. constraints 먼저 적용 ---
    std::vector<bool> constrained(n, false);
    for (const auto& c : constraints) {
        assert(c.who >= 0 && c.who < n);
        assert(c.where != nullptr);

        if (occupied_next[c.where->id] != -1) return false;

        C_next[c.who]              = c.where;
        occupied_next[c.where->id] = c.who;
        constrained[c.who]         = true;
    }

    // --- 2. 나머지 에이전트를 랜덤 순서로 처리 ---
    std::vector<int> order;
    order.reserve(n);
    for (int i = 0; i < n; ++i) {
        if (!constrained[i]) order.push_back(i);
    }
    std::shuffle(order.begin(), order.end(), rng);

    for (int agent : order) {
        if (C_next[agent] != nullptr) continue;
        funcPIBT(agent, C_now, C_next, timestep);
    }

    for (int agent = 0; agent < n; ++agent) {
        if (C_next[agent] == nullptr) return false;
    }

    // --- 3. 최종 검증 (① vertex, ② swap) ---
    if (CollisionChecker::has_vertex_collision(C_next))       return false;
    if (CollisionChecker::has_swap_collision(C_now, C_next))  return false;

    return true;
}

// ============================================================
//  funcPIBT  (재귀)
//
//  에이전트 agent_id 의 다음 위치 후보를 시도한다.
//  후보: neighbors(목표까지 거리 오름차순으로 정렬) + 현재 위치(대기)
//
//  각 후보 v 에 대해:
//    - ③ non-passing collision 검사 (collision.edge_history 참조)
//    - ② swap collision 검사 (caller 와의 교차 방지)
//    - occupied_next[v] 에 다른 에이전트가 있으면 재귀적으로 먼저 처리
//
//  순환 감지 (in_stack):
//    priority inheritance 연쇄에서 A→B→C→A 같은 순환이 생기면
//    in_stack[blocker] == true 가 되어 해당 후보를 건너뛴다.
//    in_stack[agent_id] 는 이 함수의 진입/퇴장 시 관리된다.
//
//  에이전트를 위치에 배치할 때:
//    - 이전 tentative 위치(C_next[agent_id])를 반드시 먼저 해제한다
//      (occupied_next[old] = -1)
//    - 그래야 재귀 호출 후 caller 가 해당 자리를 차지할 수 있다
//
//  record_edge 는 여기서 하지 않는다.
//  확정된 이동의 기록은 planner::expand() 가 담당한다.
// ============================================================
bool PIBT::funcPIBT(int agent_id,
                    const Config& C_now,
                    Config& C_next,
                    int timestep,
                    int caller_id)
{
    (void)caller_id;
    in_stack[agent_id] = true;   // 재귀 스택에 진입

    Vertex* curr = C_now[agent_id];
    const auto& candidates = candidate_order[agent_id][curr->id];

    // 현재 tentative 위치를 해제하고 v 에 배치하는 헬퍼
    auto place = [&](Vertex* v) {
        if (C_next[agent_id] != nullptr) {
            occupied_next[C_next[agent_id]->id] = -1;
        }
        C_next[agent_id]     = v;
        occupied_next[v->id] = agent_id;
    };

    for (Vertex* v : candidates) {
        const int vid = v->id;

        // ③ non-passing collision 검사
        if (!collision.is_non_passing_free(curr->id, vid, timestep)) {
            continue;
        }

        // ② swap collision: C_now에서 v를 점유한 에이전트가 이미 curr로 배치됐으면 교차 발생.
        //   caller_id 에 한정하지 않고, 이미 배치된 모든 에이전트에 대해 검사한다.
        {
            const int c_now_at_v = occupied_now[vid];
            if (c_now_at_v != -1 &&
                C_next[c_now_at_v] != nullptr &&
                C_next[c_now_at_v] == curr) {
                continue;
            }
        }

        int blocker = occupied_next[vid];

        if (blocker == -1 || blocker == agent_id) {
            place(v);
            in_stack[agent_id] = false;
            return true;
        }

        // 순환 감지: blocker 가 이미 재귀 스택 위에 있으면
        // 이 후보를 건너뛰어 무한 재귀를 방지한다.
        if (in_stack[blocker]) continue;

        // priority inheritance: blocker 를 먼저 이동시킨다
        funcPIBT(blocker, C_now, C_next, timestep, agent_id);

        if (occupied_next[vid] == -1) {
            place(v);
            in_stack[agent_id] = false;
            return true;
        }
    }

    // 모든 후보 실패 → 현재 위치를 유지한다.
    // 이미 다른 에이전트가 현재 위치를 선점했더라도 C_next 에는 기록해
    // 상위 레벨이 vertex conflict 로 분기할 수 있게 한다.
    if (occupied_next[curr->id] == -1 || occupied_next[curr->id] == agent_id) {
        place(curr);
    } else {
        C_next[agent_id] = curr;
    }
    in_stack[agent_id] = false;
    return false;
}
