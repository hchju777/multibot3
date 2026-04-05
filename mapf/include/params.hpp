#pragma once

#include <string>

enum class ObjectiveType {
    SOC,
    MAKESPAN,
};

// ============================================================
//  PlannerParams  —  §7.3 파라미터
// ============================================================
struct PlannerParams {
    ObjectiveType objective          = ObjectiveType::SOC;
    double initial_quality_threshold = 1.2;   // optimal 대비 X배
    int    initial_timeout_ms        = 5000;  // 최초 계획 hard limit
    int    replan_timeout_ms         = 500;   // 재계획 hard limit
    int    parallel_search_workers   = 1;     // 동일 planner 병렬 restart 수
    int    seed                      = 42;    // RNG seed
};

// ============================================================
//  PreprocessingParams  —  §5.3 파라미터
// ============================================================
struct PreprocessingParams {
    bool virtual_lock = true;  // narrow corridor → virtual lock vertex
};

// ============================================================
//  Params  —  전체 파라미터 집합
//
//  사용:
//    Params p = Params::load("config/params/default.yaml");
//    Params p = Params::defaults();  // YAML 없이 기본값 사용
// ============================================================
struct Params {
    PlannerParams      planner;
    PreprocessingParams preprocessing;

    // YAML 파일에서 로드 (없는 키는 기본값 유지)
    static Params load(const std::string& path);

    // 기본값만 사용
    static Params defaults() { return Params{}; }
};
