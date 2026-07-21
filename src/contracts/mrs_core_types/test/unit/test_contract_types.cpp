// test_contract_types.cpp — 계약 미러 열거의 값과 `severity_of` 매핑을 지키는 벽.
//
// ## 왜 값을 손으로 적는가
// 이 헤더의 열거들은 `.msg` 의 **미러이지 정본이 아니다**. 구현을 호출해 얻은 값을 기대값으로
// 쓰면 아무것도 검증하지 않으므로, 계약 레지스트리가 고정한 숫자를 리터럴로 적는다. 값이 바뀌면
// 이 테스트가 먼저 깨져 계약 변경 게이트를 거치지 않은 드리프트를 드러낸다.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6)
// - 도메인 enum 이 `.msg` 상수와 실제로 일치하는가: 그 대조는 `mrs_msg_convert` 의
//   `static_assert` 미러 벽이 한다(이 패키지는 `mrs_interfaces` 를 볼 수 없다 — C1).
// - 사다리 라우팅이 SYNC_LOST 를 계획 문제로 오인하지 않는가: `mrs_ladder` 의 몫이며 pysim 이상.
// - QP feasibility · livelock · 물리 재현성: **unit 티어로 판정할 수 없다. isaac 필수.**

#include <cstdint>

#include <gtest/gtest.h>

#include "mrs/contract_types.hpp"

// ── severity_of (계약 L-04 매핑표) ───────────────────────────────────────────

// WINDOW_EXPIRING 만이 SOFT 다 — "실행은 가능하나 여유가 줄고 있다".
TEST(SeverityOf, WindowExpiringIsSoft)
{
  EXPECT_EQ(mrs::severity_of(mrs::EscalationReason::WINDOW_EXPIRING), mrs::EscalationSeverity::SOFT);
}

// 나머지 4종은 전부 HARD. 하나라도 SOFT 로 새면 사다리가 즉시 대응해야 할 사건을 지연시킨다.
TEST(SeverityOf, AllOtherReasonsAreHard)
{
  EXPECT_EQ(mrs::severity_of(mrs::EscalationReason::BLOCKED), mrs::EscalationSeverity::HARD);
  EXPECT_EQ(mrs::severity_of(mrs::EscalationReason::QP_INFEASIBLE), mrs::EscalationSeverity::HARD);
  EXPECT_EQ(
    mrs::severity_of(mrs::EscalationReason::LOCALIZATION_LOST), mrs::EscalationSeverity::HARD);
  EXPECT_EQ(mrs::severity_of(mrs::EscalationReason::SYNC_LOST), mrs::EscalationSeverity::HARD);
}

// SYNC_LOST 는 가장 늦게 추가된 사유다 — 매핑에서 누락되어 SOFT 로 떨어지는 회귀를 따로 못박는다.
TEST(SeverityOf, SyncLostIsHardNotSoft)
{
  EXPECT_NE(mrs::severity_of(mrs::EscalationReason::SYNC_LOST), mrs::EscalationSeverity::SOFT);
}

// ── 계약 고정 열거값 (전부 손으로 적은 기대값) ───────────────────────────────

TEST(ContractEnums, EscalationReasonValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationReason::BLOCKED), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationReason::WINDOW_EXPIRING), 1U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationReason::QP_INFEASIBLE), 2U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationReason::LOCALIZATION_LOST), 3U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationReason::SYNC_LOST), 4U);
}

TEST(ContractEnums, EscalationSeverityValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationSeverity::HARD), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::EscalationSeverity::SOFT), 1U);
}

TEST(ContractEnums, RungValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R0_LOCAL_ABSORB), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R1_BTPG), 1U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R2_MILP), 2U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R3_REPLAN), 3U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R4_REASSIGN), 4U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::Rung::R5_OPERATOR), 5U);
}

// RungEvent 는 "스키마 드리프트 = 실험 무효"를 스스로 선언한 메시지다 — 값 5개 전부 못박는다.
TEST(ContractEnums, RungTransitionValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTransition::ENTER), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTransition::ABSORBED), 1U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTransition::ESCALATE), 2U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTransition::GUARD_REJECT), 3U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTransition::ESCAPE_HATCH), 4U);
}

TEST(ContractEnums, RungTriggerKindValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTriggerKind::HARD), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RungTriggerKind::SOFT), 1U);
}

TEST(ContractEnums, RevisionKindValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RevisionKind::NEW), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::RevisionKind::TRUNCATE), 1U);
}

// ── 센티넬 상수 ─────────────────────────────────────────────────────────────

TEST(ContractSentinels, IdSentinelsMatchContract)
{
  EXPECT_EQ(mrs::ROBOT_ID_NONE, 65535U);      // 2^16 - 1
  EXPECT_EQ(mrs::NODE_ID_NONE, 4294967295U);  // 2^32 - 1
  EXPECT_EQ(mrs::EDGE_ID_NONE, 4294967295U);  // 2^32 - 1
  EXPECT_EQ(mrs::EVENT_ID_UNASSIGNED, 0U);
}

// 불변식 E1 — event_id 0 은 미할당이고 런타임에 금지된다(계측이 상관관계를 잃는다).
TEST(ContractSentinels, EventIdZeroIsInvalidAndOthersAreValid)
{
  EXPECT_FALSE(mrs::is_valid_event_id(0U));
  EXPECT_TRUE(mrs::is_valid_event_id(1U));
  EXPECT_TRUE(mrs::is_valid_event_id(77U));
}

// ── 창 기본값 ───────────────────────────────────────────────────────────────

// 기본 생성된 ExecutionWindow 는 "전 구간 유효(-1) + NEW" 여야 한다 — 개정 불변식의 기준점.
TEST(ExecutionWindowDefaults, DefaultIsNewWithFullValidity)
{
  const mrs::ExecutionWindow window;
  EXPECT_EQ(window.revision_kind, mrs::RevisionKind::NEW);
  EXPECT_EQ(window.valid_through_segment_index, -1);
  EXPECT_EQ(window.robot_id, mrs::ROBOT_ID_NONE);
  EXPECT_EQ(window.roadmap_version, mrs::ROADMAP_VERSION_UNSPECIFIED);
  EXPECT_TRUE(window.segments.empty());
}

// 창 세그먼트의 노드는 균일 뷰 강타입이고 기본값은 센티넬이다.
TEST(ExecutionWindowDefaults, WindowSegmentNodesDefaultToUniformSentinel)
{
  const mrs::WindowSegment segment;
  EXPECT_TRUE(segment.node_from.is_none());
  EXPECT_TRUE(segment.node_to.is_none());
  EXPECT_EQ(mrs::WindowSegment{}.node_from.VIEW_KIND, mrs::ViewKind::UNIFORM);
}
