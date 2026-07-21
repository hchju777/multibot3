/**
 * @file sim_convert.cpp
 * @brief sim 경계 변환 — [0a] tracer bullet 경로 3함수 구현 + 나머지 스텁.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 미구현 함수는 기본 생성
 * @ref mrs::convert::ConvertResult (`ok = false`)를 반환하며, 미구현을 실제 실패 사유처럼
 * 보이는 값으로 채우지 않는다.
 *
 * ## 구현된 함수 (본문 있음)
 * `to_msg(RobotObservation)` · `from_msg(RobotState)` · `to_msg(SimMetricSample)`.
 *
 * @note `backend_name` 화이트리스트는 이 파일에 두지 않는다(R-15 (c3)) — 허용 문자열은
 *       `mrs_sim_abstraction` 이 상수로 노출하고 여기서는 참조만 한다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cmath>
#include <utility>
#include <vector>

namespace mrs::convert
{
namespace
{

/**
 * @brief 로봇 상태의 연속량 범위를 검사한다 (L-11, 발행·수신 공용).
 *
 * @param[in] v_mps 몸체 선속도 [m/s]. 자료형 `double`. 유한해야 한다.
 * @param[in] omega_rps 몸체 각속도 [rad/s]. 자료형 `double`. 유한해야 한다.
 * @param[in] edge_progress 현재 엣지 진행률. 자료형 `double`. `[0, 1]` 이어야 한다.
 * @return `bool` — 세 값이 계약 범위 안이면 true. NaN 은 비교가 전부 거짓이므로 함께 걸린다.
 */
[[nodiscard]] bool is_robot_state_in_range(
  double v_mps, double omega_rps, double edge_progress) noexcept
{
  if (!std::isfinite(v_mps) || !std::isfinite(omega_rps))
  {
    return false;
  }

  return edge_progress >= 0.0 && edge_progress <= 1.0;
}

} // namespace

ConvertResult to_msg(
  const mrs::RobotObservation & observation, double stamp_s, mrs_interfaces::msg::RobotState & out)
{
  // ① 뷰 종류(V1) — 노드 부착은 균일 뷰 기준이다(계약 §0.1 정본표, RobotState 행).
  if (observation.view_scope.view_kind != ViewKind::UNIFORM)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }

  // ② 스코프 자기 유효성. 백엔드가 뷰를 몰라 센티넬 + edge_progress 로 보고하는 경우에도
  //    `view_scope` 자체는 유효해야 한다(계약이 정한 정상 경로).
  if (!is_usable_scope(observation.view_scope))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ③ 필드 범위 — 연속량.
  if (!is_robot_state_in_range(observation.v_mps, observation.omega_rps, observation.edge_progress))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // 자세의 각도 구간·유한성. 단일 자세용 **발행 측** 검사 헬퍼가 `common_convert` 에 없어
  // 배열 헬퍼를 1원소로 쓴다 — 구간 판정을 여기서 다시 쓰면 두 벌이 되어 즉시 드리프트한다.
  std::vector<mrs_interfaces::msg::Pose2D> pose_buffer;
  const ConvertResult pose_result =
    poses_to_msg(std::vector<mrs::Pose2D>{observation.body_pose}, pose_buffer);
  if (!pose_result.ok)
  {
    return pose_result;
  }

  // ④ 시각 2건. 관측 시각은 header.stamp 와 다를 수 있으므로 따로 변환한다.
  builtin_interfaces::msg::Time stamp;
  const ConvertResult stamp_result = seconds_to_time(stamp_s, stamp);
  if (!stamp_result.ok)
  {
    return stamp_result;
  }

  builtin_interfaces::msg::Time observation_stamp;
  const ConvertResult observation_stamp_result =
    seconds_to_time(observation.uncertainty.observation_time_s, observation_stamp);
  if (!observation_stamp_result.ok)
  {
    return observation_stamp_result;
  }

  // 지역 버퍼에 쌓고 성공한 뒤에만 대입한다 — 실패 시 out 을 부분 채움 상태로 남기지 않는다.
  mrs_interfaces::msg::RobotState filled;
  filled.header.stamp = stamp;
  filled.header.frame_id = "map"; // 계약 §0 좌표계 규약 + `RobotState.msg` 헤더 주석
  filled.robot_id = observation.robot_id;
  filled.pose = pose_buffer.front();
  filled.v_mps = observation.v_mps;
  filled.omega_rps = observation.omega_rps;
  filled.view_scope = to_msg(observation.view_scope);
  // 센티넬(4294967295)은 "엣지 위"/"미상"을 뜻하는 **유효값**이다 — 발행 측에서 거르지 않는다.
  filled.occupied_node = node_id_to_msg(observation.occupied_node);
  filled.next_node = node_id_to_msg(observation.next_node);
  filled.edge_progress = observation.edge_progress;
  filled.observation.pos_std_m = observation.uncertainty.pos_std_m;
  filled.observation.theta_std_rad = observation.uncertainty.theta_std_rad;
  filled.observation.partial_observation = observation.uncertainty.partial_observation;
  filled.observation.observation_stamp = observation_stamp;

  out = std::move(filled);
  return convert_ok();
}

ConvertResult from_msg(
  const mrs_interfaces::msg::RobotState & msg, const mrs::ViewScope & expected,
  mrs::RobotObservation & out)
{
  // ① 뷰 종류(V1) + 인스턴스 스코프(V2). 이 메시지는 `view_scope` 를 싣는다.
  ViewScope received;
  const ConvertResult scope_parse_result = from_msg(msg.view_scope, received);
  if (!scope_parse_result.ok)
  {
    return scope_parse_result;
  }

  const ConvertResult scope_match_result = match_scope(received, expected);
  if (!scope_match_result.ok)
  {
    return scope_match_result;
  }

  // ② 필드 범위 — 연속량과 자세.
  if (!is_robot_state_in_range(msg.v_mps, msg.omega_rps, msg.edge_progress))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  Pose2D body_pose;
  const ConvertResult pose_result = from_msg(msg.pose, body_pose);
  if (!pose_result.ok)
  {
    return pose_result;
  }

  // 노드 id 랩. 센티넬은 "엣지 위"/"미상"으로 **유효**하므로 ALLOW 정책이다.
  UniformNodeId occupied_node{};
  const ConvertResult occupied_result =
    node_id_from_msg(msg.occupied_node, NoneNodePolicy::ALLOW, occupied_node);
  if (!occupied_result.ok)
  {
    return occupied_result;
  }

  UniformNodeId next_node{};
  const ConvertResult next_result =
    node_id_from_msg(msg.next_node, NoneNodePolicy::ALLOW, next_node);
  if (!next_result.ok)
  {
    return next_result;
  }

  // ③ 시각 가드. `header.stamp` 는 도메인 `RobotObservation` 에 대응 필드가 없어 여기서
  //    소실된다 — 수신 시각 계측은 호출 노드가 메시지에서 직접 읽는다(노드 id 가 아니므로
  //    V3 위반이 아니다).
  double observation_time_s = 0.0;
  const ConvertResult observation_time_result =
    time_to_seconds(msg.observation.observation_stamp, observation_time_s);
  if (!observation_time_result.ok)
  {
    return observation_time_result;
  }

  mrs::RobotObservation parsed;
  parsed.robot_id = msg.robot_id;
  parsed.body_pose = body_pose;
  parsed.v_mps = msg.v_mps;
  parsed.omega_rps = msg.omega_rps;
  parsed.view_scope = received;
  parsed.occupied_node = occupied_node;
  parsed.next_node = next_node;
  parsed.edge_progress = msg.edge_progress;
  parsed.uncertainty.pos_std_m = msg.observation.pos_std_m;
  parsed.uncertainty.theta_std_rad = msg.observation.theta_std_rad;
  parsed.uncertainty.partial_observation = msg.observation.partial_observation;
  parsed.uncertainty.observation_time_s = observation_time_s;

  out = std::move(parsed);
  return convert_ok();
}

ConvertResult to_msg(const mrs::SimCapabilities & caps, mrs_interfaces::msg::SimCapabilities & out)
{
  (void)caps;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const mrs::SimMetricSample & sample, double stamp_s, mrs_interfaces::msg::SimMetricSample & out)
{
  // ① 필드 범위 — 키가 비면 표본을 어느 지표에 적립할지 정할 수 없다.
  if (sample.key.empty())
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // NaN 만 거른다. ±inf 는 계약이 금지하지 않았으므로 없는 기준으로 걸러 [0a] 실측을 막지 않는다.
  if (std::isnan(sample.value))
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

  mrs_interfaces::msg::SimMetricSample filled;
  filled.header.stamp = stamp;
  filled.key = sample.key;
  filled.value = sample.value;
  // robot_id = 65535 는 **전역 지표로 유효**하다 — 오류로 처리하지 않는다.
  filled.robot_id = sample.robot_id;
  // 태그가 틀리면 충실도 부족 지표가 연구 검증으로 승격되어 kill-gate 정직성이 깨진다.
  filled.fidelity_tag = sample.pending_isaac
                          ? mrs_interfaces::msg::SimMetricSample::TAG_OPEN_PENDING_ISAAC
                          : mrs_interfaces::msg::SimMetricSample::TAG_VALID;

  out = std::move(filled);
  return convert_ok();
}

ConvertResult from_request(
  const mrs_interfaces::srv::SimInject::Request & req, const mrs::ViewScope & expected,
  mrs::FaultInjection & out)
{
  (void)req;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

} // namespace mrs::convert
