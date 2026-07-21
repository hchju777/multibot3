// test_startup_checks.cpp — 기동 거부 게이트 2종의 판정과 **반환 극성**을 지키는 벽.
//
// ## 왜 극성이 벽인가
// `startup_checks.hpp` 의 두 함수는 의도적으로 같은 극성("통과 = true")을 쓴다. 한쪽이 뒤집히면
// 호출부가 조건을 반대로 쓰기 쉽고, 그러면 기동 게이트가 **조용히 무력화**된다. 조용한 위반은
// F5 결과를 통째로 무효화하므로(architecture §1.7-A/B) 극성 자체를 테스트로 못박는다.
//
// ## 이 파일이 검증하지 **못하는** 것 (architecture §6)
// - ROS 그래프 API 가 실제로 발행자를 몇 개 세는가: rclcpp 호출은 unit 티어에 없다(C1).
//   여기서 검증되는 것은 "센 값을 받아 판정하는 순수 함수"뿐이며, 조회 부분은 pysim/isaac 몫이다.
// - Δt_h 가 실제로 시뮬 스텝과 정렬되어 돌아가는가: `/clock` 점프·틱 병합은 pysim 이상에서만
// 관측된다.
// - QP feasibility · livelock · 물리 재현성: **unit 티어로 판정할 수 없다. isaac 필수.**

#include <cstddef>

#include <gtest/gtest.h>

#include "mrs/startup_checks.hpp"

// ── is_integer_multiple (R-A2) ───────────────────────────────────────────────

// Δt_h = 0.1, dt = 0.05 → 비 2.0. 확정서 기본 구성이 통과해야 한다.
TEST(StartupChecks, IntegerMultipleAcceptsExactRatioTwo)
{
  EXPECT_TRUE(mrs::is_integer_multiple(0.1, 0.05));
}

// 비 1.0 (스텝마다 재계획) 도 정수배다.
TEST(StartupChecks, IntegerMultipleAcceptsRatioOne)
{
  EXPECT_TRUE(mrs::is_integer_multiple(0.05, 0.05));
}

// 0.1 / 0.03 = 3.333… — 정수배가 아니므로 기동을 거부해야 한다.
TEST(StartupChecks, IntegerMultipleRejectsNonIntegerRatio)
{
  EXPECT_FALSE(mrs::is_integer_multiple(0.1, 0.03));
}

// 0.05 / 0.1 = 0.5 → 가장 가까운 정수가 0. 재계획 주기가 스텝보다 짧으면 성립할 수 없다.
// (round() 만 보고 "0.5 는 0 에 가까우니 통과" 로 새는 회귀를 가르는 케이스)
TEST(StartupChecks, IntegerMultipleRejectsRatioBelowOne)
{
  EXPECT_FALSE(mrs::is_integer_multiple(0.05, 0.1));
}

// 비양수 입력은 전부 거부 — 0 나눗셈·음수 주기로 도는 구성을 막는다.
TEST(StartupChecks, IntegerMultipleRejectsNonPositiveInputs)
{
  EXPECT_FALSE(mrs::is_integer_multiple(0.0, 0.05));
  EXPECT_FALSE(mrs::is_integer_multiple(0.1, 0.0));
  EXPECT_FALSE(mrs::is_integer_multiple(-0.1, 0.05));
  EXPECT_FALSE(mrs::is_integer_multiple(0.1, -0.05));
  EXPECT_FALSE(mrs::is_integer_multiple(0.0, 0.0));
}

// 허용오차 안쪽: 비 = 2 * (1 + 5e-7) → 상대 잔차 5e-7 < 1e-6 이므로 통과.
// (기대값은 손으로 계산: nearest=2, residual=1e-6, 임계 = 1e-6 * 2 = 2e-6 → residual < 임계)
TEST(StartupChecks, IntegerMultipleAcceptsWithinRelativeTolerance)
{
  const double sim_step_s = 0.05;
  const double replan_period_s = sim_step_s * 2.0 * (1.0 + 5.0e-7);
  EXPECT_TRUE(mrs::is_integer_multiple(replan_period_s, sim_step_s));
}

// 허용오차 바깥: 비 = 2 * (1 + 1e-4) → 상대 잔차 1e-4 ≫ 1e-6 이므로 거부.
TEST(StartupChecks, IntegerMultipleRejectsOutsideRelativeTolerance)
{
  const double sim_step_s = 0.05;
  const double replan_period_s = sim_step_s * 2.0 * (1.0 + 1.0e-4);
  EXPECT_FALSE(mrs::is_integer_multiple(replan_period_s, sim_step_s));
}

// 허용오차 상수가 조용히 느슨해지지 않게 값 자체를 못박는다.
TEST(StartupChecks, RelativeToleranceConstantIsOneMicro)
{
  EXPECT_DOUBLE_EQ(mrs::STEP_RATIO_RELATIVE_TOLERANCE, 1e-6);
}

// ── is_single_publisher_ok (R-05 / 계약 L-15) ────────────────────────────────

// 0 개(아직 안 뜸)와 1 개(정상)는 통과. 2 개 이상은 N5 를 조용히 깨므로 기동 거부.
TEST(StartupChecks, SinglePublisherAcceptsZeroAndOne)
{
  EXPECT_TRUE(mrs::is_single_publisher_ok(0U));
  EXPECT_TRUE(mrs::is_single_publisher_ok(1U));
}

TEST(StartupChecks, SinglePublisherRejectsTwoOrMore)
{
  EXPECT_FALSE(mrs::is_single_publisher_ok(2U));
  EXPECT_FALSE(mrs::is_single_publisher_ok(3U));
  EXPECT_FALSE(mrs::is_single_publisher_ok(static_cast<std::size_t>(64U)));
}

// ── 극성 일치 (헤더 @note 가 명시한 설계 결정 자체) ──────────────────────────

// 두 게이트 모두 "통과 = true / 위반 = false". 한쪽만 뒤집혀도 이 테스트가 실패한다.
TEST(StartupChecks, BothGatesSharePassEqualsTruePolarity)
{
  // 통과해야 하는 입력 → 둘 다 true
  EXPECT_TRUE(mrs::is_integer_multiple(0.1, 0.05));
  EXPECT_TRUE(mrs::is_single_publisher_ok(1U));

  // 위반 입력 → 둘 다 false
  EXPECT_FALSE(mrs::is_integer_multiple(0.1, 0.03));
  EXPECT_FALSE(mrs::is_single_publisher_ok(2U));

  // 극성이 서로 반대가 아님을 직접 진술: 같은 "정상 구성"에서 두 반환이 같아야 한다.
  EXPECT_EQ(mrs::is_integer_multiple(0.1, 0.05), mrs::is_single_publisher_ok(1U));
  EXPECT_EQ(mrs::is_integer_multiple(0.1, 0.03), mrs::is_single_publisher_ok(2U));
}
