#pragma once

#include "graph.hpp"
#include "dist_table.hpp"
#include "collision.hpp"
#include "params.hpp"
#include "pibt.hpp"
#include "reservation_table.hpp"
#include "stop_condition.hpp"
#include "solution.hpp"

#include <vector>
#include <unordered_map>
#include <queue>
#include <random>
#include <memory>
#include <chrono>

// ============================================================
//  Instance
//  MAPF 문제 인스턴스 (그래프 + 시작 + 목표)
// ============================================================
struct Instance {
    const Graph&          G;
    std::vector<Vertex*>  starts;
    std::vector<Vertex*>  goals;

    int num_agents() const { return static_cast<int>(starts.size()); }
};

// ============================================================
//  LNode  (Low-Level Search Node)
//  LaCAM★ 의 low-level constraint tree 노드.
//  "agent who 는 다음 timestep에 where 에 있어야 한다" 를 표현.
// ============================================================
struct LNode {
    int     who;
    Vertex* where;
    int     depth;
    LNode*  parent;

    LNode(int who, Vertex* where, int depth, LNode* parent)
        : who(who), where(where), depth(depth), parent(parent) {}
};

// ============================================================
//  HNode  (High-Level Search Node)
//  Configuration C 와 그 A* 값, low-level 탐색 큐를 보유.
// ============================================================
struct HNode {
    Config  C;          // 이 노드의 configuration
    int     g_val;      // goal 까지 실제 cost (timestep 수)
    int     h_val;      // heuristic (sum of BFS distances)
    HNode*  parent;     // 경로 역추적용

    // low-level constraint tree 탐색 큐
    std::queue<LNode*> ll_queue;

    int f_val() const { return g_val + h_val; }

    HNode(const Config& C, int g, int h, HNode* parent)
        : C(C), g_val(g), h_val(h), parent(parent) {}
};

struct OpenEntry {
    HNode* node = nullptr;
    int g_snapshot = 0;
    int h_snapshot = 0;
    int f_snapshot = 0;
};

struct OpenEntryCompare {
    bool operator()(const OpenEntry& a, const OpenEntry& b) const
    {
        if (a.f_snapshot != b.f_snapshot) return a.f_snapshot > b.f_snapshot;
        if (a.h_snapshot != b.h_snapshot) return a.h_snapshot > b.h_snapshot;
        return a.g_snapshot > b.g_snapshot;
    }
};

// Config 해시 (unordered_map 키로 사용)
struct ConfigHash {
    std::size_t operator()(const Config& C) const {
        std::size_t h = 2166136261u;
        for (const Vertex* v : C) {
            h ^= static_cast<std::size_t>(v->id);
            h *= 16777619u;
        }
        return h;
    }
};

// ============================================================
//  Planner
//  LaCAM★ high-level DFS/A* 탐색기
//
//  solve() 를 호출하면 StopCondition 이 만족될 때까지
//  탐색을 수행하고 최선의 Solution 을 반환한다.
//
//  탐색 전략:
//    - OPEN: stack (DFS) — 첫 solution 을 빠르게 찾기 위해
//    - solution 발견 후에도 anytime 방식으로 계속 탐색
//    - 방문한 HNode 는 해시맵에 메모이제이션
// ============================================================
class Planner {
public:
    Planner(const Instance& ins,
            const DistTable& D,
            StopCondition& stop,
            std::mt19937& rng,
            PlannerParams params = PlannerParams{},
            const ReservationTable* reservations = nullptr);

    ~Planner();

    // 탐색 실행. solution 미발견 시 빈 Solution 반환.
    Solution solve();

private:
    struct PlannerStats {
        int hl_expansions = 0;
        int pibt_calls = 0;
        int goal_validations = 0;
        double pibt_ms = 0.0;
    };

    const Instance&  ins;
    const DistTable& D;
    StopCondition&   stop;
    std::mt19937&    rng;
    PlannerParams    params;
    const ReservationTable* reservations;

    CollisionChecker collision;
    PIBT             pibt;

    // 방문한 configuration → HNode 메모이제이션
    std::unordered_map<Config, HNode*, ConfigHash> visited;

    // OPEN best-first queue
    std::priority_queue<OpenEntry,
                        std::vector<OpenEntry>,
                        OpenEntryCompare> open;

    // 메모리 관리용 노드 목록
    std::vector<HNode*> all_hnodes;
    std::vector<LNode*> all_lnodes;

    // best solution 역추적 결과
    Solution best_solution;
    int      best_cost;
    PlannerStats stats;

    // expand() hot path scratch buffers. Single-threaded planner reuse is safe.
    std::vector<Vertex*> forced_scratch;
    std::vector<Constraint> constraints_scratch;

    // -------------------------------------------------------
    //  heuristic: sum of BFS distances
    // -------------------------------------------------------
    int heuristic(const Config& C) const;

    // -------------------------------------------------------
    //  get_or_create_hnode
    //  C 가 이미 방문됐으면 기존 노드 반환 (g_val 갱신 포함)
    //  새 configuration 이면 생성 후 OPEN 에 push
    // -------------------------------------------------------
    HNode* get_or_create_hnode(const Config& C, int g, HNode* parent);

    // -------------------------------------------------------
    //  expand
    //  HNode 를 한 단계 확장한다.
    //  low-level 큐에서 LNode 꺼내기 → constraints 수집 →
    //  PIBT 호출 → 새 HNode 생성
    //  반환값: 새 configuration 생성 성공 여부
    // -------------------------------------------------------
    bool expand(HNode* h);

    // -------------------------------------------------------
    //  extract_solution
    //  HNode 에서 goal 까지의 경로를 역추적해 Solution 생성
    // -------------------------------------------------------
    Solution extract_solution(HNode* h) const;

    // -------------------------------------------------------
    //  sum_of_costs
    //  각 에이전트가 goal 에 처음 도달한 timestep 의 합 (true SOC).
    //  ins.goals 를 참조하므로 non-static.
    // -------------------------------------------------------
    int sum_of_costs(const Solution& sol) const;

};
