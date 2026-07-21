/**
 * @file test_tick_scheduler.cpp
 * @brief `TickScheduler` unit 티어 회귀 테스트 — R-20 정정이 지켜지는지 결정론적으로 검증한다.
 *
 * 이 파일이 지키는 명제는 넷이다.
 *  1. **샘플러 지터는 결번을 만들지 못한다.** 발행은 인덱스가 정하고 타이머는 관측만 한다.
 *  2. **중복은 발생하지 않는다.** 같은 인덱스는 `HOLD` 이므로 억제할 중복이 애초에 없다
 *     (구 구현의 "중복 억제 → 스케줄 밀림 → 영구 결번" 경로가 사라졌다는 증거).
 *  3. **진짜 결번은 보고된다.** 시계가 Δt_h 넘게 점프하면 `missed_seq_count` 가 그 수만큼 선다.
 *  4. **재동기는 멈추지 않는다.** 시계 역행 시 t0 를 다시 잡고 감소한 번호를 실제로 낸다.
 *
 * ⚠ rclcpp 를 쓰지 않는다(unit 티어 정의 — `tools/check_boundaries.sh` 검사 [6]/[7]).
 */

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "mrs_ros_plan_tick/tick_scheduler.hpp"

namespace
{

/** @brief 기본 Δt_h [s] — 계약 L-15 기본값. */
constexpr double PERIOD_S = 0.1;

/**
 * @brief `sim_bridge` 와 같은 방식으로 시뮬 시각 수열을 만든다.
 *
 * `sim_time_s_ += sim_step_s`(double 누산) → `seconds_to_time` 의 나노초 **반올림** →
 * 노드가 `now().seconds()` 로 되받는 값. 즉 실제 pysim 이 이 노드에 먹이는 값과 같은 격자다.
 *
 * @param[in] step_s 시뮬 스텝 [s]. 자료형 `double`.
 * @param[in] steps 만들 표본 수. 자료형 `int`.
 * @return `std::vector<double>` — 시뮬 시각 수열 [s].
 */
std::vector<double> make_sim_clock(double step_s, int steps)
{
  std::vector<double> clock;
  clock.reserve(static_cast<std::size_t>(steps));
  double accumulated_s = 0.0;
  for (int i = 0; i < steps; ++i)
  {
    accumulated_s += step_s;
    clock.push_back(std::round(accumulated_s * 1e9) / 1e9);
  }
  return clock;
}

/**
 * @brief 시각 수열을 스케줄러에 먹이고 발행된 번호를 모은다(발행은 항상 커밋한다).
 *
 * @param[in,out] scheduler 대상 스케줄러. 자료형 `mrs::TickScheduler &`.
 * @param[in] clock 시각 수열 [s]. 자료형 `const std::vector<double> &`.
 * @return `std::vector<std::uint32_t>` — 발행된 `tick_seq` 를 순서대로 담은 목록.
 */
std::vector<std::uint32_t> drive(mrs::TickScheduler & scheduler, const std::vector<double> & clock)
{
  std::vector<std::uint32_t> published;
  for (double now_s : clock)
  {
    const mrs::TickSample sample = scheduler.peek(now_s);
    scheduler.commit(sample);
    if (sample.action == mrs::TickAction::PUBLISH)
    {
      published.push_back(sample.tick_seq);
    }
  }
  return published;
}

/**
 * @brief 번호 목록이 결번·중복 없이 1씩 증가하는지 확인한다.
 *
 * @param[in] published 발행된 번호 목록. 자료형 `const std::vector<std::uint32_t> &`.
 * @return `bool` — 연속 증가이면 true. 비어 있으면 false(발행이 아예 없던 것을 통과시키지 않는다).
 */
bool is_dense_increasing(const std::vector<std::uint32_t> & published)
{
  if (published.empty())
  {
    return false;
  }
  for (std::size_t i = 1U; i < published.size(); ++i)
  {
    if (published[i] != published[i - 1U] + 1U)
    {
      return false;
    }
  }
  return true;
}

} // namespace

// ── 1. 샘플러가 Δt_h 보다 빠르면 결번도 중복도 없다 ────────────────────────────────────

TEST(TickScheduler, DenseSamplingProducesNoGapAndNoDuplicate)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  const std::vector<double> clock = make_sim_clock(PERIOD_S / 4.0, 4000);
  const std::vector<std::uint32_t> published = drive(scheduler, clock);

  EXPECT_TRUE(is_dense_increasing(published));
  EXPECT_EQ(scheduler.missed_seq_count(), 0U);
  EXPECT_EQ(scheduler.resync_count(), 0U);
  EXPECT_EQ(scheduler.clock_reject_count(), 0U);
  EXPECT_EQ(published.size(), scheduler.published_count());
  // 표본 4000 개 중 발행은 약 1000 건, 나머지는 HOLD — "중복 억제"가 아니라 애초에 안 만든다.
  EXPECT_EQ(scheduler.published_count() + scheduler.hold_count(), clock.size());
}

// ── 2. 샘플러 지터는 틱 정확도를 흔들지 못한다 (R-20 의 근인 재현 방지) ─────────────────
//
// 구 구현이 실제로 겪은 상황: 타이머가 공칭 10 Hz 대신 ~11.4 Hz 로 발화했다. 그때 구 구현은
// 중복을 억제하며 스케줄이 밀려 번호를 잃었다. 새 규칙에서는 같은 지터가 HOLD 로 흡수된다.

TEST(TickScheduler, JitteredSamplerNeverSkipsAnIndex)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  std::vector<double> clock;
  double now_s = 0.0;
  const double jitter[] = {0.0231, 0.0189, 0.0402, 0.0091, 0.0333, 0.0275};
  for (int i = 0; i < 3000; ++i)
  {
    now_s += jitter[i % 6];
    clock.push_back(now_s);
  }

  const std::vector<std::uint32_t> published = drive(scheduler, clock);
  EXPECT_TRUE(is_dense_increasing(published));
  EXPECT_EQ(scheduler.missed_seq_count(), 0U);
}

// ── 3. 시뮬 스텝 구성이 달라져도 결번은 0 이다 (실측 수용 기준의 unit 티어 거울) ──────────
//
// 특히 `sim_step_s == Δt_h` 는 함정이다: `double` 로 `(t − t0)/Δt_h` 를 나누면 진짜 몫이
// 정수인데도 이진 표현이 정수 바로 아래로 떨어져 `floor` 가 1을 잃고, 인덱스가 멈췄다가 2씩
// 뛰어 **가짜 결번**이 만들어진다(실측 약 10 %). 정수 나노초 평가가 그것을 없앤다.

TEST(TickScheduler, GapFreeAcrossSimStepConfigurations)
{
  const double steps[] = {0.05, 0.1, 0.025, 0.02};
  for (double step_s : steps)
  {
    mrs::TickScheduler scheduler(PERIOD_S);
    const std::vector<std::uint32_t> published =
      drive(scheduler, make_sim_clock(step_s, static_cast<int>(300.0 / step_s)));
    EXPECT_TRUE(is_dense_increasing(published)) << "sim_step_s=" << step_s;
    EXPECT_EQ(scheduler.missed_seq_count(), 0U) << "sim_step_s=" << step_s;
  }
}

// ── 4. 진짜 결번은 숨기지 않는다 ───────────────────────────────────────────────────────

TEST(TickScheduler, ClockJumpIsReportedAsRealGap)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(10.0));
  scheduler.commit(scheduler.peek(10.05));

  const mrs::TickSample jumped = scheduler.peek(10.5); // Δt_h 의 5배 점프
  EXPECT_EQ(jumped.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(jumped.tick_seq, 5U);
  EXPECT_EQ(jumped.missed_seq_count, 4U);
  EXPECT_NEAR(jumped.clock_advance_s, 0.45, 1e-9);

  scheduler.commit(jumped);
  EXPECT_EQ(scheduler.missed_seq_count(), 4U);
}

TEST(TickScheduler, SinglePeriodAdvanceIsNotAGap)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(4.0));

  const mrs::TickSample next = scheduler.peek(4.1); // 정확히 Δt_h — 번호가 사라지지 않았다
  EXPECT_EQ(next.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(next.tick_seq, 1U);
  EXPECT_EQ(next.missed_seq_count, 0U);
}

// ── 5. 발행 조건: 인덱스가 커질 때에만 ─────────────────────────────────────────────────

TEST(TickScheduler, SameIndexHoldsInsteadOfDuplicating)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(2.0));

  const mrs::TickSample same = scheduler.peek(2.099999);
  EXPECT_EQ(same.action, mrs::TickAction::HOLD);
  scheduler.commit(same);
  EXPECT_EQ(scheduler.published_count(), 1U);
  EXPECT_EQ(scheduler.hold_count(), 1U);
  EXPECT_EQ(scheduler.missed_seq_count(), 0U);
}

TEST(TickScheduler, PeekHasNoSideEffect)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  const mrs::TickSample first = scheduler.peek(7.0);
  const mrs::TickSample second = scheduler.peek(7.0);

  EXPECT_EQ(first.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(second.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(second.tick_seq, first.tick_seq);
  EXPECT_EQ(scheduler.published_count(), 0U);
  EXPECT_FALSE(scheduler.has_published());
}

TEST(TickScheduler, UncommittedPublishRetriesTheSameIndex)
{
  // 발행에 실패한 표본을 커밋하지 않으면 그 인덱스는 소모되지 않는다 — 변환 가드가 새 결번을
  // 만들어내지 않게 하는 규약이며, 노드는 `seconds_to_time` 실패 시 정확히 이렇게 행동한다.
  mrs::TickScheduler scheduler(PERIOD_S);
  const mrs::TickSample dropped = scheduler.peek(1.0);
  EXPECT_EQ(dropped.action, mrs::TickAction::PUBLISH);

  const mrs::TickSample retried = scheduler.peek(1.02);
  EXPECT_EQ(retried.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(retried.tick_seq, dropped.tick_seq);
  EXPECT_EQ(scheduler.missed_seq_count(), 0U);
}

// ── 6. 재동기 — "큰 인덱스일 때만 발행"이 재시작을 얼려서는 안 된다 ─────────────────────

TEST(TickScheduler, ClockRegressionReanchorsAndKeepsPublishing)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  drive(scheduler, make_sim_clock(PERIOD_S / 4.0, 400));
  const std::uint32_t before = scheduler.last_published_seq();
  EXPECT_GT(before, 0U);

  const mrs::TickSample restarted = scheduler.peek(0.05); // 시뮬 재시작
  EXPECT_EQ(restarted.action, mrs::TickAction::PUBLISH);
  EXPECT_TRUE(restarted.resynced);
  EXPECT_EQ(restarted.tick_seq, 0U);
  EXPECT_EQ(restarted.missed_seq_count, 0U);

  scheduler.commit(restarted);
  EXPECT_EQ(scheduler.resync_count(), 1U);

  const mrs::TickSample next = scheduler.peek(0.15);
  EXPECT_EQ(next.action, mrs::TickAction::PUBLISH);
  EXPECT_EQ(next.tick_seq, 1U);
}

// ── 7. R-A1 산식 자체는 그대로다 ───────────────────────────────────────────────────────

TEST(TickScheduler, TickTimeFollowsEpochPlusIndexTimesPeriod)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(3.25)); // t0 = 3.25

  const mrs::TickSample sample = scheduler.peek(3.87);
  EXPECT_EQ(sample.tick_seq, 6U); // floor((3.87 − 3.25)/0.1) = 6
  EXPECT_NEAR(sample.tick_time_s, 3.25 + 6.0 * PERIOD_S, 1e-9);
}

// ── 8. 가드 — 의심스러운 틱보다 발행하지 않는 편이 낫다 ─────────────────────────────────

TEST(TickScheduler, UnusableClockValuesAreRejectedWithoutStateChange)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(5.0));

  const double bad_values[] = {
    std::nan(""), std::numeric_limits<double>::infinity(), -1.0,
    mrs::TickScheduler::MAX_CLOCK_SECONDS * 2.0};
  for (double bad : bad_values)
  {
    const mrs::TickSample sample = scheduler.peek(bad);
    EXPECT_EQ(sample.action, mrs::TickAction::CLOCK_REJECTED);
    scheduler.commit(sample);
  }

  EXPECT_EQ(scheduler.clock_reject_count(), 4U);
  EXPECT_EQ(scheduler.published_count(), 1U);
  EXPECT_EQ(scheduler.last_published_seq(), 0U);
}

TEST(TickScheduler, IndexBeyondUint32IsRejected)
{
  mrs::TickScheduler scheduler(PERIOD_S);
  scheduler.commit(scheduler.peek(0.0));

  // uint32 최대 틱 번호 = 약 4.29e9 개 × 0.1 s ≈ 4.29e8 s. 그보다 뒤의 시각은 범위 밖이다.
  const mrs::TickSample sample = scheduler.peek(1.0e9);
  EXPECT_EQ(sample.action, mrs::TickAction::RANGE_REJECTED);
  scheduler.commit(sample);
  EXPECT_EQ(scheduler.range_reject_count(), 1U);
  EXPECT_EQ(scheduler.published_count(), 1U);
}

// ── 9. 생성자·권장 주기 ────────────────────────────────────────────────────────────────

TEST(TickScheduler, ConstructorRejectsUnusablePeriods)
{
  // 람다를 거치는 이유는 most vexing parse 회피다 — `mrs::TickScheduler(x)` 를 그대로 쓰면
  // 컴파일러가 그것을 **변수 선언**으로 읽는다.
  const auto construct = [](double period_s) { return mrs::TickScheduler(period_s); };

  EXPECT_THROW(construct(0.0), std::invalid_argument);
  EXPECT_THROW(construct(-0.1), std::invalid_argument);
  EXPECT_THROW(construct(std::nan("")), std::invalid_argument);
  EXPECT_THROW(construct(1.0e-12), std::invalid_argument); // 나노초 격자 아래
  EXPECT_NO_THROW(construct(PERIOD_S));
}

TEST(TickScheduler, RecommendedSamplePeriodIsQuarterOfReplanPeriod)
{
  EXPECT_NEAR(mrs::TickScheduler::recommended_sample_period_s(PERIOD_S), 0.025, 1e-12);
  EXPECT_EQ(mrs::TickScheduler::recommended_sample_period_s(0.0), 0.0);
  EXPECT_EQ(mrs::TickScheduler::recommended_sample_period_s(std::nan("")), 0.0);
}
