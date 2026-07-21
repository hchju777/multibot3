#pragma once

/**
 * @file common_convert.hpp
 * @brief 변환 공통 기반 — 시각 가드·스코프 대조·기본 값 타입 변환.
 *
 * 이 헤더의 것들은 **각각 9곳 안팎에서 반복**된다(시각 변환 9곳, 스코프 대조 9곳).
 * 개별 함수마다 손으로 쓰면 9벌이 흩어지고, 그것은 "변환은 한 곳에서만"이라는 이 패키지의
 * 존재 이유를 스스로 위반하는 형태다. 그래서 나머지 변환보다 **먼저** 확정한다
 * (오케스트레이터 R-15 착수 순서 구속).
 *
 * @note `rclcpp` 에 의존하지 않는다. `builtin_interfaces/msg/Time` 은 rosidl 생성 타입이라
 *       `mrs_interfaces` 만으로 충분하다.
 */

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include "builtin_interfaces/msg/time.hpp"
#include "mrs/contract_types.hpp"       // EscalationReason·EscalationSeverity·Rung·RungTransition
                                        // ·RungTriggerKind·RevisionKind
#include "mrs/i_local_planner.hpp"      // AdoptionStatus
#include "mrs/i_sim_backend.hpp"        // FaultKind·PhysicsFidelity
#include "mrs/reorder_types.hpp"        // ReorderAck
#include "mrs/view_ids.hpp"
#include "mrs_interfaces/msg/pose2_d.hpp"
#include "mrs_interfaces/msg/view_scope.hpp"
#include "mrs_msg_convert/convert_result.hpp"
#include "mrs_task_alloc/lifelong_task_allocator.hpp" // ReassignTrigger·ReassignResult

namespace mrs::convert
{

// ── 시각 변환 (가드 포함) ──────────────────────────────────────────────────────
//
// 도메인은 시각을 `double` 초(시뮬 시계 기준 절대시각)로 다루고 와이어는
// `builtin_interfaces/msg/Time`(int32 sec + uint32 nanosec)을 쓴다. 두 표현의 반올림 방향·
// 음수·NaN·표현 범위 초과 처리 규약이 계약 어디에도 없었으므로 여기서 고정한다.

/**
 * @brief 도메인 시각(초)을 와이어 `Time` 으로 바꾼다.
 *
 * 규약: 음수·NaN·무한대·`int32` 초 범위 초과는 **실패**다. 0 으로 잘라내거나 포화시키지 않는다 —
 * 조용한 치환은 창 만료 판정(불변식 W5)을 엉뚱한 시점으로 옮기기 때문이다.
 * 나노초는 **가장 가까운 격자점으로 반올림**한다 — 버림은 정수 나노초로 누산된 시뮬 시각이
 * `double` 초를 거치는 동안 격자 아래로 1 ns 미끄러지게 만들고, 그러면 `floor((t - t0)/Δt_h)` 로
 * 틱 번호를 만드는 수신자가 직전 틱을 재계산해 **틱 결번**이 생긴다(계약 L-15, [0a] §4-① 후속).
 *
 * @param[in] seconds 시뮬 시계 기준 절대시각 [s]. 자료형 `double`. 음수·NaN 은 거부된다.
 * @param[out] out 변환된 와이어 시각. 자료형 `builtin_interfaces::msg::Time`.
 *             실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`, 가드 위반이면 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult seconds_to_time(double seconds, builtin_interfaces::msg::Time & out);

/**
 * @brief 와이어 `Time` 을 도메인 시각(초)으로 바꾼다.
 *
 * 규약: 음수 `sec`, `nanosec >= 1e9` 는 **실패**다(rosidl 은 이를 막지 않는다).
 *
 * @param[in] time 와이어 시각. 자료형 `builtin_interfaces::msg::Time`.
 * @param[out] out_seconds 시뮬 시계 기준 절대시각 [s]. 자료형 `double`.
 *             실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`, 가드 위반이면 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult time_to_seconds(
  const builtin_interfaces::msg::Time & time, double & out_seconds);

// ── 뷰 스코프 ──────────────────────────────────────────────────────────────────

/**
 * @brief 도메인 뷰 종류를 와이어 `VIEW_KIND_*` 상수로 바꾼다.
 * @param[in] kind 뷰 종류. 자료형 `mrs::ViewKind`.
 * @return `std::uint8_t` — `ViewScope.msg` 의 `VIEW_KIND_*` 값.
 */
[[nodiscard]] std::uint8_t view_kind_to_msg(ViewKind kind) noexcept;

/**
 * @brief 와이어 `VIEW_KIND_*` 상수를 도메인 뷰 종류로 바꾼다.
 *
 * @warning 알 수 없는 값을 기본값으로 치환하지 않는다 — 상위 버전 발행자가 보낸 새 종류를
 *          조용히 오해석하는 경로이기 때문이다.
 *
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 뷰 종류. 자료형 `mrs::ViewKind`. 실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 집합 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult view_kind_from_msg(std::uint8_t raw, ViewKind & out);

/**
 * @brief 도메인 뷰 스코프를 와이어 메시지로 바꾼다.
 * @param[in] scope 뷰 스코프. 자료형 `mrs::ViewScope`.
 * @return `mrs_interfaces::msg::ViewScope` — 대응 와이어 메시지.
 */
[[nodiscard]] mrs_interfaces::msg::ViewScope to_msg(const ViewScope & scope);

/**
 * @brief 와이어 뷰 스코프를 도메인 타입으로 바꾼다.
 * @param[in] msg 와이어 뷰 스코프. 자료형 `mrs_interfaces::msg::ViewScope`.
 * @param[out] out 변환된 스코프. 자료형 `mrs::ViewScope`. 실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 종류가 집합 밖이면 `ENUM_OUT_OF_RANGE`,
 *         스코프가 사용 불가(지도 버전 0 등)이면 `FIELD_RANGE_VIOLATION`.
 */
[[nodiscard]] ConvertResult from_msg(const mrs_interfaces::msg::ViewScope & msg, ViewScope & out);

/**
 * @brief 수신한 스코프가 기대와 일치하는지 대조한다 (규칙 V1·V2 강제 지점).
 *
 * 종류와 인스턴스를 **분리해 판정**한다 — 둘을 한 사유로 뭉치면 노드가 회복 방법을 고를 수 없기
 * 때문이다. 종류 불일치는 발행자 버그이므로 폐기가 끝이지만, 인스턴스 불일치는 지도가 개정된
 * 정상 상황일 수 있어 **MapRegistry 재조회로 회복**한다(서비스 응답으로는 STALE_VERSION).
 *
 * @param[in] received 메시지가 실어 온 스코프. 자료형 `mrs::ViewScope`.
 * @param[in] expected 수신자가 현재 유효하다고 보는 스코프. 자료형 `mrs::ViewScope`.
 *            보통 호출 노드가 들고 있는 MapRegistry 캐시 키다.
 * @return `ConvertResult` — 일치하면 `ok`, 종류가 다르면 @ref ConvertStatus::VIEW_KIND_MISMATCH,
 *         인스턴스가 다르면 @ref ConvertStatus::VIEW_SCOPE_MISMATCH.
 */
[[nodiscard]] ConvertResult match_scope(const ViewScope & received, const ViewScope & expected);

/**
 * @brief 평면 쌍(`roadmap_version` + `view_id`)으로 실린 스코프를 기대와 대조한다.
 *
 * `ExecutionWindow` 처럼 `ViewScope` 메시지 대신 평면 쌍을 이미 갖고 있던 계약에 쓴다(규칙 V2).
 * 종류는 계약이 필드마다 고정하므로 인자로 받는다.
 *
 * @param[in] roadmap_version 메시지가 실어 온 지도 버전. 자료형 `std::uint64_t`.
 * @param[in] view_id 메시지가 실어 온 뷰 인스턴스 id. 자료형 `std::uint32_t`.
 * @param[in] fixed_kind 계약이 이 메시지에 고정한 뷰 종류. 자료형 `mrs::ViewKind`.
 * @param[in] expected 수신자가 현재 유효하다고 보는 스코프. 자료형 `mrs::ViewScope`.
 * @return `ConvertResult` — @ref match_scope 와 같은 판정 규약.
 */
[[nodiscard]] ConvertResult match_scope_flat(
  std::uint64_t roadmap_version,
  std::uint32_t view_id,
  ViewKind fixed_kind,
  const ViewScope & expected);

// ── 열거 왕복 (도메인 enum ↔ 와이어 `uint8`) ──────────────────────────────────
//
// 아래 12쌍은 전부 @ref view_kind_to_msg / @ref view_kind_from_msg 와 **같은 형태**다.
//  - 발행(`*_to_msg`)은 실패할 수 없다 — 도메인 enum 은 정의상 집합 안이다. 값 반환 + `noexcept`.
//  - 수신(`*_from_msg`)은 **와이어 상수와 대조**한다. 도메인 enum 값으로 캐스팅하면 미러가
//    어긋났을 때(계약이 값을 바꿨는데 C++ enum 이 안 따라온 경우) 아무도 잡지 못한다.
//  - ⛔ **알 수 없는 값을 기본값으로 치환하지 않는다.** 상위 버전 발행자가 보낸 새 값을 조용히
//    오해석하는 경로이며, 그것이 계약 §0.2 "사유 뭉갬"의 가장 흔한 형태다. 폐기가 정답이다.
//
// @note 미러 정합(도메인 값 == 와이어 상수)은 `common_convert.cpp` 의 `static_assert` 벽이
//       **컴파일 시각에** 잠근다. 그래서 발행 방향의 `static_cast` 가 안전하다.

/**
 * @brief 도메인 에스컬레이션 사유를 와이어 `REASON_*` 상수로 바꾼다 (L-04).
 * @param[in] reason 에스컬레이션 사유. 자료형 `mrs::EscalationReason`.
 * @return `std::uint8_t` — `EscalationReport.msg` 의 `REASON_*` 값.
 */
[[nodiscard]] std::uint8_t escalation_reason_to_msg(EscalationReason reason) noexcept;

/**
 * @brief 와이어 `REASON_*` 상수를 도메인 에스컬레이션 사유로 바꾼다 (L-04).
 *
 * @warning `SYNC_LOST` 가 다른 값으로 뭉개지면 D-08 라우팅이 계획 문제로 오인해 재계획을
 *          오발동하고, 그 결과 F2 발동률·F6 스래싱 계측이 오염된다. 집합 밖은 폐기다.
 *
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 사유. 자료형 `mrs::EscalationReason`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..4}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult escalation_reason_from_msg(std::uint8_t raw, EscalationReason & out);

/**
 * @brief 도메인 심각도를 와이어 `SEVERITY_*` 상수로 바꾼다 (L-04).
 * @note 값의 정본은 계약 L-04 매핑표이고 그 코드 정본은 `mrs::severity_of` 다. 이 함수는
 *       그 결과를 와이어로 옮기기만 한다 — 여기서 사유→심각도를 다시 매핑하지 말 것.
 * @param[in] severity 심각도. 자료형 `mrs::EscalationSeverity`.
 * @return `std::uint8_t` — `EscalationReport.msg` 의 `SEVERITY_*` 값.
 */
[[nodiscard]] std::uint8_t escalation_severity_to_msg(EscalationSeverity severity) noexcept;

/**
 * @brief 와이어 `SEVERITY_*` 상수를 도메인 심각도로 바꾼다 (L-04).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 심각도. 자료형 `mrs::EscalationSeverity`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult escalation_severity_from_msg(
  std::uint8_t raw, EscalationSeverity & out);

/**
 * @brief 도메인 가로대 식별자를 와이어 `RUNG_*` 상수로 바꾼다 (L-13).
 * @param[in] rung 가로대 식별자. 자료형 `mrs::Rung`.
 * @return `std::uint8_t` — `RungEvent.msg` 의 `RUNG_*` 값.
 */
[[nodiscard]] std::uint8_t rung_to_msg(Rung rung) noexcept;

/**
 * @brief 와이어 `RUNG_*` 상수를 도메인 가로대 식별자로 바꾼다 (L-13).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 가로대. 자료형 `mrs::Rung`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..5}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult rung_from_msg(std::uint8_t raw, Rung & out);

/**
 * @brief 도메인 가로대 전이를 와이어 `TRANSITION_*` 상수로 바꾼다 (L-13).
 * @param[in] transition 전이 종류. 자료형 `mrs::RungTransition`.
 * @return `std::uint8_t` — `RungEvent.msg` 의 `TRANSITION_*` 값.
 */
[[nodiscard]] std::uint8_t rung_transition_to_msg(RungTransition transition) noexcept;

/**
 * @brief 와이어 `TRANSITION_*` 상수를 도메인 가로대 전이로 바꾼다 (L-13).
 * @note `RungEvent` 는 스스로 "스키마 드리프트 = 실험 무효"를 선언한다. 범위 밖 값을 관용하면
 *       F2 발동률·F6 스래싱 카운터가 조용히 오염된다.
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 전이. 자료형 `mrs::RungTransition`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..4}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult rung_transition_from_msg(std::uint8_t raw, RungTransition & out);

/**
 * @brief 도메인 트리거 출처를 와이어 `TRIGGER_*` 상수로 바꾼다 (L-13).
 * @warning `mrs::EscalationSeverity` 와 값 코딩이 같아 보이나 의미가 다르다(심각도 vs 출처).
 *          계약이 별개 열거로 규정했으므로 두 변환을 공유하지 말 것.
 * @param[in] trigger_kind 트리거 출처. 자료형 `mrs::RungTriggerKind`.
 * @return `std::uint8_t` — `RungEvent.msg` 의 `TRIGGER_*` 값.
 */
[[nodiscard]] std::uint8_t rung_trigger_kind_to_msg(RungTriggerKind trigger_kind) noexcept;

/**
 * @brief 와이어 `TRIGGER_*` 상수를 도메인 트리거 출처로 바꾼다 (L-13).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 트리거 출처. 자료형 `mrs::RungTriggerKind`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult rung_trigger_kind_from_msg(std::uint8_t raw, RungTriggerKind & out);

/**
 * @brief 도메인 개정 종류를 와이어 `REVISION_*` 상수로 바꾼다 (L-01, load-bearing seam).
 * @param[in] revision_kind 개정 종류. 자료형 `mrs::RevisionKind`.
 * @return `std::uint8_t` — `ExecutionWindow.msg` 의 `REVISION_*` 값.
 */
[[nodiscard]] std::uint8_t revision_kind_to_msg(RevisionKind revision_kind) noexcept;

/**
 * @brief 와이어 `REVISION_*` 상수를 도메인 개정 종류로 바꾼다 (L-01).
 * @warning ⛔ **미지의 값을 `NEW` 로 폴백시키지 말 것.** 실제로는 `TRUNCATE` 계열인 개정을
 *          신규 창으로 오해석하면 커밋 접두가 통째로 교체되어 T1 의 I-QP 전제가 깨진다.
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 개정 종류. 자료형 `mrs::RevisionKind`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult revision_kind_from_msg(std::uint8_t raw, RevisionKind & out);

/**
 * @brief 도메인 채택 종류를 와이어 `qp_status` 값으로 바꾼다 (L-14).
 * @note 도메인 열거에는 값 2 가 **없다** — `QP_TICK_JITTER` 는 v1.1.0 에서 발행 금지됐고, 그
 *       금지를 표현 자체로 강제한 결과다. 따라서 이 함수는 2 를 만들어 낼 수 없다.
 * @param[in] status 채택 종류·폴백 사유. 자료형 `mrs::AdoptionStatus`.
 * @return `std::uint8_t` — `CommitStatus.msg` 의 `QP_*`/`FALLBACK_*` 값 (2 는 반환되지 않는다).
 */
[[nodiscard]] std::uint8_t adoption_status_to_msg(AdoptionStatus status) noexcept;

/**
 * @brief 와이어 `qp_status` 값을 도메인 채택 종류로 바꾼다 (L-14).
 * @warning **값 2(`QP_TICK_JITTER`)는 거부한다.** 계약이 발행을 금지한 값이고 도메인에 대응
 *          열거가 없다. 이것을 `FALLBACK_GENERIC` 으로 접으면 B5 폴백 사유별 분해 계측이
 *          "지터가 폴백을 유발했다"는 철회된 모델을 되살린다(T4 §6.1).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 채택 종류. 자료형 `mrs::AdoptionStatus`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1, 3, 4, 5}` 밖(**2 포함**)이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult adoption_status_from_msg(std::uint8_t raw, AdoptionStatus & out);

/**
 * @brief 도메인 결함 종류를 와이어 `FAULT_*` 상수로 바꾼다 (L-19 `SimInject.srv`).
 * @param[in] kind 결함 종류. 자료형 `mrs::FaultKind`.
 * @return `std::uint8_t` — `SimInject.srv` 요청의 `FAULT_*` 값.
 */
[[nodiscard]] std::uint8_t fault_kind_to_msg(FaultKind kind) noexcept;

/**
 * @brief 와이어 `FAULT_*` 상수를 도메인 결함 종류로 바꾼다 (L-19).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 결함 종류. 자료형 `mrs::FaultKind`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..5}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 *         호출자는 이 사유를 `RESULT_UNSUPPORTED` 로 응답한다(대상 부적합과 구별할 것).
 */
[[nodiscard]] ConvertResult fault_kind_from_msg(std::uint8_t raw, FaultKind & out);

/**
 * @brief 도메인 물리 충실도를 와이어 `FIDELITY_*` 상수로 바꾼다 (`SimCapabilities.msg`).
 * @param[in] fidelity 물리 충실도 등급. 자료형 `mrs::PhysicsFidelity`.
 * @return `std::uint8_t` — `SimCapabilities.msg` 의 `FIDELITY_*` 값.
 */
[[nodiscard]] std::uint8_t physics_fidelity_to_msg(PhysicsFidelity fidelity) noexcept;

/**
 * @brief 와이어 `FIDELITY_*` 상수를 도메인 물리 충실도로 바꾼다 (`SimCapabilities.msg`).
 * @warning 값이 틀리면 sim-runner 의 `OPEN(pending-isaac)` 자동 태깅이 무너져 충실도 부족
 *          지표가 연구 검증으로 승격된다. 기본값 치환은 곧 kill-gate 정직성 손상이다.
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 충실도. 자료형 `mrs::PhysicsFidelity`.
 *             실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1, 2}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult physics_fidelity_from_msg(std::uint8_t raw, PhysicsFidelity & out);

/**
 * @brief 도메인 재할당 트리거를 와이어 `TRIGGER_*` 상수로 바꾼다 (`ReassignRequest.srv`).
 * @param[in] trigger 트리거 종류. 자료형 `mrs::ReassignTrigger`.
 * @return `std::uint8_t` — `ReassignRequest.srv` 요청의 `TRIGGER_*` 값.
 */
[[nodiscard]] std::uint8_t reassign_trigger_to_msg(ReassignTrigger trigger) noexcept;

/**
 * @brief 와이어 `TRIGGER_*` 상수를 도메인 재할당 트리거로 바꾼다 (`ReassignRequest.srv`).
 * @note `RungTriggerKind`·`EscalationSeverity` 와 값 코딩이 같아 보이나 **별개 열거**다(D-02 v2).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 트리거. 자료형 `mrs::ReassignTrigger`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0, 1}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult reassign_trigger_from_msg(std::uint8_t raw, ReassignTrigger & out);

/**
 * @brief 도메인 재할당 결과를 와이어 `RESULT_*` 상수로 바꾼다 (`ReassignRequest.srv` 응답).
 * @param[in] result 처리 결과. 자료형 `mrs::ReassignResult`.
 * @return `std::uint8_t` — `ReassignRequest.srv` 응답의 `RESULT_*` 값.
 */
[[nodiscard]] std::uint8_t reassign_result_to_msg(ReassignResult result) noexcept;

/**
 * @brief 와이어 `RESULT_*` 상수를 도메인 재할당 결과로 바꾼다 (`ReassignRequest.srv` 응답).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 결과. 자료형 `mrs::ReassignResult`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..4}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult reassign_result_from_msg(std::uint8_t raw, ReassignResult & out);

/**
 * @brief 도메인 접수 결과를 와이어 `RESULT_*` 상수로 바꾼다 (`ReorderPassing.srv` 응답).
 * @param[in] ack 접수 결과. 자료형 `mrs::ReorderAck`.
 * @return `std::uint8_t` — `ReorderPassing.srv` 응답의 `RESULT_*` 값.
 */
[[nodiscard]] std::uint8_t reorder_ack_to_msg(ReorderAck ack) noexcept;

/**
 * @brief 와이어 `RESULT_*` 상수를 도메인 접수 결과로 바꾼다 (`ReorderPassing.srv` 응답).
 * @param[in] raw 와이어 값. 자료형 `std::uint8_t`.
 * @param[out] out 변환된 접수 결과. 자료형 `mrs::ReorderAck`. 실패 시 내용은 변경되지 않는다.
 * @return `ConvertResult` — 집합 `{0..3}` 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 */
[[nodiscard]] ConvertResult reorder_ack_from_msg(std::uint8_t raw, ReorderAck & out);

// ── 노드 id 랩/언랩 (강타입 ↔ 와이어 `uint32`) ────────────────────────────────
//
// 계약 §0.1 V3 는 `uint32` ↔ 강타입 변환을 이 패키지의 **단독 소유**로 못박는다. 뷰가 3종이라
// 손으로 쓰면 같은 코드가 뷰마다 3벌, 필드마다 다시 흩어진다. 그래서 템플릿 하나로 모은다.
//
// 여기서 다루는 것은 **뷰 인스턴스가 아니라 값 자체**다 — 인스턴스 대조(V2)는 @ref match_scope /
// @ref match_scope_flat 의 몫이고, 종류 고정(V1)은 호출 함수가 어느 `ViewNodeId<K>` 를 넘기는가로
// 결정된다. 두 책임을 이 헬퍼에 합치지 않는 이유는, 한 메시지가 노드 id 를 여러 개 실어도
// 스코프 대조는 **한 번**이면 되기 때문이다.

/**
 * @brief 랩(수신) 시 "해당 없음" 센티넬(4294967295)을 허용할지의 정책.
 *
 * 필드마다 다르다 — `RobotState.occupied_node` 는 "엣지 위에 있어 점유 노드가 없음"이 **정상**
 * 이고, `WindowSegment.node_from` 은 센티넬이 오면 그 자체로 창이 무효다. 정책을 인자로 받지
 * 않으면 둘 중 하나를 반드시 틀리게 처리하게 된다.
 *
 * @note `bool` 대신 열거를 쓰는 이유는 호출부 가독성이다 — `node_id_from_msg(raw, true, out)` 는
 *       읽는 사람이 무엇이 참인지 알 수 없다.
 */
enum class NoneNodePolicy : std::uint8_t
{
  /** @brief 센티넬 금지. 오면 @ref ConvertStatus::FIELD_RANGE_VIOLATION 으로 폐기한다. */
  REJECT = 0,
  /** @brief 센티넬은 "해당 없음"을 뜻하는 정상 값이다. 그대로 랩한다. */
  ALLOW = 1
};

/**
 * @brief 어떤 타입이 @ref mrs::ViewNodeId 인스턴스인지 판정하는 형질.
 * @tparam T 판정할 타입.
 */
template <typename T>
struct IsViewNodeId : std::false_type
{
};

/**
 * @brief @ref IsViewNodeId 의 부분 특수화 — 뷰 종류 3종 전부를 이 하나가 덮는다.
 * @tparam K 뷰 종류.
 */
template <ViewKind K>
struct IsViewNodeId<ViewNodeId<K>> : std::true_type
{
};

/**
 * @brief @ref IsViewNodeId 의 값 별칭.
 * @tparam T 판정할 타입.
 */
template <typename T>
inline constexpr bool IS_VIEW_NODE_ID = IsViewNodeId<T>::value;

/**
 * @brief 강타입 노드 id 를 와이어 `uint32` 로 언랩한다 (발행 방향). 실패할 수 없다.
 *
 * 센티넬 판정을 하지 않는 이유: 발행 측에서 "해당 없음"이 유효한지는 **필드마다 다르고**
 * 그것을 아는 것은 계약별 변환 함수다. 여기서 일률적으로 거르면 `RobotState.occupied_node`
 * 같은 정상 경로가 막힌다.
 *
 * @tparam NodeIdT 노드 id 강타입. `mrs::ViewNodeId<K>` 여야 한다
 *         (아래 `static_assert` — 맨 `std::uint32_t` 나 다른 정수형을 넘기면 컴파일 거부).
 * @param[in] id 언랩할 노드 id. 자료형 `NodeIdT`. 뷰 @p K 안에서의 노드 번호를 담는다.
 * @return `std::uint32_t` — 원시 노드 번호. 센티넬이면 4294967295.
 */
template <typename NodeIdT>
[[nodiscard]] constexpr std::uint32_t node_id_to_msg(NodeIdT id) noexcept
{
  // 제약을 두는 이유: 이 헬퍼가 맨 정수까지 받아 주면 "언랩했다"는 흔적만 남고 뷰 강타입이
  // 우회된다 — 그것은 규칙 V3 가 막으려는 바로 그 형태다. 컴파일 단계에서 끊는다.
  static_assert(
    IS_VIEW_NODE_ID<NodeIdT>,
    "node_id_to_msg 는 mrs::ViewNodeId<K> 에만 적용된다 (규칙 V3 — 맨 uint32 우회 금지)");
  return id.value();
}

/**
 * @brief 와이어 `uint32` 를 강타입 노드 id 로 랩한다 (수신 방향).
 *
 * @tparam NodeIdT 노드 id 강타입. `mrs::ViewNodeId<K>` 여야 한다(`static_assert` 로 강제).
 *         @p out 으로부터 추론되므로 호출부가 명시할 필요가 없고, 동시에 **어느 뷰로 랩하는가**를
 *         호출부가 타입으로 선언하게 된다(규칙 V1).
 * @param[in] raw 와이어 노드 번호. 자료형 `std::uint32_t`. 4294967295 = 센티넬.
 * @param[in] none_policy 센티넬 허용 여부. 자료형 @ref NoneNodePolicy.
 *            그 필드에서 "해당 없음"이 계약상 정상인지에 따라 호출부가 고른다.
 * @param[out] out 랩된 노드 id. 자료형 `NodeIdT`. 실패 시 내용은 **변경되지 않는다**.
 * @return `ConvertResult` — 성공이면 `ok`. `none_policy == REJECT` 인데 센티넬이 오면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
template <typename NodeIdT>
[[nodiscard]] ConvertResult node_id_from_msg(
  std::uint32_t raw, NoneNodePolicy none_policy, NodeIdT & out)
{
  static_assert(
    IS_VIEW_NODE_ID<NodeIdT>,
    "node_id_from_msg 는 mrs::ViewNodeId<K> 에만 적용된다 (규칙 V3 — 맨 uint32 우회 금지)");

  const NodeIdT wrapped{raw};
  if (none_policy == NoneNodePolicy::REJECT && wrapped.is_none())
  {
    return convert_fail(ConvertStatus::FIELD_RANGE_VIOLATION);
  }

  out = wrapped;
  return convert_ok();
}

/**
 * @brief 강타입 노드 id 배열을 와이어 `uint32` 배열로 언랩한다 (발행 방향). 실패할 수 없다.
 *
 * @tparam NodeIdT 노드 id 강타입. `mrs::ViewNodeId<K>` 여야 한다(`static_assert` 로 강제).
 * @param[in] ids 언랩할 노드 id 배열. 자료형 `std::vector<NodeIdT>`.
 *            **빈 배열은 유효**하다 — 비었다는 사실 자체가 오류 신호는 아니다.
 * @return `std::vector<std::uint32_t>` — 입력과 같은 길이·순서의 원시 노드 번호 배열.
 */
template <typename NodeIdT>
[[nodiscard]] std::vector<std::uint32_t> node_ids_to_msg(const std::vector<NodeIdT> & ids)
{
  static_assert(
    IS_VIEW_NODE_ID<NodeIdT>,
    "node_ids_to_msg 는 mrs::ViewNodeId<K> 에만 적용된다 (규칙 V3 — 맨 uint32 우회 금지)");

  std::vector<std::uint32_t> raw;
  raw.reserve(ids.size());
  for (const NodeIdT & id : ids)
  {
    raw.push_back(id.value());
  }
  return raw;
}

/**
 * @brief 와이어 `uint32` 배열을 강타입 노드 id 배열로 랩한다 (수신 방향).
 *
 * **원소 하나라도 실패하면 전체 실패**이고 @p out 은 손대지 않는다. 부분 변환된 배열을 도메인에
 * 넘기면 경로·위반 목록이 조용히 짧아지고, 짧아진 경로는 "정상적으로 짧은 경로"와 구별되지
 * 않는다 — 폐기해야 할 메시지가 유효한 계획으로 둔갑하는 형태다.
 *
 * @tparam NodeIdT 노드 id 강타입. `mrs::ViewNodeId<K>` 여야 한다(`static_assert` 로 강제).
 * @param[in] raw 와이어 노드 번호 배열. 자료형 `std::vector<std::uint32_t>`.
 *            **빈 배열은 유효**하다.
 * @param[in] none_policy 센티넬 허용 여부. 자료형 @ref NoneNodePolicy.
 *            배열 전 원소에 같게 적용된다.
 * @param[out] out 랩된 노드 id 배열. 자료형 `std::vector<NodeIdT>`.
 *             성공 시에만 대입되며, 실패 시 **기존 내용이 보존된다**(부분 채움이 없다).
 * @return `ConvertResult` — 성공이면 `ok`. 어느 원소든 센티넬 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
template <typename NodeIdT>
[[nodiscard]] ConvertResult node_ids_from_msg(
  const std::vector<std::uint32_t> & raw, NoneNodePolicy none_policy, std::vector<NodeIdT> & out)
{
  static_assert(
    IS_VIEW_NODE_ID<NodeIdT>,
    "node_ids_from_msg 는 mrs::ViewNodeId<K> 에만 적용된다 (규칙 V3 — 맨 uint32 우회 금지)");

  std::vector<NodeIdT> wrapped;
  wrapped.reserve(raw.size());
  for (const std::uint32_t value : raw)
  {
    NodeIdT element{};
    const ConvertResult result = node_id_from_msg(value, none_policy, element);
    if (!result.ok)
    {
      // out 을 건드리지 않은 채 빠져나간다 — 부분 변환 배열을 도메인에 넘기지 않는다.
      return result;
    }
    wrapped.push_back(element);
  }

  out = std::move(wrapped);
  return convert_ok();
}

// ── 기본 값 타입 ───────────────────────────────────────────────────────────────

/**
 * @brief 도메인 평면 자세를 와이어 메시지로 바꾼다.
 * @note 이 자세는 **몸체점 q** 다. NID 오프셋점 z 는 L4 내부 표현이며 계약에 노출하지 않는다.
 * @param[in] pose 평면 자세(map 프레임, 몸체점 q). 자료형 `mrs::Pose2D`.
 * @return `mrs_interfaces::msg::Pose2D` — 대응 와이어 메시지.
 */
[[nodiscard]] mrs_interfaces::msg::Pose2D to_msg(const Pose2D & pose);

/**
 * @brief 와이어 평면 자세를 도메인 타입으로 바꾼다.
 *
 * 각도가 `[-pi, pi]` 밖이거나 유한하지 않으면 실패다 — 계약이 이 구간을 불변식으로 선언했고,
 * 구간 밖 값이 흘러들면 회전 비용 추정(T2-R1)이 경계에서 뒤집힌다.
 *
 * @param[in] msg 와이어 평면 자세. 자료형 `mrs_interfaces::msg::Pose2D`.
 * @param[out] out 변환된 자세. 자료형 `mrs::Pose2D`. 실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 범위·유한성 위반이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
[[nodiscard]] ConvertResult from_msg(const mrs_interfaces::msg::Pose2D & msg, Pose2D & out);

/**
 * @brief 도메인 평면 자세 배열을 와이어 메시지 배열로 바꾼다 (발행 방향).
 *
 * `CommitStatus.commit_hull_vertices`(L-14)·`TransformNode` 응답처럼 자세 배열을 싣는 계약이
 * 여러 곳이라 배열 변환을 한 곳에 모은다.
 *
 * @warning **이 함수는 단일 @ref to_msg(const Pose2D &) 의 N 배가 아니다.** 단일 발행 변환은
 *          순수 필드 복사(실패 불가)로 확정돼 있으나, L-14 는 정점의 각도 범위·유한성 위반을
 *          발행 측에서 `FIELD_RANGE_VIOLATION` 으로 거부하라고 요구한다. 그 검사를 여기에 둔다 —
 *          단일 함수의 반환 규약을 바꾸면 이미 확정된 계약측 서술이 흔들리기 때문이다.
 * @note 원소 하나라도 실패하면 **전체 실패**다. 위반 정점만 빼고 내보내면 볼록포가 조용히
 *       다른 도형이 되고, L3 의 A1(`C_i ⊆ span(Pi_i)`) 검사가 틀린 입력으로 통과한다.
 *
 * @param[in] poses 도메인 자세 배열(map 프레임, 몸체점 q). 자료형 `std::vector<mrs::Pose2D>`.
 *            ⚠ **빈 배열은 유효**하다("커밋 없음/정지") — 오류로 처리하지 않는다.
 * @param[out] out 변환된 메시지 배열. 자료형 `std::vector<mrs_interfaces::msg::Pose2D>`.
 *             성공 시에만 대입되며, 실패 시 **기존 내용이 보존된다**.
 * @return `ConvertResult` — 성공이면 `ok`. 어느 원소든 비유한이거나 `theta_rad` 가
 *         `[-pi, pi]` 밖이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
[[nodiscard]] ConvertResult poses_to_msg(
  const std::vector<Pose2D> & poses, std::vector<mrs_interfaces::msg::Pose2D> & out);

/**
 * @brief 와이어 평면 자세 배열을 도메인 타입 배열로 바꾼다 (수신 방향).
 *
 * 원소별 검사는 단일 @ref from_msg(const mrs_interfaces::msg::Pose2D &, Pose2D &) 에 위임한다 —
 * 각도 구간 판정이 두 벌이 되면 즉시 드리프트한다.
 *
 * @note 원소 하나라도 실패하면 **전체 실패**다(@ref poses_to_msg 와 같은 이유).
 *
 * @param[in] msgs 와이어 자세 배열. 자료형 `std::vector<mrs_interfaces::msg::Pose2D>`.
 *            **빈 배열은 유효**하다.
 * @param[out] out 변환된 도메인 자세 배열. 자료형 `std::vector<mrs::Pose2D>`.
 *             성공 시에만 대입되며, 실패 시 **기존 내용이 보존된다**.
 * @return `ConvertResult` — 성공이면 `ok`. 어느 원소든 범위·유한성 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
[[nodiscard]] ConvertResult poses_from_msg(
  const std::vector<mrs_interfaces::msg::Pose2D> & msgs, std::vector<Pose2D> & out);

} // namespace mrs::convert
