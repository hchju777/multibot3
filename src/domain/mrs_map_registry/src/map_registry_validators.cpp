/**
 * @file map_registry_validators.cpp
 * @brief roadmap 정적 검사기 1·3·4 (D-11 v6, theory T1-R6/T5/T6/T7). 예외를 던지지 않는다.
 *
 * 검사기 3(통로 폭)은 v_max 없이 도는 **순수기하 3단**이다 — 임계는 ρ·L* 에서 계산한다
 * (2ρ / 2ρ+L* / 2(ρ+L*), 검사기 4 는 4ρ). v_max 의존 닫힌형은 v_max=0 이면 미산출(-1,
 * pending-[0b]). 검사기 2(biconnected)는 [1] 에서 미평가(U-23)라 실행하지 않는다.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "map_registry_impl.hpp"

namespace mrs
{

namespace
{

/**
 * @brief 검사기 1 — well-formed 위반 엔드포인트를 찾는다 (lifelong-mapd-2017 Def.1).
 *
 * 두 엔드포인트 사이에 **타 엔드포인트를 경유하지 않는** 경로가 존재해야 한다. 각 엔드포인트에서
 * BFS 하되, 프런티어가 **다른 엔드포인트에 도달하면 표시만 하고 확장하지 않는다**(그 엔드포인트를
 * 통과하는 경로를 배제). 어떤 엔드포인트 쌍이 이 규칙으로 서로 도달 불가면 둘 다 위반이다.
 *
 * @param[in] view 물리 뷰. 자료형 `const RoadmapViewData &`.
 * @param[in] index 물리 뷰 인접 인덱스. 자료형 `const mapreg_detail::GraphIndex &`.
 * @param[out] out_violating 위반 엔드포인트 노드 id(bare, 오름차순 유일). 자료형
 * `std::vector<PhysicalNodeId> &`.
 * @return `bool` — 모든 엔드포인트 쌍이 규칙을 만족하면 true.
 */
[[nodiscard]] bool check_well_formed(
  const RoadmapViewData & view, const mapreg_detail::GraphIndex & index,
  std::vector<PhysicalNodeId> & out_violating)
{
  std::unordered_set<std::uint32_t> endpoints;
  for (const RoadmapNodeView & node : view.nodes)
  {
    if (node.is_endpoint)
    {
      endpoints.insert(node.node_id);
    }
  }

  std::unordered_set<std::uint32_t> violating; // bare id, 중복 제거
  for (std::uint32_t source : endpoints)
  {
    // source 에서 BFS — 타 엔드포인트는 도달만 하고 확장하지 않는다.
    std::unordered_set<std::uint32_t> visited{source};
    std::unordered_set<std::uint32_t> reached_endpoints;
    std::queue<std::uint32_t> frontier;
    frontier.push(source);

    while (!frontier.empty())
    {
      const std::uint32_t u = frontier.front();
      frontier.pop();
      const auto adj = index.neighbors.find(u);
      if (adj == index.neighbors.end())
      {
        continue;
      }
      for (std::uint32_t v : adj->second)
      {
        if (!visited.insert(v).second)
        {
          continue;
        }
        if (v != source && endpoints.count(v) != 0)
        {
          reached_endpoints.insert(v); // 도달 표시, 확장 금지(경유 배제)
        }
        else
        {
          frontier.push(v); // 비-엔드포인트만 확장
        }
      }
    }

    // source 가 도달 못 한 타 엔드포인트가 있으면 그 쌍은 위반.
    for (std::uint32_t other : endpoints)
    {
      if (other != source && reached_endpoints.count(other) == 0)
      {
        violating.insert(source);
        violating.insert(other);
      }
    }
  }

  std::vector<std::uint32_t> sorted(violating.begin(), violating.end());
  std::sort(sorted.begin(), sorted.end());
  out_violating.clear();
  out_violating.reserve(sorted.size());
  for (std::uint32_t id : sorted)
  {
    out_violating.push_back(PhysicalNodeId{id});
  }
  return out_violating.empty();
}

} // namespace

MapResult<RoadmapValidationResult> MapRegistry::Impl::validate(
  std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
  double inflation_radius_m, double nid_offset_l_m, double v_max_mps, double omega_max_rps) const
{
  if (!loaded)
  {
    return MapResult<RoadmapValidationResult>::fail(MapStatus::MAP_NOT_LOADED);
  }

  // 입도 검사 대상 균일 뷰 해석 (0 = 물리만, 입도 검사 생략).
  const RoadmapViewData * gran_view = nullptr;
  if (view_id != VIEW_ID_PHYSICAL)
  {
    if (has_uniform && view_id == uniform.view_id)
    {
      gran_view = &uniform;
    }
    else
    {
      return MapResult<RoadmapValidationResult>::fail(MapStatus::VIEW_NOT_FOUND);
    }
  }

  RoadmapValidationResult res;

  // ── 순수기하 임계 (ρ·L* 에서 계산 — v_max 무입력) ──────────────────────────────────
  const double two_rho = 2.0 * inflation_radius_m;                   // 통과   2ρ
  const double turn90 = (2.0 * inflation_radius_m) + nid_offset_l_m; // 90°점턴 2ρ+L*
  const double uturn = 2.0 * (inflation_radius_m + nid_offset_l_m);  // U턴    2(ρ+L*)
  const double meet_pass = 4.0 * inflation_radius_m;                 // 교행(검사기4) 4ρ
  res.pass_width_min_m = two_rho;
  res.turn90_width_min_m = turn90;
  res.uturn_width_min_m = uturn;
  res.meet_pass_width_min_m = meet_pass;
  res.required_r_clear_m = robot_radius_m;

  // ── 검사기 1 — well-formed ─────────────────────────────────────────────────────────
  const bool reach_ok =
    check_well_formed(physical, physical_index, res.violating_wellformed_node_ids);
  std::size_t non_task_endpoints = 0;
  for (const RoadmapNodeView & node : physical.nodes)
  {
    if (node.is_endpoint && !node.is_task_endpoint)
    {
      ++non_task_endpoints;
    }
  }
  const bool count_ok = non_task_endpoints >= robot_count;
  res.well_formed = reach_ok && count_ok;
  if (!count_ok)
  {
    res.messages.push_back(
      "well-formed: 비작업 엔드포인트 수 " + std::to_string(non_task_endpoints) + " < 로봇 수 " +
      std::to_string(robot_count));
  }

  // ── 검사기 3(통로 폭 3단) + 검사기 4(advisory) — 물리 엣지 대상 ──────────────────────
  res.pass_width_ok = true;
  for (const RoadmapEdgeView & edge : physical.edges)
  {
    const double w = edge.corridor_width_m;
    if (!(w > two_rho)) // 통과폭 하드 (strict >)
    {
      res.pass_width_ok = false;
      res.narrow_pass_edge_ids.push_back(PhysicalEdgeId{edge.edge_id});
    }
    if (w < turn90) // 정보성 — 제자리 90° 회전 불가
    {
      res.turn90_incapable_edge_ids.push_back(PhysicalEdgeId{edge.edge_id});
    }
    if (w < uturn) // 정보성 — 360° 무후진 U턴 불가
    {
      res.uturn_incapable_edge_ids.push_back(PhysicalEdgeId{edge.edge_id});
    }
    if (w < meet_pass) // 검사기 4 advisory — 교행 불가 mutex-후보 (RESULT_PASS 불변)
    {
      res.mutex_candidate_edge_ids.push_back(PhysicalEdgeId{edge.edge_id});
    }
  }

  // ── r_clear 하드 하한 (claim_radius >= r) — 물리 노드 대상 ──────────────────────────
  res.r_clear_ok = true;
  for (const RoadmapNodeView & node : physical.nodes)
  {
    if (node.claim_radius_m < robot_radius_m)
    {
      res.r_clear_ok = false;
      res.messages.push_back(
        "r_clear: 노드 " + std::to_string(node.node_id) + " 의 claim_radius 가 r 미만");
    }
  }

  // ── 세분화 입도 하한 (균일 뷰 지정 시만; 물리만 검사면 vacuously true) ───────────────
  if (gran_view == nullptr)
  {
    res.unit_granularity_ok = true; // 입도 검사 생략 (view_id = 0)
  }
  else
  {
    res.unit_granularity_ok = true;
    for (const RoadmapEdgeView & edge : gran_view->edges)
    {
      if (edge.length_m < uturn) // l_unit 하한 2(ρ+L*)
      {
        res.unit_granularity_ok = false;
        res.violating_uniform_edge_ids.push_back(UniformEdgeId{edge.edge_id});
      }
    }
  }

  // ── v_max 의존 닫힌형 (pending-[0b] — v_max=0 이면 -1 미산출) ──────────────────────
  // required_min_width_m = 2(r + v_max/ω_max) 는 v_max 실측 후에만 산출된다([0b]). derated 전역
  // 요약은 정의가 없어(디레이팅은 엣지별 speed_limit 로 반영) 항상 -1 미산출로 둔다.
  if (v_max_mps > 0.0 && omega_max_rps > 0.0)
  {
    res.required_min_width_m = 2.0 * (robot_radius_m + (v_max_mps / omega_max_rps));
  }

  return MapResult<RoadmapValidationResult>::ok(std::move(res));
}

} // namespace mrs
