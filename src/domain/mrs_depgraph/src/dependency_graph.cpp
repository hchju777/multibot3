#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "mrs_depgraph/dependency_graph.hpp"

namespace mrs
{

bool DependencyGraph::build_from_paths(
  const std::vector<RobotPath> & paths, std::uint64_t roadmap_version, std::uint32_t view_id)
{
  // ① 스코프 자기 유효성 — 0 은 계약이 런타임에 금지한다(수신자 100% 폐기).
  if (roadmap_version == ROADMAP_VERSION_UNSPECIFIED)
  {
    return false;
  }

  // ② 검증 + 로봇 인덱싱. 실패 시 저장 상태를 **건드리지 않고** false — 부분 구축을 만들지 않는다.
  std::unordered_map<std::uint16_t, std::size_t> robot_index;
  robot_index.reserve(paths.size());
  for (std::size_t i = 0; i < paths.size(); ++i)
  {
    const RobotPath & path = paths[i];
    if (path.robot_id == ROBOT_ID_NONE)
    {
      return false; // 센티넬 로봇 id
    }
    if (!robot_index.emplace(path.robot_id, i).second)
    {
      return false; // 로봇 id 중복 — 방문열이 어느 로봇 것인지 모호해진다
    }
    // 방문열: 센티넬 노드 금지·시각 유한·arrival 단조 비감소(시간 역행은 손상된 계획).
    double prev_arrival = -std::numeric_limits<double>::infinity();
    for (const TimedNodeVisit & visit : path.visits)
    {
      if (visit.node_id.is_none())
      {
        return false;
      }
      if (!std::isfinite(visit.arrival_time_s) || visit.arrival_time_s < prev_arrival)
      {
        return false;
      }
      prev_arrival = visit.arrival_time_s;
    }
  }

  // ③ Type-2 통행순서 구축 — 노드별 방문을 (arrival_time, robot_id, visit_index) 오름차순 정렬.
  //    목록의 앞 원소가 그 노드를 먼저 통과해야 한다(adg-2019, 순서=공간). R2 reorder 가 나중에
  //    이 순서를 바꾼다([3]/[4]) — 여기서는 L2 계획의 도착시각 순서가 초기 통행순서다.
  std::unordered_map<std::uint32_t, std::vector<VisitRef>> node_visit_order;
  for (std::size_t i = 0; i < paths.size(); ++i)
  {
    for (std::size_t k = 0; k < paths[i].visits.size(); ++k)
    {
      node_visit_order[paths[i].visits[k].node_id.value()].push_back(VisitRef{i, k});
    }
  }
  const auto order_less = [&paths](const VisitRef & a, const VisitRef & b)
  {
    const double ta = paths[a.robot_index].visits[a.visit_index].arrival_time_s;
    const double tb = paths[b.robot_index].visits[b.visit_index].arrival_time_s;
    if (ta != tb)
    {
      return ta < tb;
    }
    const std::uint16_t ra = paths[a.robot_index].robot_id;
    const std::uint16_t rb = paths[b.robot_index].robot_id;
    if (ra != rb)
    {
      return ra < rb; // 동시각 — robot_id 로 결정론적 tie-break
    }
    return a.visit_index < b.visit_index; // 같은 로봇이 같은 시각에 두 번 — 방문 순서
  };
  for (auto & entry : node_visit_order)
  {
    std::sort(entry.second.begin(), entry.second.end(), order_less);
  }

  // ④ window_seq 보존 — 재빌드 전 존재한 로봇은 seq 를 이어받고(W1 단조), 신규 로봇은 1 부터.
  std::unordered_map<std::uint16_t, std::uint32_t> next_seq;
  next_seq.reserve(paths.size());
  for (const RobotPath & path : paths)
  {
    const auto it = next_window_seq_.find(path.robot_id);
    next_seq.emplace(path.robot_id, it == next_window_seq_.end() ? 1U : it->second);
  }

  // ⑤ 커밋 — 전 검증을 통과했으므로 여기부터 실패하지 않는다. 저장 상태를 한 번에 교체한다.
  paths_ = paths;
  robot_index_ = std::move(robot_index);
  node_visit_order_ = std::move(node_visit_order);
  next_window_seq_ = std::move(next_seq);
  released_count_.assign(paths.size(), 0U);
  cleared_count_.assign(paths.size(), 0U);
  roadmap_version_ = roadmap_version;
  view_id_ = view_id;
  built_ = true;
  return true;
}

bool DependencyGraph::release_next_window(RobotId robot_id, ExecutionWindow & out_window)
{
  (void)robot_id;
  (void)out_window;
  throw std::logic_error(
    "not implemented: DependencyGraph::release_next_window — Phase 5 (coordination-builder) 대상");
}

void DependencyGraph::on_progress_event(RobotId robot_id, UniformNodeId cleared_node, double time_s)
{
  (void)robot_id;
  (void)cleared_node;
  (void)time_s;
  throw std::logic_error(
    "not implemented: DependencyGraph::on_progress_event — Phase 5 (coordination-builder) 대상");
}

bool DependencyGraph::splice_partial_plan(
  RobotId robot_id, const std::vector<RobotPath> & new_partial_paths,
  const std::vector<FrozenOrder> & frozen_orders, const CommitState & commit_boundary)
{
  (void)robot_id;
  (void)new_partial_paths;
  (void)frozen_orders;
  (void)commit_boundary;
  throw std::logic_error(
    "not implemented: DependencyGraph::splice_partial_plan — Phase 5 (coordination-builder) 대상");
}

void DependencyGraph::invalidate_stale_windows(double now_s)
{
  (void)now_s;
  throw std::logic_error("not implemented: DependencyGraph::invalidate_stale_windows — Phase 5 "
                         "(coordination-builder) 대상");
}

} // namespace mrs
