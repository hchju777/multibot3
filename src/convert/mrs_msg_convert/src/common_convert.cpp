/**
 * @file common_convert.cpp
 * @brief 변환 공통 기반 구현 — 시각 가드·스코프 대조·기본 값 타입.
 */

#include "mrs_msg_convert/common_convert.hpp"

#include <cmath>
#include <limits>
#include <utility>
#include <vector>

// 열거 왕복의 **와이어 상수 정본**. 도메인 enum 값을 그대로 캐스팅하지 않고 이 상수들과
// 대조해야 미러 드리프트가 잡힌다(계약 §0.2).
#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/msg/sim_capabilities.hpp"
#include "mrs_interfaces/srv/reassign_request.hpp"
#include "mrs_interfaces/srv/reorder_passing.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"

namespace mrs::convert
{
namespace
{

/** @brief 나노초/초 환산 상수. */
constexpr double NANOSECONDS_PER_SECOND = 1.0e9;
/** @brief `builtin_interfaces/Time.sec` 는 int32 다 — 그 상한을 초 단위로 둔다. */
constexpr double MAX_REPRESENTABLE_SECONDS =
  static_cast<double>(std::numeric_limits<std::int32_t>::max());
/** @brief 나노초 필드의 상한(배타). 이 값 이상이면 초 필드로 올라갔어야 한다. */
constexpr std::uint32_t NANOSECOND_FIELD_LIMIT = 1000000000U;

/** @brief 원주율 — 계약 §0 이 자세 각도에 건 구간 `[-pi, pi]` 의 경계값. */
constexpr double PI_VALUE = 3.14159265358979323846;

/**
 * @brief 자세 3성분이 계약 §0 의 유한성·각도 구간 불변식을 만족하는지 검사한다.
 *
 * 단일 변환과 배열 변환이 각자 구간을 판정하면 즉시 드리프트하므로 이 파일 안에 하나만 둔다.
 *
 * @param[in] x_m 위치 x [m], map 프레임. 자료형 `double`.
 * @param[in] y_m 위치 y [m], map 프레임. 자료형 `double`.
 * @param[in] theta_rad 방위각 [rad], map 프레임 +x 기준 CCW. 자료형 `double`.
 * @return `bool` — 세 성분이 전부 유한하고 `theta_rad ∈ [-pi, pi]` 이면 true.
 */
[[nodiscard]] bool is_pose_in_contract_range(double x_m, double y_m, double theta_rad) noexcept
{
  if (!std::isfinite(x_m) || !std::isfinite(y_m) || !std::isfinite(theta_rad))
  {
    return false;
  }

  return theta_rad >= -PI_VALUE && theta_rad <= PI_VALUE;
}

// ── 와이어 상수 별칭 ───────────────────────────────────────────────────────────
using WireCommitStatus = mrs_interfaces::msg::CommitStatus;
using WireEscalationReport = mrs_interfaces::msg::EscalationReport;
using WireExecutionWindow = mrs_interfaces::msg::ExecutionWindow;
using WireRungEvent = mrs_interfaces::msg::RungEvent;
using WireSimCapabilities = mrs_interfaces::msg::SimCapabilities;
using WireReassignRequest = mrs_interfaces::srv::ReassignRequest::Request;
using WireReassignResponse = mrs_interfaces::srv::ReassignRequest::Response;
using WireReorderResponse = mrs_interfaces::srv::ReorderPassing::Response;
using WireSimInjectRequest = mrs_interfaces::srv::SimInject::Request;

// ── 미러 정합 벽 (컴파일 시각) ─────────────────────────────────────────────────
//
// 도메인 enum 은 `.msg`/`.srv` 의 **미러이지 정본이 아니다**. 발행 방향이 `static_cast` 로
// 값을 그대로 내보낼 수 있는 근거가 바로 이 벽이며, 벽이 없으면 계약이 값을 바꿨을 때
// 아무 진단 없이 틀린 값이 와이어로 나간다. 수신 방향의 `switch` 는 와이어 상수를 직접
// 대조하므로 이 벽과 **이중 방어**가 된다.

static_assert(
  static_cast<std::uint8_t>(EscalationReason::BLOCKED) == WireEscalationReport::REASON_BLOCKED &&
    static_cast<std::uint8_t>(EscalationReason::WINDOW_EXPIRING) ==
      WireEscalationReport::REASON_WINDOW_EXPIRING &&
    static_cast<std::uint8_t>(EscalationReason::QP_INFEASIBLE) ==
      WireEscalationReport::REASON_QP_INFEASIBLE &&
    static_cast<std::uint8_t>(EscalationReason::LOCALIZATION_LOST) ==
      WireEscalationReport::REASON_LOCALIZATION_LOST &&
    static_cast<std::uint8_t>(EscalationReason::SYNC_LOST) ==
      WireEscalationReport::REASON_SYNC_LOST,
  "EscalationReason 미러가 EscalationReport.msg 의 REASON_* 와 어긋났다 (계약 L-04)");

static_assert(
  static_cast<std::uint8_t>(EscalationSeverity::HARD) == WireEscalationReport::SEVERITY_HARD &&
    static_cast<std::uint8_t>(EscalationSeverity::SOFT) == WireEscalationReport::SEVERITY_SOFT,
  "EscalationSeverity 미러가 EscalationReport.msg 의 SEVERITY_* 와 어긋났다 (계약 L-04)");

static_assert(
  static_cast<std::uint8_t>(Rung::R0_LOCAL_ABSORB) == WireRungEvent::RUNG_R0 &&
    static_cast<std::uint8_t>(Rung::R1_BTPG) == WireRungEvent::RUNG_R1 &&
    static_cast<std::uint8_t>(Rung::R2_MILP) == WireRungEvent::RUNG_R2 &&
    static_cast<std::uint8_t>(Rung::R3_REPLAN) == WireRungEvent::RUNG_R3 &&
    static_cast<std::uint8_t>(Rung::R4_REASSIGN) == WireRungEvent::RUNG_R4 &&
    static_cast<std::uint8_t>(Rung::R5_OPERATOR) == WireRungEvent::RUNG_R5,
  "Rung 미러가 RungEvent.msg 의 RUNG_* 와 어긋났다 (계약 L-13 — 스키마 드리프트 = 실험 무효)");

static_assert(
  static_cast<std::uint8_t>(RungTransition::ENTER) == WireRungEvent::TRANSITION_ENTER &&
    static_cast<std::uint8_t>(RungTransition::ABSORBED) == WireRungEvent::TRANSITION_ABSORBED &&
    static_cast<std::uint8_t>(RungTransition::ESCALATE) == WireRungEvent::TRANSITION_ESCALATE &&
    static_cast<std::uint8_t>(RungTransition::GUARD_REJECT) ==
      WireRungEvent::TRANSITION_GUARD_REJECT &&
    static_cast<std::uint8_t>(RungTransition::ESCAPE_HATCH) ==
      WireRungEvent::TRANSITION_ESCAPE_HATCH,
  "RungTransition 미러가 RungEvent.msg 의 TRANSITION_* 와 어긋났다 (계약 L-13)");

static_assert(
  static_cast<std::uint8_t>(RungTriggerKind::HARD) == WireRungEvent::TRIGGER_HARD &&
    static_cast<std::uint8_t>(RungTriggerKind::SOFT) == WireRungEvent::TRIGGER_SOFT,
  "RungTriggerKind 미러가 RungEvent.msg 의 TRIGGER_* 와 어긋났다 (계약 L-13)");

static_assert(
  static_cast<std::uint8_t>(RevisionKind::NEW) == WireExecutionWindow::REVISION_NEW &&
    static_cast<std::uint8_t>(RevisionKind::TRUNCATE) == WireExecutionWindow::REVISION_TRUNCATE,
  "RevisionKind 미러가 ExecutionWindow.msg 의 REVISION_* 와 어긋났다 (계약 L-01 seam)");

// AdoptionStatus 에는 값 2(QP_TICK_JITTER) 가 **일부러 없다**. 그 값이 도메인에 없다는 사실
// 자체가 "발행 금지"의 구현이며, 아래 벽은 나머지 5값의 정합만 잠근다.
static_assert(
  static_cast<std::uint8_t>(AdoptionStatus::QP_OK) == WireCommitStatus::QP_OK &&
    static_cast<std::uint8_t>(AdoptionStatus::FALLBACK_GENERIC) == WireCommitStatus::QP_FALLBACK &&
    static_cast<std::uint8_t>(AdoptionStatus::FALLBACK_NO_INDEX) ==
      WireCommitStatus::FALLBACK_NO_INDEX &&
    static_cast<std::uint8_t>(AdoptionStatus::FALLBACK_NEIGHBOR_STALE) ==
      WireCommitStatus::FALLBACK_NEIGHBOR_STALE &&
    static_cast<std::uint8_t>(AdoptionStatus::FALLBACK_SOLVER) == WireCommitStatus::FALLBACK_SOLVER,
  "AdoptionStatus 미러가 CommitStatus.msg 의 qp_status 값과 어긋났다 (계약 L-14 v1.1.0)");

static_assert(
  WireCommitStatus::QP_TICK_JITTER == 2U,
  "QP_TICK_JITTER 의 값이 바뀌었다 — adoption_status_from_msg 의 거부 근거가 무너진다");

static_assert(
  static_cast<std::uint8_t>(FaultKind::COMM_DELAY) == WireSimInjectRequest::FAULT_COMM_DELAY &&
    static_cast<std::uint8_t>(FaultKind::COMM_DROP) == WireSimInjectRequest::FAULT_COMM_DROP &&
    static_cast<std::uint8_t>(FaultKind::CORRIDOR_CLOSE) ==
      WireSimInjectRequest::FAULT_CORRIDOR_CLOSE &&
    static_cast<std::uint8_t>(FaultKind::CORRIDOR_OPEN) ==
      WireSimInjectRequest::FAULT_CORRIDOR_OPEN &&
    static_cast<std::uint8_t>(FaultKind::ROBOT_STALL) == WireSimInjectRequest::FAULT_ROBOT_STALL &&
    static_cast<std::uint8_t>(FaultKind::ROBOT_FAULT) == WireSimInjectRequest::FAULT_ROBOT_FAULT,
  "FaultKind 미러가 SimInject.srv 의 FAULT_* 와 어긋났다 (계약 L-19)");

static_assert(
  static_cast<std::uint8_t>(PhysicsFidelity::NONE) == WireSimCapabilities::FIDELITY_NONE &&
    static_cast<std::uint8_t>(PhysicsFidelity::KINEMATIC) ==
      WireSimCapabilities::FIDELITY_KINEMATIC &&
    static_cast<std::uint8_t>(PhysicsFidelity::FULL_PHYSICS) ==
      WireSimCapabilities::FIDELITY_FULL_PHYSICS,
  "PhysicsFidelity 미러가 SimCapabilities.msg 의 FIDELITY_* 와 어긋났다");

static_assert(
  static_cast<std::uint8_t>(ReassignTrigger::HARD) == WireReassignRequest::TRIGGER_HARD &&
    static_cast<std::uint8_t>(ReassignTrigger::SOFT_REPEATED_FAILURE) ==
      WireReassignRequest::TRIGGER_SOFT_REPEATED_FAILURE,
  "ReassignTrigger 미러가 ReassignRequest.srv 의 TRIGGER_* 와 어긋났다 (D-02 v2)");

static_assert(
  static_cast<std::uint8_t>(ReassignResult::REASSIGNED) ==
      WireReassignResponse::RESULT_REASSIGNED &&
    static_cast<std::uint8_t>(ReassignResult::QUEUED_AGING) ==
      WireReassignResponse::RESULT_QUEUED_AGING &&
    static_cast<std::uint8_t>(ReassignResult::COOLDOWN_ACTIVE) ==
      WireReassignResponse::RESULT_COOLDOWN_ACTIVE &&
    static_cast<std::uint8_t>(ReassignResult::ESCALATED_R5) ==
      WireReassignResponse::RESULT_ESCALATED_R5 &&
    static_cast<std::uint8_t>(ReassignResult::REJECTED) == WireReassignResponse::RESULT_REJECTED,
  "ReassignResult 미러가 ReassignRequest.srv 응답의 RESULT_* 와 어긋났다 (D-02 v2)");

static_assert(
  static_cast<std::uint8_t>(ReorderAck::ACCEPTED) == WireReorderResponse::RESULT_ACCEPTED &&
    static_cast<std::uint8_t>(ReorderAck::REJECTED_BUSY) ==
      WireReorderResponse::RESULT_REJECTED_BUSY &&
    static_cast<std::uint8_t>(ReorderAck::REJECTED_SCOPE) ==
      WireReorderResponse::RESULT_REJECTED_SCOPE &&
    static_cast<std::uint8_t>(ReorderAck::REJECTED_STALE) ==
      WireReorderResponse::RESULT_REJECTED_STALE,
  "ReorderAck 미러가 ReorderPassing.srv 응답의 RESULT_* 와 어긋났다 (Q-2)");

} // namespace

ConvertResult seconds_to_time(double seconds, builtin_interfaces::msg::Time & out)
{
  if (!std::isfinite(seconds) || seconds < 0.0 || seconds > MAX_REPRESENTABLE_SECONDS)
  {
    return convert_fail(ConvertStatus::TIME_CONVERSION_GUARD);
  }

  // ⚠ **가장 가까운 나노초 격자점으로 반올림**한다. 버림이 아니다.
  //
  // 버림이었을 때의 실측 결함([0a] 후속): 시뮬 시각이 정수 나노초로 누산되어도 도메인이 그것을
  // `double` 초로 받는 순간 격자값이 아래로 어긋난다(예: 8 100 000 000 ns → `double` 8.1 →
  // `floor(0.1 * 1e9)` = 99 999 999). 그 결과 `/clock` 이 `nanosec: 8099999999` 로 나가고,
  // `floor((t - t0)/Δt_h)` 로 틱 번호를 만드는 수신자가 직전 틱을 재계산한다(계약 L-15 문언).
  // 반올림하면 정수 나노초에서 출발한 값이 **정확히 그 값으로 되돌아온다**.
  //
  // "반올림이면 왕복 시 값이 커질 수 있다"는 우려는 남지만 상계가 0.5 ns 이며, 한 번 격자에
  // 올라간 값은 다시 변환해도 움직이지 않는다(멱등). 격자 이탈이 만드는 틱 오독 쪽이 크다.
  const double total_nanoseconds = std::round(seconds * NANOSECONDS_PER_SECOND);
  if (!std::isfinite(total_nanoseconds) || total_nanoseconds < 0.0)
  {
    return convert_fail(ConvertStatus::TIME_CONVERSION_GUARD);
  }

  const auto total_ns = static_cast<std::int64_t>(total_nanoseconds);
  const std::int64_t whole_seconds = total_ns / static_cast<std::int64_t>(NANOSECOND_FIELD_LIMIT);

  // 반올림이 상한을 넘겨 올림한 경우를 여기서 잡는다(위 범위 검사만으로는 부족하다).
  if (whole_seconds > static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()))
  {
    return convert_fail(ConvertStatus::TIME_CONVERSION_GUARD);
  }

  out.sec = static_cast<std::int32_t>(whole_seconds);
  out.nanosec = static_cast<std::uint32_t>(
    total_ns - whole_seconds * static_cast<std::int64_t>(NANOSECOND_FIELD_LIMIT));
  return convert_ok();
}

ConvertResult time_to_seconds(const builtin_interfaces::msg::Time & time, double & out_seconds)
{
  // rosidl 은 이 불변식을 강제하지 않는다 — 여기서 막지 않으면 음수 시각이 도메인에 흘러든다.
  if (time.sec < 0 || time.nanosec >= NANOSECOND_FIELD_LIMIT)
  {
    return convert_fail(ConvertStatus::TIME_CONVERSION_GUARD);
  }

  out_seconds =
    static_cast<double>(time.sec) + static_cast<double>(time.nanosec) / NANOSECONDS_PER_SECOND;
  return convert_ok();
}

std::uint8_t view_kind_to_msg(ViewKind kind) noexcept
{
  return static_cast<std::uint8_t>(kind);
}

ConvertResult view_kind_from_msg(std::uint8_t raw, ViewKind & out)
{
  switch (raw)
  {
    case mrs_interfaces::msg::ViewScope::VIEW_KIND_PHYSICAL:
      out = ViewKind::PHYSICAL;
      return convert_ok();
    case mrs_interfaces::msg::ViewScope::VIEW_KIND_UNIFORM:
      out = ViewKind::UNIFORM;
      return convert_ok();
    case mrs_interfaces::msg::ViewScope::VIEW_KIND_SKELETON:
      out = ViewKind::SKELETON;
      return convert_ok();
    default:
      // 기본값으로 치환하지 않는다 — 상위 버전 발행자의 새 종류를 조용히 오해석하게 된다.
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

mrs_interfaces::msg::ViewScope to_msg(const ViewScope & scope)
{
  mrs_interfaces::msg::ViewScope msg;
  msg.view_kind = view_kind_to_msg(scope.view_kind);
  msg.roadmap_version = scope.roadmap_version;
  msg.view_id = scope.view_id;
  return msg;
}

ConvertResult from_msg(const mrs_interfaces::msg::ViewScope & msg, ViewScope & out)
{
  ViewKind kind{};
  const ConvertResult kind_result = view_kind_from_msg(msg.view_kind, kind);
  if (!kind_result.ok)
  {
    return kind_result;
  }

  ViewScope parsed;
  parsed.view_kind = kind;
  parsed.roadmap_version = msg.roadmap_version;
  parsed.view_id = msg.view_id;

  // 지도 버전 0(미지정)과 "물리 뷰인데 view_id != 0" 은 계약이 런타임에 금지한다.
  if (!is_usable_scope(parsed))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  out = parsed;
  return convert_ok();
}

ConvertResult match_scope(const ViewScope & received, const ViewScope & expected)
{
  // 종류와 인스턴스를 분리 판정한다 — 노드의 회복 방법이 다르기 때문이다.
  // 종류 불일치는 발행자 버그(폐기가 끝), 인스턴스 불일치는 지도 개정일 수 있다(재조회로 회복).
  if (received.view_kind != expected.view_kind)
  {
    return convert_fail(ConvertStatus::VIEW_KIND_MISMATCH);
  }

  if (received.roadmap_version != expected.roadmap_version || received.view_id != expected.view_id)
  {
    return convert_fail(ConvertStatus::VIEW_SCOPE_MISMATCH);
  }

  return convert_ok();
}

ConvertResult match_scope_flat(
  std::uint64_t roadmap_version, std::uint32_t view_id, ViewKind fixed_kind,
  const ViewScope & expected)
{
  ViewScope received;
  received.view_kind = fixed_kind;
  received.roadmap_version = roadmap_version;
  received.view_id = view_id;

  if (!is_usable_scope(received))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  return match_scope(received, expected);
}

// ── 열거 왕복 ─────────────────────────────────────────────────────────────────
//
// 형태는 전부 같다: 발행은 `static_cast`(위 미러 벽이 정합을 잠근다), 수신은 **와이어 상수
// `switch`** + `default` 에서 폐기. `default` 에 기본값 대입을 넣지 않는 것이 이 절의 핵심이다.

std::uint8_t escalation_reason_to_msg(EscalationReason reason) noexcept
{
  return static_cast<std::uint8_t>(reason);
}

ConvertResult escalation_reason_from_msg(std::uint8_t raw, EscalationReason & out)
{
  switch (raw)
  {
    case WireEscalationReport::REASON_BLOCKED:
      out = EscalationReason::BLOCKED;
      return convert_ok();
    case WireEscalationReport::REASON_WINDOW_EXPIRING:
      out = EscalationReason::WINDOW_EXPIRING;
      return convert_ok();
    case WireEscalationReport::REASON_QP_INFEASIBLE:
      out = EscalationReason::QP_INFEASIBLE;
      return convert_ok();
    case WireEscalationReport::REASON_LOCALIZATION_LOST:
      out = EscalationReason::LOCALIZATION_LOST;
      return convert_ok();
    case WireEscalationReport::REASON_SYNC_LOST:
      out = EscalationReason::SYNC_LOST;
      return convert_ok();
    default:
      // BLOCKED 로 흘리지 않는다 — SYNC_LOST 계열이 뭉개지면 R3/R4 가 오발동하고
      // F2 발동률·F6 스래싱 계측이 오염된다(계약 §0.2 뭉갬 금지).
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t escalation_severity_to_msg(EscalationSeverity severity) noexcept
{
  return static_cast<std::uint8_t>(severity);
}

ConvertResult escalation_severity_from_msg(std::uint8_t raw, EscalationSeverity & out)
{
  switch (raw)
  {
    case WireEscalationReport::SEVERITY_HARD:
      out = EscalationSeverity::HARD;
      return convert_ok();
    case WireEscalationReport::SEVERITY_SOFT:
      out = EscalationSeverity::SOFT;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t rung_to_msg(Rung rung) noexcept
{
  return static_cast<std::uint8_t>(rung);
}

ConvertResult rung_from_msg(std::uint8_t raw, Rung & out)
{
  switch (raw)
  {
    case WireRungEvent::RUNG_R0:
      out = Rung::R0_LOCAL_ABSORB;
      return convert_ok();
    case WireRungEvent::RUNG_R1:
      out = Rung::R1_BTPG;
      return convert_ok();
    case WireRungEvent::RUNG_R2:
      out = Rung::R2_MILP;
      return convert_ok();
    case WireRungEvent::RUNG_R3:
      out = Rung::R3_REPLAN;
      return convert_ok();
    case WireRungEvent::RUNG_R4:
      out = Rung::R4_REASSIGN;
      return convert_ok();
    case WireRungEvent::RUNG_R5:
      out = Rung::R5_OPERATOR;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t rung_transition_to_msg(RungTransition transition) noexcept
{
  return static_cast<std::uint8_t>(transition);
}

ConvertResult rung_transition_from_msg(std::uint8_t raw, RungTransition & out)
{
  switch (raw)
  {
    case WireRungEvent::TRANSITION_ENTER:
      out = RungTransition::ENTER;
      return convert_ok();
    case WireRungEvent::TRANSITION_ABSORBED:
      out = RungTransition::ABSORBED;
      return convert_ok();
    case WireRungEvent::TRANSITION_ESCALATE:
      out = RungTransition::ESCALATE;
      return convert_ok();
    case WireRungEvent::TRANSITION_GUARD_REJECT:
      out = RungTransition::GUARD_REJECT;
      return convert_ok();
    case WireRungEvent::TRANSITION_ESCAPE_HATCH:
      out = RungTransition::ESCAPE_HATCH;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t rung_trigger_kind_to_msg(RungTriggerKind trigger_kind) noexcept
{
  return static_cast<std::uint8_t>(trigger_kind);
}

ConvertResult rung_trigger_kind_from_msg(std::uint8_t raw, RungTriggerKind & out)
{
  switch (raw)
  {
    case WireRungEvent::TRIGGER_HARD:
      out = RungTriggerKind::HARD;
      return convert_ok();
    case WireRungEvent::TRIGGER_SOFT:
      out = RungTriggerKind::SOFT;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t revision_kind_to_msg(RevisionKind revision_kind) noexcept
{
  return static_cast<std::uint8_t>(revision_kind);
}

ConvertResult revision_kind_from_msg(std::uint8_t raw, RevisionKind & out)
{
  switch (raw)
  {
    case WireExecutionWindow::REVISION_NEW:
      out = RevisionKind::NEW;
      return convert_ok();
    case WireExecutionWindow::REVISION_TRUNCATE:
      out = RevisionKind::TRUNCATE;
      return convert_ok();
    default:
      // NEW 로 폴백시키지 않는다 — 부분 폐기를 신규 창으로 오해석하면 커밋 접두가 통째로
      // 교체되어 T1 의 I-QP 전제(정리 T1.1 귀납)가 깨진다.
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t adoption_status_to_msg(AdoptionStatus status) noexcept
{
  return static_cast<std::uint8_t>(status);
}

ConvertResult adoption_status_from_msg(std::uint8_t raw, AdoptionStatus & out)
{
  switch (raw)
  {
    case WireCommitStatus::QP_OK:
      out = AdoptionStatus::QP_OK;
      return convert_ok();
    case WireCommitStatus::QP_FALLBACK:
      out = AdoptionStatus::FALLBACK_GENERIC;
      return convert_ok();
    case WireCommitStatus::FALLBACK_NO_INDEX:
      out = AdoptionStatus::FALLBACK_NO_INDEX;
      return convert_ok();
    case WireCommitStatus::FALLBACK_NEIGHBOR_STALE:
      out = AdoptionStatus::FALLBACK_NEIGHBOR_STALE;
      return convert_ok();
    case WireCommitStatus::FALLBACK_SOLVER:
      out = AdoptionStatus::FALLBACK_SOLVER;
      return convert_ok();
    default:
      // 값 2(QP_TICK_JITTER)가 여기로 온다. 계약이 발행을 금지한 값이므로 도메인에 대응이
      // 없고, FALLBACK_GENERIC 으로 접으면 B5 폴백 사유별 분해가 철회된 지터 모델을 되살린다.
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t fault_kind_to_msg(FaultKind kind) noexcept
{
  return static_cast<std::uint8_t>(kind);
}

ConvertResult fault_kind_from_msg(std::uint8_t raw, FaultKind & out)
{
  switch (raw)
  {
    case WireSimInjectRequest::FAULT_COMM_DELAY:
      out = FaultKind::COMM_DELAY;
      return convert_ok();
    case WireSimInjectRequest::FAULT_COMM_DROP:
      out = FaultKind::COMM_DROP;
      return convert_ok();
    case WireSimInjectRequest::FAULT_CORRIDOR_CLOSE:
      out = FaultKind::CORRIDOR_CLOSE;
      return convert_ok();
    case WireSimInjectRequest::FAULT_CORRIDOR_OPEN:
      out = FaultKind::CORRIDOR_OPEN;
      return convert_ok();
    case WireSimInjectRequest::FAULT_ROBOT_STALL:
      out = FaultKind::ROBOT_STALL;
      return convert_ok();
    case WireSimInjectRequest::FAULT_ROBOT_FAULT:
      out = FaultKind::ROBOT_FAULT;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t physics_fidelity_to_msg(PhysicsFidelity fidelity) noexcept
{
  return static_cast<std::uint8_t>(fidelity);
}

ConvertResult physics_fidelity_from_msg(std::uint8_t raw, PhysicsFidelity & out)
{
  switch (raw)
  {
    case WireSimCapabilities::FIDELITY_NONE:
      out = PhysicsFidelity::NONE;
      return convert_ok();
    case WireSimCapabilities::FIDELITY_KINEMATIC:
      out = PhysicsFidelity::KINEMATIC;
      return convert_ok();
    case WireSimCapabilities::FIDELITY_FULL_PHYSICS:
      out = PhysicsFidelity::FULL_PHYSICS;
      return convert_ok();
    default:
      // 충실도를 낮은 등급으로 치환하면 충실도 부족 지표가 연구 검증으로 승격된다.
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t reassign_trigger_to_msg(ReassignTrigger trigger) noexcept
{
  return static_cast<std::uint8_t>(trigger);
}

ConvertResult reassign_trigger_from_msg(std::uint8_t raw, ReassignTrigger & out)
{
  switch (raw)
  {
    case WireReassignRequest::TRIGGER_HARD:
      out = ReassignTrigger::HARD;
      return convert_ok();
    case WireReassignRequest::TRIGGER_SOFT_REPEATED_FAILURE:
      out = ReassignTrigger::SOFT_REPEATED_FAILURE;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t reassign_result_to_msg(ReassignResult result) noexcept
{
  return static_cast<std::uint8_t>(result);
}

ConvertResult reassign_result_from_msg(std::uint8_t raw, ReassignResult & out)
{
  switch (raw)
  {
    case WireReassignResponse::RESULT_REASSIGNED:
      out = ReassignResult::REASSIGNED;
      return convert_ok();
    case WireReassignResponse::RESULT_QUEUED_AGING:
      out = ReassignResult::QUEUED_AGING;
      return convert_ok();
    case WireReassignResponse::RESULT_COOLDOWN_ACTIVE:
      out = ReassignResult::COOLDOWN_ACTIVE;
      return convert_ok();
    case WireReassignResponse::RESULT_ESCALATED_R5:
      out = ReassignResult::ESCALATED_R5;
      return convert_ok();
    case WireReassignResponse::RESULT_REJECTED:
      out = ReassignResult::REJECTED;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

std::uint8_t reorder_ack_to_msg(ReorderAck ack) noexcept
{
  return static_cast<std::uint8_t>(ack);
}

ConvertResult reorder_ack_from_msg(std::uint8_t raw, ReorderAck & out)
{
  switch (raw)
  {
    case WireReorderResponse::RESULT_ACCEPTED:
      out = ReorderAck::ACCEPTED;
      return convert_ok();
    case WireReorderResponse::RESULT_REJECTED_BUSY:
      out = ReorderAck::REJECTED_BUSY;
      return convert_ok();
    case WireReorderResponse::RESULT_REJECTED_SCOPE:
      out = ReorderAck::REJECTED_SCOPE;
      return convert_ok();
    case WireReorderResponse::RESULT_REJECTED_STALE:
      out = ReorderAck::REJECTED_STALE;
      return convert_ok();
    default:
      return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }
}

mrs_interfaces::msg::Pose2D to_msg(const Pose2D & pose)
{
  mrs_interfaces::msg::Pose2D msg;
  msg.x_m = pose.x_m;
  msg.y_m = pose.y_m;
  msg.theta_rad = pose.theta_rad;
  return msg;
}

ConvertResult from_msg(const mrs_interfaces::msg::Pose2D & msg, Pose2D & out)
{
  // 계약이 [-pi, pi] 를 불변식으로 선언한다. 여기서 조용히 정규화하지 않는 이유는,
  // 구간 밖 값이 온 것 자체가 발행 측 결함이고 그것을 감추면 회전 비용 추정(T2-R1)이
  // 경계에서 뒤집히는 원인을 잃기 때문이다.
  if (!is_pose_in_contract_range(msg.x_m, msg.y_m, msg.theta_rad))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  out.x_m = msg.x_m;
  out.y_m = msg.y_m;
  out.theta_rad = msg.theta_rad;
  return convert_ok();
}

ConvertResult poses_to_msg(
  const std::vector<Pose2D> & poses, std::vector<mrs_interfaces::msg::Pose2D> & out)
{
  std::vector<mrs_interfaces::msg::Pose2D> converted;
  converted.reserve(poses.size());

  for (const Pose2D & pose : poses)
  {
    // 발행 측 검사다. 단일 to_msg 는 순수 필드 복사로 확정돼 있어 여기서만 걸린다
    // (헤더의 @warning 참조 — L-14 가 발행 측 거부를 요구한다).
    if (!is_pose_in_contract_range(pose.x_m, pose.y_m, pose.theta_rad))
    {
      // 위반 정점만 빼고 내보내지 않는다 — 볼록포가 조용히 다른 도형이 된다.
      return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
    }
    converted.push_back(to_msg(pose));
  }

  out = std::move(converted);
  return convert_ok();
}

ConvertResult poses_from_msg(
  const std::vector<mrs_interfaces::msg::Pose2D> & msgs, std::vector<Pose2D> & out)
{
  std::vector<Pose2D> converted;
  converted.reserve(msgs.size());

  for (const mrs_interfaces::msg::Pose2D & msg : msgs)
  {
    Pose2D pose;
    const ConvertResult result = from_msg(msg, pose);
    if (!result.ok)
    {
      // out 을 건드리지 않은 채 빠져나간다 — 부분 변환 배열을 도메인에 넘기지 않는다.
      return result;
    }
    converted.push_back(pose);
  }

  out = std::move(converted);
  return convert_ok();
}

} // namespace mrs::convert
