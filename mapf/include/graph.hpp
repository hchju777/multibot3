#pragma once

#include <vector>
#include <unordered_map>

// ============================================================
//  Vertex
//  - 로봇이 위치할 수 있는 그래프의 노드
//  - id: 고유 식별자 (0-indexed)
//  - neighbors: 인접 vertex 포인터 목록 (undirected)
// ============================================================
struct Vertex {
    int id;
    std::vector<Vertex*> neighbors;

    explicit Vertex(int id) : id(id) {}
};

// ============================================================
//  Graph  G = (V, E)
//  - 일반 그래프 (grid 미사용)
//  - 생성 후 add_vertex / add_edge 로 구성
//  - 사용 전 반드시 is_biconnected() 검증 필요
// ============================================================
class Graph {
public:
    // 유효한 vertex 목록 (외부에서 읽기 전용으로 사용)
    std::vector<Vertex*> V;

    Graph() = default;
    ~Graph();

    // Move semantics: V 소유권 이전 (이후 source는 빈 Graph)
    Graph(Graph&& other) noexcept : V(std::move(other.V)) {}
    Graph& operator=(Graph&& other) noexcept {
        if (this != &other) {
            for (auto* v : V) delete v;
            V = std::move(other.V);
        }
        return *this;
    }

    // 복사 금지 (raw pointer 소유 → double-free 방지)
    Graph(const Graph&)            = delete;
    Graph& operator=(const Graph&) = delete;

    // vertex 추가 (id는 자동 부여: 0, 1, 2, ...)
    Vertex* add_vertex();

    // undirected edge 추가
    void add_edge(Vertex* u, Vertex* v);
    void add_edge(int uid, int vid);

    int num_vertices() const { return static_cast<int>(V.size()); }

    // -------------------------------------------------------
    //  Biconnected 검증
    //  Tarjan iterative DFS 로 articulation point 탐색.
    //  재귀 DFS 를 사용하지 않아 대형 그래프에서도 스택 오버플로 없음.
    //  그래프가 비어 있거나 vertex가 1개 이하이면 false 반환.
    //  articulation point가 하나라도 있으면 false 반환.
    // -------------------------------------------------------
    bool is_biconnected() const;
};
