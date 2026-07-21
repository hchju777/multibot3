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
 * ## ⛔ 틱 외삽 금지 — 발행 측에서도 지킨다
 * 콜백이 늦어 여러 틱 경계를 건너뛰었더라도 **놓친 틱을 소급 발행하지 않는다.** 지어낸 틱은
 * 로봇마다 다른 스텝 지수 h 를 배포해 (A1) 지수 합의를 조용히 깨뜨린다(계약 L-15 S4).
 * 건너뛴 구간은 **결번 그대로** 두고 카운터로만 남긴다 — 그것이 [0a] 의 측정 대상이다.
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
#include <limits>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/plan_tick.hpp"
#include "mrs_msg_convert/convert_result.hpp"

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
   * @brief 노드를 생성하고 파라미터·발행자·타이머를 구성한다.
   * @throws std::invalid_argument `replan_period_s` 가 유한한 양수가 아닐 때. 기동 시점의
   *         정적 파라미터 정합 위반은 **조용한 폴백 대신 기동 실패**로 처리한다
   *         (nav2-reference §2-B1: 정적 파라미터 정합은 기동 게이트에서 판정).
   */
  PlanTickNode();
  ~PlanTickNode() override = default;

private:
  /**
   * @brief 주기 타이머 콜백 — 현재 시계로부터 틱을 계산해 최대 1건 발행한다.
   *
   * 예외는 전부 여기서 흡수한다(CLAUDE.md 규율 2). 안전 폴백은 **발행하지 않는 것**이다 —
   * 값이 의심스러운 틱을 내보내는 것보다 결번이 낫다(결번은 수신자가 탐지할 수 있고,
   * 틀린 틱은 탐지되지 않는다).
   * @return void
   */
  void on_timer();

  /**
   * @brief 현재 시각으로부터 `PlanTick` 메시지를 채운다 (R-A1 순수 산식 + 시각 변환 가드).
   *
   * @param[in] now_seconds 노드 시계의 현재 시각 [s]. 자료형 `double`.
   *            `use_sim_time: true` 이면 `/clock` 기준, 실물이면 시스템 시계 기준이다.
   * @param[out] out 채울 틱 메시지. 자료형 `mrs_interfaces::msg::PlanTick`.
   *             실패 시 내용은 변경되지 않는다.
   * @return `bool` — 채우기에 성공하면 true. 시각이 비유한·음수이거나, `tick_seq` 가
   *         `uint32` 표현 범위를 넘거나, 시각 변환 가드에 걸리면 false(발행 금지).
   */
  [[nodiscard]] bool compute_tick_message(double now_seconds, mrs_interfaces::msg::PlanTick & out);

  /**
   * @brief 현재 시각으로부터 `tick_seq` 를 구한다 (R-A1 의 `floor((t - t0)/Δt_h)`).
   *
   * @param[in] now_seconds 노드 시계의 현재 시각 [s]. 자료형 `double`.
   * @param[out] out_seq 계산된 틱 번호. 자료형 `std::uint32_t`.
   *             실패 시 내용은 변경되지 않는다.
   * @return `bool` — 성공하면 true. 시각이 비유한·음수이거나 결과가 `uint32` 범위를
   *         벗어나면 false.
   */
  [[nodiscard]] bool tick_seq_from_clock(double now_seconds, std::uint32_t & out_seq);

  /**
   * @brief R-A1 의 기준 시각 t0 를 고정하거나(최초) 시계 역행 시 다시 고정한다.
   *
   * 생성자 시점에 고정하지 않는 이유: `use_sim_time` 에서 `/clock` 도착 전 시계는 0 이고,
   * 그 0 을 t0 로 삼으면 첫 틱 번호가 폭주한다. 시계 역행(시뮬 재시작)은 t0 를 다시 고정하며,
   * 그 결과 감소한 `tick_seq` 를 수신자는 결번이 아니라 **재동기**로 해석한다(계약 L-15).
   *
   * @param[in] now_seconds 유효성 검사를 통과한 현재 시각 [s]. 자료형 `double`.
   * @return void
   */
  void anchor_epoch(double now_seconds);

  /**
   * @brief 계산된 `tick_seq` 를 발행해도 되는지 판정하고 중복·재동기를 계측한다.
   *
   * 중복(같은 seq 재발행)만 막는다. **결번은 막지 않는다** — 결번을 메우는 것이 곧 틱 외삽이다.
   *
   * @param[in] tick_seq 이번에 계산된 틱 번호. 자료형 `std::uint32_t`.
   * @return `bool` — 발행해야 하면 true, 직전과 같은 번호라 억제해야 하면 false.
   */
  [[nodiscard]] bool accept_sequence(std::uint32_t tick_seq);

  /**
   * @brief 직전 발행 번호와의 간격을 보고 결번을 계측한다 (메우지 않는다).
   * @param[in] tick_seq 이번에 발행할 틱 번호. 자료형 `std::uint32_t`.
   *            `last_published_seq_` 보다 크다고 전제한다.
   * @return void
   */
  void note_sequence_gap(std::uint32_t tick_seq);

  /**
   * @brief 변환 실패를 사유별로 적립한다 (계약 §0.2 — 사유를 뭉개지 않는다).
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return void
   */
  void count_convert_failure(mrs::convert::ConvertStatus status);

  /** @brief `ConvertStatus` 열거자 개수 (계약 §0.2 정본표 7행 — 사유별 폐기 카운터의 길이). */
  static constexpr std::size_t CONVERT_STATUS_COUNT = 7U;
  /** @brief `tick_seq`(`uint32`)가 담을 수 있는 최대값의 실수 표현 — 범위 가드 상한. */
  static constexpr double MAX_TICK_SEQ_AS_DOUBLE =
    static_cast<double>(std::numeric_limits<std::uint32_t>::max());
  /** @brief 반복 경고의 스로틀 주기 [ms]. 10 Hz 루프의 로그 폭주를 막는다(계약 §0.2 THROTTLE). */
  static constexpr std::uint32_t LOG_THROTTLE_MS = 5000U;

  rclcpp::CallbackGroup::SharedPtr timer_callback_group_; ///< MutuallyExclusive (nav2 §2-A1)
  rclcpp::Publisher<mrs_interfaces::msg::PlanTick>::SharedPtr tick_pub_; ///< `/plan_tick` 발행자
  rclcpp::TimerBase::SharedPtr timer_;                                   ///< 주기 타이머(노드 시계)

  double replan_period_s_{0.1}; ///< Δt_h [s]. 계약 L-15 기본값 0.1 (theory T1 §6.1)
  double t0_seconds_{0.0};      ///< R-A1 의 t0 [s] — 첫 유효 콜백 시각에 고정된다
  bool t0_initialized_{false};  ///< t0 가 고정됐는지 여부

  std::uint32_t last_published_seq_{0U}; ///< 마지막으로 발행한 `tick_seq`
  bool has_published_{false};            ///< 1건이라도 발행했는지 여부

  std::uint64_t skipped_seq_count_{0U};          ///< 발행 측 결번 누적 — [0a] 측정 대상
  std::uint64_t duplicate_suppressed_count_{0U}; ///< 같은 seq 중복 억제 횟수 — [0a] 측정 대상
  std::uint64_t resync_count_{0U};               ///< 시계 역행으로 t0 를 재고정한 횟수
  std::uint64_t range_guard_reject_count_{0U};   ///< 시각·범위 가드로 발행을 포기한 횟수

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
