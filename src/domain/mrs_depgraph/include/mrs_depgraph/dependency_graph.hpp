#pragma once

/**
 * @file dependency_graph.hpp
 * @brief ADG/SADG 자료구조 — 실행 창 릴리스·splice 무결성 검사 (D-05, D-03 v3).
 *
 * theory T2 정의 T2.1(커밋 경계)·정리 T2.2(splice 후 비순환·무역전)·술어 A1~A9 의 코드 소재지.
 * ExecutionWindow(★ load-bearing seam, L-01)의 발행 전 불변식 W1~W7 은 이 클래스가 검사한다.
 * 알고리즘 본문은 Phase 5(coordination-builder)가 채운다.
 */

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief ADG/SADG 의존성 그래프 — 릴리스·접합·실효의 단일 소유자.
 */
class DependencyGraph
{
public:
  DependencyGraph() = default;
  ~DependencyGraph() = default;

  /**
   * @brief L2 산출 경로들로부터 의존성 그래프를 (재)구축한다.
   * @param[in] paths 로봇별 시각 부여 경로 (D-03 표준형).
   * @param[in] roadmap_version 계획이 사용한 지도 버전.
   * @param[in] view_id 계획이 사용한 균일/골격 뷰 id.
   * @return bool 구축 성공 여부.
   */
  bool build_from_paths(
    const std::vector<RobotPath> & paths, std::uint64_t roadmap_version, std::uint32_t view_id);

  /**
   * @brief 의존성 충족 구간을 실행 창으로 릴리스한다 (ADG 이벤트 구동 실행, adg-2019).
   * @param[in] robot_id 대상 로봇.
   * @param[out] out_window 릴리스할 ExecutionWindow (불변식 W1~W7 검사 통과분만).
   * @return bool 릴리스할 창이 있으면 true.
   */
  bool release_next_window(RobotId robot_id, ExecutionWindow & out_window);

  /**
   * @brief 로봇 진행 이벤트를 반영한다(노드 클리어 → 하위 의존성 해소 판정).
   * @param[in] robot_id 진행한 로봇. 자료형 `mrs::RobotId`.
   * @param[in] cleared_node 클리어된 노드 id. 자료형 `mrs::UniformNodeId` (균일 뷰 강타입 —
   *            진행/클리어 노드는 ExecutionWindow·FrozenOrder 와 같은 공간이고 계약이 UNIFORM 으로
   *            고정한다, 계약 §0.1).
   * @param[in] time_s 이벤트 발생 시각 [s]. 자료형 `double`.
   * @return void
   */
  void on_progress_event(RobotId robot_id, UniformNodeId cleared_node, double time_s);

  /**
   * @brief R3 부분 재계획 결과를 커밋 경계 안쪽으로 접합한다 (splice, 정리 T2.2).
   * @param[in] robot_id 대상 로봇.
   * @param[in] new_partial_paths 새 부분 경로.
   * @param[in] frozen_orders 무역전 강제 대상 (T2 조건 (C3)).
   * @param[in] commit_boundary 접합 기준 커밋 경계 (정의 T2.1).
   * @return bool 접합 성공 여부(A3 no_orphan·A5 acyclic_after·A6 no_reversal 통과 시만 true).
   */
  bool splice_partial_plan(
    RobotId robot_id, const std::vector<RobotPath> & new_partial_paths,
    const std::vector<FrozenOrder> & frozen_orders, const CommitState & commit_boundary);

  /**
   * @brief staleness repair — 만료된 창을 실효시킨다 (v3 NF7, window_valid_until 전용 경로).
   * @param[in] now_s 현재 시각 [s].
   * @return void
   */
  void invalidate_stale_windows(double now_s);

  /**
   * @brief 현재 그래프가 사용 중인 지도 버전을 반환한다.
   * @return std::uint64_t roadmap_version.
   */
  std::uint64_t roadmap_version() const noexcept
  {
    return roadmap_version_;
  }

private:
  /**
   * @brief ADG 정점 참조 — 어느 로봇의 몇 번째 방문인가 (Type-2 통행순서 목록의 원소).
   *
   * 정점 자체(노드·시각)는 @ref paths_ 가 소유하고, 이 참조는 그 안의 위치만 가리킨다 — 좌표·시각
   * 을 복제하지 않아 재계획 시 한 곳만 갱신하면 된다.
   */
  struct VisitRef
  {
    std::size_t robot_index{0}; ///< @ref paths_ 내 로봇 인덱스
    std::size_t visit_index{0}; ///< 그 로봇 경로(@ref RobotPath::visits) 내 방문 인덱스
  };

  std::uint64_t roadmap_version_{0}; ///< 현재 그래프가 참조하는 지도 버전 (0 = 미구축)
  std::uint32_t view_id_{0};         ///< 계획이 사용한 균일 뷰 id (창 스코프 에코)
  bool built_{false};                ///< build_from_paths 성공 여부

  std::vector<RobotPath> paths_; ///< 로봇별 시각 부여 경로. Type-1 간선(로봇 내 시간순)은 방문열의
                                 ///< 연속 원소로 암묵 표현된다. 인덱스 = @ref robot_index_ 의 값

  /**
   * @brief 균일 노드 id(raw) → 그 노드를 방문하는 (로봇, 방문) 목록 (arrival_time_s 오름차순, 동
   *        시각은 robot_id tie-break). **Type-2 간선(통행순서/충돌 선행)의 표현체**다 — 목록의 앞
   *        원소가 뒤 원소보다 먼저 그 노드를 통과해야 한다(adg-2019, 순서는 공간으로 표현).
   */
  std::unordered_map<std::uint32_t, std::vector<VisitRef>> node_visit_order_;

  std::unordered_map<std::uint16_t, std::size_t> robot_index_; ///< robot_id → @ref paths_ 인덱스

  // ── 릴리스·진행 프론티어 (@ref paths_ 병렬 — build 에서 초기화, D-05b 릴리스/진행이 사용) ──────
  std::vector<std::size_t> released_count_; ///< 로봇별 릴리스된 방문 수 (0 = 미릴리스, W3 프론티어)
  std::vector<std::size_t> cleared_count_; ///< 로봇별 클리어된 방문 수 (on_progress_event 가 전진)

  /**
   * @brief robot_id → 다음 발급할 window_seq (W1 로봇별 단조증가). **재빌드 간 보존**한다 — 재계획
   *        이 seq 를 되돌리면 수신 L4 가 신·구 창을 구별하지 못한다(W1 위반).
   */
  std::unordered_map<std::uint16_t, std::uint32_t> next_window_seq_;
};

} // namespace mrs
