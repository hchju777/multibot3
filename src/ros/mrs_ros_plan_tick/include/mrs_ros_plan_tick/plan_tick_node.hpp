#pragma once

/**
 * @file plan_tick_node.hpp
 * @brief plan_tick_node — 동기 재계획 틱 `/plan_tick` 의 유일한 발행자 (R-05, architecture §1.7).
 *
 * ## 이 노드가 하는 일의 전부
 * **타이머 1개 + 발행 1개.** 그 외 아무 일도 하지 않는다(architecture §1.7-A, 계약 L-15).
 * 판정 근거: `sadg_service`(§1.6 이 순수 이벤트 구동으로 규정 — 자기모순)·`sim_bridge`(실물에서
 * 사라지는 노드에 정리의 전제를 심는 것은 sim 누수 금지 위반)·`ladder_orchestrator`(D-08 재시작이
 * 틱을 끊어 F5·F6 가 교락) 전부 기각되고 전용 노드로 확정됐다.
 * 이 노드의 **유일한 실패 모드는 정지**이며, 정지는 `/plan_tick` 에만 건 liveliness lease
 * (3·Δt_h, 계약 Q-9)로 전 구독자가 즉시 관측한다.
 *
 * ## 틱 = 시계의 순수 함수 (R-A1)
 *      tick_seq  = floor((t - t0) / replan_period_s)
 *      tick_time = t0 + tick_seq * replan_period_s
 * **자유 증가 카운터로 만들지 않는다.** 타이머 콜백이 늦거나 병합돼도 seq 가 표류하지 않고
 * **결번으로 드러나는 것**이 이 정의의 요점이며, 수신자가 유실을 탐지할 수 있는 유일한 근거다.
 * `tick_seq` 감소는 수신자가 **재동기**로 해석한다(계약 L-15).
 *
 * ## 타이머는 발행 주체가 아니라 **샘플러**다 (R-20 정정)
 * R-A1 은 번호에만 적용되고 **발행 시점**에는 적용되지 않았다 — 번호는 시계에서 나오는데 발행은
 * 독립적인 타이머가 정했고, 그 타이머가 공칭보다 잦게 발화하면(실측 ~11.4 Hz vs 10 Hz) 중복이
 * 억제되면서 스케줄이 밀려 **번호 하나가 영영 발행되지 않았다**([0a] 실측 결번 20~22 %).
 * 정정 후:
 * - 타이머 주기는 `tick_sample_period_s`(기본 = Δt_h/4)이며, **그 정확도가 틱 정확도를 좌우하지
 *   않는다.** 발행 시점은 오직 인덱스가 정한다.
 * - **인덱스가 직전 발행분보다 클 때에만 발행**한다 ⇒ 중복이 생기지 않으므로 중복 억제 로직도,
 *   그것이 만들던 가짜 결번도 없다.
 * - 판정 자체는 `mrs::TickScheduler`(ROS 무의존 순수 로직)가 하고 이 노드는 옮기기만 한다.
 *
 * ## ⛔ 틱 외삽 금지 — 발행 측에서도 지킨다
 * 시계가 Δt_h 넘게 점프해 번호가 사라졌더라도 **놓친 틱을 소급 발행하지 않는다.** 지어낸 틱은
 * 로봇마다 다른 스텝 지수 h 를 배포해 (A1) 지수 합의를 조용히 깨뜨린다(계약 L-15 S4).
 * 사라진 구간은 **결번 그대로** 두고, 그때 시계가 실제로 얼마나 뛰었는지를 함께 경고로 남긴다 —
 * 수리 후 관측되는 결번은 **전부 이 종류**여야 한다(R-20).
 *
 * ## 도메인 무의존
 * 이 패키지는 **어떤 도메인 패키지에도 의존하지 않는다**(architecture §2.2 통과 기준,
 * `tools/check_boundaries.sh` 검사 [4]). 유일한 추가 의존은 L2 변환 패키지 `mrs_msg_convert`
 * 이며, 그 이유는 R-18 의 구속이다 — 시각 변환은 반드시 `mrs::convert::seconds_to_time` 를
 * 경유해야 한다(음수·NaN·범위 가드가 그 헬퍼 한 곳에 있고, 노드가 손으로 쓰면 가드가 샌다).
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/plan_tick.hpp"
#include "mrs_msg_convert/convert_result.hpp"
#include "mrs_ros_plan_tick/tick_scheduler.hpp"

namespace mrs
{

/**
 * @brief `/plan_tick` 발행 전용 노드 (계약 L-15, 발행자는 시스템 전체에서 정확히 1개).
 *
 * @note 단일 발행자 강제는 이 노드가 아니라 **수신 측**과 `mrs_bringup` 의 몫이다 —
 *       각 `l4_traj_node` 가 기동 시 그래프 API 로 발행자 수를 확인해 2 이상이면 기동을
 *       거부한다(계약 L-15). 이 노드는 자기 인스턴스 수를 알 수 없으므로 검사하지 않는다.
 */
class PlanTickNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·발행자·샘플 타이머를 구성한다.
   * @throws std::invalid_argument `replan_period_s` 가 유한한 양수가 아니거나,
   *         `tick_sample_period_s` 가 비유한 또는 Δt_h/2 초과일 때. 기동 시점의 정적 파라미터
   *         정합 위반은 **조용한 폴백 대신 기동 실패**로 처리한다(nav2-reference §2-B1).
   */
  PlanTickNode();
  ~PlanTickNode() override = default;

private:
  /**
   * @brief 샘플 타이머 콜백 — 현재 시계를 한 번 재고, 새 인덱스일 때에만 발행한다.
   *
   * 예외는 전부 여기서 흡수한다(CLAUDE.md 규율 2). 안전 폴백은 **발행하지 않는 것**이다 —
   * 값이 의심스러운 틱을 내보내는 것보다 결번이 낫다(결번은 수신자가 탐지할 수 있고,
   * 틀린 틱은 탐지되지 않는다).
   * @return void
   */
  void on_sample_timer();

  /**
   * @brief 발행 판정이 난 표본을 메시지로 만들어 `/plan_tick` 에 싣고, 성공 시 커밋한다.
   *
   * 변환에 실패하면 **커밋하지 않는다** — 그 인덱스는 소모되지 않고 다음 표본에서 다시
   * 시도되므로, 변환 가드가 새 결번을 만들어내지 않는다.
   *
   * @param[in] sample 발행 판정(`TickAction::PUBLISH`)이 난 표본. 자료형 `const mrs::TickSample &`.
   * @return void
   */
  void publish_tick(const TickSample & sample);

  /**
   * @brief 거부된 표본(시계 값 불량·인덱스 범위 초과)을 스로틀 로그로 남긴다.
   * @param[in] sample 거부 판정이 난 표본. 자료형 `const mrs::TickSample &`.
   * @param[in] now_seconds 그 판정을 낳은 시계 값 [s]. 자료형 `double`. 로그에만 쓴다.
   * @return void
   */
  void report_rejection(const TickSample & sample, double now_seconds);

  /**
   * @brief 발행 직후 재동기·**실재 결번**을 경고로 남긴다 (계측을 지우지 않는다).
   * @param[in] sample 방금 발행한 표본. 자료형 `const mrs::TickSample &`.
   * @return void
   */
  void report_publish_anomalies(const TickSample & sample);

  /**
   * @brief 변환 실패를 사유별로 적립한다 (계약 §0.2 — 사유를 뭉개지 않는다).
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return void
   */
  void count_convert_failure(mrs::convert::ConvertStatus status);

  /**
   * @brief `tick_sample_period_s` 파라미터를 읽어 실제로 쓸 샘플 주기를 정한다.
   *
   * @return `double` — 샘플 타이머 주기 [s]. 파라미터가 0 이하이면 권장값(Δt_h/4)을 쓴다.
   * @throws std::invalid_argument 값이 비유한이거나 Δt_h/2 를 넘을 때. Δt_h/2 를 넘는 샘플
   *         주기는 콜백 한 번만 늦어도 인덱스를 건너뛰어 **가짜 결번을 구조적으로 재도입**한다 —
   *         그것이 바로 R-20 이 없앤 결함이므로 기동 시점에 거부한다.
   */
  [[nodiscard]] double resolve_sample_period_s();

  /** @brief `ConvertStatus` 열거자 개수 (계약 §0.2 정본표 7행 — 사유별 폐기 카운터의 길이). */
  static constexpr std::size_t CONVERT_STATUS_COUNT = 7U;
  /** @brief 반복 경고의 스로틀 주기 [ms]. 샘플 루프의 로그 폭주를 막는다(계약 §0.2 THROTTLE). */
  static constexpr std::uint32_t LOG_THROTTLE_MS = 5000U;
  /** @brief 허용하는 샘플 주기의 상한 배수 — Δt_h 의 이 배수를 넘으면 기동을 거부한다. */
  static constexpr double MAX_SAMPLE_PERIOD_RATIO = 0.5;

  rclcpp::CallbackGroup::SharedPtr timer_callback_group_; ///< MutuallyExclusive (nav2 §2-A1)
  rclcpp::Publisher<mrs_interfaces::msg::PlanTick>::SharedPtr tick_pub_; ///< `/plan_tick` 발행자
  rclcpp::TimerBase::SharedPtr sample_timer_;                            ///< 샘플 타이머(노드 시계)

  double replan_period_s_{0.1};      ///< Δt_h [s]. 계약 L-15 기본값 0.1 (theory T1 §6.1)
  double tick_sample_period_s_{0.0}; ///< 샘플 주기 [s] — 발행 정확도와 무관한 관측 주기

  /**
   * @brief 발행 판정을 내리는 순수 로직 (R-20). `optional` 인 이유는 Δt_h 를 파라미터로 읽은
   *        **뒤에야** 생성할 수 있기 때문이다 — 힙 할당 없이 생성 시점만 늦춘다.
   */
  std::optional<TickScheduler> scheduler_;

  /** @brief `ConvertStatus` 사유별 변환 실패 누적 카운터 (인덱스 = 열거값). */
  std::array<std::uint64_t, CONVERT_STATUS_COUNT> convert_failure_counts_{};

  /**
   * @brief 로그 스로틀 전용 steady 시계. 노드 시계(시뮬 시계)를 쓰지 않는다.
   *
   * `RCLCPP_*_THROTTLE` 은 마지막 기록 시각을 0 으로 초기화하고 `now >= last + interval` 일 때만
   * 통과시킨다. 시뮬 시계도 0 에서 시작하므로 **기동 직후 첫 스로틀 창이 통째로 삼켜진다** —
   * [0a] 실측에서 배관 결손이 처음에 완전히 조용했던 원인이다(0a 보고서 §4-④). 배속을 올리면
   * 스로틀 주기가 함께 빨라지고 시뮬이 멈추면 스로틀이 영원히 굳는 문제도 같은 뿌리다.
   *
   * @note 계약 §0 의 "시각은 시뮬 시계, 벽시계 혼용 금지"는 **메시지에 실리는 시각**의 규약이다.
   *       이 시계 값은 와이어에도 도메인에도 들어가지 않는다 — 진단 출력의 속도 제한 전용이며,
   *       `sim_bridge` 는 같은 이유로 이미 steady 시계를 쓰고 있었고 그 노드만 §4-④ 를 피했다.
   */
  rclcpp::Clock log_throttle_clock_{RCL_STEADY_TIME};
};

} // namespace mrs
