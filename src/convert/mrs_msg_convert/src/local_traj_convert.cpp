/**
 * @file local_traj_convert.cpp
 * @brief L-24(LocalPlanShare)·cmd_vel 변환 — [0a] tracer bullet 경로 3함수 구현.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 실패는 전부 @ref mrs::convert::ConvertResult
 * 로 보고하고 호출 노드가 안전 폴백한다.
 *
 * ## 길이 불변식이 왜 발행 측에도 걸리는가
 * `control_points.size() == 2 * num_segments * (bernstein_degree + 1)` 는 발행·수신 **양쪽**에서
 * 검사한다. 발행 측 검사를 생략하면 이웃이 세그먼트 경계를 잘못 잡은 계획을 받아 시프트
 * 연산자 S 가 **조용히 틀린 ĉ_j** 를 계산한다 — 폐기됐어야 할 계획이 유효한 이웃 계획으로 둔갑한다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

namespace mrs::convert
{
namespace
{

/**
 * @brief 채택 계획의 형상 불변식을 검사한다 (L-24, 발행·수신 공용).
 *
 * 발행 측과 수신 측이 각자 판정하면 즉시 드리프트하므로 이 파일 안에 하나만 둔다.
 * 길이 비교는 `std::size_t` 로 **승격해서** 한다 — 두 인자가 `uint8` 이라 곱셈을 그대로 두면
 * 오버플로해서 틀린 기대 길이와 대조하게 된다.
 *
 * @param[in] control_points 채택 계획 c^(h) 의 제어점(평탄 (x,y) 배열). 자료형 `std::vector<double>`.
 * @param[in] num_segments 지평 세그먼트 수 H. 자료형 `std::uint8_t`. 1 이상이어야 한다.
 * @param[in] bernstein_degree Bernstein 차수 n. 자료형 `std::uint8_t`. 1 이상이어야 한다.
 * @param[in] segment_duration_s 세그먼트 길이 Δt [s]. 자료형 `double`. 유한·양수여야 한다.
 * @param[in] shift_ratio_u u = Δt_h/Δt. 자료형 `double`. 유한이고 개구간 (0, 1) 이어야 한다.
 * @return `bool` — 다섯 검사(하한 2건·세그먼트 길이·길이 불변식·원소 유한성·u 범위)를
 *         전부 통과하면 true.
 */
[[nodiscard]] bool is_plan_shape_valid(
  const std::vector<double> & control_points, std::uint8_t num_segments,
  std::uint8_t bernstein_degree, double segment_duration_s, double shift_ratio_u) noexcept
{
  // num_segments = 0 이면 기대 길이도 0 이 되어 빈 배열이 "정합"으로 통과한다. 하한이 필요하다.
  if (num_segments < 1U || bernstein_degree < 1U)
  {
    return false;
  }

  if (!std::isfinite(segment_duration_s) || segment_duration_s <= 0.0)
  {
    return false;
  }

  const std::size_t expected_size = std::size_t{2U} * static_cast<std::size_t>(num_segments) *
                                    (static_cast<std::size_t>(bernstein_degree) + std::size_t{1U});
  if (control_points.size() != expected_size)
  {
    return false;
  }

  for (const double value : control_points)
  {
    if (!std::isfinite(value))
    {
      return false;
    }
  }

  // 개구간이다. 0 이나 1 은 시프트 자체가 의미를 잃는다(u = Δt_h/Δt, N4 조건 Δt_h < Δt).
  return std::isfinite(shift_ratio_u) && shift_ratio_u > 0.0 && shift_ratio_u < 1.0;
}

} // namespace

ConvertResult make_local_plan_share(
  mrs::RobotId robot_id, std::uint32_t tick_seq, const std::vector<double> & control_points,
  std::uint8_t num_segments, std::uint8_t bernstein_degree, double segment_duration_s,
  double shift_ratio_u, double stamp_s, mrs_interfaces::msg::LocalPlanShare & out)
{
  // ① 필드 범위 — 형상 불변식 5건.
  if (!is_plan_shape_valid(
        control_points, num_segments, bernstein_degree, segment_duration_s, shift_ratio_u))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ② 시각 가드.
  builtin_interfaces::msg::Time stamp;
  const ConvertResult stamp_result = seconds_to_time(stamp_s, stamp);
  if (!stamp_result.ok)
  {
    return stamp_result;
  }

  // 지역 버퍼에 쌓고 성공한 뒤에만 대입한다 — 실패 시 out 을 부분 채움 상태로 남기지 않는다.
  mrs_interfaces::msg::LocalPlanShare filled;
  filled.header.stamp = stamp;
  filled.header.frame_id = "map"; // 계약 §0 좌표계 규약 + `LocalPlanShare.msg` 헤더 주석
  filled.robot_id = robot_id;
  filled.tick_seq = tick_seq;
  filled.num_segments = num_segments;
  filled.bernstein_degree = bernstein_degree;
  filled.control_points = control_points;
  filled.segment_duration_s = segment_duration_s;
  filled.shift_ratio_u = shift_ratio_u;

  out = std::move(filled);
  return convert_ok();
}

ConvertResult from_msg(
  const mrs_interfaces::msg::LocalPlanShare & msg, mrs::NeighborTrajectory & out)
{
  // 필드 범위 — 발행 측과 같은 형상 불변식 5건.
  // ⛔ `tick_seq` 로 폐기 판정을 하지 않는다: 감소는 유실이 아니라 재동기이며 fresh_j(h-1)
  //    판정은 도메인(L4) 몫이다.
  // ⛔ `shift_ratio_u` 를 정규화·클램프하지 않는다: 수신자가 자기 값과 대조해 (A1) 을
  //    판정하는데 조용한 치환은 그 대조를 무력화한다. 범위 밖은 폐기다.
  if (!is_plan_shape_valid(
        msg.control_points, msg.num_segments, msg.bernstein_degree, msg.segment_duration_s,
        msg.shift_ratio_u))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  mrs::NeighborTrajectory parsed;
  parsed.robot_id = msg.robot_id;
  parsed.tick_seq = msg.tick_seq;
  parsed.control_points = msg.control_points;
  parsed.shift_ratio_u = msg.shift_ratio_u;
  // age_s 는 현재 시각 인자가 없어 채우지 않는다(감사 L9 — 계측 전용이며 안전 근거 아님).
  // num_segments·bernstein_degree·segment_duration_s 는 도메인 타입에 자리가 없어 검사 후
  // 버려진다(감사 L8). 둘 다 헤더 @note 의 [2] 전 해소 대상이다.

  out = std::move(parsed);
  return convert_ok();
}

ConvertResult make_cmd_vel(const mrs::LocalPlanOutput & output, geometry_msgs::msg::Twist & out)
{
  // NaN/inf 지령이 구동기에 그대로 전달되면 거동이 정의되지 않는다. 발행 직전 여기서 막고
  // 호출자는 정지 지령으로 폴백한다.
  if (!std::isfinite(output.cmd_v_mps) || !std::isfinite(output.cmd_omega_rps))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  geometry_msgs::msg::Twist filled; // 기본 생성이 전 성분 0 이다 — 나머지 4성분은 그대로 둔다.
  filled.linear.x = output.cmd_v_mps;
  filled.angular.z = output.cmd_omega_rps;

  out = filled;
  return convert_ok();
}

} // namespace mrs::convert
