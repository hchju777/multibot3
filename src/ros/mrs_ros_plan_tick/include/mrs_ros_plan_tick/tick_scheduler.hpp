#pragma once

/**
 * @file tick_scheduler.hpp
 * @brief `TickScheduler` — "발행도 시계의 함수"를 실현하는 **순수 로직** (오케스트레이터 R-20).
 *
 * ## 이 파일이 존재하는 이유 (R-20 판정)
 * R-A1 은 "틱은 시계의 결정론적 함수"로 정의했으나, 그 규칙이 `tick_seq` **번호**에만 적용되고
 * **발행 시점**에는 적용되지 않았다. 번호는 `floor((t − t0)/Δt_h)` 로 시계에서 나오는데 발행은
 * 독립적인 rclcpp 타이머가 정했다. 둘이 잠겨 있지 않아 타이머가 공칭보다 잦게 발화하고
 * (실측 ~11.4 Hz vs 10 Hz), 그때마다 중복 1건이 억제되면서 스케줄이 한 주기 밀려 **틱 번호
 * 하나가 영영 발행되지 않았다**([0a] 실측 결번 20~22 %).
 *
 * ## 정정된 규칙 — 이 클래스가 구현하는 것
 * 1. **타이머는 발행 주체가 아니라 샘플러다.** 주기는 "인덱스를 놓치지 않을 만큼 빠르면 족함"
 *    이며 정확도가 틱 정확도를 좌우하지 않는다(@ref mrs::TickScheduler::recommended_sample_period_s).
 * 2. **발행 조건 = 계산된 인덱스가 직전 발행 인덱스보다 클 때에만.** 같은 인덱스를 다시 보면
 *    그냥 `HOLD` 다 — 중복이 **발생 자체를 하지 않으므로** "중복 억제 → 스케줄 밀림" 경로가
 *    구조적으로 사라진다. (구 `accept_sequence` 의 중복 억제 로직은 도달 불가 경로가 되어 제거됨.)
 * 3. **진짜 결번만 보고한다.** 인덱스가 2 이상 뛰면 시계가 Δt_h 넘게 점프했다는 뜻이므로 실재하는
 *    결번이다 — @ref mrs::TickSample::missed_seq_count 로 드러내고 메우지 않는다(틱 외삽 금지).
 *
 * ## ⛔ 바꾸지 않은 것
 * `tick_seq = floor((t − t0)/Δt_h)`, `tick_time = t0 + tick_seq·Δt_h` (R-A1) 와 R-A2(Δt_h 가
 * 시뮬 스텝의 정수배)는 **그대로**다. 바뀐 것은 그 산식을 **언제 발행에 쓰는가**뿐이다.
 *
 * @note **평가는 정수 나노초 격자에서 한다**(산식이 아니라 산식의 *계산 방법*). `double` 로
 *       `(t − t0)/Δt_h` 를 나누면 진짜 몫이 정수인 경우조차 이진 표현이 정수 바로 아래로 떨어져
 *       `floor` 가 1 을 잃는다. 실측: `sim_step_s = Δt_h = 0.1` 구성에서 **인덱스의 약 10 %가
 *       한 번 멈췄다가 2 씩 뛴다** — 시계는 정확히 Δt_h 씩 갔는데도 그렇다. 그 상태로 두면
 *       규칙 3 의 "진짜 결번"이 부동소수 잡음으로 오염되어 R-20 의 요구(관측되는 결번은 전부
 *       시계 점프여야 한다)를 만족할 수 없다. ROS 시각은 본래 정수 나노초이고 `/clock` 도
 *       나노초 격자 위에 있으므로(R-20 이 승인한 `seconds_to_time` 반올림), 정수 연산이
 *       **더 충실한** 구현이다.
 *
 * ## 의존성 없음 = unit 티어에서 검증 가능
 * 이 헤더는 rclcpp·ROS 메시지·`mrs_msg_convert` 어디에도 의존하지 않는다. `plan_tick_node` 는
 * 이 결정을 그대로 옮기는 I/O 어댑터가 되고, 발행 규칙 자체는 ROS 없이 결정론적으로 테스트된다
 * (architecture §6 unit 티어 정의 / `tools/check_boundaries.sh` 검사 [6][7]).
 */

#include <cstdint>

namespace mrs
{

/**
 * @brief 한 번의 샘플이 낳는 판정 (@ref mrs::TickScheduler::peek 의 결과 종류).
 */
enum class TickAction : std::uint8_t
{
  /** @brief 새 인덱스다 — 발행한다. */
  PUBLISH = 0,
  /** @brief 인덱스가 직전 발행분과 같다 — 발행하지 않는다. **정상이며 대다수의 샘플이 여기다.** */
  HOLD = 1,
  /** @brief 시계 값이 비유한·음수·표현 범위 밖이다 — 발행 금지(값이 의심스러운 틱보다 결번이 낫다). */
  CLOCK_REJECTED = 2,
  /** @brief 인덱스가 `uint32` 표현 범위를 넘었다 — 발행 금지. */
  RANGE_REJECTED = 3,
};

/**
 * @brief 한 시점의 시계 표본에 대한 판정 결과 (부작용 없는 값 객체).
 *
 * @note `peek` 는 이 구조체를 만들기만 하고 상태를 바꾸지 않는다. 상태 반영은 호출자가
 *       @ref mrs::TickScheduler::commit 으로 **명시적으로** 한다 — 발행에 실패했는데
 *       "발행했다"고 기록해 인덱스를 조용히 잃는 일이 없게 하기 위함이다.
 */
struct TickSample
{
  /** @brief 이 표본에 대한 판정. 자료형 `mrs::TickAction`. */
  TickAction action{TickAction::HOLD};
  /** @brief 계산된 틱 번호 `floor((t − t0)/Δt_h)`. 자료형 `std::uint32_t`. 거부 시 의미 없음. */
  std::uint32_t tick_seq{0U};
  /** @brief 계획 기준 시각 `t0 + tick_seq·Δt_h` [s]. 자료형 `double`. 거부 시 의미 없음. */
  double tick_time_s{0.0};
  /**
   * @brief 직전 커밋 표본 이후 시계가 전진한 양 [s]. 자료형 `double`. 첫 표본이면 0.
   *
   * @ref missed_seq_count 가 0 보다 클 때 **그 결번이 진짜임을 같은 줄에서 증명**하는 값이다
   * (이 값이 Δt_h 를 넘었으므로 번호가 사라졌다). 진단 로그 전용이며 와이어에 실리지 않는다.
   */
  double clock_advance_s{0.0};
  /**
   * @brief 이 표본이 커밋되면 t0 가 될 시각 [ns]. 자료형 `std::int64_t`. 재동기 시에만 바뀐다.
   * @note 초가 아니라 나노초인 이유는 `peek`→`commit` 왕복에서 값이 **한 비트도 변하면 안 되기**
   *       때문이다. 실물 시각(약 1.8e18 ns)은 `double` 왕복 시 약 256 ns 가 흔들린다.
   */
  std::int64_t epoch_ns{0};
  /** @brief 이 표본을 뜬 시계 값 [ns]. 자료형 `std::int64_t`. 커밋 시 역행 탐지 기준이 된다. */
  std::int64_t sampled_at_ns{0};
  /**
   * @brief **실재하는** 결번의 개수 (직전 발행 번호와 이번 번호 사이에 빠진 번호 수).
   *
   * 자료형 `std::uint32_t`. 0 보다 크다는 것은 두 샘플 사이에 시계가 Δt_h 를 넘겨 점프했다는
   * 뜻이다 — 샘플러 지터로는 이 값이 커질 수 없다(그때는 인덱스가 그대로여서 `HOLD` 다).
   */
  std::uint32_t missed_seq_count{0U};
  /** @brief 시계 역행을 탐지해 t0 를 다시 고정하는 표본인지 여부. 자료형 `bool`. */
  bool resynced{false};
};

/**
 * @brief 시계 표본으로부터 "발행할 것인가·무엇을 발행할 것인가"를 결정하는 순수 상태기계.
 *
 * 사용 규약(2단계):
 * @code
 *   const mrs::TickSample sample = scheduler.peek(clock_seconds);
 *   if (sample.action == mrs::TickAction::PUBLISH && publish_succeeded(sample))
 *   {
 *     scheduler.commit(sample);   // 발행에 성공한 뒤에만 인덱스를 소모한다
 *   }
 * @endcode
 * `PUBLISH` 를 커밋하지 않으면 그 인덱스는 소모되지 않으므로 **다음 샘플에서 다시 시도**된다.
 */
class TickScheduler
{
public:
  /**
   * @brief 재계획 주기를 고정해 스케줄러를 만든다.
   *
   * @param[in] replan_period_s Δt_h [s]. 자료형 `double`. 유한한 양수여야 하며, 나노초 격자에서
   *            표현 가능한 값이어야 한다(반올림 후 1 ns 이상).
   * @throws std::invalid_argument 주기가 유한한 양수가 아니거나 나노초 격자에서 0 이 될 때.
   *         **조용한 기본값 폴백을 하지 않는다** — 잘못된 주기로 낸 틱은 전 로봇의 스텝 지수
   *         합의를 조용히 깨뜨린다(계약 L-15).
   */
  explicit TickScheduler(double replan_period_s);

  /**
   * @brief 시계 표본 하나를 보고 판정만 내린다(상태 불변).
   *
   * @param[in] now_seconds 노드 시계의 현재 시각 [s]. 자료형 `double`. `use_sim_time: true` 이면
   *            `/clock` 기준, 실물이면 시스템 시계 기준이다 — **같은 코드가 돈다**(architecture
   *            §1.7-B "코드 경로가 하나").
   * @return `mrs::TickSample` — 판정·틱 번호·기준 시각·실재 결번 수. `action` 이 `PUBLISH` 가
   *         아니면 `tick_seq`·`tick_time_s` 는 의미가 없다.
   */
  [[nodiscard]] TickSample peek(double now_seconds) const noexcept;

  /**
   * @brief 표본의 판정을 상태·계수기에 반영한다.
   *
   * @param[in] sample @ref peek 가 돌려준 표본. 자료형 `const mrs::TickSample &`.
   *            `PUBLISH` 를 커밋하면 그 인덱스가 소모되어 다시 발행되지 않는다.
   * @return void
   */
  void commit(const TickSample & sample) noexcept;

  /**
   * @brief 샘플 주기의 권장 기본값을 돌려준다 — **Δt_h / 4**.
   *
   * 근거: R-20 이 "타이머는 샘플러이며 주기는 인덱스를 놓치지 않을 만큼 빠르면 족하다(Δt_h/4
   * 이하 권장)"로 규정했다. 4 로 나누는 이유는 한 틱 구간을 **4 회** 관측해, 콜백 한 번을
   * 통째로 놓치거나 실행이 한 번 늦어도 인덱스가 넘어가지 않게 하는 것이다(여유 3 회).
   * 그보다 훨씬 잦게 돌릴 이유는 없다 — 발행 시점의 정확도를 정하는 것은 샘플 주기가 아니라
   * `floor` 산식이고, 샘플을 늘려도 **틱 정확도는 좋아지지 않고 CPU 만 쓴다**.
   *
   * @param[in] replan_period_s Δt_h [s]. 자료형 `double`.
   * @return `double` — 권장 샘플 주기 [s]. 입력이 유한한 양수가 아니면 0 을 돌려준다.
   */
  [[nodiscard]] static double recommended_sample_period_s(double replan_period_s) noexcept;

  /** @brief Δt_h [s] 를 돌려준다. @return `double` — 생성자가 고정한 재계획 주기. */
  [[nodiscard]] double replan_period_s() const noexcept;
  /** @brief 1건이라도 발행했는지. @return `bool` — 발행 이력이 있으면 true. */
  [[nodiscard]] bool has_published() const noexcept;
  /** @brief 마지막으로 발행한 번호. @return `std::uint32_t` — 발행 이력이 없으면 0. */
  [[nodiscard]] std::uint32_t last_published_seq() const noexcept;
  /** @brief 직전 커밋된 표본의 시계 값 [ns]. @return `std::int64_t` — 표본이 없으면 0. */
  [[nodiscard]] std::int64_t last_sample_ns() const noexcept;
  /** @brief 발행 누적 수. @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t published_count() const noexcept;
  /** @brief 같은 인덱스라 발행하지 않은 샘플 누적 수. @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t hold_count() const noexcept;
  /** @brief **실재 결번** 누적 수(시계 점프로 사라진 번호). @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t missed_seq_count() const noexcept;
  /** @brief 시계 역행으로 t0 를 다시 고정한 횟수. @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t resync_count() const noexcept;
  /** @brief 시계 값이 쓸 수 없어 거부한 샘플 수. @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t clock_reject_count() const noexcept;
  /** @brief 인덱스가 `uint32` 범위를 넘어 거부한 샘플 수. @return `std::uint64_t`. */
  [[nodiscard]] std::uint64_t range_reject_count() const noexcept;

  /** @brief 1 초의 나노초 수. */
  static constexpr std::int64_t NANOSECONDS_PER_SECOND = 1000000000LL;
  /**
   * @brief 받아들이는 시계 값의 상한 [s].
   *
   * `int64` 나노초(약 9.22e18 ns ≈ 9.22e9 s)를 넘기지 않기 위한 가드다. 실물 경로의 시스템
   * 시각(약 1.8e9 s)은 이 안에 넉넉히 든다.
   */
  static constexpr double MAX_CLOCK_SECONDS = 9.0e9;

private:
  /**
   * @brief 초 단위 시각을 정수 나노초로 반올림한다(범위 가드 포함).
   * @param[in] seconds 변환할 시각 [s]. 자료형 `double`.
   * @param[out] out_nanoseconds 변환 결과 [ns]. 자료형 `std::int64_t`. 실패 시 불변.
   * @return `bool` — 유한·비음수·상한 이내이면 true.
   */
  [[nodiscard]] static bool to_nanoseconds(double seconds, std::int64_t & out_nanoseconds) noexcept;

  /**
   * @brief 표본의 기준 시각(t0)·시계 역행 여부·시계 전진량을 채운다.
   * @param[in,out] sample `sampled_at_ns` 가 채워진 표본. 자료형 `mrs::TickSample &`.
   *                `epoch_ns`·`resynced`·`clock_advance_s` 가 설정된다.
   * @return void
   */
  void anchor(TickSample & sample) const noexcept;

  /**
   * @brief R-A1 산식으로 `tick_seq`·`tick_time_s` 를 채운다 (정수 나노초 평가).
   * @param[in,out] sample `sampled_at_ns`·`epoch_ns` 가 채워진 표본. 자료형 `mrs::TickSample &`.
   * @return `bool` — 채우기에 성공하면 true. 범위를 벗어나면 false 이며 `action` 이
   *         `RANGE_REJECTED` 로 설정된다.
   */
  [[nodiscard]] bool fill_index(TickSample & sample) const noexcept;

  /**
   * @brief 발행할지(`PUBLISH`) 보류할지(`HOLD`) 판정하고 **실재 결번** 수를 센다.
   * @param[in,out] sample 인덱스까지 채워진 표본. 자료형 `mrs::TickSample &`.
   *                `action` 과 `missed_seq_count` 가 설정된다.
   * @return void
   */
  void classify(TickSample & sample) const noexcept;

  std::int64_t period_ns_;            ///< Δt_h [ns] — 나노초 격자 위의 정본값
  double replan_period_s_;            ///< Δt_h [s] — 메시지에 싣는 값(생성자 입력 그대로)
  std::int64_t epoch_ns_{0};          ///< R-A1 의 t0 [ns]
  std::int64_t last_sample_ns_{0};    ///< 마지막으로 커밋된 표본의 시계 값 [ns] — 역행 탐지 기준
  bool anchored_{false};              ///< t0 가 고정됐는지 여부
  bool has_published_{false};         ///< 1건이라도 발행했는지 여부
  std::uint32_t last_published_seq_{0U}; ///< 마지막으로 발행한 `tick_seq`

  std::uint64_t published_count_{0U};    ///< 발행 누적
  std::uint64_t hold_count_{0U};         ///< 같은 인덱스 보류 누적
  std::uint64_t missed_seq_count_{0U};   ///< **실재** 결번 누적 (시계 점프)
  std::uint64_t resync_count_{0U};       ///< 시계 역행 재동기 누적
  std::uint64_t clock_reject_count_{0U}; ///< 시계 값 거부 누적
  std::uint64_t range_reject_count_{0U}; ///< 인덱스 범위 거부 누적
};

} // namespace mrs
