/**
 * @file map_registry_ingest.cpp
 * @brief 물리 뷰 주입 — 구조 무결성 확인 + 저장 + graph_index 구축 + 버전 발급. 그리고 뷰 조회.
 *
 * 예외를 던지지 않는다. 로더(`map_registry_loader.cpp`)가 파싱한 물리 뷰를 받아, 인덱스가
 * 성립할 수 있는지(중복 노드 id·매달린 엣지 참조 없음)를 확인한 뒤에만 저장한다.
 */

#include <cstdint>
#include <unordered_set>
#include <utility>

#include "map_registry_impl.hpp"

namespace mrs
{

namespace
{

/**
 * @brief 물리 뷰에서 인접 인덱스를 구축한다. 모든 노드가 (고립이어도) 키로 등장한다.
 * @param[in] view 물리 뷰. 자료형 `const RoadmapViewData &`. 엣지 참조는 유효해야 한다(선행 확인).
 * @return `mapreg_detail::GraphIndex` — 노드 → 인접 노드·부속 엣지.
 */
[[nodiscard]] mapreg_detail::GraphIndex build_graph_index(const RoadmapViewData & view)
{
  mapreg_detail::GraphIndex index;
  index.neighbors.reserve(view.nodes.size());
  index.incident_edges.reserve(view.nodes.size());

  // 고립 노드도 키로 남긴다 — 검사기 1 이 차수 0 을 위반으로 보도록.
  for (const RoadmapNodeView & node : view.nodes)
  {
    index.neighbors.try_emplace(node.node_id);
    index.incident_edges.try_emplace(node.node_id);
  }

  for (const RoadmapEdgeView & edge : view.edges)
  {
    index.neighbors[edge.node_a].push_back(edge.node_b);
    index.neighbors[edge.node_b].push_back(edge.node_a);
    index.incident_edges[edge.node_a].push_back(edge.edge_id);
    index.incident_edges[edge.node_b].push_back(edge.edge_id);
  }

  return index;
}

} // namespace

MapResult<std::uint64_t> MapRegistry::Impl::ingest(RoadmapViewData physical_in)
{
  // ① 노드 id 유일성 — 인덱스가 노드를 유일하게 지목할 수 있어야 한다.
  std::unordered_set<std::uint32_t> node_ids;
  node_ids.reserve(physical_in.nodes.size());
  for (const RoadmapNodeView & node : physical_in.nodes)
  {
    if (node.node_id == ROADMAP_ID_NONE || !node_ids.insert(node.node_id).second)
    {
      return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
    }
  }

  // ② 엣지 참조 무결성 — 매달린 엣지가 있으면 인덱스·검사기가 성립하지 않는다.
  std::unordered_set<std::uint32_t> edge_ids;
  edge_ids.reserve(physical_in.edges.size());
  for (const RoadmapEdgeView & edge : physical_in.edges)
  {
    if (edge.edge_id == ROADMAP_ID_NONE || !edge_ids.insert(edge.edge_id).second)
    {
      return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
    }
    if (node_ids.count(edge.node_a) == 0 || node_ids.count(edge.node_b) == 0)
    {
      return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
    }
  }

  // ③ 저장 — 물리 뷰 스코프를 못박고 새 버전을 발급한다(단조증가).
  const std::uint64_t issued_version = roadmap_version + 1U;
  physical_in.view_kind = ViewKind::PHYSICAL;
  physical_in.view_id = VIEW_ID_PHYSICAL;
  physical_in.roadmap_version = issued_version;

  physical = std::move(physical_in);
  physical_index = build_graph_index(physical);

  // ④ 재로드는 파생 뷰·대응 맵을 무효화한다 — id 공간이 갱신되기 때문(GetUniformView 유일성 규약).
  has_uniform = false;
  uniform = RoadmapViewData{};
  has_skeleton = false;
  skeleton = RoadmapViewData{};
  next_view_id = 1U;
  phys_to_uniform_node.clear();
  uniform_to_phys_node.clear();
  phys_to_skeleton_node.clear();
  skeleton_to_phys_node.clear();

  roadmap_version = issued_version;
  loaded = true;
  return MapResult<std::uint64_t>::ok(issued_version);
}

MapResult<RoadmapViewData> MapRegistry::Impl::get_view(std::uint32_t view_id) const
{
  if (!loaded)
  {
    return MapResult<RoadmapViewData>::fail(MapStatus::MAP_NOT_LOADED);
  }
  if (view_id == VIEW_ID_PHYSICAL)
  {
    return MapResult<RoadmapViewData>::ok(physical);
  }
  if (has_uniform && view_id == uniform.view_id)
  {
    return MapResult<RoadmapViewData>::ok(uniform);
  }
  if (has_skeleton && view_id == skeleton.view_id)
  {
    return MapResult<RoadmapViewData>::ok(skeleton);
  }
  return MapResult<RoadmapViewData>::fail(MapStatus::VIEW_NOT_FOUND);
}

} // namespace mrs
