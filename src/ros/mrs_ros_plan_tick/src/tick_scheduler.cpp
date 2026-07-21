/**
 * @file tick_scheduler.cpp
 * @brief `TickScheduler` 구현 — R-A1 산식의 정수 나노초 평가 + R-20 발행 규칙.
 *
 * ⚠ 이 파일에도 **알고리즘은 없다.** `floor((t − t0)/Δt_h)` 는 계약 L-15 가 문언으로 못박은
 *   정의이고, 여기 있는 것은 그 정의를 손실 없이 계산하고 "새 인덱스일 때만 발행"을 판정하는
 *   상태기계뿐이다.
 */

#include <cmath>
#include <limits>
#include <stdexcept>

#include "mrs_ros_plan_tick/tick_scheduler.hpp"

namespace mrs
{
namespace
{

/** @brief `tick_seq`(`uint32`)의 최대값 — 인덱스 범위 가드 상한. */
constexpr std::int64_t MAX_TICK_SEQ =
  static_cast<std::int64_t>(std::numeric_limits<std::uint32_t>::max());

/**
 * @brief R-20 권장 샘플 주기의 분모 — 한 틱 구간을 몇 번 관측할 것인가.
 *
 * 4 = "콜백을 한 번 놓쳐도 인덱스가 넘어가지 않는다"의 최소 여유(3 회 남음). R-20 이 규정한
 * "Δt_h/4 이하 권장"의 상한값을 그대로 기본값으로 삼는다 — 더 잦게 돌려도 틱 정확도는
 * 좋아지지 않는다(정확도는 샘플 주기가 아니라 `floor` 산식이 정한다).
 */
constexpr double SAMPLE_PERIOD_DIVISOR = 4.0;

} // namespace

TickScheduler::TickScheduler(double replan_period_s)
    : period_ns_(0), replan_period_s_(replan_period_s)
{
  if (
    !std::isfinite(replan_period_s) || replan_period_s <= 0.0 ||
    replan_period_s > MAX_CLOCK_SECONDS)
  {
    throw std::invalid_argument("replan_period_s must be a finite positive number");
  }

  const double period_ns =
    std::round(replan_period_s * static_cast<double>(NANOSECONDS_PER_SECOND));
  if (period_ns < 1.0)
  {
    // 나노초 격자에서 0 이 되는 주기는 0 으로 나누는 것과 같다. 조용히 1 ns 로 올리지 않는다 —
    // 그런 주기로 낸 틱은 어차피 의미가 없고, 기동 실패가 유일하게 정직한 결과다.
    throw std::invalid_argument("replan_period_s is smaller than the nanosecond grid");
  }
  period_ns_ = static_cast<std::int64_t>(period_ns);
}

TickSample TickScheduler::peek(double now_seconds) const noexcept
{
  TickSample sample;

  if (!to_nanoseconds(now_seconds, sample.sampled_at_ns))
  {
    sample.action = TickAction::CLOCK_REJECTED;
    return sample;
  }

  anchor(sample);
  if (!fill_index(sample))
  {
    return sample; // fill_index 가 RANGE_REJECTED 를 이미 설정했다
  }
  classify(sample);
  return sample;
}

void TickScheduler::anchor(TickSample & sample) const noexcept
{
  // 시계 역행 = 발행자/시뮬 재시작. 계약 L-15 는 `tick_seq` 감소를 결번이 아니라 **재동기**로
  // 규정하므로, t0 를 다시 고정하고 발행 이력을 리셋해 감소한 번호가 실제로 나가게 한다.
  // ⚠ "새 인덱스일 때만 발행" 규칙만 두고 이 경로를 빼면 재동기 뒤 틱이 **영영 멈춘다**.
  const bool regressed = anchored_ && sample.sampled_at_ns < last_sample_ns_;
  sample.resynced = regressed;
  sample.epoch_ns = (!anchored_ || regressed) ? sample.sampled_at_ns : epoch_ns_;
  sample.clock_advance_s = anchored_ ? static_cast<double>(sample.sampled_at_ns - last_sample_ns_) /
                                         static_cast<double>(NANOSECONDS_PER_SECOND)
                                     : 0.0;
}

bool TickScheduler::fill_index(TickSample & sample) const noexcept
{
  // R-A1: tick_seq = floor((t − t0)/Δt_h). 정수 나노초라 몫이 정확하다(헤더 @note 참조).
  const std::int64_t elapsed_ns = sample.sampled_at_ns - sample.epoch_ns;
  if (elapsed_ns < 0)
  {
    // 도달 불가여야 한다(역행은 anchor 에서 t0 재고정으로 흡수된다). 그래도 조용히 지나치지
    // 않는다 — 여기서 음수를 나누면 `floor` 의미가 어긋난 번호가 와이어로 나간다.
    sample.action = TickAction::RANGE_REJECTED;
    return false;
  }

  const std::int64_t raw_seq = elapsed_ns / period_ns_; // elapsed_ns >= 0 이므로 floor 와 같다
  if (raw_seq > MAX_TICK_SEQ)
  {
    sample.action = TickAction::RANGE_REJECTED;
    return false;
  }

  sample.tick_seq = static_cast<std::uint32_t>(raw_seq);
  // R-A1: tick_time = t0 + tick_seq·Δt_h. 나노초에서 더한 뒤 초로 옮긴다(누적 오차 없음).
  sample.tick_time_s = static_cast<double>(sample.epoch_ns + raw_seq * period_ns_) /
                       static_cast<double>(NANOSECONDS_PER_SECOND);
  return true;
}

void TickScheduler::classify(TickSample & sample) const noexcept
{
  if (!has_published_ || sample.resynced)
  {
    sample.action = TickAction::PUBLISH;
    return;
  }

  if (sample.tick_seq > last_published_seq_)
  {
    sample.action = TickAction::PUBLISH;
    // 인덱스가 2 이상 뛰었다 = 두 샘플 사이에 시계가 Δt_h 를 넘겨 점프했다 = **실재하는 결번**.
    // 샘플러 지터로는 여기 오지 못한다(그때는 인덱스가 같아 HOLD 다).
    sample.missed_seq_count = sample.tick_seq - last_published_seq_ - 1U;
    return;
  }

  // 같은(또는 더 작은) 인덱스 = 아직 다음 틱 경계가 아니다.
  // 중복을 "억제"하는 것이 아니라 **만들지 않는다** — 그것이 R-20 의 요점이다.
  sample.action = TickAction::HOLD;
}

void TickScheduler::commit(const TickSample & sample) noexcept
{
  switch (sample.action)
  {
    case TickAction::PUBLISH:
      epoch_ns_ = sample.epoch_ns;
      anchored_ = true;
      has_published_ = true;
      last_published_seq_ = sample.tick_seq;
      last_sample_ns_ = sample.sampled_at_ns;
      ++published_count_;
      missed_seq_count_ += static_cast<std::uint64_t>(sample.missed_seq_count);
      if (sample.resynced)
      {
        ++resync_count_;
      }
      break;

    case TickAction::HOLD:
      // t0 는 건드리지 않는다 — 보류 표본은 기준 시각을 바꿀 권한이 없다.
      last_sample_ns_ = sample.sampled_at_ns;
      ++hold_count_;
      break;

    case TickAction::CLOCK_REJECTED:
      // 시계 값을 믿을 수 없으므로 역행 탐지 기준(last_sample_ns_)도 갱신하지 않는다.
      ++clock_reject_count_;
      break;

    case TickAction::RANGE_REJECTED:
      ++range_reject_count_;
      break;
  }
}

double TickScheduler::recommended_sample_period_s(double replan_period_s) noexcept
{
  if (!std::isfinite(replan_period_s) || replan_period_s <= 0.0)
  {
    return 0.0;
  }
  return replan_period_s / SAMPLE_PERIOD_DIVISOR;
}

bool TickScheduler::to_nanoseconds(double seconds, std::int64_t & out_nanoseconds) noexcept
{
  if (!std::isfinite(seconds) || seconds < 0.0 || seconds > MAX_CLOCK_SECONDS)
  {
    return false;
  }
  const double nanoseconds = std::round(seconds * static_cast<double>(NANOSECONDS_PER_SECOND));
  if (!std::isfinite(nanoseconds) || nanoseconds < 0.0)
  {
    return false;
  }
  out_nanoseconds = static_cast<std::int64_t>(nanoseconds);
  return true;
}

double TickScheduler::replan_period_s() const noexcept
{
  return replan_period_s_;
}

bool TickScheduler::has_published() const noexcept
{
  return has_published_;
}

std::uint32_t TickScheduler::last_published_seq() const noexcept
{
  return last_published_seq_;
}

std::int64_t TickScheduler::last_sample_ns() const noexcept
{
  return last_sample_ns_;
}

std::uint64_t TickScheduler::published_count() const noexcept
{
  return published_count_;
}

std::uint64_t TickScheduler::hold_count() const noexcept
{
  return hold_count_;
}

std::uint64_t TickScheduler::missed_seq_count() const noexcept
{
  return missed_seq_count_;
}

std::uint64_t TickScheduler::resync_count() const noexcept
{
  return resync_count_;
}

std::uint64_t TickScheduler::clock_reject_count() const noexcept
{
  return clock_reject_count_;
}

std::uint64_t TickScheduler::range_reject_count() const noexcept
{
  return range_reject_count_;
}

} // namespace mrs
