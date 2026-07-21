#pragma once

/**
 * @file msg_convert.hpp
 * @brief 도메인 타입 <-> mrs_interfaces 메시지 변환의 유일한 장소 (architecture §2.1 L2).
 *
 * ros/ 의 어떤 노드도 mrs_interfaces 메시지 필드를 직접 도메인 타입 필드에 대입하지 않는다 —
 * 전부 이 패키지의 함수를 거친다. 이 계층이 없으면 스키마 변경이 노드 코드 곳곳에 흩어져
 * 계약 변경 게이트(contract-registry §7)가 무력화된다.
 *
 * ## 예외 정책 (R-15 (c))
 * **이 패키지의 함수는 밖으로 예외를 던지지 않는다. 변환 실패는 반환값으로 보고하고 호출자가
 * 안전 폴백한다.** 근거는 계약 §0.1 규칙 V3 다 — 수신 실패의 규정 거동이 "폐기 + 로그 + 카운터"
 * 이고 예외는 명시적으로 금지된다. 실패를 예외로 올리면 "무엇을 몇 번 폐기했는가"를 적립할
 * 자리가 사라진다.
 *
 * ## 반환 규약 — 결과 struct + out 파라미터 (R-15 (a))
 * 실패할 수 있는 함수는 값이 아니라 @ref ConvertResult 를 반환하고 결과는 out 파라미터로 낸다.
 * `bool` 1비트로는 계약을 지킬 수 없다 — `PartialReplan.srv` 가 `RESULT_STALE_VERSION` 과
 * `RESULT_REJECTED_BAD_REQUEST` 를 **서로 다른 응답 코드로 구별하라고 요구**하기 때문이다.
 * 사유의 정본표는 계약 §0.2 이며 C++ 미러는 @ref ConvertStatus 다(`convert_result.hpp`).
 *
 * **판정은 여기서, 보고는 노드에서** (R-15 (b) = b3). 이 패키지는 사유만 반환한다. 로그 발행·
 * 폐기 카운터 적립·안전 폴백은 호출자인 `mrs_ros_*` 노드의 몫이다. 이 패키지에 `rclcpp` 를
 * 끌어들이면 아키텍처 §2.2 의 의존 못박기를 이탈하고 unit 티어가 ROS 없이 돌지 못한다.
 *
 * @note 실패가 구조적으로 불가능한 함수(순수 필드 복사이고 범위·열거·시각 변환이 하나도 없는
 *       경우)만 값 반환을 유지한다. 이 헤더에는 그런 함수가 **하나도 없다** — 노드 id 를 싣는
 *       함수는 센티넬·스코프 검사가 있고, 그렇지 않은 함수도 최소한 `double` 유한성이나
 *       `builtin_interfaces/Time` 변환 가드에 걸린다. 따라서 전 함수가 `ConvertResult` 를
 *       반환한다. `Pose2D`·`ViewScope` 의 **발행** 방향만이 유일한 예외이고 그것은 이미
 *       `common_convert.hpp` 에 값 반환으로 확정돼 있다(여기서 재선언하지 않는다).
 *
 * ## 수신 함수의 기대 스코프 (R-15 (d) = d1)
 * 노드 id 를 다루는 수신 함수는 `const mrs::ViewScope & expected` 를 받아 **함수 안에서**
 * 대조한다(계약 §0.1 V3 — 단일 소유). 대조는 두 단계다:
 *  1. **종류(V1)** — 뷰 종류는 계약이 필드마다 고정하므로(§0.1 뷰 종류 정본표) 함수 안에
 *     하드코딩하고, `expected.view_kind` 와도 대조해 **호출자가 엉뚱한 뷰를 기대하는 경우**를
 *     잡는다. 두 대조는 목적이 다르다 — 전자는 발행자 버그, 후자는 수신자 배선 버그다.
 *  2. **인스턴스(V2)** — `(roadmap_version, view_id)` 가 `expected` 와 다르면
 *     @ref ConvertStatus::VIEW_SCOPE_MISMATCH 다. 노드는 MapRegistry 재조회로 회복한다.
 *
 * `expected` 는 대조 대상일 뿐 아니라 **랩(uint32 → 강타입)이 일어나는 id 공간의 선언**이다.
 * 그래서 메시지가 스코프를 싣지 않는 계약(`CommitState`·`SimInject`)에도 인자로 둔다 — 그
 * 경우 인스턴스 대조는 성립하지 않고 종류 대조와 랩 대상 명시만 남는다(각 함수 `@note` 참조).
 *
 * ## 발행 함수의 봉투 인자 (R-15 (e))
 * 발행 함수는 `header.stamp`·`roadmap_version`·`view_id`·`plan_epoch`·`event_id` 중
 * **그 메시지가 실제로 갖는 필드**를 인자로 받는다. 메시지에 자리가 없는 봉투는 받지 않는다.
 *  - `stamp` 는 도메인 표현인 `double` 초(시뮬 시계 절대시각)로 받고 변환은 **반드시**
 *    `seconds_to_time()` 을 거친다(`common_convert.hpp`). 가드가 9곳에 흩어지면 안 된다.
 *  - 스코프는 평면 두 값이 아니라 `mrs::ViewScope` 한 덩어리로 받는다. 종류까지 함께 검사해야
 *    "버전은 맞는데 뷰 종류가 틀린" 발행을 막을 수 있다.
 *
 * @note **파라미터 struct(`PublishEnvelope`)를 도입하지 않았다.** 봉투 필드 집합이 계약마다
 *       다르기 때문이다 — 예를 들어 `ExecutionWindow` 에는 `event_id` 자리가 없고
 *       `SimMetricSample` 에는 스코프 자리가 없다. 공통 struct 를 쓰면 호출자가 채운 값이
 *       **그 메시지에 자리가 없다는 이유로 조용히 버려지는** 경로가 생기며, 그것이 정확히 이
 *       패키지가 막으려는 실패 형태다. 명시 인자는 컴파일러가 계약별 봉투를 강제하게 한다.
 * @note 도메인 값 타입이 **이미 나르는** 봉투 값(예: `mrs::ExecutionWindow` 의
 *       `plan_epoch`·`roadmap_version`·`view_id`)은 인자로 중복해 받지 않는다. 같은 값에
 *       정본이 둘 생기면 불일치 시 어느 쪽을 믿을지 정할 근거가 계약 어디에도 없다.
 *
 * ## 유예 항목 (계약 U-18)
 * `mrs::RoadmapViewData` 에 뷰 종류 판별자가 없고 `mrs::RoadmapValidationResult` 가 뷰별 4배열로
 * 분리돼 있지 않다. 계약 U-18 이 "뷰 강타입 적용은 [1](D-11 MapRegistry) 시점"으로 유예했으므로
 * 도메인 헤더를 고치지 않고 **[0a] 는 현행 형태로** 둔다. 해당 함수에 `@note` 로 표시했다.
 */

#include <cstdint>
#include <vector>

#include "mrs/contract_types.hpp"
#include "mrs/view_ids.hpp"
#include "mrs/i_path_solver.hpp"        // PlanRequestEntry
#include "mrs/i_local_planner.hpp"      // NeighborTrajectory, LocalPlanOutput, AdoptionStatus
#include "mrs/i_judge_policy.hpp"       // JudgeObservation, JudgeDecision, GuardDecision
// RobotObservation, SimCapabilities, FaultInjection, SimMetricSample
#include "mrs/i_sim_backend.hpp"
#include "mrs/reorder_types.hpp"        // ReorderRequest, ReorderAck
#include "mrs_task_alloc/lifelong_task_allocator.hpp" // ReassignTrigger, ReassignResult
#include "mrs_metrics/metrics_collector.hpp"           // RungEventRecord
#include "mrs_map_registry/map_registry.hpp"           // RoadmapViewData, RoadmapValidationResult

#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_msg_convert/convert_result.hpp"

#include "geometry_msgs/msg/twist.hpp"
#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/msg/robot_path.hpp"
#include "mrs_interfaces/msg/task_assignment.hpp"
#include "mrs_interfaces/msg/commit_state.hpp"
#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_interfaces/msg/frozen_order.hpp"
#include "mrs_interfaces/msg/local_plan_share.hpp"
#include "mrs_interfaces/msg/judge_verdict.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/msg/observation_uncertainty.hpp"
#include "mrs_interfaces/msg/sim_capabilities.hpp"
#include "mrs_interfaces/msg/sim_metric_sample.hpp"
#include "mrs_interfaces/msg/roadmap_node.hpp"
#include "mrs_interfaces/msg/roadmap_edge.hpp"
#include "mrs_interfaces/msg/node_mapping.hpp"
#include "mrs_interfaces/srv/get_roadmap.hpp"
#include "mrs_interfaces/srv/reassign_request.hpp"
#include "mrs_interfaces/srv/partial_replan.hpp"
#include "mrs_interfaces/srv/plan_paths.hpp"
#include "mrs_interfaces/srv/reorder_passing.hpp"
#include "mrs_interfaces/srv/validate_map.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"

namespace mrs::convert
{

// ── ExecutionWindow (★ load-bearing seam, L-01) ─────────────────────────────
/**
 * @brief 도메인 ExecutionWindow 를 `ExecutionWindow.msg` 로 변환한다.
 *
 * 뷰 종류: `segments[].node_from/node_to` 와 `predecessor_constraints[].node_id` 는 전부
 * **UNIFORM** 이다(계약 §0.1 뷰 종류 정본표, `ExecutionWindow` 행). 인스턴스 스코프는 이
 * 메시지의 `roadmap_version` + `view_id` 평면 쌍이며(V2), 도메인 구조체가 그 쌍을 이미
 * 나르므로 봉투 인자로 받지 않는다.
 *
 * 필수 검사:
 *  - `schema_version` 을 @ref mrs::EXECUTION_WINDOW_SCHEMA_VERSION 으로 **스탬프**한다.
 *  - 스코프 자기 유효성 — `roadmap_version != 0`(`is_usable_scope`). 0 이 나가면 V3 를 지키는
 *    수신자가 100% 폐기한다.
 *  - 개정 불변식 — `NEW` 이면 `valid_through_segment_index == -1`,
 *    `TRUNCATE` 이면 `segments` 가 비어 있고 `valid_through_segment_index >= 0`.
 *    (`TRUNCATE` + 음수 인덱스는 `CommitStatus.committed_through_segment_index` 가 `uint32`
 *    이므로 불변식 W7 을 로봇 상태와 무관하게 무조건 위반한다.)
 *  - 노드 id 센티넬(4294967295) 미포함.
 *  - 시각 2건(`stamp_s`, `window_valid_until_s`)의 `seconds_to_time()` 가드.
 *
 * @param[in] window 도메인 실행 창. 자료형 `mrs::ExecutionWindow`. 노드 id 는 UNIFORM 강타입.
 * @param[in] stamp_s 릴리스 시각 [s]. 자료형 `double`. 시뮬 시계 기준 **절대시각**.
 *            `header.stamp` 가 된다.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::ExecutionWindow`.
 *             실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`. 스코프 미지정·개정 불변식 위반·센티넬 노드이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/robot_{0,1}/execution_window`).
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::ExecutionWindow & window, double stamp_s,
  mrs_interfaces::msg::ExecutionWindow & out);

/**
 * @brief `ExecutionWindow.msg` 를 도메인 ExecutionWindow 로 변환한다.
 *
 * 뷰 종류: **UNIFORM** 고정(계약 §0.1 정본표). 이 메시지는 `ViewScope` 를 싣지 않고
 * `roadmap_version` + `view_id` 평면 쌍만 실으므로(V2) 대조는 `match_scope_flat()` 으로 하며
 * 종류는 함수가 UNIFORM 을 공급한다.
 *
 * 필수 검사:
 *  - `schema_version == mrs::EXECUTION_WINDOW_SCHEMA_VERSION` — 불일치는 폐기.
 *  - 스코프 대조 — `match_scope_flat(msg.roadmap_version, msg.view_id, UNIFORM, expected)`.
 *  - `revision_kind ∈ {0, 1}` — 미지의 값을 `NEW` 로 폴백시키지 않는다.
 *  - 개정 불변식(발행 측 @ref to_msg 와 같은 조합)과 W7 유도 거부.
 *  - `window_valid_until` 의 `time_to_seconds()` 가드.
 *
 * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::ExecutionWindow`.
 * @param[in] expected 수신자가 현재 유효하다고 보는 뷰 스코프. 자료형 `mrs::ViewScope`.
 *            `view_kind` 는 UNIFORM 이어야 한다(그 밖이면 수신자 배선 오류다).
 * @param[out] out 변환된 도메인 실행 창. 자료형 `mrs::ExecutionWindow`.
 *             실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`.
 *         `schema_version` 불일치 @ref ConvertStatus::SCHEMA_VERSION_MISMATCH,
 *         종류 불일치 @ref ConvertStatus::VIEW_KIND_MISMATCH,
 *         인스턴스 불일치 @ref ConvertStatus::VIEW_SCOPE_MISMATCH,
 *         `revision_kind` 범위 밖 @ref ConvertStatus::ENUM_OUT_OF_RANGE,
 *         불변식·센티넬 위반 @ref ConvertStatus::FIELD_RANGE_VIOLATION,
 *         시각 가드 위반 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로.
 */
[[nodiscard]] ConvertResult from_msg(
  const mrs_interfaces::msg::ExecutionWindow & msg, const mrs::ViewScope & expected,
  mrs::ExecutionWindow & out);

// ── EscalationReport ─────────────────────────────────────────────────────
/**
 * @brief 에스컬레이션 사유·심각도를 담아 `EscalationReport.msg` 를 만든다.
 *
 * 뷰 종류: 이 메시지의 `blocking_node_id` 는 **UNIFORM** 이고 스코프는 v2.0.0 에서 신설된
 * `view_scope` 필드가 나른다(계약 §0.1 정본표, `EscalationReport` 행). 따라서 @p scope 의
 * `view_kind` 는 UNIFORM 이어야 한다.
 *
 * 필수 검사:
 *  - `is_valid_event_id(event_id)` — 불변식 E1. 0 이면 계측이 상관관계를 잃는다.
 *  - `severity` 는 @ref mrs::severity_of 의 결과를 그대로 쓴다. **if 문으로 재매핑하지 않는다** —
 *    L-04 매핑표의 코드 정본이 그 함수이고 두 곳에 두면 즉시 드리프트한다.
 *  - `reason` 이 도메인 열거 집합 안인지(맨 캐스트 금지).
 *  - `is_usable_scope(scope)` 이고 `scope.view_kind == UNIFORM`.
 *  - `stamp_s` 의 `seconds_to_time()` 가드.
 *
 * @param[in] robot_id 보고 로봇 id. 자료형 `mrs::RobotId`(`std::uint16_t`). 65535 금지.
 * @param[in] event_id 교란 상관 키. 자료형 `mrs::EventId`(`std::uint64_t`). 0 금지(E1).
 * @param[in] reason 도메인 에스컬레이션 사유. 자료형 `mrs::EscalationReason`.
 * @param[in] window_seq 보고 시점 수용 중이던 창 seq. 자료형 `std::uint32_t`.
 * @param[in] scope 이 보고가 쓰는 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM 고정.
 * @param[in] stamp_s 보고 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::EscalationReport`.
 *             실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`. `event_id == 0`·스코프 사용 불가이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 스코프 종류가 UNIFORM 이 아니면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, `reason` 이 열거 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/robot_{0,1}/escalation_report`).
 * @note 상세 7필드(`current_segment_index`·`blocking_robot_id`·`stalled_duration_s`·`pose`·
 *       `v_mps`·`omega_rps`·`blocking_node_id`)는 인자에 없어 기본값으로 나간다. 감사 항목 L5 의
 *       유예이며 [0a] l4 는 창 소진만 보고하므로 성립한다. **[2] 전 해소 대상**이다.
 */
[[nodiscard]] ConvertResult make_escalation_report(
  mrs::RobotId robot_id, mrs::EventId event_id, mrs::EscalationReason reason,
  std::uint32_t window_seq, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::EscalationReport & out);

/**
 * @brief `EscalationReport.msg` 의 `reason` 필드를 도메인 enum 으로 변환한다.
 *
 * 뷰 종류: 이 함수는 노드 id 를 꺼내지 않지만, 메시지의 `view_scope` 가 **UNIFORM** 이 아니면
 * 같은 메시지의 `blocking_node_id` 가 다른 id 공간의 값이라는 뜻이므로 **메시지 전체를 폐기**
 * 한다(계약 §0.1 정본표 + `ViewScope.msg` 수신 규약). 그래서 @p expected 를 받는다.
 *
 * 필수 검사:
 *  - 스코프 대조 — `from_msg(msg.view_scope, ...)` 후 `match_scope(received, expected)`.
 *  - `reason ∈ {0..4}` — 확정 집합. **미지의 값을 `BLOCKED`/`QP_INFEASIBLE` 로 흘리지 않는다.**
 *    `SYNC_LOST` 가 뭉개지면 D-08 라우팅이 계획 문제로 오인해 재계획을 오발동하고, 그 결과
 *    F2 발동률·F6 스래싱 계측이 오염된다(계약 §0.2 뭉갬 금지 규칙).
 *
 * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::EscalationReport`.
 * @param[in] expected 수신자의 현재 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out 변환된 사유. 자료형 `mrs::EscalationReason`. 실패 시 내용은 정의되지 않는다.
 * @return `ConvertResult` — 성공이면 `ok`. 종류 불일치 @ref ConvertStatus::VIEW_KIND_MISMATCH,
 *         인스턴스 불일치 @ref ConvertStatus::VIEW_SCOPE_MISMATCH,
 *         `reason`·`view_kind` 가 집합 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE,
 *         스코프가 사용 불가(지도 버전 0 등)이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note [0a] tracer bullet 경로 — `ladder_orchestrator` 가 수신·기록한다.
 * @note `severity` 를 `severity_of(reason)` 과 대조하지 **않는다**. 계약에 "수신자가 대조하고
 *       불일치 시 폐기하라"는 문언이 없으므로 근거 없는 폐기 사유를 추가하지 않는다.
 */
[[nodiscard]] ConvertResult reason_from_msg(
  const mrs_interfaces::msg::EscalationReport & msg, const mrs::ViewScope & expected,
  mrs::EscalationReason & out);

// ── PlannedPaths / RobotPath / PlanRequestEntry ──────────────────────────
/**
 * @brief 도메인 로봇 경로 목록을 `PlannedPaths.msg` 로 변환한다.
 *
 * 뷰 종류: `paths[].visits[].node_id` 는 **UNIFORM** 이다(계약 §0.1 정본표,
 * `TimedNodeVisit`/`RobotPath` 행). 이 메시지의 스코프는 `roadmap_version` + `view_id` 평면
 * 쌍이며(V2) 도메인 표현 `std::vector<mrs::RobotPath>` 에는 그 봉투가 없어 인자로 받는다.
 *
 * 필수 검사:
 *  - `is_usable_scope(scope)` 이고 `scope.view_kind == UNIFORM`.
 *  - 노드 id 센티넬 미포함, `arrival_time_s` 유한·비음(시각 가드).
 *  - `paths` 가 비어 있는 것은 **정당한 값**이다(`PlanPaths.srv` 가 빈 해를 허용) — 실패
 *    신호로 쓰지 않는다.
 *
 * @param[in] paths 로봇별 시각 부여 경로. 자료형 `std::vector<mrs::RobotPath>` (D-03 표준형).
 * @param[in] is_partial R3 부분 재계획 결과 여부. 자료형 `bool`. true = 영향 집합만 포함.
 * @param[in] event_id 이 계획을 유발한 사건의 상관 키. 자료형 `mrs::EventId`.
 *            **정기 계획이면 0 이 정당하다** — 이 필드에는 E1 을 적용하지 않는다.
 * @param[in] plan_epoch 계획 세대 번호. 자료형 `std::uint32_t`. 단조증가.
 * @param[in] scope 계획이 사용한 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM 고정.
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::PlannedPaths`.
 * @return `ConvertResult` — 성공이면 `ok`. 스코프 사용 불가·센티넬 노드이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 종류 불일치이면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult to_msg(
  const std::vector<mrs::RobotPath> & paths, bool is_partial, mrs::EventId event_id,
  std::uint32_t plan_epoch, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::PlannedPaths & out);

/**
 * @brief `PlannedPaths.msg` 를 도메인 로봇 경로 목록으로 변환한다.
 *
 * 뷰 종류: **UNIFORM**(계약 §0.1 정본표). 스코프는 평면 쌍이므로 `match_scope_flat()` 을 쓴다.
 *
 * 필수 검사:
 *  - 스코프 대조 — `match_scope_flat(msg.roadmap_version, msg.view_id, UNIFORM, expected)`.
 *  - `arrival_time` 의 `time_to_seconds()` 가드, 방문열의 시각 단조증가.
 *  - 노드 id 센티넬 미포함.
 *  - ⛔ `schema_version` 검사는 **하지 않는다** — 이 계약에 그 필드가 없다. 하는 척하는 코드를
 *    넣으면 방어선이 없는 상태로 감사를 통과한다.
 *
 * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::PlannedPaths`.
 * @param[in] expected 수신자의 현재 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out 변환된 경로 목록. 자료형 `std::vector<mrs::RobotPath>`.
 *             실패 시 내용은 정의되지 않으며 호출자는 사용해서는 안 된다.
 * @return `ConvertResult` — 성공이면 `ok`. 종류·인스턴스 불일치이면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH / @ref ConvertStatus::VIEW_SCOPE_MISMATCH,
 *         센티넬·단조성 위반이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION,
 *         시각 가드 위반이면 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult from_msg(
  const mrs_interfaces::msg::PlannedPaths & msg, const mrs::ViewScope & expected,
  std::vector<mrs::RobotPath> & out);

/**
 * @brief `PlanPaths.srv` 요청의 `PlanRequestEntry` 배열을 도메인 표현으로 변환한다.
 *
 * 뷰 종류: `entries[].start_node`/`goal_node` 는 **UNIFORM** 이다(계약 §0.1 정본표,
 * `PlanRequestEntry` 행). 스코프는 요청의 `roadmap_version` + `view_id` 평면 쌍이다.
 *
 * 필수 검사:
 *  - 스코프 대조 — 불일치는 서버가 `RESULT_STALE_VERSION` 으로 응답해야 하므로
 *    @ref ConvertStatus::VIEW_SCOPE_MISMATCH 로 **구별해** 반환한다.
 *  - `start_theta_rad` 유한이고 `[-pi, pi]`.
 *  - `start_node`/`goal_node` 센티넬 방어(기본 생성값이 곧 센티넬이므로 미기입이 그대로 온다).
 *  - `event_id` 는 **0 이 정당하다**(정기/신규 작업 도착) — E1 을 적용하지 않는다.
 *
 * @param[in] req 요청. 자료형 `mrs_interfaces::srv::PlanPaths::Request`.
 * @param[in] expected 서버가 보유한 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out 변환된 요청 항목 목록. 자료형 `std::vector<mrs::PlanRequestEntry>`.
 * @return `ConvertResult` — 성공이면 `ok`. 종류·인스턴스 불일치이면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH / @ref ConvertStatus::VIEW_SCOPE_MISMATCH,
 *         각도 범위·센티넬 위반이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @warning `PlanPaths.srv` 에는 **형식 위반 전용 응답 코드가 없다**. 호출자는 형식 위반을
 *          `RESULT_INFEASIBLE` 로 뭉개지 말 것 — 솔버가 해를 못 찾은 것과 요청이 틀린 것을
 *          같은 코드로 보내면 F2 계측이 오염된다.
 */
[[nodiscard]] ConvertResult from_request(
  const mrs_interfaces::srv::PlanPaths::Request & req, const mrs::ViewScope & expected,
  std::vector<mrs::PlanRequestEntry> & out);

// ── TaskAssignment ────────────────────────────────────────────────────────
/**
 * @brief 배정 결과로 `TaskAssignment.msg` 를 만든다.
 *
 * 뷰 종류: `pickup_node`/`delivery_node` 는 **PHYSICAL** 이다(계약 §0.1 정본표,
 * `TaskAssignment` 행 — 작업 엔드포인트는 물리 지도의 개념이다). 균일 뷰 노드를 넣으면
 * 강타입이 컴파일 단계에서 막는다. 스코프는 v2.0.0 신설 `view_scope` 필드가 나른다.
 *
 * 필수 검사:
 *  - `scope.view_kind == PHYSICAL` **이고** `scope.view_id == 0`(물리 뷰 규약, `is_usable_scope`).
 *  - `pickup_node`/`delivery_node` 센티넬 미포함.
 *  - `waiting_time_s` 유한·비음.
 *  - `stamp_s` 시각 가드.
 *  - ⚠ `robot_id == 65535` 는 **회수(unassign)로 유효**하다 — 오류로 처리하지 않는다.
 *
 * @param[in] task_id 작업 id. 자료형 `std::uint64_t`. lifelong 전 구간 유일.
 * @param[in] robot_id 배정 로봇 id. 자료형 `mrs::RobotId`. 65535 = 회수(유효값).
 * @param[in] pickup_node 픽업 노드. 자료형 `mrs::PhysicalNodeId`(물리 뷰 강타입).
 * @param[in] delivery_node 딜리버리 노드. 자료형 `mrs::PhysicalNodeId`.
 * @param[in] assignment_epoch 배정 세대 번호. 자료형 `std::uint32_t`. 단조증가.
 * @param[in] waiting_time_s 작업 도착 후 대기 시간 [s]. 자료형 `double`. 기아율 계측 원천.
 * @param[in] reassign_count 이 작업의 누적 재할당 횟수. 자료형 `std::uint32_t`.
 * @param[in] scope 노드 id 2건의 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 PHYSICAL 고정.
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::TaskAssignment`.
 * @return `ConvertResult` — 성공이면 `ok`. 스코프 종류가 PHYSICAL 이 아니면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, 스코프 사용 불가·센티넬·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult make_task_assignment(
  std::uint64_t task_id, mrs::RobotId robot_id, mrs::PhysicalNodeId pickup_node,
  mrs::PhysicalNodeId delivery_node, std::uint32_t assignment_epoch, double waiting_time_s,
  std::uint32_t reassign_count, const mrs::ViewScope & scope, double stamp_s,
  mrs_interfaces::msg::TaskAssignment & out);

// ── CommitState / CommitStatus / FrozenOrder ─────────────────────────────
/**
 * @brief 도메인 CommitState 를 `CommitState.msg` 로 변환한다 (`PartialReplan.srv` 요청 원소).
 *
 * 뷰 종류: `boundary_node` 는 **UNIFORM** 이다(계약 §0.1 정본표, `CommitState` 행). 이 메시지는
 * 스코프를 싣지 않으며 **담은 `PartialReplan` 요청의 쌍이 스코프**다(V2) — 그래서 이 함수는
 * 봉투 인자를 받지 않는다.
 *
 * 필수 검사:
 *  - `predicted_theta_rad` 유한이고 `[-pi, pi]`. **T2-R1 필수 필드**이므로 무단 클램프·기본값
 *    대입을 하지 않는다 — 조용한 치환은 회전 비용 추정을 경계에서 뒤집는다.
 *  - `theta` 는 **몸체점 q** 기준을 유지한다. NID 오프셋 L 을 가감하지 않는다(계약 §0).
 *  - `predicted_arrival_s` 의 `seconds_to_time()` 가드.
 *  - `boundary_node` 센티넬 미포함.
 *
 * @param[in] state 도메인 커밋 상태. 자료형 `mrs::CommitState`.
 * @param[out] out 채울 메시지. 자료형 `mrs_interfaces::msg::CommitState`.
 * @return `ConvertResult` — 성공이면 `ok`. 각도 범위·센티넬 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::CommitState & state, mrs_interfaces::msg::CommitState & out);

/**
 * @brief `CommitState.msg` 를 도메인 CommitState 로 변환한다.
 *
 * 뷰 종류: **UNIFORM**(계약 §0.1 정본표). @p expected 는 여기서 **랩이 일어나는 id 공간의
 * 선언**으로 쓰인다 — 이 메시지는 스코프를 싣지 않아 인스턴스 대조가 성립하지 않는다.
 * 인스턴스 대조는 이 원소를 담은 @ref from_request (`PartialReplan::Request`) 가 수행한다.
 *
 * 필수 검사:
 *  - `expected.view_kind == UNIFORM` 이고 `is_usable_scope(expected)`.
 *  - `predicted_theta_rad` 유한이고 `[-pi, pi]`.
 *  - `predicted_arrival` 의 `time_to_seconds()` 가드.
 *
 * @param[in] msg 원본 메시지. 자료형 `mrs_interfaces::msg::CommitState`.
 * @param[in] expected 이 원소를 담은 요청의 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out 변환된 커밋 상태. 자료형 `mrs::CommitState`.
 * @return `ConvertResult` — 성공이면 `ok`. `expected` 종류가 UNIFORM 이 아니면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, 각도 범위 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @warning 상위 요청이 실패를 흡수해서는 안 된다. 원소 하나가 실패하면 요청 **전체**를 거부한다 —
 *          `affected_robots` 와의 순서 일치(P1)가 깨지므로 부분 성공이 성립하지 않는다.
 */
[[nodiscard]] ConvertResult from_msg(
  const mrs_interfaces::msg::CommitState & msg, const mrs::ViewScope & expected,
  mrs::CommitState & out);

/**
 * @brief 도메인 FrozenOrder 목록을 `FrozenOrder.msg` 목록으로 변환한다.
 *
 * 뷰 종류: `predecessor_node_id`/`successor_node_id` 는 **UNIFORM** 이다(계약 §0.1 정본표,
 * `FrozenOrder` 행) — 릴리스된 창의 노드 공간과 같아야 한다. 스코프는 담은 `PartialReplan`
 * 요청이 나르므로 이 함수는 봉투 인자를 받지 않는다.
 *
 * 필수 검사:
 *  - 로봇 id 2건이 센티넬(65535)이 아니고 서로 다르다.
 *  - 노드 id 2건이 센티넬이 아니다.
 *
 * @param[in] orders 도메인 무역전 강제 목록. 자료형 `std::vector<mrs::FrozenOrder>`.
 *            **비어 있는 것은 유효**하다.
 * @param[out] out 변환된 메시지 목록. 자료형 `std::vector<mrs_interfaces::msg::FrozenOrder>`.
 * @return `ConvertResult` — 성공이면 `ok`. 센티넬·자기모순 쌍이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @warning **위반 항목만 빼고 나머지를 내보내지 말 것.** 부분 누락은 (C3) 무역전 강제를 조용히
 *          약화시켜 정리 T2.2 의 전제를 깬다. 실패하면 요청 자체를 만들지 않고 R3 승격을 포기한다.
 * @note 순환·자기모순(A→B ∧ B→A) 탐지는 이 함수의 소관이 아니다 — 순서 그래프 전역 판정이
 *       필요하며 L3 응답 채택 전(A5 acyclic·A6 no_reversal)에 수행한다.
 */
[[nodiscard]] ConvertResult to_msg(
  const std::vector<mrs::FrozenOrder> & orders,
  std::vector<mrs_interfaces::msg::FrozenOrder> & out);

/**
 * @brief L4 커밋 상태로 `CommitStatus.msg` 를 만든다 (T2-R6).
 *
 * 뷰 종류: `commit_boundary_node` 는 균일 뷰 노드라고 `.msg` 주석이 규정하나 **계약 §0.1
 * 정본표에 `CommitStatus` 행이 없다**. 정본표가 유일한 근거이므로 이 함수는 노드 id 를 인자로
 * 받지 않고 해당 필드를 채우지 않는다(감사 항목 U3 — contract-keeper 판정 대기).
 *
 * 필수 검사:
 *  - `qp_status` — 인자를 `mrs::AdoptionStatus` 강타입으로 받아 **`QP_TICK_JITTER`(값 2)가
 *    표현 자체로 불가능**하게 한다. 그 값은 v1.1.0 에서 발행 금지됐다.
 *  - `commit_hull_vertices` 각 정점의 각도 범위·유한성(`common_convert.hpp` 의 `to_msg(Pose2D)`).
 *    ⚠ 정점 배열이 **비어 있는 것은 유효**하다("커밋 없음/정지") — 오류로 처리하지 않는다.
 *  - `stamp_s` 시각 가드.
 *
 * @param[in] robot_id 보고 로봇 id. 자료형 `mrs::RobotId`.
 * @param[in] window_seq 현재 수용 중인 창 seq. 자료형 `std::uint32_t`.
 * @param[in] committed_through_segment_index 커밋 접두 끝 인덱스. 자료형 `std::uint32_t`.
 * @param[in] commit_hull_vertices 커밋 볼록포 정점열(map 프레임, CCW).
 *            자료형 `std::vector<mrs::Pose2D>`. 빈 배열은 유효값.
 * @param[in] qp_status 채택 종류·폴백 사유. 자료형 `mrs::AdoptionStatus`.
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::CommitStatus`.
 * @return `ConvertResult` — 성공이면 `ok`. 정점 각도 범위·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, `qp_status` 가 열거 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note 7필드(`commit_boundary_node`·`predicted_arrival`·`predicted_theta_rad`·
 *       `brake_distance_m`·`qp_solve_time_s`·`neighbor_traj_max_age_s`·
 *       `last_rejected_window_seq`)는 인자에 없어 기본값으로 나간다. 감사 항목 L4 의 유예이며
 *       `/robot_i/commit_status` 가 [0a] 최소 토픽에 없어 성립한다. **[2] 전 해소 대상**이다.
 */
[[nodiscard]] ConvertResult make_commit_status(
  mrs::RobotId robot_id, std::uint32_t window_seq, std::uint32_t committed_through_segment_index,
  const std::vector<mrs::Pose2D> & commit_hull_vertices, mrs::AdoptionStatus qp_status,
  double stamp_s, mrs_interfaces::msg::CommitStatus & out);

// ── LocalPlanShare <-> NeighborTrajectory (T4-R3) ────────────────────────
/**
 * @brief 채택 계획으로 `LocalPlanShare.msg` 를 만든다 (매 틱 발행, 정의 T4.1-(3)).
 *
 * 뷰 종류: 이 메시지에는 노드 id 필드가 **하나도 없다** — 스코프 인자도 없다.
 *
 * 필수 검사:
 *  - `num_segments >= 1`, `bernstein_degree >= 1`, `segment_duration_s` 유한·양수.
 *  - `control_points.size() == 2 * num_segments * (bernstein_degree + 1)`.
 *    비교는 `std::size_t` 로 승격해서 한다 — 두 인자가 `uint8` 이라 곱셈이 오버플로한다.
 *  - `control_points` 전 원소 유한(NaN/inf 금지).
 *  - `shift_ratio_u` 유한이고 개구간 `(0, 1)`.
 *  - `stamp_s` 시각 가드.
 *
 * @param[in] robot_id 발행 로봇 id. 자료형 `mrs::RobotId`.
 * @param[in] tick_seq 스텝 지수 h. 자료형 `std::uint32_t`. (A1) 합의·신선도 판정의 키.
 * @param[in] control_points 채택 계획 c^(h) 의 제어점. 자료형 `std::vector<double>`.
 *            평탄 (x,y) 배열이며 **오프셋점 z** 다(계약 §0 자세 기준점 규약의 유일한 예외).
 *            몸체점 q 로 바꾸지 말 것.
 * @param[in] num_segments 지평 세그먼트 수 H. 자료형 `std::uint8_t`.
 * @param[in] bernstein_degree Bernstein 차수 n. 자료형 `std::uint8_t`.
 * @param[in] segment_duration_s 세그먼트 길이 Δt [s]. 자료형 `double`. 양수.
 * @param[in] shift_ratio_u u = Δt_h/Δt. 자료형 `double`. 개구간 (0,1).
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::LocalPlanShare`.
 * @return `ConvertResult` — 성공이면 `ok`. 길이 불변식·범위·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/robot_{0,1}/local_plan_share`).
 * @warning **실패했다고 발행 자체를 거르면 안 된다.** 매 틱 발행이 정의 T4.1-(3)이고 빠지면
 *          이웃이 `fresh_j` 거짓으로 동반 감속한다. 노드 루프가 인증 폴백 계획으로 재구성해
 *          발행하는 경로를 가져야 한다(ros-builder 소관).
 */
[[nodiscard]] ConvertResult make_local_plan_share(
  mrs::RobotId robot_id, std::uint32_t tick_seq, const std::vector<double> & control_points,
  std::uint8_t num_segments, std::uint8_t bernstein_degree, double segment_duration_s,
  double shift_ratio_u, double stamp_s, mrs_interfaces::msg::LocalPlanShare & out);

/**
 * @brief `LocalPlanShare.msg` 를 이웃 궤적 도메인 표현으로 변환한다.
 *
 * 뷰 종류: 노드 id 필드가 없다 — `expected` 인자를 받지 않는다.
 *
 * 필수 검사: @ref make_local_plan_share 와 동일한 5건(세그먼트 수·차수 하한, 세그먼트 길이,
 * 제어점 길이 불변식, 원소 유한성, `shift_ratio_u` 범위).
 *
 * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::LocalPlanShare`.
 * @param[out] out 변환된 이웃 궤적. 자료형 `mrs::NeighborTrajectory`.
 * @return `ConvertResult` — 성공이면 `ok`. 길이 불변식·범위·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note [0a] tracer bullet 경로.
 * @warning ⛔ **`tick_seq` 로 폐기 판정을 하지 말 것.** 감소는 유실이 아니라 재동기이며,
 *          `fresh_j(h-1)` 판정은 도메인(L4) 몫이다.
 * @warning ⛔ **`shift_ratio_u` 를 정규화·클램프·기본값 대체하지 말 것.** 수신자가 자기 값과
 *          대조해 (A1) 을 판정하는데, 조용한 치환은 그 대조를 무력화한다. 범위 밖은 폐기다.
 * @note 폐기는 안전한 기본 거동이다 — 도메인이 `FALLBACK_NEIGHBOR_STALE` 로 흡수한다
 *       (정리 T4.3). 호출 노드의 폐기 카운터가 폴백률 계측과 연결돼야 한다.
 * @note `NeighborTrajectory` 에 `num_segments`·`bernstein_degree`·`segment_duration_s` 가 없어
 *       검사 후 버려진다(감사 항목 L8). `age_s` 도 현재 시각 인자가 없어 채우지 못한다(L9).
 *       둘 다 **[2] 전 해소 대상**이다 — 시프트 연산자가 세그먼트 경계를 못 찾으면 조용히 틀린
 *       이웃 계획을 계산한다.
 */
[[nodiscard]] ConvertResult from_msg(
  const mrs_interfaces::msg::LocalPlanShare & msg, mrs::NeighborTrajectory & out);

/**
 * @brief 국소 계획 출력의 속도 지령을 `geometry_msgs/Twist` 로 만든다 (R-15 (f2) 신규).
 *
 * 뷰 종류: 노드 id 를 다루지 않는다.
 *
 * 필수 검사:
 *  - `cmd_v_mps`·`cmd_omega_rps` **유한**. NaN/inf 지령은 구동기에 그대로 전달되면 정의되지
 *    않은 거동을 낳으므로 발행 직전 이 지점에서 막는다.
 *
 * @param[in] output 국소 계획 출력. 자료형 `mrs::LocalPlanOutput`. 몸체점 q 기준 지령이다.
 * @param[out] out 채울 발행 메시지. 자료형 `geometry_msgs::msg::Twist`.
 *             `linear.x` = 선속도 [m/s], `angular.z` = 각속도 [rad/s], 나머지 성분은 0.
 * @return `ConvertResult` — 성공이면 `ok`. 비유한 지령이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION (호출자는 정지 지령으로 폴백한다).
 * @note [0a] tracer bullet 경로 (`/robot_{0,1}/cmd_vel`).
 * @note `Twist` 는 `header` 가 없어 봉투 인자를 받지 않는다.
 */
[[nodiscard]] ConvertResult make_cmd_vel(
  const mrs::LocalPlanOutput & output, geometry_msgs::msg::Twist & out);

// ── JudgeVerdict ──────────────────────────────────────────────────────────
/**
 * @brief 판정 결과로 `JudgeVerdict.msg` 를 만든다 (D-06, R2↔R3 단일 경계).
 *
 * 뷰 종류: 노드 id 필드가 없다 — 스코프 인자도 없다.
 *
 * 필수 검사:
 *  - `is_valid_event_id(event_id)` — 불변식 E1.
 *  - `completion_excess_s`·`threshold_s`·`decision.phi_hat_s` 유한.
 *  - `decision.direct_affected` 의 로봇 id 센티넬 미포함.
 *  - `stamp_s` 시각 가드.
 *
 * @param[in] event_id 교란 상관 키. 자료형 `mrs::EventId`. 0 금지(E1).
 * @param[in] decision 판정 결과. 자료형 `mrs::JudgeDecision`
 *            (`promote_r3`·`direct_affected`·`phi_hat_s`).
 * @param[in] completion_excess_s 판정량 [s]. 자료형 `double`. 예상−계획 완료시각의 최대값.
 * @param[in] threshold_s 적용된 소프트 트리거 임계 [s]. 자료형 `double`.
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::JudgeVerdict`.
 * @return `ConvertResult` — 성공이면 `ok`. `event_id == 0`·비유한 값·센티넬 로봇이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @warning ⛔ R1(BTPG) 관련 필드를 이 메시지에 추가하지 말 것 — D-04 v2 의 btpg↛judge 독립성
 *          결정을 되돌리는 계약 위반이다.
 */
[[nodiscard]] ConvertResult make_judge_verdict(
  mrs::EventId event_id, const mrs::JudgeDecision & decision, double completion_excess_s,
  double threshold_s, double stamp_s, mrs_interfaces::msg::JudgeVerdict & out);

// ── RungEvent (계측, L-13) ────────────────────────────────────────────────
/**
 * @brief 계측 레코드로 `RungEvent.msg` 를 만든다.
 *
 * 뷰 종류: 노드 id 필드가 없다(`edge_set_fingerprint` 는 해시이지 id 가 아니다).
 *
 * 필수 검사:
 *  - `schema_version` 을 @ref mrs::RUNG_EVENT_SCHEMA_VERSION 으로 **스탬프**한다.
 *  - `is_valid_event_id(event_id)` — M1·M5 의 그룹 키다.
 *  - `rung ∈ {0..5}`, `transition ∈ {0..4}`, `trigger_kind ∈ {0,1}`. 도메인 레코드가
 *    `transition`·`trigger_kind` 를 맨 `std::uint8_t` 로 들고 있어 **범위 검사가 유일한 방어선**
 *    이다.
 *  - `phi_hat_before_s`·`duration_s` 유한.
 *  - `stamp_s` 시각 가드.
 *  - ⚠ `phi_hat_after_s` 의 **NaN 은 유효값**("미산출")이다 — 유한성 필터로 거르지 말 것.
 *
 * @param[in] record 도메인 계측 레코드. 자료형 `mrs::RungEventRecord`.
 * @param[in] stamp_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::RungEvent`.
 * @return `ConvertResult` — 성공이면 `ok`. `event_id == 0`·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 열거 범위 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/ladder/rung_event` — ladder 가 실제 발행하는 경우).
 * @note 이 메시지는 스스로 "스키마 드리프트 = 실험 무효"를 선언한다. 범위 밖 값을 관용하면
 *       F2 발동률·F6 스래싱 카운터가 조용히 오염된다.
 * @note `detail` 문자열은 도메인 레코드에 대응 필드가 없어 비운다(감사 항목 L7).
 *       `reorder_job_id` 상관의 유일한 통로이므로 **[2] 전 해소 대상**이다.
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::RungEventRecord & record, double stamp_s, mrs_interfaces::msg::RungEvent & out);

// ── RobotState / ObservationUncertainty ──────────────────────────────────
/**
 * @brief 시뮬 백엔드 관측을 `RobotState.msg` 로 변환한다.
 *
 * 뷰 종류: `occupied_node`/`next_node` 는 **UNIFORM** 이다(계약 §0.1 정본표,
 * `RobotState`/`RobotObservation` 행). 스코프는 도메인 `RobotObservation.view_scope` 가 이미
 * 나르므로 봉투 인자로 받지 않는다.
 *
 * 필수 검사:
 *  - `observation.view_scope.view_kind == UNIFORM` 이고 `is_usable_scope`.
 *  - `edge_progress ∈ [0, 1]`, `v_mps`·`omega_rps` 유한.
 *  - `body_pose` 의 각도 범위·유한성.
 *  - `stamp_s` 및 `uncertainty.observation_time_s` 시각 가드(두 시각은 다를 수 있다).
 *  - ⚠ 노드 id 센티넬(4294967295)은 **유효값**이다("엣지 위"/"미상") — 오류로 처리하지 않는다.
 *
 * @param[in] observation 도메인 관측. 자료형 `mrs::RobotObservation`
 *            (`ISimBackend::sense()` 결과). 자세는 **몸체점 q** 다.
 * @param[in] stamp_s 관측 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::RobotState`.
 * @return `ConvertResult` — 성공이면 `ok`. 스코프 종류가 UNIFORM 이 아니면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, 스코프 사용 불가·범위 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/robot_{0,1}/robot_state`).
 * @note 백엔드가 뷰를 모르면 노드 부착을 **하지 않고** 센티넬 + `edge_progress` 로 보고하는 것이
 *       계약이 정한 정상 경로다. 그 경우에도 `view_scope` 자체는 유효해야 한다.
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::RobotObservation & observation, double stamp_s,
  mrs_interfaces::msg::RobotState & out);

/**
 * @brief `RobotState.msg` 를 도메인 관측으로 변환한다.
 *
 * 뷰 종류: **UNIFORM**(계약 §0.1 정본표). 스코프는 `view_scope` 필드가 나르므로
 * `from_msg(msg.view_scope, ...)` 후 `match_scope(received, expected)` 로 대조한다.
 *
 * 필수 검사: @ref to_msg 와 같은 범위 검사 + 스코프 종류·인스턴스 대조.
 *
 * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::RobotState`.
 * @param[in] expected 수신자의 현재 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out 변환된 관측. 자료형 `mrs::RobotObservation`.
 * @return `ConvertResult` — 성공이면 `ok`. 종류·인스턴스 불일치이면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH / @ref ConvertStatus::VIEW_SCOPE_MISMATCH,
 *         `view_kind` 값이 집합 밖이면 @ref ConvertStatus::ENUM_OUT_OF_RANGE,
 *         범위 위반이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION,
 *         시각 가드 위반이면 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로. 폐기 시 호출자는 **마지막 관측을 유지**한다(안전 폴백).
 */
[[nodiscard]] ConvertResult from_msg(
  const mrs_interfaces::msg::RobotState & msg, const mrs::ViewScope & expected,
  mrs::RobotObservation & out);

// ── Sim 경계 (Capabilities / MetricSample / FaultInjection) ──────────────
/**
 * @brief 도메인 SimCapabilities 를 `SimCapabilities.msg` 로 변환한다 (Q-8).
 *
 * 뷰 종류: 노드 id 필드가 없다. 이 메시지에는 `header` 도 없어 봉투 인자를 받지 않는다.
 *
 * 필수 검사:
 *  - `physics_fidelity` 가 도메인 열거 집합 안 — 값이 틀리면 sim-runner 의
 *    `OPEN(pending-isaac)` 자동 태깅이 무너져 충실도 부족 지표가 연구 검증으로 승격된다.
 *  - `nominal_realtime_factor > 0`, `actuate_to_state_latency_s >= 0`, 둘 다 유한.
 *  - `backend_name` 이 비어 있지 않음.
 *
 * @param[in] caps 백엔드 자기신고 값. 자료형 `mrs::SimCapabilities`.
 * @param[out] out 채울 메시지. 자료형 `mrs_interfaces::msg::SimCapabilities`.
 * @return `ConvertResult` — 성공이면 `ok`. 빈 이름·범위 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 충실도 열거가 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE.
 * @note `backend_name` **허용 문자열 목록을 이 패키지에 두지 않는다**(R-15 (c3)). 화이트리스트는
 *       `mrs_sim_abstraction` 이 상수로 노출하고 여기서는 참조만 한다 — convert 코드 라인에
 *       백엔드 이름 리터럴을 두면 경계 검사(C2 누수)가 FAIL 하며, 그것은 검사기가 실제로
 *       동작한다는 방증이므로 검사기가 아니라 배치를 고친다.
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::SimCapabilities & caps, mrs_interfaces::msg::SimCapabilities & out);

/**
 * @brief 도메인 SimMetricSample 을 `SimMetricSample.msg` 로 변환한다.
 *
 * 뷰 종류: 노드 id 필드가 없다.
 *
 * 필수 검사:
 *  - `key` 가 비어 있지 않음.
 *  - `value` 가 NaN 이 아님.
 *  - `stamp_s` 시각 가드.
 *  - ⚠ `robot_id == 65535` 는 **전역 지표로 유효**하다 — 오류로 처리하지 않는다.
 *
 * @param[in] sample metrics tap 표본. 자료형 `mrs::SimMetricSample`.
 *            `pending_isaac` 이 `fidelity_tag`(TAG_VALID / TAG_OPEN_PENDING_ISAAC)로 매핑된다.
 * @param[in] stamp_s 표본 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
 * @param[out] out 채울 발행 메시지. 자료형 `mrs_interfaces::msg::SimMetricSample`.
 * @return `ConvertResult` — 성공이면 `ok`. 빈 키·NaN 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note [0a] tracer bullet 경로 (`/sim/metrics_tap`).
 * @note 태그가 틀리면 충실도 부족 지표가 연구 검증으로 승격되어 kill-gate 정직성이 깨진다.
 */
[[nodiscard]] ConvertResult to_msg(
  const mrs::SimMetricSample & sample, double stamp_s,
  mrs_interfaces::msg::SimMetricSample & out);

/**
 * @brief `SimInject.srv` 요청을 도메인 FaultInjection 으로 변환한다.
 *
 * 뷰 종류: `target_edge_id` 는 **PHYSICAL 엣지**다(계약 §0.1 정본표, `SimInject` 행 — 통로
 * 개폐는 물리 지도의 사건이다). 이 요청은 스코프를 싣지 않으므로 @p expected 는 **랩 대상
 * id 공간의 선언**으로만 쓰이며 인스턴스 대조는 성립하지 않는다.
 *
 * 필수 검사:
 *  - `expected.view_kind == PHYSICAL`.
 *  - `fault_kind ∈ {0..5}`.
 *  - `FAULT_COMM_DROP` 이면 `magnitude ∈ [0, 1]`(손실률).
 *  - `duration_s >= 0` 유한 — ⚠ **0 은 "해제 전까지"로 유효**하다.
 *  - `CORRIDOR_CLOSE`/`CORRIDOR_OPEN` 인데 `target_edge_id` 가 센티넬이면 거부.
 *
 * @param[in] req 요청. 자료형 `mrs_interfaces::srv::SimInject::Request`.
 * @param[in] expected 엣지 id 가 속한 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 PHYSICAL.
 * @param[out] out 변환된 결함 주입 요청. 자료형 `mrs::FaultInjection`.
 * @return `ConvertResult` — 성공이면 `ok`. `expected` 종류가 PHYSICAL 이 아니면
 *         @ref ConvertStatus::VIEW_KIND_MISMATCH, `fault_kind` 가 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, 대상·강도·지속시간 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note 호출자는 `ENUM_OUT_OF_RANGE`(지원하지 않는 종류)를 `RESULT_UNSUPPORTED`,
 *       `FIELD_RANGE_VIOLATION`(대상 부적합)을 `RESULT_BAD_TARGET` 으로 **구별해** 응답해야 한다.
 * @note 엣지 id 는 아직 강타입이 아니다(뷰 강타입화 범위는 노드 3종). [1](D-11)에서 재판정한다.
 */
[[nodiscard]] ConvertResult from_request(
  const mrs_interfaces::srv::SimInject::Request & req, const mrs::ViewScope & expected,
  mrs::FaultInjection & out);

// ── MapRegistry 3뷰 (D-11) ────────────────────────────────────────────────
/**
 * @brief 도메인 뷰 데이터를 `RoadmapNode.msg` 배열로 변환한다.
 *
 * 뷰 종류: **담은 응답이 고정**한다 — `GetRoadmap`→PHYSICAL / `GetUniformView`→UNIFORM /
 * `GetDependencySkeleton`→SKELETON(계약 §0.1 정본표 3행).
 *
 * 필수 검사:
 *  - `view.roadmap_version != 0`.
 *  - `node_id` 센티넬 미포함, 뷰 내 중복 없음.
 *  - `is_task_endpoint ⟹ is_endpoint`.
 *  - `x_m`·`y_m` 유한, `claim_radius_m` 유한·양수.
 *    ⚠ `claim_radius_m` 의 **산정식은 OPEN**(theory T1 O6)이므로 0·음수·비유한만 거부하고
 *    상한·하한 판정은 하지 않는다. 없는 기준으로 거르면 [0a] 실측 자체가 막힌다.
 *
 * @param[in] view 도메인 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 * @param[out] out 변환된 노드 배열. 자료형 `std::vector<mrs_interfaces::msg::RoadmapNode>`.
 * @return `ConvertResult` — 성공이면 `ok`. 지도 버전 미지정·센티넬·중복·불변식 위반·비유한
 *         값이면 @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note [0a] tracer bullet 경로 (`map_registry/GetRoadmap`).
 * @note **뷰 종류 대조(V1)를 수행하지 못한다** — `mrs::RoadmapViewData` 에 종류 판별자가 없어
 *       세 뷰가 같은 시그니처를 쓴다. 계약 U-18 유예를 적용해 [0a] 는 현행 형태로 두며
 *       `@note [1](D-11)에서 해소 예정`. 그때까지 "어느 뷰인가"는 호출 노드가 어느 서비스
 *       핸들러에서 부르는가로만 보장된다.
 */
[[nodiscard]] ConvertResult nodes_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapNode> & out);

/**
 * @brief 도메인 뷰 데이터를 `RoadmapEdge.msg` 배열로 변환한다.
 *
 * 뷰 종류: @ref nodes_to_msg 와 동일 규칙(담은 응답이 고정). 엣지 id 는 아직 강타입이 아니다.
 *
 * 필수 검사:
 *  - `view.roadmap_version != 0`.
 *  - `edge_id`·`node_a`·`node_b` 센티넬 미포함.
 *  - `length_m` 유한·양수, `corridor_width_m >= 0`, `speed_limit_mps >= 0`, 전부 유한.
 *
 * @param[in] view 도메인 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 * @param[out] out 변환된 엣지 배열. 자료형 `std::vector<mrs_interfaces::msg::RoadmapEdge>`.
 * @return `ConvertResult` — 성공이면 `ok`. 지도 버전 미지정·센티넬·범위 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note [0a] tracer bullet 경로.
 * @note 참조 무결성(`node_a`/`node_b` 가 `nodes[]` 안)·퇴화 엣지·자기루프·중복 엣지 검사는
 *       **ValidateMap 검사기 1·2 의 소관**이며 여기서 재구현하지 않는다.
 * @note 뷰 종류 판별자 부재는 @ref nodes_to_msg 와 동일 — [1](D-11)에서 해소 예정.
 */
[[nodiscard]] ConvertResult edges_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::RoadmapEdge> & out);

/**
 * @brief 도메인 뷰 대응표를 `NodeMapping.msg` 배열로 변환한다.
 *
 * 뷰 종류: 계약 §0.1 규칙 **V4 의 유일한 허용 예외**다 — 한 메시지에 두 종류가 공존한다.
 * `from_node_id` 는 **항상 UNIFORM**, `to_node_id` 는 **SKELETON**(collapse 용례),
 * `to_edge_id` 는 **PHYSICAL 엣지**(subdivision 용례)다(정본표 2행).
 *
 * 필수 검사:
 *  - `to_node_id` 와 `to_edge_id` 의 **배타성** — 정확히 하나만 유효하고 다른 하나는 센티넬.
 *  - `to_edge_id` 가 유효하면 `s ∈ [0, 1]` 이고 유한.
 *  - `from_node_id` 센티넬 미포함, 중복 없음(대응이 함수여야 한다).
 *  - **방향 역전 금지** — 세 필드가 전부 `std::uint32_t` 라 뒤바꿔도 컴파일된다. 뒤집히면
 *    D-11 왕복 무손실 테스트가 "항등 테스트인 척"하게 되므로 여기서 막아야 한다.
 *
 * @param[in] view 도메인 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 * @param[out] out 변환된 대응표 배열. 자료형 `std::vector<mrs_interfaces::msg::NodeMapping>`.
 * @return `ConvertResult` — 성공이면 `ok`. 배타성·범위·중복 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note `mrs::RoadmapViewData.mappings` 가 세분화(subdivision)와 collapse 를 **한 필드에** 담아
 *       어느 불변식을 걸지 결정할 판별자가 없다. 계약 U-18 유예를 적용해 [0a] 는 현행 형태로
 *       두며 `@note [1](D-11)에서 해소 예정`. 그때까지는 원소별 배타성으로만 용례를 유추한다.
 * @note 세분화 격자·collapse 왕복 무손실 검증은 **MapRegistry(C4)** 의 소관이다. 이 함수 밖에서
 *       세분화·collapse 를 재구현하지 않는다.
 */
[[nodiscard]] ConvertResult mappings_to_msg(
  const mrs::RoadmapViewData & view, std::vector<mrs_interfaces::msg::NodeMapping> & out);

/**
 * @brief 도메인 뷰 데이터로 `GetRoadmap.srv` 응답을 채운다 (R-15 (f2) 신규).
 *
 * 뷰 종류: 이 응답의 `nodes[].node_id`·`edges[]` 는 **PHYSICAL** 이다(계약 §0.1 정본표,
 * `GetRoadmap 응답` 행). 스코프는 응답의 `roadmap_version` 이며 물리 뷰의 `view_id` 는 항상 0 이다.
 *
 * 필수 검사:
 *  - `view.roadmap_version != 0`.
 *  - `view.view_id == 0`(물리 뷰 규약).
 *  - `known_version == view.roadmap_version` 이면 `RESULT_UNCHANGED` 를 채우고 `nodes`/`edges`
 *    를 **비운다**(대역폭 규약). 그 외에는 `RESULT_OK` 와 함께 @ref nodes_to_msg /
 *    @ref edges_to_msg 결과를 싣는다.
 *  - `frame_id` 는 항상 `"map"` 이다(계약 §0 좌표계 규약).
 *
 * @param[in] view 도메인 물리 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 * @param[in] known_version 클라이언트가 이미 캐시한 지도 버전. 자료형 `std::uint64_t`.
 *            0 = 캐시 없음(항상 전량 전송).
 * @param[out] resp 채울 응답. 자료형 `mrs_interfaces::srv::GetRoadmap::Response`.
 * @return `ConvertResult` — 성공이면 `ok`. 지도 버전 미지정·물리 뷰 규약 위반, 또는 하위
 *         @ref nodes_to_msg / @ref edges_to_msg 가 낸 실패 사유를 **그대로** 전달한다
 *         (주로 @ref ConvertStatus::FIELD_RANGE_VIOLATION).
 * @note [0a] tracer bullet 경로 — `map_registry/GetRoadmap` 은 [0a] 유일한 map_registry 서비스다.
 */
[[nodiscard]] ConvertResult fill_response(
  const mrs::RoadmapViewData & view, std::uint64_t known_version,
  mrs_interfaces::srv::GetRoadmap::Response & resp);

/**
 * @brief 도메인 검증 결과를 `ValidateMap.srv` 응답 필드로 변환해 채운다.
 *
 * 뷰 종류: 응답의 `violating_physical_*` 는 **PHYSICAL**, `violating_uniform_*` 는 **UNIFORM**
 * 이다(계약 §0.1 정본표, `ValidateMap 응답` 행).
 *
 * 필수 검사:
 *  - 4 bool 이 전부 true 일 때만 `RESULT_PASS`, 하나라도 false 면 `RESULT_FAIL`.
 *    ⚠ 채우지 않으면 실패가 `RESULT_PASS=0` 으로 나간다 — 기본값이 곧 오탐이다.
 *  - `roadmap_version` != 0, 응답 스코프는 요청과 같은 값을 에코한다.
 *  - @p view_id 가 0 이면 입도 검사를 하지 않았다는 뜻이므로 균일 뷰 위반 목록은 비어야 한다.
 *  - `required_min_width_m`·`derated_v_max_mps` 유한·비음.
 *
 * @param[in] result 도메인 검증 결과. 자료형 `mrs::RoadmapValidationResult`.
 * @param[in] roadmap_version 응답 스코프로 에코할 지도 버전. 자료형 `std::uint64_t`.
 *            요청의 값과 같아야 한다.
 * @param[in] view_id 입도 검사에 사용한 균일 뷰 id. 자료형 `std::uint32_t`. 0 = 미검사.
 * @param[out] resp 채울 응답. 자료형 `mrs_interfaces::srv::ValidateMap::Response`.
 * @return `ConvertResult` — 성공이면 `ok`. 지도 버전 미지정·뷰 규약 위반·비유한 값이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note **뷰별 4배열을 채우지 못한다.** 응답은 위반 목록을 뷰별 4배열로 분리해 요구하는데
 *       `mrs::RoadmapValidationResult` 는 뷰가 섞인 2배열(`violating_node_ids`·
 *       `violating_edge_ids`)만 갖고 있어 어느 원소가 어느 뷰인지 판별할 정보가 없다.
 *       계약 U-18 유예를 적용해 [0a] 는 현행 형태로 두며(위반 목록은 비운 채 bool 판정만 싣는다)
 *       `@note [1](D-11)에서 해소 예정`. 억지로 한 배열에 몰아넣으면 v2.0.0 major 변경으로
 *       분리한 목적(뷰 혼합 배열 제거)이 그대로 무효화된다.
 * @note `required_min_width_m`·`derated_v_max_mps` 를 이 함수가 **재계산하지 않는다** — T1 공식의
 *       소유자는 검사기 3이며 경계 계층이 도메인 출력을 재유도해 채점하는 배치는 소관 밖이다.
 */
[[nodiscard]] ConvertResult fill_response(
  const mrs::RoadmapValidationResult & result, std::uint64_t roadmap_version,
  std::uint32_t view_id, mrs_interfaces::srv::ValidateMap::Response & resp);

// ── ReassignRequest.srv <-> ReassignTrigger/ReassignResult (D-02 v2) ────
/**
 * @brief `ReassignRequest.srv` 요청의 `trigger_type` 을 도메인 enum 으로 변환한다.
 *
 * 뷰 종류: 노드 id 필드가 없다 — `expected` 인자를 받지 않는다.
 *
 * 필수 검사:
 *  - `trigger_type ∈ {0, 1}`.
 *  - `is_valid_event_id(req.event_id)` — 불변식 E1.
 *  - `last_failure_reason ∈ {0..4} ∪ {255}` — ⚠ **255 = "사유 없음"으로 유효**하다. 이 값은
 *    `EscalationReport.REASON_*` 와 **공유 계약**이므로 두 곳이 갈리면 D-02 분기표가 무효가 된다.
 *
 * @param[in] req 요청. 자료형 `mrs_interfaces::srv::ReassignRequest::Request`.
 * @param[out] out 변환된 트리거 종류. 자료형 `mrs::ReassignTrigger`.
 * @return `ConvertResult` — 성공이면 `ok`. `trigger_type`·`last_failure_reason` 이 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, `event_id == 0` 이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION. 호출자는 둘 다 `RESULT_REJECTED` 로 응답한다.
 */
[[nodiscard]] ConvertResult trigger_from_request(
  const mrs_interfaces::srv::ReassignRequest::Request & req, mrs::ReassignTrigger & out);

/**
 * @brief 도메인 처리 결과를 `ReassignRequest.srv` 응답 필드로 채운다.
 *
 * 뷰 종류: 노드 id 필드가 없다 — 봉투 인자도 없다(이 응답에는 `header` 가 없다).
 *
 * 필수 검사:
 *  - `result` 가 도메인 열거 집합 안.
 *  - `result == REASSIGNED` 이면 `new_robot_id != 65535`, **그 밖의 결과이면 65535**.
 *  - `result == COOLDOWN_ACTIVE` 일 때만 `earliest_retry_s` 를 읽어 `seconds_to_time()` 로
 *    채운다. 그 밖의 결과에서는 계약이 이 필드를 "유효하지 않음"으로 규정하므로 인자를
 *    읽지 않는다 — 읽으면 의미 없는 시각이 응답에 실린다.
 *
 * @param[in] result 도메인 처리 결과. 자료형 `mrs::ReassignResult`.
 * @param[in] new_robot_id 재배정된 로봇 id. 자료형 `mrs::RobotId`.
 *            `REASSIGNED` 가 아니면 65535 여야 한다.
 * @param[in] earliest_retry_s 재시도 가능 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
 *            `COOLDOWN_ACTIVE` 일 때만 읽힌다.
 * @param[out] resp 채울 응답. 자료형 `mrs_interfaces::srv::ReassignRequest::Response`.
 * @return `ConvertResult` — 성공이면 `ok`. `result` 가 열거 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, `new_robot_id` 조합 불변식 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION, 쿨다운 시각 가드 위반이면
 *         @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @note `message` 문자열은 채우지 않는다 — 사람이 읽을 진단용이며 계약이 기계 판정 금지를
 *       명시했다. 필요하면 호출 노드가 덧붙인다.
 */
[[nodiscard]] ConvertResult fill_response(
  mrs::ReassignResult result, mrs::RobotId new_robot_id, double earliest_retry_s,
  mrs_interfaces::srv::ReassignRequest::Response & resp);

// ── PartialReplan.srv (D-03 v3 splice) ───────────────────────────────────
/**
 * @brief `PartialReplan.srv` 요청의 `commit_states`/`frozen_orders` 를 도메인 표현으로 변환한다.
 *
 * 뷰 종류: `commit_states[].boundary_node` 와 `frozen_orders[].*` 는 전부 **UNIFORM** 이다
 * (계약 §0.1 정본표, `CommitState`·`FrozenOrder` 행). 스코프는 요청의 `roadmap_version` +
 * `view_id` 평면 쌍이며(V2) 이 함수가 원소들의 스코프 소유자다.
 *
 * 필수 검사:
 *  - **P1: `affected_robots.size() == commit_states.size()` 이고 순서가 일치**한다.
 *  - `affected_robots` 가 비어 있으면 거부.
 *  - `is_valid_event_id(req.event_id)` — 불변식 E1.
 *  - 스코프 대조 — `match_scope_flat(req.roadmap_version, req.view_id, UNIFORM, expected)`.
 *  - 원소별 검사는 @ref from_msg (`CommitState`) 및 FrozenOrder 규칙에 위임한다.
 *
 * @param[in] req 요청. 자료형 `mrs_interfaces::srv::PartialReplan::Request`.
 * @param[in] expected 서버가 보유한 뷰 스코프. 자료형 `mrs::ViewScope`. 종류는 UNIFORM.
 * @param[out] out_commit_states 변환된 커밋 상태 목록. 자료형 `std::vector<mrs::CommitState>`.
 *             실패 시 **비워진다**.
 * @param[out] out_frozen_orders 변환된 무역전 강제 목록. 자료형 `std::vector<mrs::FrozenOrder>`.
 *             실패 시 **비워진다**.
 * @return `ConvertResult` — 성공이면 `ok`.
 *         **스코프 불일치는 @ref ConvertStatus::VIEW_SCOPE_MISMATCH 로, 형식 위반(길이 불일치·
 *         빈 집합·`event_id == 0`·원소 범위)은 @ref ConvertStatus::FIELD_RANGE_VIOLATION 으로
 *         구별해 반환한다** — 계약이 전자를 `RESULT_STALE_VERSION`, 후자를
 *         `RESULT_REJECTED_BAD_REQUEST` 로 서로 다르게 응답하라고 요구하기 때문이다.
 *         뷰 종류가 다르면 @ref ConvertStatus::VIEW_KIND_MISMATCH,
 *         원소 시각 가드 위반이면 @ref ConvertStatus::TIME_CONVERSION_GUARD.
 * @warning **부분 성공을 만들지 말 것.** 원소 하나가 실패하면 요청 전체를 거부한다 — P1 의
 *          순서 일치가 깨지면 커밋 상태가 엉뚱한 로봇에 붙는다.
 * @note 봉투 3건(`base_plan_epoch`·`budget_s`·`affected_robots`)은 이 함수의 out 에 실리지
 *       않는다(감사 항목 L2). 호출 노드가 요청에서 직접 읽되 그 값들은 노드 id 가 아니므로
 *       V3 위반이 아니다. 도메인 봉투 타입 신설은 **[2] 전 해소 대상**이다.
 */
[[nodiscard]] ConvertResult from_request(
  const mrs_interfaces::srv::PartialReplan::Request & req, const mrs::ViewScope & expected,
  std::vector<mrs::CommitState> & out_commit_states,
  std::vector<mrs::FrozenOrder> & out_frozen_orders);

// ── ReorderPassing.srv <-> ReorderRequest/ReorderAck (Q-2 service 바인딩) ─
/**
 * @brief `ReorderPassing.srv` 요청을 도메인 ReorderRequest 로 변환한다.
 *
 * 뷰 종류: 노드 id 필드가 없다 — `expected` 인자를 받지 않는다.
 *
 * 필수 검사:
 *  - `tier ∈ {0, 1}`. 도메인 `ReorderRequest` 에 tier 필드가 없어 **검사만 하고 버린다**
 *    (아래 note 참조).
 *  - `is_valid_event_id(req.event_id)` — 불변식 E1.
 *  - `budget_s` 유한·양수.
 *  - `scope_robots` 의 로봇 id 센티넬 미포함. ⚠ **빈 집합은 "전역"으로 유효**하다.
 *  - ⛔ slack 관련 필드를 도입하지 말 것 — D-04 v2 의 btpg↛judge 독립성 결정을 되돌린다.
 *
 * @param[in] req 요청. 자료형 `mrs_interfaces::srv::ReorderPassing::Request`.
 * @param[out] out 변환된 도메인 요청. 자료형 `mrs::ReorderRequest`
 *             (IReorderR1/IReorderR2 공용).
 * @return `ConvertResult` — 성공이면 `ok`. `tier` 가 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, `event_id == 0`·예산·센티넬 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 * @note **`tier` 는 out 에 실리지 않는다.** inproc 바인딩에서는 어느 포트를 호출하는가가 곧
 *       tier 이므로 도메인 타입이 그 필드를 갖지 않는데, ROS 서비스 바인딩은 단일 엔드포인트라
 *       라우팅에 tier 가 필요하다. 라우팅용으로는 호출 노드가 `req.tier` 를 직접 읽는다 —
 *       노드 id 가 아니므로 V3 위반이 아니다. 두 바인딩의 "필드·의미론 동일" 요건과의 화해는
 *       미결(감사 항목 U12)이며 contract-keeper 판정 대상이다.
 */
[[nodiscard]] ConvertResult from_request(
  const mrs_interfaces::srv::ReorderPassing::Request & req, mrs::ReorderRequest & out);

/**
 * @brief 도메인 ReorderAck 를 `ReorderPassing.srv` 응답 필드로 채운다.
 *
 * 뷰 종류: 노드 id 필드가 없다 — 봉투 인자도 없다(이 응답에는 `header` 가 없다).
 *
 * 필수 검사:
 *  - `ack` 가 도메인 열거 집합 안.
 *  - **`ack == ACCEPTED` 이면 `job_id != 0`.** 0 이면 RungEvent `detail` 을 통한 상관이 끊겨
 *    "그 재조정이 무엇을 했는가"를 사후에 복원할 수 없다.
 *
 * @param[in] ack 도메인 접수 결과. 자료형 `mrs::ReorderAck`.
 * @param[in] job_id 접수 식별자. 자료형 `std::uint64_t`. `ACCEPTED` 일 때만 유효(0 아님).
 * @param[out] resp 채울 응답. 자료형 `mrs_interfaces::srv::ReorderPassing::Response`.
 * @return `ConvertResult` — 성공이면 `ok`. `ack` 가 열거 집합 밖이면
 *         @ref ConvertStatus::ENUM_OUT_OF_RANGE, `job_id` 불변식 위반이면
 *         @ref ConvertStatus::FIELD_RANGE_VIOLATION.
 */
[[nodiscard]] ConvertResult fill_response(
  mrs::ReorderAck ack, std::uint64_t job_id,
  mrs_interfaces::srv::ReorderPassing::Response & resp);

} // namespace mrs::convert
