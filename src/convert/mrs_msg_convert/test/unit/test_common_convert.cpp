// test_common_convert.cpp — 공통 헬퍼(열거 왕복 · 노드 id 랩/언랩 · Pose2D 배열)의 긍정 경로.
//
// ## 이 파일이 지키는 것
// 열거 왕복 항등과 집합 밖 값 거부, `NoneNodePolicy` 두 분기, 배열 헬퍼의 "원소 하나 실패 →
// 전체 실패 + out 보존", Pose2D 각도 구간 [-pi, pi] 판정.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6 충실도 티어 표)
// - **컴파일이 거부되는가**: `test/negative/common_convert_negative.cpp` 의 ctest 부정 컴파일
//   테스트가 담당한다. 여기 있는 어떤 assertion 도 그 벽을 대신하지 못한다.
// - 도메인 enum 이 `.msg` 상수와 일치하는가: `common_convert.cpp` 의 `static_assert` 미러 벽이
//   컴파일 시각에 강제한다(이 테스트가 그 벽을 재검증하지는 않는다).
// - DDS 왕복·QoS·타이밍: unit 티어에는 ROS 배선이 없다. pysim 이상.
// - **QP feasibility · livelock · 물리 재현성: 여기서 판정할 수 없다. isaac 필수(§5.3).**

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "mrs_msg_convert/common_convert.hpp"

namespace
{

// 손으로 적은 센티넬 — 2^32 - 1.
constexpr std::uint32_t NONE_RAW = 4294967295U;

} // namespace

// ── 노드 id 랩/언랩 ─────────────────────────────────────────────────────────

// 3뷰 전부에서 언랩이 원시 값을 그대로 낸다.
TEST(NodeIdHelpers, UnwrapReturnsRawValueForAllThreeViews)
{
  EXPECT_EQ(mrs::convert::node_id_to_msg(mrs::PhysicalNodeId{7U}), 7U);
  EXPECT_EQ(mrs::convert::node_id_to_msg(mrs::UniformNodeId{9U}), 9U);
  EXPECT_EQ(mrs::convert::node_id_to_msg(mrs::SkeletonNodeId{11U}), 11U);
}

// 정상 값 랩은 성공하고 out 에 그 값이 들어간다.
TEST(NodeIdHelpers, WrapAcceptsOrdinaryValue)
{
  mrs::UniformNodeId wrapped{};
  const mrs::convert::ConvertResult result =
    mrs::convert::node_id_from_msg(42U, mrs::convert::NoneNodePolicy::REJECT, wrapped);
  EXPECT_TRUE(result.ok);
  EXPECT_EQ(wrapped.value(), 42U);
}

// REJECT 정책에서 센티넬이 오면 폐기하고 **out 을 건드리지 않는다**.
// (창 세그먼트처럼 센티넬이 곧 무효인 필드를 지키는 벽)
TEST(NodeIdHelpers, WrapRejectsSentinelUnderRejectPolicyAndLeavesOutIntact)
{
  mrs::UniformNodeId keep{3U};
  const mrs::convert::ConvertResult result =
    mrs::convert::node_id_from_msg(NONE_RAW, mrs::convert::NoneNodePolicy::REJECT, keep);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, mrs::convert::ConvertStatus::FIELD_RANGE_VIOLATION);
  EXPECT_EQ(keep.value(), 3U);
}

// ALLOW 정책에서는 센티넬이 "해당 없음"을 뜻하는 정상 값이다.
// (RobotState.occupied_node 처럼 노드 미점유가 정상인 필드)
TEST(NodeIdHelpers, WrapAcceptsSentinelUnderAllowPolicy)
{
  mrs::UniformNodeId none_ok{5U};
  const mrs::convert::ConvertResult result =
    mrs::convert::node_id_from_msg(NONE_RAW, mrs::convert::NoneNodePolicy::ALLOW, none_ok);
  EXPECT_TRUE(result.ok);
  EXPECT_TRUE(none_ok.is_none());
}

// 배열: 원소 하나가 실패하면 전체 실패이고 out 은 손대지 않는다 — 부분 변환 배열이 도메인에
// 흘러 "정상적으로 짧은 경로"로 둔갑하는 것을 막는 벽.
TEST(NodeIdHelpers, ArrayWrapFailsWholeAndPreservesOutOnAnyElementViolation)
{
  const std::vector<std::uint32_t> raw{1U, NONE_RAW, 3U};
  std::vector<mrs::UniformNodeId> out{mrs::UniformNodeId{99U}};

  const mrs::convert::ConvertResult result =
    mrs::convert::node_ids_from_msg(raw, mrs::convert::NoneNodePolicy::REJECT, out);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, mrs::convert::ConvertStatus::FIELD_RANGE_VIOLATION);
  ASSERT_EQ(out.size(), 1U);
  EXPECT_EQ(out[0].value(), 99U);
}

// 배열 왕복 항등: 언랩(랩(raw)) == raw.
TEST(NodeIdHelpers, ArrayRoundTripIsIdentity)
{
  const std::vector<std::uint32_t> raw{1U, 2U, 3U};
  std::vector<mrs::UniformNodeId> wrapped;
  ASSERT_TRUE(mrs::convert::node_ids_from_msg(raw, mrs::convert::NoneNodePolicy::REJECT, wrapped).ok);
  ASSERT_EQ(wrapped.size(), 3U);
  EXPECT_EQ(mrs::convert::node_ids_to_msg(wrapped), raw);
}

// 빈 배열은 유효하다 — 비었다는 사실 자체가 오류 신호는 아니다.
TEST(NodeIdHelpers, EmptyArrayIsValid)
{
  std::vector<mrs::UniformNodeId> out{mrs::UniformNodeId{1U}};
  EXPECT_TRUE(mrs::convert::node_ids_from_msg({}, mrs::convert::NoneNodePolicy::REJECT, out).ok);
  EXPECT_TRUE(out.empty());
  EXPECT_TRUE(mrs::convert::node_ids_to_msg(std::vector<mrs::UniformNodeId>{}).empty());
}

// ── 열거 왕복 ───────────────────────────────────────────────────────────────

// EscalationReason: 값 4 = SYNC_LOST (계약 고정), 집합 밖 5 는 거부.
TEST(EnumConvert, EscalationReasonRoundTripsAndRejectsOutOfRange)
{
  mrs::EscalationReason reason{};
  EXPECT_TRUE(mrs::convert::escalation_reason_from_msg(4U, reason).ok);
  EXPECT_EQ(reason, mrs::EscalationReason::SYNC_LOST);
  EXPECT_EQ(mrs::convert::escalation_reason_to_msg(mrs::EscalationReason::SYNC_LOST), 4U);

  EXPECT_EQ(
    mrs::convert::escalation_reason_from_msg(5U, reason).reason,
    mrs::convert::ConvertStatus::ENUM_OUT_OF_RANGE);
}

// AdoptionStatus 값 2 = 철회된 `QP_TICK_JITTER` — R-17 판정에 따라 **수신 거부**여야 한다.
// 이 값을 받아들이면 철회된 지터 모델이 B5 폴백 사유 분해로 되살아난다.
TEST(EnumConvert, AdoptionStatusRejectsWithdrawnQpTickJitterValueTwo)
{
  mrs::AdoptionStatus status{mrs::AdoptionStatus::QP_OK};
  const mrs::convert::ConvertResult result = mrs::convert::adoption_status_from_msg(2U, status);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, mrs::convert::ConvertStatus::ENUM_OUT_OF_RANGE);
  EXPECT_EQ(status, mrs::AdoptionStatus::QP_OK); // 실패 시 out 미오염
}

// 값 5 = FALLBACK_SOLVER 는 정상 수신된다 (거부가 전면 차단으로 번지지 않았다).
TEST(EnumConvert, AdoptionStatusAcceptsFallbackSolverValueFive)
{
  mrs::AdoptionStatus status{};
  EXPECT_TRUE(mrs::convert::adoption_status_from_msg(5U, status).ok);
  EXPECT_EQ(status, mrs::AdoptionStatus::FALLBACK_SOLVER);
}

// RevisionKind 는 NEW(0)/TRUNCATE(1) 뿐 — 2 는 집합 밖이며 NEW 로 폴백하면 안 된다.
TEST(EnumConvert, RevisionKindRejectsValueTwoWithoutFallingBackToNew)
{
  mrs::RevisionKind kind{mrs::RevisionKind::TRUNCATE};
  const mrs::convert::ConvertResult result = mrs::convert::revision_kind_from_msg(2U, kind);
  EXPECT_EQ(result.reason, mrs::convert::ConvertStatus::ENUM_OUT_OF_RANGE);
  EXPECT_EQ(kind, mrs::RevisionKind::TRUNCATE); // 폴백 대입이 일어나지 않았다
}

// ReorderAck 값 3 = REJECTED_STALE.
TEST(EnumConvert, ReorderAckRoundTripsValueThree)
{
  mrs::ReorderAck ack{};
  EXPECT_TRUE(mrs::convert::reorder_ack_from_msg(3U, ack).ok);
  EXPECT_EQ(ack, mrs::ReorderAck::REJECTED_STALE);
}

// FaultKind 는 0..5 — 6 은 집합 밖.
TEST(EnumConvert, FaultKindRejectsValueSix)
{
  mrs::FaultKind kind{};
  EXPECT_EQ(
    mrs::convert::fault_kind_from_msg(6U, kind).reason,
    mrs::convert::ConvertStatus::ENUM_OUT_OF_RANGE);
}

// ── Pose2D 배열 ─────────────────────────────────────────────────────────────

// 왕복 항등: 각도 -1.0 rad 이 부호까지 보존된다.
TEST(PoseConvert, ArrayRoundTripPreservesValues)
{
  const std::vector<mrs::Pose2D> poses{{1.0, 2.0, 0.5}, {3.0, 4.0, -1.0}};
  std::vector<mrs_interfaces::msg::Pose2D> msgs;
  ASSERT_TRUE(mrs::convert::poses_to_msg(poses, msgs).ok);
  ASSERT_EQ(msgs.size(), 2U);

  std::vector<mrs::Pose2D> back;
  ASSERT_TRUE(mrs::convert::poses_from_msg(msgs, back).ok);
  ASSERT_EQ(back.size(), 2U);
  EXPECT_DOUBLE_EQ(back[0].x_m, 1.0);
  EXPECT_DOUBLE_EQ(back[1].theta_rad, -1.0);
}

// theta = 4.0 rad 은 [-pi, pi] 밖 — 거부하고 out 을 보존한다.
TEST(PoseConvert, RejectsThetaOutsidePiIntervalAndPreservesOut)
{
  std::vector<mrs::Pose2D> poses{{1.0, 2.0, 0.5}, {3.0, 4.0, 4.0}};
  std::vector<mrs_interfaces::msg::Pose2D> keep{mrs_interfaces::msg::Pose2D{}};

  const mrs::convert::ConvertResult result = mrs::convert::poses_to_msg(poses, keep);
  EXPECT_FALSE(result.ok);
  EXPECT_EQ(result.reason, mrs::convert::ConvertStatus::FIELD_RANGE_VIOLATION);
  EXPECT_EQ(keep.size(), 1U);
}

// 빈 배열은 유효하다.
TEST(PoseConvert, EmptyArrayIsValid)
{
  std::vector<mrs::Pose2D> out{mrs::Pose2D{}};
  EXPECT_TRUE(mrs::convert::poses_from_msg({}, out).ok);
  EXPECT_TRUE(out.empty());
}
