/**
 * @file metrics_convert.cpp
 * @brief L-13(RungEvent) 변환 — [0a] tracer bullet 경로 1함수 구현.
 *
 * 예외를 던지지 않는다(계약 §0.1 V3, R-15 (c)). 실패는 @ref mrs::convert::ConvertResult 로
 * 보고하고 호출 노드가 안전 폴백한다.
 *
 * ## 왜 범위 검사가 유일한 방어선인가
 * 도메인 `RungEventRecord` 는 `transition`·`trigger_kind` 를 맨 `std::uint8_t` 로 들고 있어
 * 타입이 값 집합을 막아 주지 못한다. `RungEvent` 는 스스로 "스키마 드리프트 = 실험 무효"를
 * 선언하므로, 범위 밖 값을 관용하면 F2 발동률·F6 스래싱 카운터가 조용히 오염된다.
 */

#include "mrs_msg_convert/msg_convert.hpp"

#include <cmath>
#include <utility>

namespace mrs::convert
{
namespace
{

/**
 * @brief 도메인 가로대 식별자가 계약 확정 집합 안의 값인지 검사한다 (L-13).
 *
 * 발행 방향은 `static_cast` 로 값을 내보내므로, 호출자가 캐스트로 만든 집합 밖 값이 그대로
 * 와이어에 실릴 수 있다. 그 경로를 막는 지점이다.
 *
 * @param[in] rung 검사할 가로대. 자료형 `mrs::Rung`.
 * @return `bool` — 집합 `{R0 … R5}` 안이면 true.
 */
[[nodiscard]] bool is_known_rung(Rung rung) noexcept
{
  switch (rung)
  {
    case Rung::R0_LOCAL_ABSORB:
    case Rung::R1_BTPG:
    case Rung::R2_MILP:
    case Rung::R3_REPLAN:
    case Rung::R4_REASSIGN:
    case Rung::R5_OPERATOR:
      return true;
  }
  return false;
}

} // namespace

ConvertResult to_msg(
  const mrs::RungEventRecord & record, double stamp_s, mrs_interfaces::msg::RungEvent & out)
{
  // ① 필드 범위 — 불변식 E1. 0 이면 M1·M5 의 그룹 키가 사라진다.
  if (!mrs::is_valid_event_id(record.event_id))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ⚠ phi_hat_after_s 의 NaN 은 **유효값**("미산출")이다 — 유한성 필터로 거르지 않는다.
  if (!std::isfinite(record.phi_hat_before_s) || !std::isfinite(record.duration_s))
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  // ② 열거 범위 3건. 도메인이 raw uint8 로 들고 있는 두 값은 **수신 방향 헬퍼로 검증**한다 —
  //    그 헬퍼가 와이어 상수와 직접 대조하므로 미러 드리프트까지 함께 잡힌다.
  if (!is_known_rung(record.rung))
  {
    return convert_fail(ConvertStatus::ENUM_OUT_OF_RANGE);
  }

  RungTransition transition{};
  const ConvertResult transition_result = rung_transition_from_msg(record.transition, transition);
  if (!transition_result.ok)
  {
    return transition_result;
  }

  RungTriggerKind trigger_kind{};
  const ConvertResult trigger_result = rung_trigger_kind_from_msg(record.trigger_kind, trigger_kind);
  if (!trigger_result.ok)
  {
    return trigger_result;
  }

  // ③ 시각 가드.
  builtin_interfaces::msg::Time stamp;
  const ConvertResult stamp_result = seconds_to_time(stamp_s, stamp);
  if (!stamp_result.ok)
  {
    return stamp_result;
  }

  // 지역 버퍼에 쌓고 성공한 뒤에만 대입한다 — 실패 시 out 을 부분 채움 상태로 남기지 않는다.
  mrs_interfaces::msg::RungEvent filled;
  filled.header.stamp = stamp;
  filled.schema_version = mrs::RUNG_EVENT_SCHEMA_VERSION;
  filled.event_id = record.event_id;
  filled.rung = rung_to_msg(record.rung);
  filled.transition = rung_transition_to_msg(transition);
  filled.trigger_kind = rung_trigger_kind_to_msg(trigger_kind);
  filled.affected_robots = record.affected_robots;
  filled.edge_set_fingerprint = record.edge_set_fingerprint;
  filled.phi_hat_before_s = record.phi_hat_before_s;
  filled.phi_hat_after_s = record.phi_hat_after_s;
  filled.attempt_count = record.attempt_count;
  filled.duration_s = record.duration_s;
  // `detail` 은 도메인 레코드에 대응 필드가 없어 비운다(감사 L7). `reorder_job_id` 상관의
  // 유일한 통로이므로 헤더 @note 대로 **[2] 전 해소 대상**이다.

  out = std::move(filled);
  return convert_ok();
}

} // namespace mrs::convert
