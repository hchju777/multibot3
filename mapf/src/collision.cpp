#include "collision.hpp"

#include <cassert>

// ============================================================
//  ① Vertex collision
//  Config 내 두 로봇이 같은 vertex를 점유하면 true 반환
//
//  O(n²) 이지만 n이 수천 이하이고 호출 빈도가 낮으므로 충분.
//  필요 시 unordered_set으로 O(n) 변환 가능.
// ============================================================
bool CollisionChecker::has_vertex_collision(const Config& C)
{
    const int n = static_cast<int>(C.size());
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (C[i] == C[j]) return true;
        }
    }
    return false;
}

// ============================================================
//  ② Swap collision
//  C_curr[i] == C_next[j] && C_curr[j] == C_next[i]
//  (두 로봇이 서로의 위치를 교차)
// ============================================================
bool CollisionChecker::has_swap_collision(const Config& C_curr,
                                          const Config& C_next)
{
    assert(C_curr.size() == C_next.size());
    const int n = static_cast<int>(C_curr.size());
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (C_curr[i] == C_next[j] &&
                C_curr[j] == C_next[i]) {
                return true;
            }
        }
    }
    return false;
}

// ============================================================
//  ③ Non-passing collision 검사
//
//  로봇이 timestep t 에서 edge (from → to) 를 이동하려 할 때,
//  직전 timestep (t-1) 에 반대 방향 (to → from) 이 이미
//  사용됐으면 충돌이므로 false 반환.
//
//  from_id == to_id (제자리 대기)는 검사 불필요 → true 반환.
// ============================================================
bool CollisionChecker::is_non_passing_free(int from_id,
                                           int to_id,
                                           int t) const
{
    if (from_id == to_id) return true;  // 대기: 검사 불필요

    // 직전 timestep에 반대 방향이 사용됐는지 확인
    if (t > 0) {
        const EdgeUse reverse{to_id, from_id, t - 1};
        if (edge_history.count(reverse)) return false;
    }
    return true;
}

// ============================================================
//  record_edge: 확정된 이동을 이력에 등록
// ============================================================
void CollisionChecker::record_edge(int from_id, int to_id, int t)
{
    if (from_id == to_id) return;  // 대기는 기록 불필요
    edge_history.insert(EdgeUse{from_id, to_id, t});
}

// ============================================================
//  clear
// ============================================================
void CollisionChecker::clear()
{
    edge_history.clear();
}
