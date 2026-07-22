/**
 * @file map_registry_loader.cpp
 * @brief 물리 roadmap YAML 로더 — **yaml-cpp 를 이 번역 단위에만 격리**한다.
 *
 * 예외를 던지지 않는다(CLAUDE.md 규율 2·5). yaml-cpp 는 파일 부재·타입 불일치·문법 오류에서
 * 예외를 던지므로 전량 try/catch 로 감싸 @ref mrs::MapResult 실패로 변환한다.
 *
 * ## 스키마 (물리 roadmap — RoadmapNode.msg / RoadmapEdge.msg 미러)
 * @code{.yaml}
 * nodes:
 *   - {id: 0, x: 1.0, y: 2.0, claim_radius: 1.11, is_endpoint: true, is_task_endpoint: false}
 * edges:
 *   - {id: 0, a: 0, b: 1, width: 2.0, speed_limit: 0.0}   # speed_limit 선택(기본 0)
 * @endcode
 * - `roadmap_version` 은 파일에 두지 않는다 — 버전은 MapRegistry 가 발급한다(단조증가).
 * - 엣지 `length` 는 파일에 두지 않고 **두 노드 좌표에서 계산**한다 → 물리 엣지 직선 전제를
 *   구성적으로 강제한다(재잠금 8차 (g)).
 * - `is_virtual` 은 물리 뷰에서 항상 false(로더가 설정).
 */

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "map_registry_impl.hpp"

namespace mrs
{

namespace
{

/** @brief 로더 내부 파싱 실패 신호 — 예외 대신 값으로 상위 try 블록에 전달한다. */
struct ParseError
{
};

/**
 * @brief YAML 노드 시퀀스를 물리 노드 목록으로 파싱한다.
 * @param[in] nodes_yaml `nodes` 시퀀스. 자료형 `const YAML::Node &`.
 * @param[out] out 파싱된 노드 목록. 자료형 `std::vector<RoadmapNodeView> &`.
 * @throws ParseError 필수 필드 부재·범위 위반(음수 claim_radius·비유한 좌표·task ⇒ ¬endpoint).
 */
void parse_nodes(const YAML::Node & nodes_yaml, std::vector<RoadmapNodeView> & out)
{
  if (!nodes_yaml || !nodes_yaml.IsSequence() || nodes_yaml.size() == 0U)
  {
    throw ParseError{};
  }

  out.reserve(nodes_yaml.size());
  for (const YAML::Node & n : nodes_yaml)
  {
    if (
      !n["id"] || !n["x"] || !n["y"] || !n["claim_radius"] || !n["is_endpoint"] ||
      !n["is_task_endpoint"])
    {
      throw ParseError{};
    }

    RoadmapNodeView node;
    node.node_id = n["id"].as<std::uint32_t>();
    node.x_m = n["x"].as<double>();
    node.y_m = n["y"].as<double>();
    node.claim_radius_m = n["claim_radius"].as<double>();
    node.is_endpoint = n["is_endpoint"].as<bool>();
    node.is_task_endpoint = n["is_task_endpoint"].as<bool>();
    node.is_virtual = false; // 물리 뷰는 항상 false

    if (
      !std::isfinite(node.x_m) || !std::isfinite(node.y_m) || !std::isfinite(node.claim_radius_m) ||
      node.claim_radius_m <= 0.0)
    {
      throw ParseError{};
    }
    if (node.is_task_endpoint && !node.is_endpoint) // 불변식: 작업 엔드포인트는 엔드포인트여야 한다
    {
      throw ParseError{};
    }

    out.push_back(node);
  }
}

/**
 * @brief YAML 엣지 시퀀스를 물리 엣지 목록으로 파싱한다(길이는 좌표에서 계산).
 * @param[in] edges_yaml `edges` 시퀀스. 자료형 `const YAML::Node &`.
 * @param[in] coords 노드 id → (x, y) 좌표. 자료형 `const std::unordered_map<...> &`.
 * @param[out] out 파싱된 엣지 목록. 자료형 `std::vector<RoadmapEdgeView> &`.
 * @throws ParseError 필수 필드 부재·범위 위반(음수 폭·비유한). 매달린 참조는 ingest 가 판정한다.
 */
void parse_edges(
  const YAML::Node & edges_yaml,
  const std::unordered_map<std::uint32_t, std::pair<double, double>> & coords,
  std::vector<RoadmapEdgeView> & out)
{
  if (!edges_yaml || !edges_yaml.IsSequence())
  {
    throw ParseError{};
  }

  out.reserve(edges_yaml.size());
  for (const YAML::Node & e : edges_yaml)
  {
    if (!e["id"] || !e["a"] || !e["b"] || !e["width"])
    {
      throw ParseError{};
    }

    RoadmapEdgeView edge;
    edge.edge_id = e["id"].as<std::uint32_t>();
    edge.node_a = e["a"].as<std::uint32_t>();
    edge.node_b = e["b"].as<std::uint32_t>();
    edge.corridor_width_m = e["width"].as<double>();
    edge.speed_limit_mps = e["speed_limit"] ? e["speed_limit"].as<double>() : 0.0;

    if (
      !std::isfinite(edge.corridor_width_m) || edge.corridor_width_m < 0.0 ||
      !std::isfinite(edge.speed_limit_mps) || edge.speed_limit_mps < 0.0)
    {
      throw ParseError{};
    }

    // 길이는 두 노드 좌표에서 계산(직선 전제). 매달린 참조면 0 으로 두고 ingest 가 거부한다.
    const auto a_it = coords.find(edge.node_a);
    const auto b_it = coords.find(edge.node_b);
    if (a_it != coords.end() && b_it != coords.end())
    {
      const double dx = a_it->second.first - b_it->second.first;
      const double dy = a_it->second.second - b_it->second.second;
      edge.length_m = std::sqrt((dx * dx) + (dy * dy));
    }

    out.push_back(edge);
  }
}

} // namespace

MapResult<std::uint64_t> MapRegistry::Impl::load(const std::string & map_file_path)
{
  RoadmapViewData parsed;
  try
  {
    const YAML::Node root = YAML::LoadFile(map_file_path);

    parse_nodes(root["nodes"], parsed.nodes);

    std::unordered_map<std::uint32_t, std::pair<double, double>> coords;
    coords.reserve(parsed.nodes.size());
    for (const RoadmapNodeView & node : parsed.nodes)
    {
      coords.emplace(node.node_id, std::make_pair(node.x_m, node.y_m));
    }

    parse_edges(root["edges"], coords, parsed.edges);
  }
  catch (const YAML::BadFile &)
  {
    return MapResult<std::uint64_t>::fail(MapStatus::FILE_NOT_FOUND);
  }
  catch (const ParseError &)
  {
    return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
  }
  catch (const YAML::Exception &) // 문법 오류·타입 불일치 등 yaml-cpp 전체
  {
    return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
  }
  catch (...) // 어떤 예외도 노드를 죽이지 않는다(규율 5)
  {
    return MapResult<std::uint64_t>::fail(MapStatus::FILE_PARSE_ERROR);
  }

  return ingest(std::move(parsed));
}

} // namespace mrs
