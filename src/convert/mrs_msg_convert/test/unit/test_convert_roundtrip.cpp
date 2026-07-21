// test_convert_roundtrip.cpp — 계약 경계 변환의 **왕복 항등**을 지키는 벽.
//
// ## 이 파일이 지키는 것
// `from_msg(to_msg(x)) == x` — 도메인이 나르는 필드가 하나도 소실·왜곡되지 않는다. 창 개정
// 조합(NEW / TRUNCATE)과 센티넬을 포함한 관측, 제어점 배열까지 전부 포함한다.
//
// 원본은 손으로 채운 리터럴에서만 나온다(`convert_test_fixtures.hpp`). 변환 결과를 다시 변환해
// 기대값으로 삼으면 항등 테스트가 자기 자신을 증명하게 되므로 그 형태를 쓰지 않는다.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6)
// - 실제 DDS 직렬화 왕복: 여기서는 메시지 **객체**만 오간다. 와이어 왕복은 pysim 이상.
// - 강타입이 컴파일 단계에서 뷰 혼입을 막는가: 부정 컴파일 테스트의 몫.
// - **QP feasibility · livelock · 물리 재현성: 판정 불가. isaac 필수(§5.3).**

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "convert_test_fixtures.hpp"

using mrs_test::make_control_points;
using mrs_test::make_observation;
using mrs_test::make_window;
using mrs_test::same_observation;
using mrs_test::same_window;
using mrs_test::uniform_scope;

// 창 왕복 항등 + 발행 시 스탬프·frame_id·schema_version 이 계약대로 찍힌다.
TEST(ExecutionWindowConvert, RoundTripPreservesAllFields)
{
  const mrs::ExecutionWindow original = make_window();

  mrs_interfaces::msg::ExecutionWindow msg;
  ASSERT_TRUE(mrs::convert::to_msg(original, 12.5, msg).ok);

  EXPECT_EQ(msg.schema_version, mrs::EXECUTION_WINDOW_SCHEMA_VERSION);
  EXPECT_EQ(msg.header.frame_id, "map");
  // 12.5 s → sec 12, nanosec 500000000 (손으로 계산한 기대값)
  EXPECT_EQ(msg.header.stamp.sec, 12);
  EXPECT_EQ(msg.header.stamp.nanosec, 500000000U);

  mrs::ExecutionWindow back;
  ASSERT_TRUE(mrs::convert::from_msg(msg, uniform_scope(42, 5), back).ok);
  EXPECT_TRUE(same_window(original, back));
}

// TRUNCATE 조합(세그먼트 없음 + valid_through_segment_index = 1)도 그대로 왕복해야 한다.
// 개정 종류가 왕복에서 NEW 로 접히면 접합 시 부분 폐기가 전체 폐기로 바뀐다.
TEST(ExecutionWindowConvert, TruncateRevisionRoundTrips)
{
  mrs::ExecutionWindow truncate = make_window();
  truncate.segments.clear();
  truncate.revision_kind = mrs::RevisionKind::TRUNCATE;
  truncate.valid_through_segment_index = 1;

  mrs_interfaces::msg::ExecutionWindow msg;
  ASSERT_TRUE(mrs::convert::to_msg(truncate, 12.5, msg).ok);

  mrs::ExecutionWindow back;
  ASSERT_TRUE(mrs::convert::from_msg(msg, uniform_scope(42, 5), back).ok);
  EXPECT_TRUE(same_window(truncate, back));
  EXPECT_EQ(back.revision_kind, mrs::RevisionKind::TRUNCATE);
  EXPECT_EQ(back.valid_through_segment_index, 1);
}

// 관측 왕복 항등 — 센티넬 next_node("미상")가 유효값으로 보존되는지가 핵심이다.
TEST(RobotStateConvert, RoundTripPreservesAllFieldsIncludingSentinelNextNode)
{
  const mrs::RobotObservation original = make_observation();
  ASSERT_TRUE(original.next_node.is_none());

  mrs_interfaces::msg::RobotState msg;
  ASSERT_TRUE(mrs::convert::to_msg(original, 12.5, msg).ok);

  mrs::RobotObservation back;
  ASSERT_TRUE(mrs::convert::from_msg(msg, uniform_scope(42, 5), back).ok);
  EXPECT_TRUE(same_observation(original, back));
  EXPECT_TRUE(back.next_node.is_none());
}

// LocalPlanShare 왕복 — 제어점 개수 불변식 2*4*(5+1) = 48 은 손으로 계산한 값이다.
TEST(LocalPlanShareConvert, RoundTripPreservesFieldsCarriedByDomain)
{
  const std::vector<double> control_points = make_control_points(4, 5);
  ASSERT_EQ(control_points.size(), 48U);

  mrs_interfaces::msg::LocalPlanShare msg;
  ASSERT_TRUE(mrs::convert::make_local_plan_share(1, 9, control_points, 4, 5, 0.5, 0.2, 12.5, msg).ok);
  EXPECT_EQ(msg.control_points.size(), 48U);

  mrs::NeighborTrajectory back;
  ASSERT_TRUE(mrs::convert::from_msg(msg, back).ok);
  EXPECT_EQ(back.robot_id, 1);
  EXPECT_EQ(back.tick_seq, 9U);
  EXPECT_EQ(back.control_points, control_points);
  EXPECT_DOUBLE_EQ(back.shift_ratio_u, 0.2);
}

// EscalationReport 왕복 — SYNC_LOST 가 다른 사유로 뭉개지지 않는다(뭉개지면 F2/F6 계측 오염).
// severity 는 severity_of(SYNC_LOST) = HARD 가 발행 시 자동으로 찍혀야 한다.
TEST(EscalationReportConvert, SyncLostRoundTripsAndStampsHardSeverity)
{
  mrs_interfaces::msg::EscalationReport msg;
  ASSERT_TRUE(mrs::convert::make_escalation_report(
                1, 77, mrs::EscalationReason::SYNC_LOST, 7, uniform_scope(42, 5), 12.5, msg)
                .ok);

  EXPECT_EQ(msg.severity, mrs_interfaces::msg::EscalationReport::SEVERITY_HARD);
  EXPECT_EQ(msg.view_scope.view_kind, 1U); // UNIFORM = 1 (계약 고정값)

  mrs::EscalationReason reason{};
  ASSERT_TRUE(mrs::convert::reason_from_msg(msg, uniform_scope(42, 5), reason).ok);
  EXPECT_EQ(reason, mrs::EscalationReason::SYNC_LOST);
}

// GetRoadmap 응답: known_version 이 낡았으면 본문을 싣고, 최신이면 UNCHANGED + 본문을 비운다.
TEST(MapRegistryConvert, GetRoadmapSendsBodyOnlyWhenClientVersionIsStale)
{
  const mrs::RoadmapViewData view = mrs_test::make_view();

  mrs_interfaces::srv::GetRoadmap::Response fresh;
  ASSERT_TRUE(mrs::convert::fill_response(view, 0, fresh).ok);
  EXPECT_EQ(fresh.result, mrs_interfaces::srv::GetRoadmap::Response::RESULT_OK);
  EXPECT_EQ(fresh.nodes.size(), 2U);
  EXPECT_EQ(fresh.edges.size(), 1U);
  EXPECT_EQ(fresh.frame_id, "map");
  EXPECT_EQ(fresh.roadmap_version, 42U);

  mrs_interfaces::srv::GetRoadmap::Response unchanged;
  ASSERT_TRUE(mrs::convert::fill_response(view, 42, unchanged).ok);
  EXPECT_EQ(unchanged.result, mrs_interfaces::srv::GetRoadmap::Response::RESULT_UNCHANGED);
  EXPECT_TRUE(unchanged.nodes.empty());
  EXPECT_TRUE(unchanged.edges.empty());
}

// cmd_vel 변환: diff-drive 2자유도만 채우고 나머지 4성분은 반드시 0 이어야 한다.
TEST(CmdVelConvert, MapsTwoDegreesOfFreedomAndZeroesTheRest)
{
  mrs::LocalPlanOutput output;
  output.cmd_v_mps = 0.4;
  output.cmd_omega_rps = -0.2;

  geometry_msgs::msg::Twist twist;
  ASSERT_TRUE(mrs::convert::make_cmd_vel(output, twist).ok);
  EXPECT_DOUBLE_EQ(twist.linear.x, 0.4);
  EXPECT_DOUBLE_EQ(twist.angular.z, -0.2);
  EXPECT_DOUBLE_EQ(twist.linear.y, 0.0);
  EXPECT_DOUBLE_EQ(twist.linear.z, 0.0);
  EXPECT_DOUBLE_EQ(twist.angular.x, 0.0);
  EXPECT_DOUBLE_EQ(twist.angular.y, 0.0);
}
