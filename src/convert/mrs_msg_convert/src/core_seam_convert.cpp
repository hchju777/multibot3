/**
 * @file core_seam_convert.cpp
 * @brief L-01/L-04 계열 변환 — [0a] tracer bullet 경로 4함수 구현 + 나머지 스텁.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 미구현 함수는 기본 생성
 * @ref mrs::convert::ConvertResult (`ok = false`, `reason = OK`)를 반환하며, 미구현을 실제
 * 실패 사유처럼 보이는 값으로 채우지 않는다 — 폐기 카운터가 존재하지 않는 결함을 가리키게 된다.
 *
 * ## 구현된 함수 (본문 있음)
 * `to_msg(ExecutionWindow)` · `from_msg(ExecutionWindow)` · `make_escalation_report` ·
 * `reason_from_msg`. 나머지는 스텁이다.
 *
 * ## 검사 순서
 * 계약 레지스트리 §0.2.1 의 표준 순서를 따른다:
 * **스키마 버전 → 뷰 종류 → 스코프 → 필드 범위 → 열거 범위 → 시각 가드**.
 * 첫 실패에서 즉시 반환한다 — 사유가 뒤 검사에 덮이면 폐기 카운터가 원인을 잃는다.
 * ⚠ `from_msg(ExecutionWindow)` 만은 **열거 범위를 필드 범위보다 먼저** 본다. 개정 불변식
 * 자체가 `revision_kind` 값에 의존하므로, 순서를 뒤집으면 미지의 열거값을 어느 불변식으로
 * 판정할지 정할 수 없기 때문이다(해당 위치에 주석으로 표시했다).
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace mrs::convert
{
namespace
{

/**
 * @brief 도메인 에스컬레이션 사유가 계약 확정 집합 안의 값인지 검사한다 (L-04).
 *
 * 발행 방향은 `static_cast` 로 값을 내보내므로, 호출자가 캐스트로 만든 집합 밖 값이 그대로
 * 와이어에 실릴 수 있다. 그 경로를 막는 유일한 지점이다.
 *
 * @param[in] reason 검사할 사유. 자료형 `mrs::EscalationReason`.
 * @return `bool` — 확정 집합 `{BLOCKED, WINDOW_EXPIRING, QP_INFEASIBLE, LOCALIZATION_LOST,
 *         SYNC_LOST}` 안이면 true.
 */
[[nodiscard]] bool is_known_escalation_reason(EscalationReason reason) noexcept
{
  switch (reason)
  {
    case EscalationReason::BLOCKED:
    case EscalationReason::WINDOW_EXPIRING:
    case EscalationReason::QP_INFEASIBLE:
    case EscalationReason::LOCALIZATION_LOST:
    case EscalationReason::SYNC_LOST:
      return true;
  }
  return false;
}

/**
 * @brief 실행 창의 개정 불변식을 검사한다 (L-01, 발행·수신 공용).
 *
 * 발행 측과 수신 측이 각자 판정하면 즉시 드리프트하므로 이 파일 안에 하나만 둔다.
 * `TRUNCATE` + 음수 인덱스는 `CommitStatus.committed_through_segment_index` 가 `uint32`(항상 ≥0)
 * 이므로 **로봇 상태와 무관하게 불변식 W7 을 무조건 위반**한다 — 상태 없는 자유함수에서도
 * 계약 유도로 거부할 수 있는 유일한 W7 파생 검사다.
 *
 * @param[in] revision_kind 개정 종류. 자료형 `mrs::RevisionKind`.
 * @param[in] segment_count 창이 싣는 세그먼트 수. 자료형 `std::size_t`.
 * @param[in] valid_through_segment_index 유효 구간 끝 인덱스. 자료형 `std::int32_t`.
 * @return `bool` — `NEW` 이면 인덱스가 -1 일 때, `TRUNCATE` 이면 세그먼트가 비어 있고 인덱스가
 *         0 이상일 때 true.
 */
[[nodiscard]] bool is_revision_consistent(
  RevisionKind revision_kind, std::size_t segment_count,
  std::int32_t valid_through_segment_index) noexcept
{
  if (revision_kind == RevisionKind::NEW)
  {
    return valid_through_segment_index == -1;
  }

  return segment_count == 0U && valid_through_segment_index >= 0;
}

} // namespace

ConvertResult to_msg(
  const mrs::ExecutionWindow & window, double stamp_s,
  mrs_interfaces::msg::ExecutionWindow & out)
{
  // ① 스코프 자기 유효성 — 0 이 나가면 V3 를 지키는 수신자가 100% 폐기한다.
  //    종류는 계약이 UNIFORM 으로 고정하므로(정본표 L-01 행) 여기서 공급한다.
  ViewScope scope;
  scope.view_kind = ViewKind::UNIFORM;
  scope.roadmap_version = window.roadmap_version;
  scope.view_id = window.view_id;
  if (!is_usable_scope(scope))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ② 개정 불변식 (+ W7 파생 거부).
  if (!is_revision_consistent(
        window.revision_kind, window.segments.size(), window.valid_through_segment_index))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ③ 노드 id 센티넬 — 미기입 노드가 창에 실리면 수신자가 해석할 수 없다.
  for (const mrs::WindowSegment & segment : window.segments)
  {
    if (segment.node_from.is_none() || segment.node_to.is_none())
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
  }
  for (const mrs::PredecessorConstraint & constraint : window.predecessor_constraints)
  {
    if (constraint.node_id.is_none())
    {
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
  }

  // ④ 시각 2건. 가드는 공통 헬퍼가 단독 소유한다(9곳에 흩어지지 않게).
  builtin_interfaces::msg::Time stamp;
  const ConvertResult stamp_result = seconds_to_time(stamp_s, stamp);
  if (!stamp_result.ok)
  {
    return stamp_result;
  }

  builtin_interfaces::msg::Time valid_until;
  const ConvertResult valid_until_result = seconds_to_time(window.window_valid_until_s, valid_until);
  if (!valid_until_result.ok)
  {
    return valid_until_result;
  }

  // 지역 버퍼에 쌓고 성공한 뒤에만 대입한다 — 실패 시 out 을 부분 채움 상태로 남기지 않는다.
  mrs_interfaces::msg::ExecutionWindow filled;
  filled.header.stamp = stamp;
  filled.header.frame_id = "map"; // 계약 §0 좌표계 규약 + `ExecutionWindow.msg` 헤더 주석
  filled.schema_version = mrs::EXECUTION_WINDOW_SCHEMA_VERSION;
  filled.robot_id = window.robot_id;
  filled.window_seq = window.window_seq;
  filled.plan_epoch = window.plan_epoch;
  filled.roadmap_version = window.roadmap_version;
  filled.view_id = window.view_id;

  filled.segments.reserve(window.segments.size());
  for (const mrs::WindowSegment & segment : window.segments)
  {
    mrs_interfaces::msg::WindowSegment segment_msg;
    segment_msg.node_from = node_id_to_msg(segment.node_from);
    segment_msg.node_to = node_id_to_msg(segment.node_to);
    filled.segments.push_back(segment_msg);
  }

  filled.predecessor_constraints.reserve(window.predecessor_constraints.size());
  for (const mrs::PredecessorConstraint & constraint : window.predecessor_constraints)
  {
    mrs_interfaces::msg::PredecessorConstraint constraint_msg;
    constraint_msg.predecessor_robot_id = constraint.predecessor_robot_id;
    constraint_msg.node_id = node_id_to_msg(constraint.node_id);
    filled.predecessor_constraints.push_back(constraint_msg);
  }

  filled.window_valid_until = valid_until;
  filled.revision_kind = revision_kind_to_msg(window.revision_kind);
  filled.valid_through_segment_index = window.valid_through_segment_index;

  out = std::move(filled);
  return convert_ok();
}

ConvertResult from_msg(
  const mrs_interfaces::msg::ExecutionWindow & msg, const mrs::ViewScope & expected,
  mrs::ExecutionWindow & out)
{
  // ① 스키마 버전 — 불일치는 폐기다(다른 필드를 해석할 근거가 없다).
  if (msg.schema_version != mrs::EXECUTION_WINDOW_SCHEMA_VERSION)
  {
    return convert_fail(ConvertStatus::SCHEMA_VERSION_MISMATCH);
  }

  // ② 뷰 종류(V1) + 인스턴스 스코프(V2). 이 메시지는 `ViewScope` 를 싣지 않고 평면 쌍만
  //    실으므로 종류는 함수가 UNIFORM 을 공급한다.
  const ConvertResult scope_result =
    match_scope_flat(msg.roadmap_version, msg.view_id, ViewKind::UNIFORM, expected);
  if (!scope_result.ok)
  {
    return scope_result;
  }

  // ③ 열거 범위. ⚠ 표준 순서에서는 필드 범위가 앞이나, 개정 불변식이 이 값에 의존하므로
  //    여기가 먼저다 — 미지의 값을 NEW 로 폴백시켜 불변식을 판정하면 부분 폐기를 신규 창으로
  //    오해석해 커밋 접두가 통째로 교체되고 T1 의 I-QP 전제가 깨진다.
  RevisionKind revision_kind{};
  const ConvertResult revision_result = revision_kind_from_msg(msg.revision_kind, revision_kind);
  if (!revision_result.ok)
  {
    return revision_result;
  }

  // ④ 필드 범위 — 개정 불변식(+ W7 파생 거부)과 노드 id 센티넬.
  if (!is_revision_consistent(revision_kind, msg.segments.size(), msg.valid_through_segment_index))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  std::vector<mrs::WindowSegment> segments;
  segments.reserve(msg.segments.size());
  for (const mrs_interfaces::msg::WindowSegment & segment_msg : msg.segments)
  {
    mrs::WindowSegment segment;
    const ConvertResult from_result =
      node_id_from_msg(segment_msg.node_from, NoneNodePolicy::REJECT, segment.node_from);
    if (!from_result.ok)
    {
      return from_result;
    }
    const ConvertResult to_result =
      node_id_from_msg(segment_msg.node_to, NoneNodePolicy::REJECT, segment.node_to);
    if (!to_result.ok)
    {
      return to_result;
    }
    segments.push_back(segment);
  }

  std::vector<mrs::PredecessorConstraint> constraints;
  constraints.reserve(msg.predecessor_constraints.size());
  for (const mrs_interfaces::msg::PredecessorConstraint & constraint_msg :
       msg.predecessor_constraints)
  {
    mrs::PredecessorConstraint constraint;
    constraint.predecessor_robot_id = constraint_msg.predecessor_robot_id;
    const ConvertResult node_result =
      node_id_from_msg(constraint_msg.node_id, NoneNodePolicy::REJECT, constraint.node_id);
    if (!node_result.ok)
    {
      return node_result;
    }
    constraints.push_back(constraint);
  }

  // ⑤ 시각 가드.
  double window_valid_until_s = 0.0;
  const ConvertResult time_result = time_to_seconds(msg.window_valid_until, window_valid_until_s);
  if (!time_result.ok)
  {
    return time_result;
  }

  mrs::ExecutionWindow parsed;
  parsed.robot_id = msg.robot_id;
  parsed.window_seq = msg.window_seq;
  parsed.plan_epoch = msg.plan_epoch;
  parsed.roadmap_version = msg.roadmap_version;
  parsed.view_id = msg.view_id;
  parsed.segments = std::move(segments);
  parsed.predecessor_constraints = std::move(constraints);
  parsed.window_valid_until_s = window_valid_until_s;
  parsed.revision_kind = revision_kind;
  parsed.valid_through_segment_index = msg.valid_through_segment_index;

  out = std::move(parsed);
  return convert_ok();
}

ConvertResult make_escalation_report(
  mrs::RobotId robot_id, mrs::EventId event_id, mrs::EscalationReason reason,
  std::uint32_t window_seq, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::EscalationReport & out)
{
  // ① 뷰 종류(V1) — 계약이 이 메시지의 `blocking_node_id` 를 UNIFORM 으로 고정한다.
  if (scope.view_kind != ViewKind::UNIFORM)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }

  // ② 스코프 자기 유효성.
  if (!is_usable_scope(scope))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ③ 필드 범위 — 불변식 E1. 0 이면 계측이 사건 상관관계를 잃는다.
  if (!mrs::is_valid_event_id(event_id))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ④ 열거 범위 — 캐스트로 만든 집합 밖 값이 와이어에 나가는 것을 막는다.
  if (!is_known_escalation_reason(reason))
  {
    return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }

  // ⑤ 시각 가드.
  builtin_interfaces::msg::Time stamp;
  const ConvertResult stamp_result = seconds_to_time(stamp_s, stamp);
  if (!stamp_result.ok)
  {
    return stamp_result;
  }

  mrs_interfaces::msg::EscalationReport filled;
  filled.header.stamp = stamp;
  filled.event_id = event_id;
  filled.robot_id = robot_id;
  filled.reason = escalation_reason_to_msg(reason);
  // severity 는 L-04 매핑표의 코드 정본인 `mrs::severity_of` 결과를 그대로 옮긴다.
  // 여기서 if 문으로 재매핑하면 정본이 둘이 되어 즉시 드리프트한다.
  filled.severity = escalation_severity_to_msg(mrs::severity_of(reason));
  filled.window_seq = window_seq;
  filled.view_scope = to_msg(scope);

  out = std::move(filled);
  return convert_ok();
}

ConvertResult reason_from_msg(
  const mrs_interfaces::msg::EscalationReport & msg, const mrs::ViewScope & expected,
  mrs::EscalationReason & out)
{
  // ① 뷰 스코프 — 종류가 UNIFORM 이 아니면 같은 메시지의 `blocking_node_id` 가 다른 id 공간의
  //    값이라는 뜻이므로 메시지 전체를 폐기한다.
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

  // ② 열거 범위 — SYNC_LOST 가 뭉개지면 D-08 라우팅이 계획 문제로 오인해 재계획을 오발동하고
  //    F2 발동률·F6 스래싱 계측이 오염된다. 폐기가 정답이다.
  EscalationReason reason{};
  const ConvertResult reason_result = escalation_reason_from_msg(msg.reason, reason);
  if (!reason_result.ok)
  {
    return reason_result;
  }

  out = reason;
  return convert_ok();
}

ConvertResult to_msg(
  const std::vector<mrs::RobotPath> & paths, bool is_partial, mrs::EventId event_id,
  std::uint32_t plan_epoch, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::PlannedPaths & out)
{
  (void)paths;
  (void)is_partial;
  (void)event_id;
  (void)plan_epoch;
  (void)scope;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::PlannedPaths & msg, const mrs::ViewScope & expected,
  std::vector<mrs::RobotPath> & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_request(
  const mrs_interfaces::srv::PlanPaths::Request & req, const mrs::ViewScope & expected,
  std::vector<mrs::PlanRequestEntry> & out)
{
  (void)req;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_task_assignment(
  std::uint64_t task_id, mrs::RobotId robot_id, mrs::PhysicalNodeId pickup_node,
  mrs::PhysicalNodeId delivery_node, std::uint32_t assignment_epoch, double waiting_time_s,
  std::uint32_t reassign_count, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::TaskAssignment & out)
{
  (void)task_id;
  (void)robot_id;
  (void)pickup_node;
  (void)delivery_node;
  (void)assignment_epoch;
  (void)waiting_time_s;
  (void)reassign_count;
  (void)scope;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(const mrs::CommitState & state, mrs_interfaces::msg::CommitState & out)
{
  (void)state;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult from_msg(
  const mrs_interfaces::msg::CommitState & msg, const mrs::ViewScope & expected,
  mrs::CommitState & out)
{
  (void)msg;
  (void)expected;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult to_msg(
  const std::vector<mrs::FrozenOrder> & orders,
  std::vector<mrs_interfaces::msg::FrozenOrder> & out)
{
  (void)orders;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  return ConvertResult{};
}

ConvertResult make_commit_status(
  mrs::RobotId robot_id, std::uint32_t window_seq, std::uint32_t committed_through_segment_index,
  const std::vector<mrs::Pose2D> & commit_hull_vertices, mrs::AdoptionStatus qp_status,
  std::uint64_t roadmap_version, std::uint32_t view_id, double stamp_s,
  mrs_interfaces::msg::CommitStatus & out)
{
  (void)robot_id;
  (void)window_seq;
  (void)committed_through_segment_index;
  (void)commit_hull_vertices;
  (void)qp_status;
  (void)roadmap_version;
  (void)view_id;
  (void)stamp_s;
  (void)out;
  // TODO(0a): 미구현 — 안전한 기본 실패를 반환한다.
  // ⚠ 시그니처만 L-14 v2.0.0(계약 §0.2.1)에 맞췄다. `CommitStatus` 는 [0a] 발행 대상이 아니다.
  return ConvertResult{};
}

} // namespace mrs::convert
