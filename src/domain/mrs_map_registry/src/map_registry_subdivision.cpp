/**
 * @file map_registry_subdivision.cpp
 * @brief 세분화(물리→균일)·collapse(균일→골격)·뷰 간 노드 변환 (D-11 v2 무손실). 예외를 던지지
 * 않는다.
 *
 * collapse 는 "차수 2 체인의 **가상** 노드"만 접는다(D-11 (iii)). 가상 노드는 subdivision 이 엣지
 * 중간에 넣은 것이라 항상 차수 2 다 — 그래서 **골격 뷰 = 물리 토폴로지**(물리 노드·엣지 그대로,
 * 골격 엣지가 균일 스텝 수를 collapse_map 으로 안다). 그 결과 물리↔골격 변환이 전단사가 된다.
 */

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "map_registry_impl.hpp"

namespace mrs
{

namespace
{

/**
 * @brief 뷰에 주어진 bare 노드 id 가 존재하는지 검사한다(저빈도 경로 — 선형 스캔).
 * @param[in] view 뷰. 자료형 `const RoadmapViewData &`.
 * @param[in] id 노드 id. 자료형 `std::uint32_t`.
 * @return `bool` — 존재하면 true.
 */
[[nodiscard]] bool has_node(const RoadmapViewData & view, std::uint32_t id) noexcept
{
  for (const RoadmapNodeView & node : view.nodes)
  {
    if (node.node_id == id)
    {
      return true;
    }
  }
  return false;
}

} // namespace

MapResult<std::uint32_t> MapRegistry::Impl::build_uniform(
  double unit_length_m, double unit_length_lower_bound_m)
{
  if (!loaded)
  {
    return MapResult<std::uint32_t>::fail(MapStatus::MAP_NOT_LOADED);
  }
  if (
    !std::isfinite(unit_length_m) || unit_length_m <= 0.0 ||
    unit_length_m < unit_length_lower_bound_m)
  {
    return MapResult<std::uint32_t>::fail(MapStatus::UNIT_LENGTH_BELOW_BOUND);
  }

  RoadmapViewData uni;
  uni.view_kind = ViewKind::UNIFORM;
  uni.roadmap_version = roadmap_version;
  uni.view_id = next_view_id;
  uni.unit_length_m = unit_length_m;
  uni.unit_length_lower_bound_m = unit_length_lower_bound_m;

  std::unordered_map<std::uint32_t, std::uint32_t> p2u; // 물리 노드 → 균일 노드
  std::unordered_map<std::uint32_t, std::uint32_t> u2p; // 균일(물리상) → 물리 노드
  std::unordered_map<std::uint32_t, std::pair<double, double>> phys_coords;
  phys_coords.reserve(physical.nodes.size());

  std::uint32_t next_u_node = 0;
  std::uint32_t next_u_edge = 0;

  // ① 물리 노드 1:1 → 균일 노드 (전단사 대응).
  for (const RoadmapNodeView & p : physical.nodes)
  {
    const std::uint32_t u = next_u_node++;
    RoadmapNodeView un;
    un.node_id = u;
    un.x_m = p.x_m;
    un.y_m = p.y_m;
    un.claim_radius_m = 0.0; // 뷰별 규약 (j): 인접 간격 최솟값/2 — 엣지 확정 후 재계산
    un.is_endpoint = p.is_endpoint;
    un.is_task_endpoint = p.is_task_endpoint;
    un.is_virtual = false;
    uni.nodes.push_back(un);
    p2u.emplace(p.node_id, u);
    u2p.emplace(u, p.node_id);
    phys_coords.emplace(p.node_id, std::make_pair(p.x_m, p.y_m));
  }

  // ② 물리 엣지 floor 균등분할 → 균일 노드·엣지·subdivision_map(용례 1, 가상 노드).
  double eff_min = std::numeric_limits<double>::infinity();
  double eff_max = 0.0;
  std::size_t subdivided_edges = 0;
  for (const RoadmapEdgeView & e : physical.edges)
  {
    const auto seg = static_cast<std::uint32_t>(std::floor(e.length_m / unit_length_m));
    if (seg < 1U) // 물리 엣지가 입도보다 짧다 — 거부(D-11 risk_note)
    {
      return MapResult<std::uint32_t>::fail(MapStatus::UNIT_LENGTH_BELOW_BOUND);
    }
    if (seg > 1U)
    {
      ++subdivided_edges;
    }
    const double eff = e.length_m / static_cast<double>(seg);
    eff_min = std::min(eff_min, eff);
    eff_max = std::max(eff_max, eff);

    const std::uint32_t ua = p2u.at(e.node_a);
    const std::uint32_t ub = p2u.at(e.node_b);
    const auto pa = phys_coords.at(e.node_a);
    const auto pb = phys_coords.at(e.node_b);

    std::uint32_t prev = ua;
    for (std::uint32_t k = 1U; k < seg; ++k)
    {
      const double t = static_cast<double>(k) / static_cast<double>(seg);
      const std::uint32_t v = next_u_node++;
      RoadmapNodeView vn;
      vn.node_id = v;
      vn.x_m = pa.first + (t * (pb.first - pa.first));
      vn.y_m = pa.second + (t * (pb.second - pa.second));
      vn.claim_radius_m = 0.0; // 아래에서 재계산
      vn.is_endpoint = false;
      vn.is_task_endpoint = false;
      vn.is_virtual = true;
      uni.nodes.push_back(vn);

      NodeMappingView m; // 용례 1: 가상 노드 → 물리 엣지 위 위치
      m.kind = MappingKind::SUBDIVISION;
      m.from_node_id = UniformNodeId{v};
      m.to_physical_edge_id = PhysicalEdgeId{e.edge_id};
      m.s = t;
      m.index = k;
      m.denominator = seg;
      uni.mappings.push_back(m);

      RoadmapEdgeView ue;
      ue.edge_id = next_u_edge++;
      ue.node_a = prev;
      ue.node_b = v;
      ue.length_m = eff;
      ue.corridor_width_m = e.corridor_width_m;
      ue.speed_limit_mps = e.speed_limit_mps;
      uni.edges.push_back(ue);
      prev = v;
    }

    RoadmapEdgeView last;
    last.edge_id = next_u_edge++;
    last.node_a = prev;
    last.node_b = ub;
    last.length_m = eff;
    last.corridor_width_m = e.corridor_width_m;
    last.speed_limit_mps = e.speed_limit_mps;
    uni.edges.push_back(last);
  }

  // ③ claim_radius 재계산 — 각 균일 노드의 최소 인접 균일 엣지 길이 / 2 (뷰별 규약 (j)).
  std::unordered_map<std::uint32_t, double> min_incident;
  for (const RoadmapEdgeView & ue : uni.edges)
  {
    for (std::uint32_t endp : {ue.node_a, ue.node_b})
    {
      const auto it = min_incident.find(endp);
      if (it == min_incident.end() || ue.length_m < it->second)
      {
        min_incident[endp] = ue.length_m;
      }
    }
  }
  for (RoadmapNodeView & un : uni.nodes)
  {
    const auto it = min_incident.find(un.node_id);
    if (it != min_incident.end())
    {
      un.claim_radius_m = it->second / 2.0;
    }
  }

  uni.effective_unit_min_m = std::isfinite(eff_min) ? eff_min : 0.0;
  uni.effective_unit_max_m = eff_max;
  uni.subdivided_edge_ratio = physical.edges.empty() ? 0.0
                                                     : static_cast<double>(subdivided_edges) /
                                                         static_cast<double>(physical.edges.size());

  // ④ 저장 — 균일 뷰를 바꾸면 골격 뷰가 무효화된다(재구성 필요).
  uniform = std::move(uni);
  phys_to_uniform_node = std::move(p2u);
  uniform_to_phys_node = std::move(u2p);
  has_uniform = true;
  const std::uint32_t issued = next_view_id;
  ++next_view_id;

  has_skeleton = false;
  skeleton = RoadmapViewData{};
  phys_to_skeleton_node.clear();
  skeleton_to_phys_node.clear();

  return MapResult<std::uint32_t>::ok(issued);
}

MapResult<std::uint32_t> MapRegistry::Impl::build_skeleton(std::uint32_t uniform_view_id)
{
  if (!loaded)
  {
    return MapResult<std::uint32_t>::fail(MapStatus::MAP_NOT_LOADED);
  }
  if (!has_uniform || uniform_view_id != uniform.view_id)
  {
    return MapResult<std::uint32_t>::fail(MapStatus::VIEW_NOT_FOUND);
  }

  RoadmapViewData skel;
  skel.view_kind = ViewKind::SKELETON;
  skel.roadmap_version = roadmap_version;
  skel.view_id = next_view_id; // 유래 균일 뷰와 종류만 다르다(같은 인스턴스 스코프)

  std::unordered_map<std::uint32_t, std::uint32_t> p2s; // 물리 노드 → 골격 노드
  std::unordered_map<std::uint32_t, std::uint32_t> s2p;
  std::unordered_map<std::uint32_t, std::uint32_t> phys_edge_to_skel_edge;

  std::uint32_t next_s_node = 0;
  std::uint32_t next_s_edge = 0;

  // ① 골격 노드 = 물리 노드(가상 노드 collapse) + collapse_map 용례 2(균일 물리상 노드 → 골격
  // 노드).
  for (const RoadmapNodeView & p : physical.nodes)
  {
    const std::uint32_t s = next_s_node++;
    RoadmapNodeView sn = p;
    sn.node_id = s;
    skel.nodes.push_back(sn);
    p2s.emplace(p.node_id, s);
    s2p.emplace(s, p.node_id);

    NodeMappingView m; // 용례 2: 균일 노드 → 골격 노드
    m.kind = MappingKind::COLLAPSE_NODE;
    m.from_node_id = UniformNodeId{phys_to_uniform_node.at(p.node_id)};
    m.to_skeleton_node_id = SkeletonNodeId{s};
    skel.mappings.push_back(m);
  }

  // ② 골격 엣지 = 물리 엣지 (스텝 수는 collapse_map 용례 3 의 denominator 로 복원).
  for (const RoadmapEdgeView & e : physical.edges)
  {
    const std::uint32_t s = next_s_edge++;
    RoadmapEdgeView se = e;
    se.edge_id = s;
    se.node_a = p2s.at(e.node_a);
    se.node_b = p2s.at(e.node_b);
    skel.edges.push_back(se);
    phys_edge_to_skel_edge.emplace(e.edge_id, s);
  }

  // ③ collapse_map 용례 3 — 삼켜진 균일 가상 노드 → 골격 엣지 + 체인 서수(균일 subdivision_map
  // 에서).
  for (const NodeMappingView & sub : uniform.mappings)
  {
    if (sub.kind != MappingKind::SUBDIVISION)
    {
      continue;
    }
    const std::uint32_t phys_edge = sub.to_physical_edge_id.value();
    const auto it = phys_edge_to_skel_edge.find(phys_edge);
    if (it == phys_edge_to_skel_edge.end())
    {
      continue; // 물리 엣지 대응이 없으면 건너뛴다(정합 위반은 발생하지 않아야 한다)
    }
    NodeMappingView m;
    m.kind = MappingKind::COLLAPSE_CHAIN;
    m.from_node_id = sub.from_node_id; // 삼켜진 균일 가상 노드
    m.to_skeleton_edge_id = SkeletonEdgeId{it->second};
    m.index = sub.index;
    m.denominator = sub.denominator;
    skel.mappings.push_back(m);
  }

  skeleton = std::move(skel);
  phys_to_skeleton_node = std::move(p2s);
  skeleton_to_phys_node = std::move(s2p);
  has_skeleton = true;
  const std::uint32_t issued = skeleton.view_id;
  ++next_view_id;

  return MapResult<std::uint32_t>::ok(issued);
}

MapResult<std::uint32_t> MapRegistry::Impl::transform(
  std::uint32_t from_view_id, std::uint32_t from_node, std::uint32_t to_view_id) const
{
  if (!loaded)
  {
    return MapResult<std::uint32_t>::fail(MapStatus::MAP_NOT_LOADED);
  }

  // 뷰 id → 종류 해석 (로드된 뷰만 유효).
  const auto resolve = [&](std::uint32_t vid, ViewKind & out) -> bool
  {
    if (vid == VIEW_ID_PHYSICAL)
    {
      out = ViewKind::PHYSICAL;
      return true;
    }
    if (has_uniform && vid == uniform.view_id)
    {
      out = ViewKind::UNIFORM;
      return true;
    }
    if (has_skeleton && vid == skeleton.view_id)
    {
      out = ViewKind::SKELETON;
      return true;
    }
    return false;
  };

  ViewKind from_kind{};
  ViewKind to_kind{};
  if (!resolve(from_view_id, from_kind) || !resolve(to_view_id, to_kind))
  {
    return MapResult<std::uint32_t>::fail(MapStatus::VIEW_NOT_FOUND);
  }

  // ① 원본 노드를 물리 노드로 사상 (모든 변환은 물리 뷰를 경유한다 — C4 단일 소유).
  std::uint32_t phys_node = 0;
  switch (from_kind)
  {
    case ViewKind::PHYSICAL:
      if (!has_node(physical, from_node))
      {
        return MapResult<std::uint32_t>::fail(MapStatus::NODE_NOT_FOUND);
      }
      phys_node = from_node;
      break;
    case ViewKind::UNIFORM:
    {
      const auto it = uniform_to_phys_node.find(from_node);
      if (it != uniform_to_phys_node.end())
      {
        phys_node = it->second;
      }
      else if (has_node(uniform, from_node))
      {
        return MapResult<std::uint32_t>::fail(MapStatus::NO_MAPPING); // 가상 노드 — 물리 노드 없음
      }
      else
      {
        return MapResult<std::uint32_t>::fail(MapStatus::NODE_NOT_FOUND);
      }
      break;
    }
    case ViewKind::SKELETON:
    {
      const auto it = skeleton_to_phys_node.find(from_node);
      if (it == skeleton_to_phys_node.end())
      {
        return MapResult<std::uint32_t>::fail(MapStatus::NODE_NOT_FOUND);
      }
      phys_node = it->second;
      break;
    }
  }

  // ② 물리 노드를 목적 뷰 노드로 사상.
  switch (to_kind)
  {
    case ViewKind::PHYSICAL:
      return MapResult<std::uint32_t>::ok(phys_node);
    case ViewKind::UNIFORM:
    {
      const auto it = phys_to_uniform_node.find(phys_node);
      return it == phys_to_uniform_node.end()
               ? MapResult<std::uint32_t>::fail(MapStatus::NO_MAPPING)
               : MapResult<std::uint32_t>::ok(it->second);
    }
    case ViewKind::SKELETON:
    {
      const auto it = phys_to_skeleton_node.find(phys_node);
      return it == phys_to_skeleton_node.end()
               ? MapResult<std::uint32_t>::fail(MapStatus::NO_MAPPING)
               : MapResult<std::uint32_t>::ok(it->second);
    }
  }

  return MapResult<std::uint32_t>::fail(MapStatus::NODE_NOT_FOUND); // 도달 불가
}

} // namespace mrs
