/**
 * @file map_registry_coords.cpp
 * @brief 노드 ↔ 연속 좌표 변환 (TransformNode MODE_NODE_TO_POSE / MODE_POSE_TO_NODE). 예외를
 * 던지지 않는다.
 *
 * 뷰 간 노드 변환(@ref MapRegistry::Impl::transform)과 다르다 — 여기서는 **단일 뷰 내부**에서
 * id ↔ 좌표만 다룬다. node→pose 는 조회(전단사 방향), pose→node 는 최근접(다대일 부분함수)이라
 * 잔차 0 이 보장되지 않는다(TransformNode.srv 재잠금 8차 (i)).
 */

#include <cmath>
#include <cstdint>
#include <limits>

#include "map_registry_impl.hpp"

namespace mrs
{

const RoadmapViewData * MapRegistry::Impl::resolve_view(std::uint32_t view_id) const noexcept
{
  if (view_id == VIEW_ID_PHYSICAL)
  {
    return &physical;
  }
  if (has_uniform && view_id == uniform.view_id)
  {
    return &uniform;
  }
  if (has_skeleton && view_id == skeleton.view_id)
  {
    return &skeleton;
  }
  return nullptr;
}

MapResult<Pose2D> MapRegistry::Impl::node_to_pose(
  std::uint32_t view_id, std::uint32_t node_id) const
{
  if (!loaded)
  {
    return MapResult<Pose2D>::fail(MapStatus::MAP_NOT_LOADED);
  }
  const RoadmapViewData * view = resolve_view(view_id);
  if (view == nullptr)
  {
    return MapResult<Pose2D>::fail(MapStatus::VIEW_NOT_FOUND);
  }

  for (const RoadmapNodeView & node : view->nodes)
  {
    if (node.node_id == node_id)
    {
      Pose2D pose;
      pose.x_m = node.x_m;
      pose.y_m = node.y_m;
      pose.theta_rad = 0.0; // 노드는 위치점이라 방위가 없다(계약 TransformNode.srv: theta 는 0).
      return MapResult<Pose2D>::ok(pose);
    }
  }
  return MapResult<Pose2D>::fail(MapStatus::NODE_NOT_FOUND);
}

MapResult<NearestNode> MapRegistry::Impl::pose_to_node(
  std::uint32_t view_id, double x_m, double y_m) const
{
  if (!loaded)
  {
    return MapResult<NearestNode>::fail(MapStatus::MAP_NOT_LOADED);
  }
  // 비유한 질의는 거리 비교를 무의미하게 만든다 — 최근접이 성립하지 않으므로 거부한다.
  if (!std::isfinite(x_m) || !std::isfinite(y_m))
  {
    return MapResult<NearestNode>::fail(MapStatus::NODE_NOT_FOUND);
  }
  const RoadmapViewData * view = resolve_view(view_id);
  if (view == nullptr)
  {
    return MapResult<NearestNode>::fail(MapStatus::VIEW_NOT_FOUND);
  }
  if (view->nodes.empty())
  {
    return MapResult<NearestNode>::fail(MapStatus::NODE_NOT_FOUND);
  }

  // 저빈도 경로(초기화·검증 전용) — 선형 최근접 스캔. 거리 비교는 제곱거리로 하고 잔차만 sqrt.
  NearestNode best;
  double best_dist_sq = std::numeric_limits<double>::infinity();
  for (const RoadmapNodeView & node : view->nodes)
  {
    const double dx = node.x_m - x_m;
    const double dy = node.y_m - y_m;
    const double dist_sq = (dx * dx) + (dy * dy);
    if (dist_sq < best_dist_sq)
    {
      best_dist_sq = dist_sq;
      best.node_id = node.node_id;
    }
  }
  best.residual_m = std::sqrt(best_dist_sq);
  return MapResult<NearestNode>::ok(best);
}

} // namespace mrs
