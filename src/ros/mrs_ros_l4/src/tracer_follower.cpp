/**
 * @file tracer_follower.cpp
 * @brief @ref tracer_follower.hpp 의 구현 — 순수 함수, rclcpp 무의존.
 */

#include <cmath>

#include "mrs_ros_l4/tracer_follower.hpp"

namespace mrs::tracer
{

SegmentAdvance evaluate_segment_advance(
  bool observation_valid, UniformNodeId occupied_node, UniformNodeId target_node, double elapsed_s,
  double segment_timeout_s) noexcept
{
  // ① 관측 도달이 우선한다. 관측이 없거나 엣지 위(센티넬)이면 도달로 보지 않는다 —
  //    미관측을 도달로 오인하면 창이 실제 진행 없이 소비되어 [0a] 계측이 거짓이 된다.
  if (
    observation_valid && !occupied_node.is_none() && !target_node.is_none() &&
    occupied_node == target_node)
  {
    return SegmentAdvance::NODE_REACHED;
  }

  // ② 타임아웃. 비유한 경과시간은 판정하지 않는다(시각 가드가 이미 걸렀어야 하는 값이다).
  if (segment_timeout_s > 0.0 && std::isfinite(elapsed_s) && elapsed_s >= segment_timeout_s)
  {
    return SegmentAdvance::TIMEOUT;
  }

  return SegmentAdvance::NOT_DUE;
}

bool is_window_live(
  const ExecutionWindow & window, std::size_t segment_index, double now_s) noexcept
{
  if (window.revision_kind != RevisionKind::NEW)
  {
    return false;
  }
  if (segment_index >= window.segments.size())
  {
    return false;
  }
  if (!std::isfinite(now_s) || !std::isfinite(window.window_valid_until_s))
  {
    return false;
  }
  return now_s < window.window_valid_until_s;
}

bool is_window_exhaustion_imminent(
  const ExecutionWindow & window, std::size_t segment_index, double now_s,
  std::size_t remaining_segment_threshold, double lead_time_s) noexcept
{
  const std::size_t total = window.segments.size();
  const std::size_t remaining = (segment_index >= total) ? 0U : (total - segment_index);
  if (remaining <= remaining_segment_threshold)
  {
    return true;
  }

  if (lead_time_s > 0.0 && std::isfinite(now_s) && std::isfinite(window.window_valid_until_s))
  {
    return (window.window_valid_until_s - now_s) <= lead_time_s;
  }

  return false;
}

FollowCommand make_follow_command(bool window_live, double follow_speed_mps) noexcept
{
  FollowCommand command;
  if (window_live && std::isfinite(follow_speed_mps) && follow_speed_mps > 0.0)
  {
    command.v_mps = follow_speed_mps;
  }
  command.omega_rps = 0.0;
  return command;
}

} // namespace mrs::tracer
