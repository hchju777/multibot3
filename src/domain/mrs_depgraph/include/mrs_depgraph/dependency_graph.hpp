#pragma once

/**
 * @file dependency_graph.hpp
 * @brief ADG/SADG 자료구조 — 실행 창 릴리스·splice 무결성 검사 (D-05, D-03 v3).
 *
 * theory T2 정의 T2.1(커밋 경계)·정리 T2.2(splice 후 비순환·무역전)·술어 A1~A9 의 코드 소재지.
 * ExecutionWindow(★ load-bearing seam, L-01)의 발행 전 불변식 W1~W7 은 이 클래스가 검사한다.
 * 알고리즘 본문은 Phase 5(coordination-builder)가 채운다.
 */

#include <cstdint>
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
  std::uint64_t roadmap_version_{0}; ///< 현재 그래프가 참조하는 지도 버전
};

} // namespace mrs
