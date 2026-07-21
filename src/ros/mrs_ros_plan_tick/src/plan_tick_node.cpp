/**
 * @file plan_tick_node.cpp
 * @brief `PlanTickNode` 구현 — 시계 샘플러 + QoS 정본 + 실재 결번 계측 (R-05, R-20).
 *
 * ⚠ 이 파일에 **알고리즘은 없다.** 틱 번호·발행 여부의 판정은 전부 `mrs::TickScheduler`(순수
 *   로직)가 하고, 여기 있는 것은 시계를 읽어 넘기고 결과를 와이어에 싣는 얇은 I/O 어댑터다.
 */

#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
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
  scheduler_.emplace(replan_period_s_); // 같은 조건을 순수 로직 쪽에서도 다시 검사한다
  tick_sample_period_s_ = resolve_sample_period_s();

  // QoS 정본(contract-registry §3): reliable / volatile / depth 1 /
  // deadline 2*Δt_h / liveliness AUTOMATIC lease 3*Δt_h.
  // ⚠ deadline·lease 는 **Δt_h** 기준이다. 샘플 주기가 아니다 — 와이어에 나가는 것은 여전히
  //    Δt_h 마다 한 건이고, 샘플러는 그 시점을 놓치지 않기 위한 내부 관측 수단일 뿐이다.
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
  // ⚠ 이 타이머는 **샘플러**다(R-20). 주기가 흔들려도 틱 번호·발행 시점은 흔들리지 않는다.
  sample_timer_ = this->create_timer(
    std::chrono::duration<double>(tick_sample_period_s_),
    std::bind(&PlanTickNode::on_sample_timer, this), timer_callback_group_);

  RCLCPP_INFO(
    this->get_logger(),
    "plan_tick_node started — replan_period_s=%.6f, tick_sample_period_s=%.6f (sampler only), "
    "use_sim_time=%s",
    replan_period_s_, tick_sample_period_s_,
    this->get_parameter("use_sim_time").as_bool() ? "true" : "false");
}

double PlanTickNode::resolve_sample_period_s()
{
  // 샘플 주기는 param 으로 노출한다(R-20). 0 이하 = "자동" — 권장값 Δt_h/4 를 쓴다.
  // 근거: 한 틱 구간을 4 회 관측하면 콜백을 한 번 놓쳐도 인덱스를 건너뛰지 않는다(여유 3 회).
  // 더 잦게 돌려도 틱 정확도는 좋아지지 않는다 — 정확도를 정하는 것은 `floor` 산식이다.
  const double requested = this->declare_parameter<double>("tick_sample_period_s", 0.0);
  if (!std::isfinite(requested))
  {
    throw std::invalid_argument("tick_sample_period_s must be a finite number");
  }
  if (requested <= 0.0)
  {
    return TickScheduler::recommended_sample_period_s(replan_period_s_);
  }
  if (requested > replan_period_s_ * MAX_SAMPLE_PERIOD_RATIO)
  {
    // Δt_h/2 를 넘는 샘플 주기는 콜백 한 번만 늦어도 인덱스를 건너뛴다 = 가짜 결번의 재도입.
    // 그것이 R-20 이 없앤 결함이므로 기동 시점에 거부한다(조용한 성능 저하로 두지 않는다).
    throw std::invalid_argument("tick_sample_period_s must not exceed replan_period_s / 2");
  }
  if (requested > TickScheduler::recommended_sample_period_s(replan_period_s_))
  {
    RCLCPP_WARN(
      this->get_logger(),
      "tick_sample_period_s=%.6f exceeds the recommended %.6f (= replan_period_s/4) — "
      "a single late callback now has less margin before an index is skipped",
      requested, TickScheduler::recommended_sample_period_s(replan_period_s_));
  }
  return requested;
}

void PlanTickNode::on_sample_timer()
{
  try
  {
    if (!scheduler_.has_value())
    {
      return; // 배선 오류 방어. 생성자가 타이머보다 먼저 스케줄러를 만들므로 도달 불가다.
    }
    const double now_seconds = this->get_clock()->now().seconds();
    const TickSample sample = scheduler_->peek(now_seconds);
    switch (sample.action)
    {
      case TickAction::PUBLISH:
        publish_tick(sample);
        break;
      case TickAction::HOLD:
        // 아직 다음 틱 경계가 아니다 — 대다수의 표본이 여기다. 정상이므로 로그를 남기지 않는다.
        scheduler_->commit(sample);
        break;
      case TickAction::CLOCK_REJECTED:
      case TickAction::RANGE_REJECTED:
        report_rejection(sample, now_seconds);
        scheduler_->commit(sample);
        break;
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "sample timer failed (safe no-op, no tick published): %s", e.what());
  }
  catch (...)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "sample timer failed with unknown exception (safe no-op, no tick published)");
  }
}

// R-18 구속: 시각 변환은 반드시 `seconds_to_time` 를 경유한다. 노드가 1e9 로 직접 나누면
// 음수·NaN·범위 가드가 헬퍼 밖으로 새어 나간다.
void PlanTickNode::publish_tick(const TickSample & sample)
{
  builtin_interfaces::msg::Time tick_time;
  const mrs::convert::ConvertResult result =
    mrs::convert::seconds_to_time(sample.tick_time_s, tick_time);
  if (!result.ok)
  {
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "seconds_to_time rejected tick_time %f s (reason=%s) — not published, index not consumed",
      sample.tick_time_s, mrs::convert::to_string(result.reason));
    return; // 커밋하지 않는다 ⇒ 이 인덱스는 다음 표본에서 다시 시도된다(새 결번을 만들지 않는다).
  }

  mrs_interfaces::msg::PlanTick msg;
  msg.tick_seq = sample.tick_seq;
  msg.tick_time = tick_time;
  msg.replan_period_s = replan_period_s_;

  tick_pub_->publish(msg);
  scheduler_->commit(sample);
  report_publish_anomalies(sample);
}

// ⛔ 결번을 메우지 않는다(틱 외삽 금지). 사라진 번호는 그대로 두고, **시계가 실제로 얼마나
//    뛰었는지**를 같은 줄에 남긴다 — R-20 이후 관측되는 결번은 전부 이 종류여야 한다.
void PlanTickNode::report_publish_anomalies(const TickSample & sample)
{
  if (sample.resynced)
  {
    RCLCPP_WARN(
      this->get_logger(),
      "clock went backwards (%.6f s) — t0 re-anchored, tick_seq restarts at %u (resync #%llu). "
      "Receivers must treat this as re-sync, not as a gap (contract L-15)",
      sample.clock_advance_s, sample.tick_seq,
      static_cast<unsigned long long>(scheduler_->resync_count()));
    return;
  }

  if (sample.missed_seq_count == 0U)
  {
    return;
  }

  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "REAL tick gap: clock advanced %.6f s (> replan_period_s %.6f) between samples — "
    "%u tick(s) never existed, not back-filled (total=%llu)",
    sample.clock_advance_s, replan_period_s_, sample.missed_seq_count,
    static_cast<unsigned long long>(scheduler_->missed_seq_count()));
}

void PlanTickNode::report_rejection(const TickSample & sample, double now_seconds)
{
  if (sample.action == TickAction::CLOCK_REJECTED)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "clock returned an unusable time (%f s) — no tick published (total=%llu)", now_seconds,
      static_cast<unsigned long long>(scheduler_->clock_reject_count() + 1U));
    return;
  }

  RCLCPP_ERROR_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "tick_seq out of uint32 range at t=%f s — no tick published (total=%llu)", now_seconds,
    static_cast<unsigned long long>(scheduler_->range_reject_count() + 1U));
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
