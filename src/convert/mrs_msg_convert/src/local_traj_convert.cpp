/**
 * @file local_traj_convert.cpp
 * @brief L-24(LocalPlanShare)·cmd_vel 변환 — **본문 미구현**. 시그니처와 문서만 확정된 상태다.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 스텁은 기본 생성 @ref mrs::convert::ConvertResult
 * (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않는다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

ConvertResult make_local_plan_share(
  mrs::RobotId robot_id, std::uint32_t tick_seq, const std::vector<double> & control_points,
  std::uint8_t num_segments, std::uint8_t bernstein_degree, double segment_duration_s,
  double shift_ratio_u, double stamp_s, mrs_interfaces::msg::LocalPlanShare & out)
{
  (void)robot_id;
  (void)tick_seq;
  (void)control_points;
  (void)num_segments;
  (void)bernstein_degree;
  (void)segment_duration_s;
  (void)shift_ratio_u;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::LocalPlanShare & msg, mrs::NeighborTrajectory & out)
{
  (void)msg;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_cmd_vel(const mrs::LocalPlanOutput & output, geometry_msgs::msg::Twist & out)
{
  (void)output;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
