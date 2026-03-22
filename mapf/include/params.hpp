#pragma once

#include <string>

// ============================================================
//  PlannerParams  —  §7.3 파라미터
// ============================================================
struct PlannerParams {
    double initial_quality_threshold = 1.2;   // optimal 대비 X배
    int    initial_timeout_ms        = 5000;  // 최초 계획 hard limit
    int    replan_timeout_ms         = 500;   // 재계획 hard limit
    int    seed                      = 42;    // RNG seed
    bool   initial_async_fallback          = true;   // INITIAL 모드에서 fallback 병렬 race
    int    initial_async_fallback_delay_ms = 50;    // 이 시간 뒤 fallback thread 시작
    int    initial_primary_grace_ms        = 100;   // fallback 준비 후 primary 추가 탐색 시간
    bool   replan_async_fallback           = true;   // REPLAN 모드에서 fallback 병렬 race
    int    replan_async_fallback_delay_ms  = 0;      // REPLAN fallback thread 시작 지연
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
