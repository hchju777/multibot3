// test_convert_rejects.cpp — 잘못된 메시지가 **실제로 거부되고, 사유가 구별되는지** 지키는 벽.
//
// ## 왜 사유 구별이 벽인가
// "거부됐다"만으로는 부족하다. 사유별 폐기 카운터가 F2 발동률·F6 스래싱 판정의 입력이므로,
// 스코프 불일치가 스키마 불일치로 뭉개지거나 미지의 열거값이 기본값으로 폴백하면 계측이
// 조용히 거짓이 된다. 그래서 모든 케이스가 `ConvertStatus` 값까지 확인한다.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6)
// - 컴파일 단계의 뷰 강타입 벽: 부정 컴파일 테스트의 몫.
// - 실제 DDS 왕복에서 이 값들이 어떻게 실려 오는가: pysim 이상.
// - **QP feasibility · livelock · 물리 재현성: 판정 불가. isaac 필수(§5.3).**

#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "convert_test_fixtures.hpp"

using mrs::convert::ConvertResult;
using mrs::convert::ConvertStatus;
using mrs_test::make_control_points;
using mrs_test::make_observation;
using mrs_test::make_planned_paths;
using mrs_test::make_view;
using mrs_test::make_window;
using mrs_test::uniform_scope;

namespace
{

const double kNaN = std::numeric_limits<double>::quiet_NaN();

/**
 * @brief 유효한 계획을 메시지로 변환해 돌려준다 (수신 방향 부정 케이스의 출발점).
 * @return `mrs_interfaces::msg::PlannedPaths` — 전 필드가 유효한 계획 메시지.
 */
mrs_interfaces::msg::PlannedPaths valid_planned_paths_msg()
{
  mrs_interfaces::msg::PlannedPaths msg;
  const ConvertResult result =
    mrs::convert::to_msg(make_planned_paths(), false, 0, 11, uniform_scope(42, 5), 12.5, msg);
  EXPECT_TRUE(result.ok);
  return msg;
}

/**
 * @brief 유효한 실행 창을 메시지로 변환해 돌려준다 (수신 방향 부정 케이스의 출발점).
 * @return `mrs_interfaces::msg::ExecutionWindow` — 전 필드가 유효한 창 메시지.
 */
mrs_interfaces::msg::ExecutionWindow valid_window_msg()
{
  mrs_interfaces::msg::ExecutionWindow msg;
  const ConvertResult result = mrs::convert::to_msg(make_window(), 12.5, msg);
  EXPECT_TRUE(result.ok);
  return msg;
}

/**
 * @brief 유효한 관측을 메시지로 변환해 돌려준다.
 * @return `mrs_interfaces::msg::RobotState` — 전 필드가 유효한 상태 메시지.
 */
mrs_interfaces::msg::RobotState valid_state_msg()
{
  mrs_interfaces::msg::RobotState msg;
  const ConvertResult result = mrs::convert::to_msg(make_observation(), 12.5, msg);
  EXPECT_TRUE(result.ok);
  return msg;
}

} // namespace

// ── 뷰 종류 / 스코프 불일치 ─────────────────────────────────────────────────

// 기대 뷰 종류가 다르면 VIEW_KIND_MISMATCH — 스코프 불일치로 뭉개지면 안 된다.
TEST(ConvertRejects, ExecutionWindowRejectsWrongViewKind)
{
  const mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  mrs::ViewScope wrong_kind = uniform_scope(42, 5);
  wrong_kind.view_kind = mrs::ViewKind::SKELETON;

  mrs::ExecutionWindow out;
  const ConvertResult result = mrs::convert::from_msg(msg, wrong_kind, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// 같은 종류라도 지도 버전이 다르면 다른 id 공간이다.
TEST(ConvertRejects, ExecutionWindowRejectsWrongRoadmapVersion)
{
  const mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  mrs::ExecutionWindow out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(43, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_SCOPE_MISMATCH);
}

// view_id 가 다르면(재세분화) 역시 다른 id 공간이다.
TEST(ConvertRejects, ExecutionWindowRejectsWrongViewId)
{
  const mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  mrs::ExecutionWindow out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 6), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_SCOPE_MISMATCH);
}

TEST(ConvertRejects, RobotStateRejectsWrongViewKindAndWrongInstance)
{
  const mrs_interfaces::msg::RobotState msg = valid_state_msg();
  mrs::RobotObservation out;

  mrs::ViewScope physical;
  physical.view_kind = mrs::ViewKind::PHYSICAL;
  physical.roadmap_version = 42;
  physical.view_id = 0;
  EXPECT_EQ(mrs::convert::from_msg(msg, physical, out).reason, ConvertStatus::VIEW_KIND_MISMATCH);

  EXPECT_EQ(
    mrs::convert::from_msg(msg, uniform_scope(99, 5), out).reason,
    ConvertStatus::VIEW_SCOPE_MISMATCH);
}

// 발행 방향도 막는다 — 도메인 스코프 종류가 UNIFORM 이 아니면 애초에 내보내지 않는다.
TEST(ConvertRejects, RobotStatePublishFixesViewKindToUniform)
{
  mrs::RobotObservation bad = make_observation();
  bad.view_scope.view_kind = mrs::ViewKind::PHYSICAL;

  mrs_interfaces::msg::RobotState out_msg;
  const ConvertResult result = mrs::convert::to_msg(bad, 12.5, out_msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// 와이어의 view_kind 가 집합 밖(99)이면 ENUM_OUT_OF_RANGE — 기본값 치환 금지.
TEST(ConvertRejects, RobotStateRejectsOutOfRangeWireViewKind)
{
  mrs_interfaces::msg::RobotState msg = valid_state_msg();
  msg.view_scope.view_kind = 99;

  mrs::RobotObservation out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::ENUM_OUT_OF_RANGE);
}

// roadmap_version = 0 은 런타임 금지값이며 범위 위반으로 분류된다.
TEST(ConvertRejects, RobotStateRejectsUnspecifiedRoadmapVersion)
{
  mrs_interfaces::msg::RobotState msg = valid_state_msg();
  msg.view_scope.roadmap_version = 0;

  mrs::RobotObservation out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── 스키마 버전 ─────────────────────────────────────────────────────────────

// 스키마 검사가 스코프 검사보다 앞선다 — 사유가 덮이면 폐기 카운터가 잘못된 축으로 쌓인다.
TEST(ConvertRejects, ExecutionWindowSchemaMismatchIsReportedBeforeScopeChecks)
{
  mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  msg.schema_version = 99;

  mrs::ExecutionWindow out;
  // 스코프까지 함께 틀리게 주어도 사유는 SCHEMA_VERSION_MISMATCH 여야 한다.
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(43, 6), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::SCHEMA_VERSION_MISMATCH);
}

// ── 시각 가드 ───────────────────────────────────────────────────────────────

TEST(ConvertRejects, TimeGuardRejectsNegativeAndNanStamp)
{
  const mrs::ExecutionWindow window = make_window();
  mrs_interfaces::msg::ExecutionWindow msg;

  EXPECT_EQ(mrs::convert::to_msg(window, -1.0, msg).reason, ConvertStatus::TIME_CONVERSION_GUARD);
  EXPECT_EQ(mrs::convert::to_msg(window, kNaN, msg).reason, ConvertStatus::TIME_CONVERSION_GUARD);
}

TEST(ConvertRejects, TimeGuardRejectsNanWindowValidUntil)
{
  mrs::ExecutionWindow window = make_window();
  window.window_valid_until_s = kNaN;

  mrs_interfaces::msg::ExecutionWindow msg;
  EXPECT_EQ(mrs::convert::to_msg(window, 12.5, msg).reason, ConvertStatus::TIME_CONVERSION_GUARD);
}

// rosidl 이 막지 않는 위반: nanosec >= 1e9. 수신 측이 스스로 잡아야 한다.
TEST(ConvertRejects, TimeGuardRejectsWireNanosecAtOrAboveOneBillion)
{
  mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  msg.window_valid_until.nanosec = 1000000000U;

  mrs::ExecutionWindow out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::TIME_CONVERSION_GUARD);
}

TEST(ConvertRejects, TimeGuardRejectsNegativeObservationTime)
{
  mrs::RobotObservation observation = make_observation();
  observation.uncertainty.observation_time_s = -0.5;

  mrs_interfaces::msg::RobotState msg;
  EXPECT_EQ(
    mrs::convert::to_msg(observation, 12.5, msg).reason, ConvertStatus::TIME_CONVERSION_GUARD);
}

// ── EscalationReport 발행 가드 ──────────────────────────────────────────────

// 사유가 집합 밖(200)이면 거부하고 out 을 오염시키지 않는다.
TEST(ConvertRejects, EscalationReasonOutOfRangeIsRejectedWithoutTouchingOut)
{
  mrs_interfaces::msg::EscalationReport msg;
  ASSERT_TRUE(mrs::convert::make_escalation_report(
                1, 77, mrs::EscalationReason::SYNC_LOST, 7, uniform_scope(42, 5), 12.5, msg)
                .ok);
  msg.reason = 200;

  mrs::EscalationReason out = mrs::EscalationReason::BLOCKED;
  const ConvertResult result = mrs::convert::reason_from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::ENUM_OUT_OF_RANGE);
  EXPECT_EQ(out, mrs::EscalationReason::BLOCKED);
}

// 발행 시 스코프 종류를 UNIFORM 으로 고정한다(기본 생성 ViewScope 는 PHYSICAL).
TEST(ConvertRejects, EscalationReportPublishFixesScopeKind)
{
  mrs_interfaces::msg::EscalationReport msg;
  const ConvertResult result = mrs::convert::make_escalation_report(
    1, 77, mrs::EscalationReason::BLOCKED, 7, mrs::ViewScope{}, 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// 불변식 E1 — event_id = 0 은 미할당이며 발행이 금지된다(계측이 상관관계를 잃는다).
TEST(ConvertRejects, EscalationReportRejectsUnassignedEventId)
{
  mrs_interfaces::msg::EscalationReport msg;
  const ConvertResult result = mrs::convert::make_escalation_report(
    1, 0, mrs::EscalationReason::BLOCKED, 7, uniform_scope(42, 5), 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── LocalPlanShare 불변식 ───────────────────────────────────────────────────

// num_segments = 0 + 빈 배열은 길이 불변식을 "형식적으로" 만족하지만 통과하면 안 된다.
TEST(ConvertRejects, LocalPlanShareRejectsZeroSegments)
{
  mrs_interfaces::msg::LocalPlanShare msg;
  const ConvertResult result =
    mrs::convert::make_local_plan_share(1, 9, {}, 0, 5, 0.5, 0.2, 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 제어점에 NaN 이 섞이면 이웃의 반평면 구성이 무의미해진다.
TEST(ConvertRejects, LocalPlanShareRejectsNanControlPoint)
{
  std::vector<double> points = make_control_points(4, 5);
  points[3] = kNaN;

  mrs_interfaces::msg::LocalPlanShare msg;
  const ConvertResult result =
    mrs::convert::make_local_plan_share(1, 9, points, 4, 5, 0.5, 0.2, 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// shift_ratio_u 는 개구간 (0, 1) — 경계값 0 과 1 은 둘 다 거부다.
TEST(ConvertRejects, LocalPlanShareRejectsShiftRatioAtOpenIntervalBoundaries)
{
  const std::vector<double> points = make_control_points(4, 5);
  mrs_interfaces::msg::LocalPlanShare msg;

  EXPECT_EQ(
    mrs::convert::make_local_plan_share(1, 9, points, 4, 5, 0.5, 0.0, 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);
  EXPECT_EQ(
    mrs::convert::make_local_plan_share(1, 9, points, 4, 5, 0.5, 1.0, 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);
}

TEST(ConvertRejects, LocalPlanShareRejectsZeroSegmentDuration)
{
  const std::vector<double> points = make_control_points(4, 5);
  mrs_interfaces::msg::LocalPlanShare msg;
  EXPECT_EQ(
    mrs::convert::make_local_plan_share(1, 9, points, 4, 5, 0.0, 0.2, 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 제어점 길이 불변식(2*segments*(degree+1)) 위반 — 하나만 빠져도 거부.
TEST(ConvertRejects, LocalPlanShareRejectsControlPointLengthViolationOnReceive)
{
  mrs_interfaces::msg::LocalPlanShare msg;
  ASSERT_TRUE(
    mrs::convert::make_local_plan_share(1, 9, make_control_points(4, 5), 4, 5, 0.5, 0.2, 12.5, msg)
      .ok);
  msg.control_points.pop_back();

  mrs::NeighborTrajectory out;
  const ConvertResult result = mrs::convert::from_msg(msg, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// NaN 지령이 구동부로 나가는 것을 경계에서 막는다(OSQP 미수렴 시의 실제 사고 경로).
TEST(ConvertRejects, CmdVelRejectsNanCommand)
{
  mrs::LocalPlanOutput output;
  output.cmd_v_mps = 0.4;
  output.cmd_omega_rps = kNaN;

  geometry_msgs::msg::Twist twist;
  const ConvertResult result = mrs::convert::make_cmd_vel(output, twist);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── MapRegistry 발행 불변식 ─────────────────────────────────────────────────

TEST(ConvertRejects, RoadmapNodesRejectDuplicateNodeId)
{
  mrs::RoadmapViewData view = make_view();
  view.nodes[1].node_id = view.nodes[0].node_id;

  std::vector<mrs_interfaces::msg::RoadmapNode> out(3);
  const ConvertResult result = mrs::convert::nodes_to_msg(view, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// is_task_endpoint ⟹ is_endpoint — 함의가 깨진 노드는 발행하지 않는다.
TEST(ConvertRejects, RoadmapNodesRejectTaskEndpointThatIsNotEndpoint)
{
  mrs::RoadmapViewData view = make_view();
  view.nodes[0].is_endpoint = false; // is_task_endpoint 는 여전히 true

  std::vector<mrs_interfaces::msg::RoadmapNode> out(3);
  const ConvertResult result = mrs::convert::nodes_to_msg(view, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

TEST(ConvertRejects, RoadmapNodesRejectZeroClaimRadius)
{
  mrs::RoadmapViewData view = make_view();
  view.nodes[0].claim_radius_m = 0.0;

  std::vector<mrs_interfaces::msg::RoadmapNode> out(3);
  EXPECT_EQ(mrs::convert::nodes_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

TEST(ConvertRejects, RoadmapNodesRejectUnspecifiedRoadmapVersion)
{
  mrs::RoadmapViewData view = make_view();
  view.roadmap_version = 0;

  std::vector<mrs_interfaces::msg::RoadmapNode> out(3);
  EXPECT_EQ(mrs::convert::nodes_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 물리 뷰인데 view_id != 0 — ViewScope.msg 규약 위반.
TEST(ConvertRejects, GetRoadmapRejectsPhysicalViewWithNonZeroViewId)
{
  mrs::RoadmapViewData view = make_view();
  view.view_id = 3;

  mrs_interfaces::srv::GetRoadmap::Response response;
  const ConvertResult result = mrs::convert::fill_response(view, 0, response);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 길이 0 엣지(퇴화) 는 발행 금지 — nav2 실배포 그래프에서 실제로 관측된 결함 유형이다.
TEST(ConvertRejects, RoadmapEdgesRejectZeroLength)
{
  mrs::RoadmapViewData view = make_view();
  view.edges[0].length_m = 0.0;

  std::vector<mrs_interfaces::msg::RoadmapEdge> out(5);
  EXPECT_EQ(mrs::convert::edges_to_msg(view, out).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── RungEvent / SimMetricSample ─────────────────────────────────────────────

namespace
{

/**
 * @brief 전 필드가 유효한 가로대 사건 기록을 만든다.
 * @return `mrs::RungEventRecord` — `phi_hat_after_s` 는 NaN(미산출, 유효값)이다.
 */
mrs::RungEventRecord make_rung_event()
{
  mrs::RungEventRecord record;
  record.event_id = 77;
  record.rung = mrs::Rung::R2_MILP;
  record.transition = 2;   // ESCALATE
  record.trigger_kind = 1; // SOFT
  record.affected_robots = {1, 2};
  record.edge_set_fingerprint = 0xABCDEF;
  record.phi_hat_before_s = 3.5;
  record.phi_hat_after_s = kNaN;
  record.attempt_count = 2;
  record.duration_s = 0.25;
  return record;
}

} // namespace

// 미산출 slack(phi_hat_after)은 NaN 이 정상값이다 — 여기서 막으면 R2 미실행 사건을 못 싣는다.
TEST(RungEventConvert, AcceptsNanPhiHatAfterAsNotComputed)
{
  mrs_interfaces::msg::RungEvent msg;
  ASSERT_TRUE(mrs::convert::to_msg(make_rung_event(), 12.5, msg).ok);

  EXPECT_EQ(msg.schema_version, mrs::RUNG_EVENT_SCHEMA_VERSION);
  EXPECT_EQ(msg.rung, 2U);         // R2_MILP
  EXPECT_EQ(msg.transition, 2U);   // ESCALATE
  EXPECT_EQ(msg.trigger_kind, 1U); // SOFT
  EXPECT_TRUE(std::isnan(msg.phi_hat_after_s));
}

// 그러나 phi_hat_before 의 NaN 은 위반이다 — 사건 진입 시점의 slack 은 반드시 산출돼 있다.
TEST(RungEventConvert, RejectsNanPhiHatBefore)
{
  mrs::RungEventRecord record = make_rung_event();
  record.phi_hat_before_s = kNaN;

  mrs_interfaces::msg::RungEvent msg;
  EXPECT_EQ(mrs::convert::to_msg(record, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

TEST(RungEventConvert, RejectsOutOfRangeTransition)
{
  mrs::RungEventRecord record = make_rung_event();
  record.transition = 9;

  mrs_interfaces::msg::RungEvent msg;
  EXPECT_EQ(mrs::convert::to_msg(record, 12.5, msg).reason, ConvertStatus::ENUM_OUT_OF_RANGE);
}

TEST(RungEventConvert, RejectsUnassignedEventId)
{
  mrs::RungEventRecord record = make_rung_event();
  record.event_id = 0;

  mrs_interfaces::msg::RungEvent msg;
  EXPECT_EQ(mrs::convert::to_msg(record, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// pending_isaac 플래그가 fidelity_tag 로 정직하게 옮겨진다 — 이 태그가 무너지면
// unit/pysim 수치가 연구 검증으로 조용히 승격된다(§5.3).
TEST(SimMetricSampleConvert, PendingIsaacFlagMapsToFidelityTag)
{
  mrs::SimMetricSample sample;
  sample.key = "collision_count";
  sample.value = 3.0;
  sample.robot_id = mrs::ROBOT_ID_NONE; // 전역 지표 — 유효
  sample.pending_isaac = true;

  mrs_interfaces::msg::SimMetricSample msg;
  ASSERT_TRUE(mrs::convert::to_msg(sample, 12.5, msg).ok);
  EXPECT_EQ(msg.fidelity_tag, mrs_interfaces::msg::SimMetricSample::TAG_OPEN_PENDING_ISAAC);

  sample.pending_isaac = false;
  ASSERT_TRUE(mrs::convert::to_msg(sample, 12.5, msg).ok);
  EXPECT_EQ(msg.fidelity_tag, mrs_interfaces::msg::SimMetricSample::TAG_VALID);
}

TEST(SimMetricSampleConvert, RejectsEmptyKeyAndNanValue)
{
  mrs::SimMetricSample sample;
  sample.key = "collision_count";
  sample.value = 3.0;
  sample.robot_id = mrs::ROBOT_ID_NONE;
  sample.pending_isaac = false;

  mrs_interfaces::msg::SimMetricSample msg;

  mrs::SimMetricSample empty_key = sample;
  empty_key.key.clear();
  EXPECT_EQ(
    mrs::convert::to_msg(empty_key, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);

  mrs::SimMetricSample nan_value = sample;
  nan_value.value = kNaN;
  EXPECT_EQ(
    mrs::convert::to_msg(nan_value, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// ── 개정 불변식 (W7 파생) ───────────────────────────────────────────────────

// TRUNCATE 인데 valid_through_segment_index 가 음수면 무엇까지 유효한지 말하지 못한다.
TEST(RevisionInvariants, TruncateRequiresNonNegativeValidThroughIndex)
{
  mrs::ExecutionWindow window = make_window();
  window.revision_kind = mrs::RevisionKind::TRUNCATE;
  window.valid_through_segment_index = -1;
  window.segments.clear();

  mrs_interfaces::msg::ExecutionWindow msg;
  EXPECT_EQ(mrs::convert::to_msg(window, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// TRUNCATE 는 부분 폐기 **통지**이므로 세그먼트를 실어 보내지 않는다.
TEST(RevisionInvariants, TruncateRequiresEmptySegments)
{
  mrs::ExecutionWindow window = make_window();
  window.revision_kind = mrs::RevisionKind::TRUNCATE;
  window.valid_through_segment_index = 1; // 세그먼트가 비어 있지 않다

  mrs_interfaces::msg::ExecutionWindow msg;
  EXPECT_EQ(mrs::convert::to_msg(window, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// NEW 는 전 구간 유효이므로 index 는 반드시 -1 이다.
TEST(RevisionInvariants, NewRequiresValidThroughIndexOfMinusOne)
{
  mrs::ExecutionWindow window = make_window();
  window.valid_through_segment_index = 3;

  mrs_interfaces::msg::ExecutionWindow msg;
  EXPECT_EQ(mrs::convert::to_msg(window, 12.5, msg).reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 미지의 revision_kind 를 NEW 로 폴백하면 부분 폐기가 새 창으로 오독된다 — 반드시 거부.
TEST(RevisionInvariants, UnknownRevisionKindIsRejectedNotFallenBackToNew)
{
  mrs_interfaces::msg::ExecutionWindow msg = valid_window_msg();
  msg.revision_kind = 7;

  mrs::ExecutionWindow out;
  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::ENUM_OUT_OF_RANGE);
}

// ── PlannedPaths (L-09) ─────────────────────────────────────────────────────
//
// `pp → /planned_paths → sadg` 는 [0a] 에서 변환 스텁 탓에 한 번도 흐르지 않았다. 아래 6건은
// 그 경로에 방어선이 실제로 서 있는지를 지킨다.

// 발행 측이 물리·골격 스코프를 넘기면 거부한다. 이 메시지는 종류를 **와이어에 싣지 않으므로**
// (평면 쌍만 실린다) 여기서 막지 않으면 수신자는 뷰가 뒤바뀐 사실을 영영 알 수 없다.
TEST(ConvertRejects, PlannedPathsPublishRejectsNonUniformScopeKind)
{
  mrs::ViewScope physical = uniform_scope(42, 0);
  physical.view_kind = mrs::ViewKind::PHYSICAL;

  mrs_interfaces::msg::PlannedPaths msg;
  const ConvertResult result =
    mrs::convert::to_msg(make_planned_paths(), false, 0, 11, physical, 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// 지도 버전 0 은 계약이 런타임에 금지한다 — 내보내면 수신자가 100% 폐기한다.
TEST(ConvertRejects, PlannedPathsPublishRejectsUnusableScope)
{
  mrs_interfaces::msg::PlannedPaths msg;
  const ConvertResult result =
    mrs::convert::to_msg(make_planned_paths(), false, 0, 11, uniform_scope(0, 5), 12.5, msg);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 수신: 인스턴스 스코프 불일치는 VIEW_SCOPE_MISMATCH 다(재조회로 회복 가능한 상황).
// 종류 불일치와 뭉개지면 노드가 회복 방법을 고를 수 없다.
TEST(ConvertRejects, PlannedPathsReceiveDistinguishesScopeFromKindMismatch)
{
  const mrs_interfaces::msg::PlannedPaths msg = valid_planned_paths_msg();

  std::vector<mrs::RobotPath> out;
  EXPECT_EQ(
    mrs::convert::from_msg(msg, uniform_scope(43, 5), out).reason,
    ConvertStatus::VIEW_SCOPE_MISMATCH);
  EXPECT_EQ(
    mrs::convert::from_msg(msg, uniform_scope(42, 6), out).reason,
    ConvertStatus::VIEW_SCOPE_MISMATCH);

  mrs::ViewScope skeleton = uniform_scope(42, 5);
  skeleton.view_kind = mrs::ViewKind::SKELETON;
  EXPECT_EQ(mrs::convert::from_msg(msg, skeleton, out).reason, ConvertStatus::VIEW_KIND_MISMATCH);
}

// 센티넬 노드가 실린 방문은 해석할 수 없다 — 양방향 모두 거부.
TEST(ConvertRejects, PlannedPathsRejectSentinelNodeInBothDirections)
{
  std::vector<mrs::RobotPath> paths = make_planned_paths();
  paths[0].visits[1].node_id = mrs::UNIFORM_NODE_ID_NONE;

  mrs_interfaces::msg::PlannedPaths msg;
  EXPECT_EQ(
    mrs::convert::to_msg(paths, false, 0, 11, uniform_scope(42, 5), 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);

  mrs_interfaces::msg::PlannedPaths wire = valid_planned_paths_msg();
  wire.paths[0].visits[1].node_id = 4294967295U;
  std::vector<mrs::RobotPath> out;
  EXPECT_EQ(
    mrs::convert::from_msg(wire, uniform_scope(42, 5), out).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 시각 단조증가는 `RobotPath.msg` 의 불변식이다. **발행 측도 검사**해야 한다 — 한쪽만 막으면
// "발행은 되는데 수신자가 전량 폐기"하는 침묵 실패가 생긴다. 등호(같은 시각)도 위반이다.
TEST(ConvertRejects, PlannedPathsRejectNonMonotonicArrivalsInBothDirections)
{
  std::vector<mrs::RobotPath> backwards = make_planned_paths();
  backwards[1].visits[2].arrival_time_s = backwards[1].visits[1].arrival_time_s - 0.5;

  mrs_interfaces::msg::PlannedPaths msg;
  EXPECT_EQ(
    mrs::convert::to_msg(backwards, false, 0, 11, uniform_scope(42, 5), 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);

  std::vector<mrs::RobotPath> equal_times = make_planned_paths();
  equal_times[0].visits[1].arrival_time_s = equal_times[0].visits[0].arrival_time_s;
  EXPECT_EQ(
    mrs::convert::to_msg(equal_times, false, 0, 11, uniform_scope(42, 5), 12.5, msg).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);

  mrs_interfaces::msg::PlannedPaths wire = valid_planned_paths_msg();
  wire.paths[1].visits[2].arrival_time = wire.paths[1].visits[0].arrival_time;
  std::vector<mrs::RobotPath> out;
  EXPECT_EQ(
    mrs::convert::from_msg(wire, uniform_scope(42, 5), out).reason,
    ConvertStatus::FIELD_RANGE_VIOLATION);
}

// 시각 가드는 사유가 FIELD_RANGE_VIOLATION 과 **구별**되어야 한다 — 폐기 카운터가 "잘못된 계획"과
// "시계 고장"을 나눠 세지 못하면 사후에 원인을 복원할 수 없다.
TEST(ConvertRejects, PlannedPathsTimeGuardIsDistinctFromFieldRange)
{
  std::vector<mrs::RobotPath> negative = make_planned_paths();
  negative[0].visits[0].arrival_time_s = -1.0;
  negative[0].visits[1].arrival_time_s = 0.5; // 단조증가는 유지 — 시각 가드만 걸리게 한다
  negative[0].visits[2].arrival_time_s = 1.0;

  mrs_interfaces::msg::PlannedPaths msg;
  EXPECT_EQ(
    mrs::convert::to_msg(negative, false, 0, 11, uniform_scope(42, 5), 12.5, msg).reason,
    ConvertStatus::TIME_CONVERSION_GUARD);

  mrs_interfaces::msg::PlannedPaths wire = valid_planned_paths_msg();
  wire.paths[0].visits[0].arrival_time.nanosec = 1000000000U; // rosidl 은 이것을 막지 않는다
  std::vector<mrs::RobotPath> out;
  EXPECT_EQ(
    mrs::convert::from_msg(wire, uniform_scope(42, 5), out).reason,
    ConvertStatus::TIME_CONVERSION_GUARD);
}
