#pragma once

#include "graph.hpp"
#include "planner.hpp"   // Instance

#include <memory>
#include <optional>
#include <string>
#include <vector>

// ============================================================
//  ConfigLoader
//  single-shot scenario YAML 또는 legacy combined YAML 에서
//  Graph 와 Instance 를 로드한다.
//
//  권장 YAML 포맷:
//    version: 1
//    map: ../../maps/corridor.yaml
//    params: ../../params/default.yaml
//    robots:
//      - id: 0
//        start: 0
//        goal: 7
//
//  legacy combined 포맷도 계속 지원:
//    graph:
//      vertices:
//        - id: 0
//      edges:
//        - [0, 1]
//    robots:
//      - id: 0
//        start: 0
//        goal: 3
//
//  주의:
//    - Instance 는 ConfigLoader 의 수명 동안 유효하다.
//      ConfigLoader 를 Instance 보다 먼저 소멸시키면 안 된다.
//    - preprocess() 를 호출하면 내부 graph_ 가 변환 후 그래프로
//      교체되고, starts_/goals_ 가 리매핑된다.
// ============================================================
class ConfigLoader {
public:
    explicit ConfigLoader(const std::string& path);

    // Virtual lock vertex 전처리 적용 (§5.3).
    // 호출 후 graph() 와 make_instance() 는 변환된 그래프를 반환한다.
    void preprocess();

    Graph& graph() { return *graph_; }

    Instance make_instance() const;
    const std::string& path() const { return path_; }
    const std::string& map_path() const { return map_path_; }
    const std::optional<std::string>& params_path() const { return params_path_; }

private:
    std::string              path_;
    std::string              map_path_;
    std::optional<std::string> params_path_;
    std::unique_ptr<Graph>  graph_;
    std::vector<Vertex*>    starts_;
    std::vector<Vertex*>    goals_;
};
