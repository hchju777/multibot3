#pragma once

#include "graph.hpp"
#include <vector>
#include <unordered_set>
#include <tuple>
#include <functional>

// Config: 한 timestep에서 모든 로봇의 위치
using Config = std::vector<Vertex*>;

// ============================================================
//  EdgeUse: (from_id, to_id, timestep)
//  non-passing collision 검사를 위한 edge 사용 이력 단위
//  from_id < to_id 방향으로 정규화하지 않음
//  — 방향(from→to)이 의미를 가지므로 그대로 저장
// ============================================================
struct EdgeUse {
    int from_id;
    int to_id;
    int timestep;

    bool operator==(const EdgeUse& o) const {
        return from_id == o.from_id &&
               to_id   == o.to_id   &&
               timestep == o.timestep;
    }
};

struct EdgeUseHash {
    std::size_t operator()(const EdgeUse& e) const {
        // FNV-1a 스타일 조합
        std::size_t h = 2166136261u;
        auto mix = [&](int val) {
            h ^= static_cast<std::size_t>(val);
            h *= 16777619u;
        };
        mix(e.from_id);
        mix(e.to_id);
        mix(e.timestep);
        return h;
    }
};

using EdgeUseSet = std::unordered_set<EdgeUse, EdgeUseHash>;

// ============================================================
//  CollisionChecker
//
//  3가지 collision 검사를 담당한다.
//
//  ① Vertex collision
//     같은 timestep t에 두 로봇이 같은 vertex를 점유
//
//  ② Swap collision
//     timestep t→t+1 에서 두 로봇이 서로 위치를 교차
//     (Qₜ[i] == Qₜ₊₁[j] && Qₜ[j] == Qₜ₊₁[i])
//
//  ③ Non-passing collision  [multibot3 추가]
//     edge (u→v)를 timestep t에 로봇 i가 사용했고,
//     timestep t+1에 로봇 j가 edge (v→u)를 사용하려 할 때 금지.
//     = 같은 edge를 다른 timestep에 반대 방향으로 통과하는 것 금지
//     구현: 3-timestep window (t-1, t, t+1)
// ============================================================
class CollisionChecker {
public:
    // ① vertex collision: Config 내에 중복 vertex가 있는지
    static bool has_vertex_collision(const Config& C);

    // ② swap collision: C_curr → C_next 에서 교차가 있는지
    static bool has_swap_collision(const Config& C_curr,
                                   const Config& C_next);

    // ③ non-passing collision 검사
    //    edge_history에 (to_id → from_id, t-1) 이 있으면
    //    (from_id → to_id, t) 는 금지된다.
    //    즉, 이전 timestep에 반대 방향이 이미 사용됐으면 false 반환.
    //
    //    from: 로봇이 현재 있는 vertex
    //    to:   로봇이 이동하려는 vertex
    //    t:    현재 timestep (이동이 완료되는 시각)
    bool is_non_passing_free(int from_id, int to_id, int t) const;

    // edge 사용을 이력에 등록
    void record_edge(int from_id, int to_id, int t);

    // 이력 초기화 (재계획 시 사용)
    void clear();

    // 현재 이력 크기 (디버그용)
    std::size_t history_size() const { return edge_history.size(); }

private:
    EdgeUseSet edge_history;
};
