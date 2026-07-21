#pragma once

/**
 * @file convert_test_fixtures.hpp
 * @brief 변환 테스트 3파일이 공유하는 픽스처 — 유효한 도메인 값과 구조 비교 술어.
 *
 * 픽스처는 **구현을 호출하지 않고** 손으로 채운 값만 담는다. 변환 결과를 다시 변환해 기대값으로
 * 삼으면 항등 테스트가 자기 자신을 증명하게 되므로, 원본은 항상 리터럴에서 나온다.
 */

#include <cstddef>
#include <cstdint>
#include <vector>

// 도메인 타입 선언은 전부 이 헤더가 끌어온다 — 테스트가 도메인 헤더 경로를 따로 알 필요가 없다.
#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs_test
{

/**
 * @brief 균일 뷰 스코프를 만든다.
 * @param[in] version 지도 버전. 자료형 `std::uint64_t`. 0 은 런타임 금지값이다.
 * @param[in] view_id 뷰 인스턴스 id. 자료형 `std::uint32_t`.
 * @return `mrs::ViewScope` — `view_kind = UNIFORM` 인 스코프.
 */
inline mrs::ViewScope uniform_scope(std::uint64_t version, std::uint32_t view_id)
{
  mrs::ViewScope scope;
  scope.view_kind = mrs::ViewKind::UNIFORM;
  scope.roadmap_version = version;
  scope.view_id = view_id;
  return scope;
}

/**
 * @brief 전 필드가 유효한 실행 창을 만든다 (개정 종류 NEW, 세그먼트 2개, 선행 제약 1개).
 * @return `mrs::ExecutionWindow` — 변환이 성공해야 하는 기준 창.
 */
inline mrs::ExecutionWindow make_window()
{
  mrs::ExecutionWindow window;
  window.robot_id = 3;
  window.window_seq = 7;
  window.plan_epoch = 11;
  window.roadmap_version = 42;
  window.view_id = 5;

  mrs::WindowSegment first;
  first.node_from = mrs::UniformNodeId{10};
  first.node_to = mrs::UniformNodeId{11};
  mrs::WindowSegment second;
  second.node_from = mrs::UniformNodeId{11};
  second.node_to = mrs::UniformNodeId{12};
  window.segments = {first, second};

  mrs::PredecessorConstraint constraint;
  constraint.predecessor_robot_id = 1;
  constraint.node_id = mrs::UniformNodeId{12};
  window.predecessor_constraints = {constraint};

  window.window_valid_until_s = 34.25;
  window.revision_kind = mrs::RevisionKind::NEW;
  window.valid_through_segment_index = -1;
  return window;
}

/**
 * @brief 두 실행 창이 전 필드에서 같은지 비교한다 (왕복 항등 판정용).
 * @param[in] lhs 좌변 창. 자료형 `mrs::ExecutionWindow`.
 * @param[in] rhs 우변 창. 자료형 `mrs::ExecutionWindow`.
 * @return `bool` — 스칼라 10필드와 두 배열의 전 원소가 같으면 true.
 */
inline bool same_window(const mrs::ExecutionWindow & lhs, const mrs::ExecutionWindow & rhs)
{
  if (
    lhs.robot_id != rhs.robot_id || lhs.window_seq != rhs.window_seq ||
    lhs.plan_epoch != rhs.plan_epoch || lhs.roadmap_version != rhs.roadmap_version ||
    lhs.view_id != rhs.view_id || lhs.window_valid_until_s != rhs.window_valid_until_s ||
    lhs.revision_kind != rhs.revision_kind ||
    lhs.valid_through_segment_index != rhs.valid_through_segment_index)
  {
    return false;
  }
  if (
    lhs.segments.size() != rhs.segments.size() ||
    lhs.predecessor_constraints.size() != rhs.predecessor_constraints.size())
  {
    return false;
  }
  for (std::size_t i = 0; i < lhs.segments.size(); ++i)
  {
    if (
      lhs.segments[i].node_from != rhs.segments[i].node_from ||
      lhs.segments[i].node_to != rhs.segments[i].node_to)
    {
      return false;
    }
  }
  for (std::size_t i = 0; i < lhs.predecessor_constraints.size(); ++i)
  {
    if (
      lhs.predecessor_constraints[i].predecessor_robot_id !=
        rhs.predecessor_constraints[i].predecessor_robot_id ||
      lhs.predecessor_constraints[i].node_id != rhs.predecessor_constraints[i].node_id)
    {
      return false;
    }
  }
  return true;
}

/**
 * @brief 전 필드가 유효한 계획(로봇 2대, 각 방문 3개)을 만든다 (L-09 표준형).
 *
 * 도착 시각은 **순증가**다 — `RobotPath.msg` 가 요구하는 불변식이며, 이 픽스처가 그 정상 형태다.
 *
 * @return `std::vector<mrs::RobotPath>` — 변환이 성공해야 하는 기준 계획.
 */
inline std::vector<mrs::RobotPath> make_planned_paths()
{
  std::vector<mrs::RobotPath> paths;
  for (std::uint16_t robot_id = 0; robot_id < 2U; ++robot_id)
  {
    mrs::RobotPath path;
    path.robot_id = robot_id;
    for (std::size_t k = 0; k < 3U; ++k)
    {
      mrs::TimedNodeVisit visit;
      visit.node_id = mrs::UniformNodeId{static_cast<std::uint32_t>(10U * (robot_id + 1U) + k)};
      // 1.5 s 간격. 로봇마다 0.25 s 어긋나게 두어 로봇별 값 뒤섞임도 왕복에서 드러나게 한다.
      visit.arrival_time_s =
        2.0 + 0.25 * static_cast<double>(robot_id) + 1.5 * static_cast<double>(k);
      path.visits.push_back(visit);
    }
    paths.push_back(path);
  }
  return paths;
}

/**
 * @brief 두 계획이 전 필드에서 같은지 비교한다 (왕복 항등 판정용).
 * @param[in] lhs 좌변 계획. 자료형 `std::vector<mrs::RobotPath>`.
 * @param[in] rhs 우변 계획. 자료형 `std::vector<mrs::RobotPath>`.
 * @return `bool` — 로봇 수·순서·`robot_id`·방문열의 노드 id 와 도착 시각이 전부 같으면 true.
 */
inline bool same_planned_paths(
  const std::vector<mrs::RobotPath> & lhs, const std::vector<mrs::RobotPath> & rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }
  for (std::size_t i = 0; i < lhs.size(); ++i)
  {
    if (lhs[i].robot_id != rhs[i].robot_id || lhs[i].visits.size() != rhs[i].visits.size())
    {
      return false;
    }
    for (std::size_t k = 0; k < lhs[i].visits.size(); ++k)
    {
      if (
        lhs[i].visits[k].node_id != rhs[i].visits[k].node_id ||
        lhs[i].visits[k].arrival_time_s != rhs[i].visits[k].arrival_time_s)
      {
        return false;
      }
    }
  }
  return true;
}

/**
 * @brief 전 필드가 유효한 로봇 관측을 만든다. `next_node` 는 센티넬(= "미상", 정상값)이다.
 * @return `mrs::RobotObservation` — 변환이 성공해야 하는 기준 관측.
 */
inline mrs::RobotObservation make_observation()
{
  mrs::RobotObservation observation;
  observation.robot_id = 2;
  observation.body_pose.x_m = 1.5;
  observation.body_pose.y_m = -2.25;
  observation.body_pose.theta_rad = 0.75;
  observation.v_mps = 0.5;
  observation.omega_rps = -0.25;
  observation.view_scope = uniform_scope(42, 5);
  observation.occupied_node = mrs::UniformNodeId{7};
  observation.next_node = mrs::UNIFORM_NODE_ID_NONE;
  observation.edge_progress = 0.25;
  observation.uncertainty.pos_std_m = 0.0;
  observation.uncertainty.theta_std_rad = 0.0;
  observation.uncertainty.partial_observation = false;
  observation.uncertainty.observation_time_s = 12.5;
  return observation;
}

/**
 * @brief 두 로봇 관측이 전 필드에서 같은지 비교한다 (왕복 항등 판정용).
 * @param[in] lhs 좌변 관측. 자료형 `mrs::RobotObservation`.
 * @param[in] rhs 우변 관측. 자료형 `mrs::RobotObservation`.
 * @return `bool` — 자세·속도·스코프·노드·불확실성 전 필드가 같으면 true.
 */
inline bool same_observation(const mrs::RobotObservation & lhs, const mrs::RobotObservation & rhs)
{
  return lhs.robot_id == rhs.robot_id && lhs.body_pose.x_m == rhs.body_pose.x_m &&
         lhs.body_pose.y_m == rhs.body_pose.y_m &&
         lhs.body_pose.theta_rad == rhs.body_pose.theta_rad && lhs.v_mps == rhs.v_mps &&
         lhs.omega_rps == rhs.omega_rps && lhs.view_scope == rhs.view_scope &&
         lhs.occupied_node == rhs.occupied_node && lhs.next_node == rhs.next_node &&
         lhs.edge_progress == rhs.edge_progress &&
         lhs.uncertainty.pos_std_m == rhs.uncertainty.pos_std_m &&
         lhs.uncertainty.theta_std_rad == rhs.uncertainty.theta_std_rad &&
         lhs.uncertainty.partial_observation == rhs.uncertainty.partial_observation &&
         lhs.uncertainty.observation_time_s == rhs.uncertainty.observation_time_s;
}

/**
 * @brief 제어점 배열을 만든다. 길이 불변식은 `2 * segments * (degree + 1)` 이다.
 * @param[in] segments 세그먼트 수. 자료형 `std::uint8_t`.
 * @param[in] degree 스플라인 차수. 자료형 `std::uint8_t`.
 * @return `std::vector<double>` — 0, 0.25, 0.5 … 로 채운 결정론적 배열.
 */
inline std::vector<double> make_control_points(std::uint8_t segments, std::uint8_t degree)
{
  const std::size_t count =
    std::size_t{2} * static_cast<std::size_t>(segments) * (static_cast<std::size_t>(degree) + 1);
  std::vector<double> points;
  points.reserve(count);
  for (std::size_t i = 0; i < count; ++i)
  {
    points.push_back(static_cast<double>(i) * 0.25);
  }
  return points;
}

/**
 * @brief 물리 뷰 roadmap 데이터를 만든다 (노드 2개, 엣지 1개).
 * @return `mrs::RoadmapViewData` — `view_id = 0` 인 물리 뷰.
 */
inline mrs::RoadmapViewData make_view()
{
  mrs::RoadmapViewData view;
  view.roadmap_version = 42;
  view.view_id = 0;

  mrs::RoadmapNodeView first;
  first.node_id = 1;
  first.x_m = 0.0;
  first.y_m = 0.0;
  first.claim_radius_m = 0.5;
  first.is_endpoint = true;
  first.is_task_endpoint = true;

  mrs::RoadmapNodeView second = first;
  second.node_id = 2;
  second.x_m = 1.0;
  second.is_task_endpoint = false;
  view.nodes = {first, second};

  mrs::RoadmapEdgeView edge;
  edge.edge_id = 100;
  edge.node_a = 1;
  edge.node_b = 2;
  edge.length_m = 1.0;
  edge.corridor_width_m = 1.2;
  edge.speed_limit_mps = 0.8;
  view.edges = {edge};
  return view;
}

} // namespace mrs_test
