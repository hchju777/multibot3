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

std::optional<PredecessorConstraint> DependencyGraph::unmet_predecessor(
  std::size_t robot_index, std::size_t visit_index) const
{
  const UniformNodeId node = paths_[robot_index].visits[visit_index].node_id;
  const auto entry = node_visit_order_.find(node.value());
  if (entry == node_visit_order_.end())
  {
    return std::nullopt; // 도달 불가(구축 시 이 방문이 목록에 들어갔다) — 방어적
  }
  const std::vector<VisitRef> & order = entry->second;

  // 이 방문의 통행순서 위치를 찾는다.
  std::size_t pos = order.size();
  for (std::size_t k = 0; k < order.size(); ++k)
  {
    if (order[k].robot_index == robot_index && order[k].visit_index == visit_index)
    {
      pos = k;
      break;
    }
  }
  if (pos == 0 || pos == order.size())
  {
    return std::nullopt; // 첫 통과자이거나(선행 없음) 미발견(방어적)
  }

  // 바로 앞 원소가 선행자. 같은 로봇이면 Type-1 자기순서(자동 충족) — 건너뛴다.
  const VisitRef & pred = order[pos - 1];
  if (pred.robot_index == robot_index)
  {
    return std::nullopt;
  }
  // 선행 로봇이 그 노드를 클리어했는가 — cleared_count_ 가 통과 방문을 지났으면 충족.
  if (cleared_count_[pred.robot_index] > pred.visit_index)
  {
    return std::nullopt; // 충족
  }
  PredecessorConstraint constraint;
  constraint.predecessor_robot_id = paths_[pred.robot_index].robot_id;
  constraint.node_id = node;
  return constraint;
}

bool DependencyGraph::release_next_window(RobotId robot_id, ExecutionWindow & out_window)
{
  if (!built_)
  {
    return false;
  }
  const auto it = robot_index_.find(robot_id);
  if (it == robot_index_.end())
  {
    return false;
  }
  const std::size_t i = it->second;
  const std::vector<TimedNodeVisit> & visits = paths_[i].visits;
  const std::size_t n = visits.size();

  const std::size_t frontier = released_count_[i];
  if (frontier + 1 >= n)
  {
    return false; // 목표 도달 — 더 릴리스할 세그먼트 없음
  }
  // 프론티어(직전 창의 끝점)가 아직 막혀 있으면 재릴리스하지 않는다 — 로봇은 여기서 대기 중이다.
  // 프론티어 0(로봇의 시작 노드)은 이미 그 자리에 있으므로 진입 제약을 적용하지 않는다.
  if (frontier > 0 && unmet_predecessor(i, frontier).has_value())
  {
    return false;
  }

  ExecutionWindow window;
  window.robot_id = robot_id;
  window.roadmap_version = roadmap_version_;
  window.view_id = view_id_;
  window.revision_kind = RevisionKind::NEW;
  window.valid_through_segment_index = -1; // NEW 규약
  window.plan_epoch = 0; // 계획 세대는 발행 노드가 봉투로 채운다(PlannedPaths)
  window.window_valid_until_s = 0.0; // 만료 시각은 발행 노드가 now+마진으로 스탬프(W5 입력게이트)

  // 프론티어에서 앞으로 세그먼트를 릴리스한다. 엔드포인트가 막히면 그 세그먼트까지 넣고 정지.
  std::size_t endpoint = frontier;
  for (std::size_t j = frontier; j + 1 < n; ++j)
  {
    WindowSegment segment;
    segment.node_from = visits[j].node_id;   // W2: 인접 세그먼트 연결(연속 방문)
    segment.node_to = visits[j + 1].node_id; // W3: 첫 세그먼트 node_from = 프론티어
    window.segments.push_back(segment);
    endpoint = j + 1;

    const std::optional<PredecessorConstraint> pred = unmet_predecessor(i, j + 1);
    if (pred.has_value())
    {
      window.predecessor_constraints.push_back(*pred);
      break; // 미충족 진입 지점 — 여기까지 릴리스하고 로봇은 이 노드 앞에서 대기
    }
  }

  released_count_[i] = endpoint;
  window.window_seq = next_window_seq_[robot_id]; // W1: 로봇별 단조증가
  ++next_window_seq_[robot_id];
  out_window = std::move(window);
  return true;
}

void DependencyGraph::on_progress_event(RobotId robot_id, UniformNodeId cleared_node, double time_s)
{
  if (!built_ || !std::isfinite(time_s))
  {
    return; // 미구축·비유한 시각은 안전 무시(예외 던지지 않는다)
  }
  const auto it = robot_index_.find(robot_id);
  if (it == robot_index_.end())
  {
    return;
  }
  const std::size_t i = it->second;
  const std::vector<TimedNodeVisit> & visits = paths_[i].visits;

  // 로봇은 방문열을 순서대로 클리어한다. 다음 예상 방문 노드와 일치할 때만 전진(순서 외·재방문
  // 불일치는 안전 무시 — 진행 상태를 조용히 앞당기지 않는다).
  if (cleared_count_[i] < visits.size() && visits[cleared_count_[i]].node_id == cleared_node)
  {
    ++cleared_count_[i];
  }
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
  // ⛔ **[0b] 입력 게이트 — 안전 no-op.** staleness repair(NF7, W5)는 릴리스된 창의 만료 시각
  //    (`window_valid_until_s`)이 지났는지로 판정하는데, 그 만료 시각의 **마진**은 동역학 상수
  //    `u_max/a_max + Δt_h`(v_max 의존)로 정해지고 v_max 는 iw.hub 자산에서 **미실측**이다
  //    (pending-[0b]). 또한 `release_next_window` 가 `now_s` 를 받지 않아 depgraph 는 만료 시각을
  //    보유하지 않는다(현재는 발행 노드가 now+마진으로 스탬프). 따라서 여기서 **의미 있는 실효를
  //    수행할 근거가 없다** — 지어낸 마진으로 창을 실효시키면 T1 I-QP 전제(커밋 경계 보호)를 깰
  //    위험이 있으므로, [0b] v_max 실측 + release 의 시간 입력 배선 전까지 **아무 것도 하지
  //    않는다**. (예외를 던지지 않는다 — 규율 #2. 조용한 부분 구현 대신 명시적 no-op + 게이트
  //    기록.)
  (void)now_s;
}

} // namespace mrs
