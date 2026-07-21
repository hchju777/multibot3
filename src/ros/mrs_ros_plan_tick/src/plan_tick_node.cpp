/**
 * @file plan_tick_node.cpp
 * @brief `PlanTickNode` 구현 — R-A1 틱 산식 + QoS 정본 + 결번·중복 계측.
 *
 * ⚠ 이 파일에 **알고리즘은 없다.** `floor((t - t0)/Δt_h)` 는 계약 L-15 가 문언으로 못박은
 *   정의이며, 이 노드는 그 정의를 그대로 옮긴 얇은 I/O 어댑터다.
 */

#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
#include <limits>
#include <stdexcept>

#include "mrs_msg_convert/common_convert.hpp"
#include "mrs_ros_plan_tick/plan_tick_node.hpp"

namespace mrs
{

PlanTickNode::PlanTickNode() : rclcpp::Node("plan_tick_node")
{
  // Δt_h. 계약 L-15 기본값 0.1 s (theory T1 §6.1, 조건 N4: Δt_h < Δt = 0.5 s).
  // ⚠ 시뮬 티어에서는 시뮬 스텝 dt 의 **정수배**여야 한다(R-A2). 그 검사는 이 노드가 아니라
  //    `mrs_bringup` 의 기동 게이트가 수행한다 — 이 노드는 dt 를 알지 못한다.
  replan_period_s_ = this->declare_parameter<double>("replan_period_s", 0.1);
  if (!std::isfinite(replan_period_s_) || replan_period_s_ <= 0.0)
  {
    // 조용한 기본값 폴백을 하지 않는다. 잘못된 주기로 낸 틱은 전 로봇의 스텝 지수 합의를
    // 조용히 깨뜨리고, 조용한 위반은 F5 결과를 무효화한다(계약 L-15).
    throw std::invalid_argument("replan_period_s must be a finite positive number");
  }

  // QoS 정본(contract-registry §3): reliable / volatile / depth 1 /
  // deadline 2*Δt_h / liveliness AUTOMATIC lease 3*Δt_h.
  // liveliness 를 거는 토픽은 `/plan_tick` **하나뿐**이다(계약 Q-9) — 이 노드의 유일한
  // 실패 모드인 정지를 전 구독자가 DDS 층에서 즉시 관측하기 위함이다.
  rclcpp::QoS qos(1);
  qos.reliable();
  qos.durability_volatile();
  qos.deadline(rclcpp::Duration::from_seconds(2.0 * replan_period_s_));
  qos.liveliness(rclcpp::LivelinessPolicy::Automatic);
  qos.liveliness_lease_duration(rclcpp::Duration::from_seconds(3.0 * replan_period_s_));

  tick_pub_ = this->create_publisher<mrs_interfaces::msg::PlanTick>("/plan_tick", qos);

  // MutuallyExclusive 콜백그룹 (nav2-reference §2-A1). 이 노드는 콜백이 하나뿐이라 사실상
  // 자명하지만, 그룹을 명시해 두면 이후 누가 콜백을 추가해도 직렬 실행이 보장된다.
  // ⚠ nav2 의 `create_callback_group(..., false)` 2인자 형태는 **전용 스레드의 별도 executor**
  //    에 수동으로 붙이는 용례다. 이 노드는 전용 스레드를 두지 않고 자신이 곧 그 전용
  //    프로세스이므로, executor 자동 등록(기본값 true)을 쓴다 — false 로 두고 등록을 잊으면
  //    타이머가 영영 돌지 않는다.
  timer_callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  // ⚠ wall timer 가 아니라 **노드 시계 타이머**다. `use_sim_time: true` 이면 `/clock` 을 따라
  //    돌아야 틱이 "시계의 순수 함수"라는 R-A1 이 성립한다. 벽시계로 돌리면 시뮬 배속에서
  //    틱과 시뮬 시각이 어긋난다. (`/clock` **발행자**만 벽시계 타이머를 쓴다 —
  //    nav2-reference §2-C2 의 부팅 데드락 회피는 `sim_bridge` 의 규율이며 여기가 아니다.)
  timer_ = this->create_timer(
    std::chrono::duration<double>(replan_period_s_), std::bind(&PlanTickNode::on_timer, this),
    timer_callback_group_);

  RCLCPP_INFO(
    this->get_logger(), "plan_tick_node started — replan_period_s=%.6f, use_sim_time=%s",
    replan_period_s_, this->get_parameter("use_sim_time").as_bool() ? "true" : "false");
}

void PlanTickNode::on_timer()
{
  try
  {
    const double now_seconds = this->get_clock()->now().seconds();

    mrs_interfaces::msg::PlanTick msg;
    if (!compute_tick_message(now_seconds, msg))
    {
      return; // 안전 폴백: 의심스러운 틱을 내보내느니 결번으로 남긴다.
    }
    if (!accept_sequence(msg.tick_seq))
    {
      return; // 같은 틱 번호의 중복 발행 억제.
    }

    tick_pub_->publish(msg);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_timer failed (safe no-op, tick skipped): %s", e.what());
  }
  catch (...)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_timer failed with unknown exception (safe no-op, tick skipped)");
  }
}

// R-18 구속: 시각 변환은 반드시 `seconds_to_time` 를 경유한다. 노드가 1e9 로 직접 나누면
// 음수·NaN·범위 가드가 헬퍼 밖으로 새어 나간다.
bool PlanTickNode::compute_tick_message(double now_seconds, mrs_interfaces::msg::PlanTick & out)
{
  std::uint32_t tick_seq = 0U;
  if (!tick_seq_from_clock(now_seconds, tick_seq))
  {
    return false;
  }

  const double tick_time_s = t0_seconds_ + static_cast<double>(tick_seq) * replan_period_s_;

  builtin_interfaces::msg::Time tick_time;
  const mrs::convert::ConvertResult result = mrs::convert::seconds_to_time(tick_time_s, tick_time);
  if (!result.ok)
  {
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "seconds_to_time rejected tick_time %f s (reason=%s) — tick skipped", tick_time_s,
      mrs::convert::to_string(result.reason));
    return false;
  }

  out.tick_seq = tick_seq;
  out.tick_time = tick_time;
  out.replan_period_s = replan_period_s_;
  return true;
}

bool PlanTickNode::tick_seq_from_clock(double now_seconds, std::uint32_t & out_seq)
{
  if (!std::isfinite(now_seconds) || now_seconds < 0.0)
  {
    ++range_guard_reject_count_;
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "clock returned an unusable time (%f s) — tick skipped", now_seconds);
    return false;
  }

  anchor_epoch(now_seconds);

  const double raw_seq = std::floor((now_seconds - t0_seconds_) / replan_period_s_);
  if (!std::isfinite(raw_seq) || raw_seq < 0.0 || raw_seq > MAX_TICK_SEQ_AS_DOUBLE)
  {
    ++range_guard_reject_count_;
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "tick_seq out of uint32 range (raw=%f) — tick skipped", raw_seq);
    return false;
  }

  out_seq = static_cast<std::uint32_t>(raw_seq);
  return true;
}

void PlanTickNode::anchor_epoch(double now_seconds)
{
  if (!t0_initialized_ || now_seconds < t0_seconds_)
  {
    t0_initialized_ = true;
    t0_seconds_ = now_seconds;
  }
}

bool PlanTickNode::accept_sequence(std::uint32_t tick_seq)
{
  if (!has_published_)
  {
    has_published_ = true;
    last_published_seq_ = tick_seq;
    return true;
  }

  // 같은 틱 경계 안에서 콜백이 두 번 돌았다. 같은 seq 를 두 번 내보내면 수신자의 결번 판정이
  // 흐려지므로 억제하고 계측만 남긴다.
  if (tick_seq == last_published_seq_)
  {
    ++duplicate_suppressed_count_;
    RCLCPP_WARN_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "duplicate tick_seq %u suppressed (total=%llu)", tick_seq,
      static_cast<unsigned long long>(duplicate_suppressed_count_));
    return false;
  }

  if (tick_seq < last_published_seq_)
  {
    ++resync_count_;
    RCLCPP_WARN(
      this->get_logger(), "clock went backwards — tick_seq %u -> %u (resync #%llu)",
      last_published_seq_, tick_seq, static_cast<unsigned long long>(resync_count_));
    last_published_seq_ = tick_seq;
    return true;
  }

  note_sequence_gap(tick_seq);
  last_published_seq_ = tick_seq;
  return true;
}

// ⛔ 결번을 메우지 않는다(틱 외삽 금지). 건너뛴 번호는 그대로 두고 카운터만 올린다 —
//    그 결번이 [0a] 의 측정 대상이다.
void PlanTickNode::note_sequence_gap(std::uint32_t tick_seq)
{
  const std::uint32_t gap = tick_seq - last_published_seq_ - 1U;
  if (gap == 0U)
  {
    return;
  }

  skipped_seq_count_ += static_cast<std::uint64_t>(gap);
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "tick callback late: %u tick(s) skipped, not back-filled (total=%llu)", gap,
    static_cast<unsigned long long>(skipped_seq_count_));
}

void PlanTickNode::count_convert_failure(mrs::convert::ConvertStatus status)
{
  const std::size_t index = static_cast<std::size_t>(status);
  if (index < convert_failure_counts_.size())
  {
    ++convert_failure_counts_[index];
  }
}

} // namespace mrs
