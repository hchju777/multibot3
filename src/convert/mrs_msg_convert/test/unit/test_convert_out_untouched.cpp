// test_convert_out_untouched.cpp — **변환 실패 시 out 파라미터가 오염되지 않는다**를 지키는 벽.
//
// ## 왜 별도 파일인가
// 계약은 실패 시 out 을 "정의되지 않음"으로만 규정했으나, 빌더가 더 강한 보장(원본 보존)을
// 선택했고 R-17/R-18 이 그것을 수용했다. 이 보장이 조용히 사라지면 **부분 채워진 값**이 도메인에
// 흘러 들어가고, 부분 변환된 배열은 "정상적으로 짧은 경로"와 구별되지 않는다 — 폐기해야 할
// 메시지가 유효한 계획으로 둔갑하는 형태다. 정상 경로 테스트로는 절대 드러나지 않는다.
//
// 각 케이스는 out 에 **식별 가능한 감시값**(4242, 999, "keep", 특정 size)을 미리 넣고, 변환
// 실패 후에도 그 값이 그대로인지 본다.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6)
// - 실패가 **어느 단계**에서 났는지의 내부 순서: 이른 실패와 늦은 실패 두 경우를 각각 넣어
//   경험적으로 덮을 뿐, 모든 중간 단계를 열거하지는 않는다.
// - **QP feasibility · livelock · 물리 재현성: 판정 불가. isaac 필수(§5.3).**

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
 * @brief 감시값을 심어 둔 실행 창 메시지를 만든다.
 * @return `mrs_interfaces::msg::ExecutionWindow` — robot_id 4242 / window_seq 999 /
 *         schema_version 7 이 들어 있는 메시지. 변환 실패 후에도 이 값들이 남아야 한다.
 */
mrs_interfaces::msg::ExecutionWindow sentinel_window_msg()
{
  mrs_interfaces::msg::ExecutionWindow msg;
  msg.robot_id = 4242;
  msg.window_seq = 999;
  msg.schema_version = 7;
  return msg;
}

} // namespace

// [1] 발행 방향, **이른 단계**(스코프) 실패 — out 메시지가 전혀 채워지지 않는다.
TEST(OutUntouched, ExecutionWindowToMsgEarlyScopeFailureLeavesOutIntact)
{
  mrs_interfaces::msg::ExecutionWindow out = sentinel_window_msg();

  mrs::ExecutionWindow bad = make_window();
  bad.roadmap_version = 0; // 첫 검사에서 실패

  const ConvertResult result = mrs::convert::to_msg(bad, 12.5, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.robot_id, 4242);
  EXPECT_EQ(out.window_seq, 999U);
  EXPECT_EQ(out.schema_version, 7U);
}

// [2] 발행 방향, **마지막 단계**(시각) 실패 — 여기까지 와도 부분 채움이 없어야 한다.
// 이른 실패만 보면 "앞에서 걸러지니 괜찮다"는 착각이 생긴다. 늦은 실패가 진짜 벽이다.
TEST(OutUntouched, ExecutionWindowToMsgLateTimeFailureLeavesOutIntact)
{
  mrs_interfaces::msg::ExecutionWindow out = sentinel_window_msg();

  mrs::ExecutionWindow bad = make_window();
  bad.window_valid_until_s = kNaN; // 마지막 단계에서 실패

  const ConvertResult result = mrs::convert::to_msg(bad, 12.5, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.robot_id, 4242);
  EXPECT_EQ(out.window_seq, 999U);
}

// [3] 수신 방향 — 세그먼트 배열 중간의 센티넬 노드로 실패. 도메인 out 이 보존돼야 한다.
TEST(OutUntouched, ExecutionWindowFromMsgSentinelNodeLeavesDomainOutIntact)
{
  mrs_interfaces::msg::ExecutionWindow msg;
  ASSERT_TRUE(mrs::convert::to_msg(make_window(), 12.5, msg).ok);
  msg.segments[1].node_to = 4294967295U; // 센티넬 노드 = 창 무효

  mrs::ExecutionWindow out;
  out.robot_id = 4242;
  out.window_seq = 999;

  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
  EXPECT_EQ(out.robot_id, 4242);
  EXPECT_EQ(out.window_seq, 999U);
  EXPECT_TRUE(out.segments.empty()); // 부분 채워진 세그먼트 배열이 남지 않았다
}

// [4] 수신 방향 — edge_progress 범위 밖(1.5).
TEST(OutUntouched, RobotStateFromMsgRangeFailureLeavesOutIntact)
{
  mrs_interfaces::msg::RobotState msg;
  ASSERT_TRUE(mrs::convert::to_msg(make_observation(), 12.5, msg).ok);
  msg.edge_progress = 1.5;

  mrs::RobotObservation out;
  out.robot_id = 4242;

  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
  EXPECT_EQ(out.robot_id, 4242);
}

// [5] 수신 방향 — 제어점 길이 불변식 위반.
TEST(OutUntouched, LocalPlanShareFromMsgLengthFailureLeavesOutIntact)
{
  mrs_interfaces::msg::LocalPlanShare msg;
  ASSERT_TRUE(
    mrs::convert::make_local_plan_share(1, 9, make_control_points(4, 5), 4, 5, 0.5, 0.2, 12.5, msg)
      .ok);
  msg.control_points.pop_back();

  mrs::NeighborTrajectory out;
  out.robot_id = 4242;

  const ConvertResult result = mrs::convert::from_msg(msg, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.robot_id, 4242);
}

// [6] cmd_vel — NaN 차단 시 이전 Twist 값이 유지된다(0 으로 덮이지 않는다).
// 0 으로 덮으면 그것이 곧 "정지 명령"이 되어 계약 S3(정지는 도달되는 것)을 우회한다.
TEST(OutUntouched, CmdVelNanFailureLeavesOutIntact)
{
  geometry_msgs::msg::Twist out;
  out.linear.x = 42.0;

  mrs::LocalPlanOutput output;
  output.cmd_v_mps = 0.4;
  output.cmd_omega_rps = kNaN;

  const ConvertResult result = mrs::convert::make_cmd_vel(output, out);
  EXPECT_FALSE(result.ok);
  EXPECT_DOUBLE_EQ(out.linear.x, 42.0);
}

// [7] nodes_to_msg — 중복 노드 id 로 실패. 기존 배열 크기가 유지된다.
TEST(OutUntouched, RoadmapNodesToMsgFailureLeavesOutIntact)
{
  mrs::RoadmapViewData view = make_view();
  view.nodes[1].node_id = view.nodes[0].node_id;

  std::vector<mrs_interfaces::msg::RoadmapNode> out(3);
  const ConvertResult result = mrs::convert::nodes_to_msg(view, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.size(), 3U);
}

// [8] edges_to_msg — 길이 0 엣지로 실패.
TEST(OutUntouched, RoadmapEdgesToMsgFailureLeavesOutIntact)
{
  mrs::RoadmapViewData view = make_view();
  view.edges[0].length_m = 0.0;

  std::vector<mrs_interfaces::msg::RoadmapEdge> out(5);
  const ConvertResult result = mrs::convert::edges_to_msg(view, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.size(), 5U);
}

// [9] fill_response — 물리 뷰 규약 위반으로 실패. 응답의 기존 필드가 유지된다.
TEST(OutUntouched, GetRoadmapFillResponseFailureLeavesResponseIntact)
{
  mrs::RoadmapViewData view = make_view();
  view.view_id = 3; // 물리 뷰인데 0 이 아니다

  mrs_interfaces::srv::GetRoadmap::Response out;
  out.roadmap_version = 4242;

  const ConvertResult result = mrs::convert::fill_response(view, 0, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.roadmap_version, 4242U);
}

// [10] RungEvent — 전이 값 집합 밖으로 실패.
TEST(OutUntouched, RungEventToMsgFailureLeavesOutIntact)
{
  mrs::RungEventRecord record;
  record.event_id = 77;
  record.rung = mrs::Rung::R2_MILP;
  record.transition = 9; // 집합 밖
  record.trigger_kind = 1;
  record.phi_hat_before_s = 3.5;
  record.phi_hat_after_s = kNaN;
  record.attempt_count = 2;
  record.duration_s = 0.25;

  mrs_interfaces::msg::RungEvent out;
  out.event_id = 4242;

  const ConvertResult result = mrs::convert::to_msg(record, 12.5, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.event_id, 4242U);
}

// [11] SimMetricSample — 빈 key 로 실패. 기존 key 문자열이 유지된다.
TEST(OutUntouched, SimMetricSampleToMsgFailureLeavesOutIntact)
{
  mrs::SimMetricSample sample;
  sample.key.clear();
  sample.value = 1.0;
  sample.robot_id = mrs::ROBOT_ID_NONE;

  mrs_interfaces::msg::SimMetricSample out;
  out.key = "keep";

  const ConvertResult result = mrs::convert::to_msg(sample, 12.5, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(out.key, "keep");
}

// [12] 배열 헬퍼 — 원소 하나 실패 시 out 배열 전체 보존(부분 변환 금지).
TEST(OutUntouched, NodeIdArrayWrapFailureLeavesOutIntact)
{
  const std::vector<std::uint32_t> raw{1U, 4294967295U, 3U};
  std::vector<mrs::UniformNodeId> out{mrs::UniformNodeId{99U}, mrs::UniformNodeId{98U}};

  const ConvertResult result =
    mrs::convert::node_ids_from_msg(raw, mrs::convert::NoneNodePolicy::REJECT, out);
  EXPECT_FALSE(result.ok);
  ASSERT_EQ(out.size(), 2U);
  EXPECT_EQ(out[0].value(), 99U);
  EXPECT_EQ(out[1].value(), 98U);
}

// [13] PlannedPaths 발행 — **마지막 단계**(방문 시각 가드) 실패. 노드 id 는 전부 정상이라
// 앞 검사를 통과하고 배열을 절반쯤 쌓은 뒤 실패하는 경로다. 여기서 out 이 오염되면 sadg 가
// 절반짜리 계획으로 릴리스 대기열을 갱신하고, 짧아진 경로는 "정상적으로 짧은 경로"와 구별되지
// 않는다 — [0a] 가 못 잡는 형태의 조용한 오염이다.
TEST(OutUntouched, PlannedPathsToMsgLateTimeFailureLeavesOutIntact)
{
  mrs_interfaces::msg::PlannedPaths out;
  out.plan_epoch = 999;
  out.event_id = 4242;
  out.paths.resize(5);

  std::vector<mrs::RobotPath> bad = make_planned_paths();
  bad[1].visits[2].arrival_time_s = kNaN; // 마지막 로봇의 마지막 방문에서 실패

  const ConvertResult result =
    mrs::convert::to_msg(bad, false, 0, 11, uniform_scope(42, 5), 12.5, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::TIME_CONVERSION_GUARD);
  EXPECT_EQ(out.plan_epoch, 999U);
  EXPECT_EQ(out.event_id, 4242U);
  EXPECT_EQ(out.paths.size(), 5U);
}

// [14] PlannedPaths 수신 — 두 번째 로봇의 방문열에서 실패한다(첫 로봇은 이미 파싱을 마친 뒤).
TEST(OutUntouched, PlannedPathsFromMsgLateFailureLeavesDomainOutIntact)
{
  mrs_interfaces::msg::PlannedPaths msg;
  ASSERT_TRUE(
    mrs::convert::to_msg(make_planned_paths(), false, 0, 11, uniform_scope(42, 5), 12.5, msg).ok);
  msg.paths[1].visits[2].node_id = 4294967295U; // 센티넬 노드

  std::vector<mrs::RobotPath> out;
  out.resize(3);
  out[0].robot_id = 4242;

  const ConvertResult result = mrs::convert::from_msg(msg, uniform_scope(42, 5), out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, ConvertStatus::FIELD_RANGE_VIOLATION);
  ASSERT_EQ(out.size(), 3U); // 부분 파싱된 2대짜리 계획으로 덮이지 않았다
  EXPECT_EQ(out[0].robot_id, 4242);
}
