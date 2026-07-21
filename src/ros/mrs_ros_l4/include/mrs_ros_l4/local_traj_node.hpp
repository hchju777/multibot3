#pragma once

/**
 * @file local_traj_node.hpp
 * @brief l4_traj_node — 로봇 1대의 온보드 노드. **[0a] tracer bullet 판(QP 없음)**.
 *
 * ## 이 파일이 지금 무엇인가 (정직 고지)
 * 확정서 D-07 의 L4 는 diff-drive NID-LSC QP 분산 MPC 다. **이 구현에는 그것이 없다.**
 * [0a] 의 목적은 연구가 아니라 **배관 실측**이므로(architecture §7, 오케스트레이터 R-01)
 * 이 노드는 다음만 한다:
 *  1. `ExecutionWindow` 를 실제 스키마·실제 변환으로 수신하고 세그먼트를 순서대로 소비한다.
 *  2. 소비 중 상수 전진 `cmd_vel` 을 낸다(QP 없음 — `mrs_ros_l4/tracer_follower.hpp` 머리말).
 *  3. 창 소진 임박 시 **실제 `EscalationReport`** 를 발행한다 — [0a] 가 재려는 상향 경로다.
 *  4. `/plan_tick` 을 구독해 도착 지터·결번을 계측한다(틱 외삽 금지 — R-05).
 *  5. `LocalPlanShare` 를 **매 틱 발행**하고 이웃 전체를 **전 구독**하여 지연·유실·실측 크기를
 *     기록한다(architecture §1.8-B·§1.8-C, Q-18).
 *
 * ⚠ **[0a] 통과는 [0] 통과가 아니다**(R-01 조건 1~3). 여기서 나온 수치로 실험 N·반복 수를
 * 확정하지 않는다.
 *
 * ## 실행 구조 (nav2-reference §2-A1 적용, 적용 범위를 좁힌 근거 포함)
 * 전 구독·발행을 **하나의 `MutuallyExclusive` 콜백그룹**에 묶는다. `Reentrant`·
 * `MultiThreadedExecutor` 는 쓰지 않는다(Nav2 클론 전체 사용 0건). A1 의 나머지 절반인
 * "전용 스레드의 별도 SingleThreadedExecutor" 는 **적용하지 않는다** — 그 격리가 값을 내는 것은
 * 블로킹 작업(sadg 의 R2 MILP)이 같은 실행자를 점유할 때이고, [0a] 의 l4 에는 블로킹 작업이
 * 하나도 없다. 반대로 스레드를 나누면 창·관측·틱 상태에 잠금이 필요해지는데, N=2 에서
 * `local_plan_share` 팬아웃은 틱당 1건이라 격리로 얻을 것이 없다. 근거 없는 병렬화는 [0a] 가
 * 재려는 지터 자체를 오염시킨다.
 *
 * ## 기동 게이트 (R-05)
 * `/plan_tick` 발행자가 2 개 이상이면 기동을 거부한다. 판정은 `mrs::is_single_publisher_ok` 가
 * 이미 갖고 있으므로 그대로 쓴다. 검사는 **벽시계 1 회성 타이머**로 지연 실행한다 — 생성자
 * 시점에는 DDS discovery 가 끝나지 않았고, `use_sim_time: true` 인데 `/clock` 이 아직 없으면
 * 시뮬 시계 타이머는 영원히 발화하지 않아 부팅 데드락이 된다(nav2-reference §2-C2 의 이유를
 * 부팅 경로에 그대로 적용).
 *
 * ## 시각·변환 규율
 *  - `builtin_interfaces/Time` ↔ `double` 변환은 **반드시** `mrs::convert::time_to_seconds` /
 *    `seconds_to_time` 를 거친다(오케스트레이터 R-18 구속). `1e9` 직접 산술 금지.
 *  - 메시지 ↔ 도메인 변환은 **전부** `mrs_msg_convert` 를 거친다. 이 노드는 메시지 필드를 도메인
 *    필드에 직접 대입하지 않는다(계약 §0.1 V3).
 *  - 판정은 convert, **보고는 노드**(R-15 (b)=b3): 변환 실패 시 이 노드가 사유별 카운터를
 *    적립하고 THROTTLE 로그를 내고 안전 폴백한다.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <builtin_interfaces/msg/time.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/u_int64.hpp>

#include "mrs/contract_types.hpp"
#include "mrs/i_sim_backend.hpp"
#include "mrs/view_ids.hpp"
#include "mrs_msg_convert/convert_result.hpp"

#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/local_plan_share.hpp"
#include "mrs_interfaces/msg/plan_tick.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"

namespace mrs
{

/**
 * @brief 이웃 1대의 `LocalPlanShare` 수신 계측 (§1.8-C 산술의 실측 검증용, Q-18).
 */
struct NeighborShareStats
{
  std::uint64_t received{0};      ///< 변환까지 성공한 수신 건수
  std::uint64_t discarded{0};     ///< 변환 실패로 폐기한 건수
  std::uint64_t gap_total{0};     ///< `tick_seq` 결번 누적 (유실 추정치)
  std::uint32_t last_tick_seq{0}; ///< 마지막으로 본 `tick_seq`
  bool has_last_tick{false};      ///< `last_tick_seq` 유효 여부
  double max_latency_s{0.0};      ///< 관측된 최대 전달 지연 [s] (발행 stamp → 수신 시각)
};

/**
 * @brief L4 온보드 노드 (로봇 1대분) — [0a] tracer bullet 판.
 */
class LocalTrajNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·콜백그룹·전 토픽을 배선한다.
   * @note R-05 기동 게이트는 생성자가 아니라 1 회성 벽시계 타이머에서 검사한다(파일 머리말).
   */
  LocalTrajNode();
  ~LocalTrajNode() override = default;

private:
  // ── 콜백 ────────────────────────────────────────────────────────────────────
  /**
   * @brief `/map_registry/roadmap_version`(transient_local) 수신 — 기대 뷰 스코프를 학습한다.
   * @param[in] msg 지도 버전. 자료형 `std_msgs::msg::UInt64::SharedPtr`. 0 은 런타임 금지값이다.
   * @return void
   */
  void on_roadmap_version(const std_msgs::msg::UInt64::SharedPtr msg);

  /**
   * @brief `/robot_{id}/execution_window` 수신 — 스코프 대조 후 수용하거나 폐기한다.
   * @param[in] msg 실행 창. 자료형 `mrs_interfaces::msg::ExecutionWindow::SharedPtr`.
   * @return void
   */
  void on_execution_window(const mrs_interfaces::msg::ExecutionWindow::SharedPtr msg);

  /**
   * @brief `/plan_tick` 수신 — 지터·결번 계측 후 한 틱 분의 소비·발행을 수행한다.
   * @param[in] msg 재계획 틱. 자료형 `mrs_interfaces::msg::PlanTick::SharedPtr`.
   * @return void
   */
  void on_plan_tick(const mrs_interfaces::msg::PlanTick::SharedPtr msg);

  /**
   * @brief `/robot_{id}/robot_state` 수신 — 자기 관측을 갱신한다.
   * @param[in] msg 관측 상태. 자료형 `mrs_interfaces::msg::RobotState::SharedPtr`.
   * @return void
   * @note 폐기 시 **마지막 관측을 유지**한다(안전 폴백 — 계약 L-11 수신 규약).
   */
  void on_robot_state(const mrs_interfaces::msg::RobotState::SharedPtr msg);

  /**
   * @brief 이웃 `/robot_{j}/local_plan_share` 수신 — 지연·유실·실측 크기를 기록한다.
   * @param[in] neighbor_robot_id 발신 이웃 로봇 id. 자료형 `std::uint16_t`.
   * @param[in] msg 이웃의 채택 계획. 자료형 `mrs_interfaces::msg::LocalPlanShare::SharedPtr`.
   * @return void
   * @note [0a] 는 QP 가 없어 이 값을 **쓰지 않는다** — 계측만 한다. 신선도 판정(`fresh_j(h-1)`)은
   *       `mrs_local_traj::NeighborFreshnessTracker` 의 소관이며 [2] 에서 붙는다.
   */
  void on_neighbor_plan_share(
    std::uint16_t neighbor_robot_id, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg);

  /**
   * @brief R-05 기동 게이트 — `/plan_tick` 발행자가 2 개 이상이면 기동을 거부한다.
   * @return void
   */
  void check_startup_gates();

  // ── 창 수용 하위 단계 ───────────────────────────────────────────────────────
  /**
   * @brief 변환된 창을 불변식 대조 후 수용한다 (W1 단조성, 개정 종류 분기).
   * @param[in] window 변환에 성공한 도메인 실행 창. 자료형 `mrs::ExecutionWindow`.
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   * @note 역행 `window_seq` 는 폐기한다(계약 L-01 실패 거동 — T2 P5 릴리스 단조성).
   */
  void apply_window(const mrs::ExecutionWindow & window, double now_s);

  /**
   * @brief `REVISION_TRUNCATE` 통지를 처리한다 (커밋 접두를 자르면 **거부**).
   * @param[in] window 변환에 성공한 TRUNCATE 통지. 자료형 `mrs::ExecutionWindow`.
   * @return void
   * @note [0a] 의 "커밋 접두"는 **이미 소비한 세그먼트 접두**다 — QP 가 없으므로 커밋 볼록포가
   *       존재하지 않는다. 계약 L-01 의 거부 규칙을 이 대응물 위에서 그대로 집행한다.
   *       거부 시 `CommitStatus.last_rejected_window_seq` 로 보고해야 하나 `/commit_status` 는
   *       [0a] 최소 토픽이 아니므로(§7) 로그·카운터로만 남긴다 — **[2] 전 해소 대상**.
   */
  void apply_window_truncate(const mrs::ExecutionWindow & window);

  // ── 틱 처리 하위 단계 (콜백을 얇게 유지하기 위한 분해) ──────────────────────
  /**
   * @brief 틱 도착 지터와 `tick_seq` 결번을 기록한다.
   * @param[in] msg 수신한 틱. 자료형 `mrs_interfaces::msg::PlanTick`.
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void record_tick_health(const mrs_interfaces::msg::PlanTick & msg, double now_s);

  /**
   * @brief 창 소비 조건을 판정해 필요하면 세그먼트 인덱스를 전진시킨다.
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void advance_segment_if_due(double now_s);

  /**
   * @brief 창 상태에 따른 `cmd_vel` 을 발행한다 (실패 시 정지 지령으로 폴백).
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void publish_cmd_vel(double now_s);

  /**
   * @brief 수신한 창의 발행→수신 지연을 기록한다 ([0a] 핵심 실측량).
   * @param[in] stamp 창 메시지의 `header.stamp`. 자료형 `builtin_interfaces::msg::Time`.
   * @param[in] window_seq 그 창의 seq. 자료형 `std::uint32_t`.
   * @param[in] now_s 수신 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void log_window_latency(
    const builtin_interfaces::msg::Time & stamp, std::uint32_t window_seq, double now_s);

  /**
   * @brief `LocalPlanShare` 의 **더미** 제어점 배열을 만든다 (규격 크기는 그대로).
   * @param[in] point_count 제어점 배열 길이 `2*H*(n+1)`. 자료형 `std::size_t`.
   * @return `std::vector<double>` — 길이 @p point_count 의 유한값 배열. 관측을 보유하면 마지막
   *         몸체 위치의 반복("제자리 계획"), 아니면 전부 0.
   * @note ⛔ 이것은 채택 계획이 아니다. [2] 에서 `ILocalPlanner` 의 `control_points` 로 대체된다.
   */
  [[nodiscard]] std::vector<double> build_share_control_points(std::size_t point_count) const;

  /**
   * @brief `LocalPlanShare` 의 실측 직렬화 크기를 1 회 기록한다 (Q-18 / §1.8-C 산술 검증).
   * @param[in] message 방금 발행한 메시지. 자료형 `mrs_interfaces::msg::LocalPlanShare`.
   * @param[in] point_count 제어점 배열 길이. 자료형 `std::size_t`.
   * @return void
   */
  void log_share_wire_size(
    const mrs_interfaces::msg::LocalPlanShare & message, std::size_t point_count);

  /**
   * @brief 더미 페이로드로 `LocalPlanShare` 를 발행한다 (매 틱, 규격 크기 그대로).
   * @param[in] tick_seq 이 틱의 스텝 지수 h. 자료형 `std::uint32_t`.
   * @param[in] replan_period_s 틱이 실어 온 Δt_h [s]. 자료형 `double`. `shift_ratio_u` 의 분자다.
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
   * @return void
   */
  void publish_plan_share(std::uint32_t tick_seq, double replan_period_s, double now_s);

  /**
   * @brief 창 소진이 임박했으면 `EscalationReport(WINDOW_EXPIRING)` 를 발행한다.
   * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
   * @return void
   * @note 창 1 개당 1 회만 발행한다(`escalated_window_seq_`) — 매 틱 발행하면 상향 경로가
   *       포화되어 [0a] 가 재려는 왕복 지연이 큐잉 지연으로 바뀐다.
   */
  void publish_escalation_if_due(double now_s);

  // ── 보조 ────────────────────────────────────────────────────────────────────
  /**
   * @brief 현재 시뮬 시각을 초로 얻는다 (R-18 구속 — 가드 헬퍼 경유).
   * @param[out] out_seconds 시뮬 시계 기준 절대시각 [s]. 자료형 `double`.
   * @return `bool` — 변환 성공이면 true. false 이면 호출자는 그 틱의 작업을 건너뛴다.
   */
  [[nodiscard]] bool now_seconds(double & out_seconds);

  /**
   * @brief 변환 실패를 사유별로 적립하고 THROTTLE 로그를 남긴다 (R-15 (b)=b3).
   * @param[in] topic_label 폐기가 일어난 경로 이름. 자료형 `const char *`.
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return void
   */
  void count_discard(const char * topic_label, mrs::convert::ConvertStatus status);

  /**
   * @brief 누적 폐기 카운터를 사유별로 로그에 덤프한다 (사후 감사용).
   * @return void
   */
  void log_discard_summary();

  // ── 파라미터 ────────────────────────────────────────────────────────────────
  int robot_id_{0};                              ///< param `robot_id`
  std::vector<std::int64_t> neighbor_robot_ids_; ///< param `neighbor_robot_ids` (전 구독 대상)
  double follow_speed_mps_{0.2};        ///< param — 창 실행 중 상수 전진 속도 [m/s]. **[0a] 잠정값**
  double segment_timeout_s_{1.0};       ///< param — 세그먼트 체류 상한 [s]. **[0a] 실측 대상**
  std::size_t escalation_remaining_segments_{1}; ///< param — 소진 임박 세그먼트 임계. **[0a] 잠정값**
  double escalation_lead_time_s_{0.5};  ///< param — 만료 선행 임계 [s]. **[0a] 실측 대상**
  int share_num_segments_{4};           ///< param — 더미 계획의 H (T1 §6.1 기본 4)
  int share_bernstein_degree_{5};       ///< param — 더미 계획의 n (T1 §6.1 기본 5)
  double share_segment_duration_s_{0.5};///< param — 더미 계획의 Δt [s] (T1 기본 0.5)
  bool measure_wire_size_{true};        ///< param — 발행 1 회 직렬화 크기 실측 여부 (Q-18)
  int startup_check_delay_ms_{1000};    ///< param — R-05 검사 지연 [ms]. **[0a] 실측 대상**
  int summary_log_interval_ticks_{100}; ///< param — 계측 요약 로그 주기 [틱]. 0 이면 비활성

  // ── 상태 ────────────────────────────────────────────────────────────────────
  mrs::ViewScope expected_scope_;   ///< 기대 뷰 스코프 (UNIFORM 고정). 지도 버전은 param 또는 학습
  bool scope_known_{false};         ///< 기대 스코프가 확정됐는지 (미확정이면 창을 수용하지 않는다)

  mrs::ExecutionWindow window_;     ///< 현재 수용 중인 실행 창
  bool has_window_{false};          ///< `window_` 유효 여부
  std::size_t segment_index_{0};    ///< 다음에 소비할 세그먼트 인덱스
  double segment_entered_s_{0.0};   ///< 현재 세그먼트 진입 시각 [s]
  std::int64_t escalated_window_seq_{-1}; ///< 이미 상향 보고한 창 seq (-1 = 없음)

  mrs::RobotObservation observation_; ///< 마지막 자기 관측
  bool has_observation_{false};       ///< `observation_` 유효 여부

  std::uint32_t last_tick_seq_{0};  ///< 마지막으로 본 `/plan_tick` 의 `tick_seq`
  bool has_tick_{false};            ///< `last_tick_seq_` 유효 여부
  std::uint64_t tick_gap_total_{0}; ///< `tick_seq` 결번 누적 (R-A1 — 유실은 결번으로 드러난다)
  double tick_jitter_max_s_{0.0};   ///< 관측된 최대 틱 도착 지터 [s]
  std::uint64_t event_id_counter_{0}; ///< 상향 보고용 상관 키 시퀀스 (E1: 0 금지)

  std::uint64_t segment_advance_reached_{0}; ///< 노드 도달로 소비한 세그먼트 수
  std::uint64_t segment_advance_timeout_{0}; ///< 타임아웃으로 소비한 세그먼트 수
  std::uint64_t truncate_reject_count_{0};   ///< 커밋 접두를 자르려 해 거부한 TRUNCATE 건수
  std::uint64_t escalation_publish_count_{0};///< 발행한 `EscalationReport` 건수
  bool share_wire_size_logged_{false};       ///< `LocalPlanShare` 실측 크기를 이미 기록했는지

  /** @brief `mrs::convert::ConvertStatus` 값별 폐기 카운터 (인덱스 = 열거값, 0..6). */
  std::array<std::uint64_t, 7> discard_by_status_{};
  std::map<std::uint16_t, NeighborShareStats> neighbor_stats_; ///< 이웃별 수신 계측

  // ── ROS 엔티티 ──────────────────────────────────────────────────────────────
  rclcpp::CallbackGroup::SharedPtr io_group_; ///< 전 콜백을 담는 단일 MutuallyExclusive 그룹

  rclcpp::Subscription<std_msgs::msg::UInt64>::SharedPtr roadmap_version_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::ExecutionWindow>::SharedPtr execution_window_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::PlanTick>::SharedPtr plan_tick_sub_;
  rclcpp::Subscription<mrs_interfaces::msg::RobotState>::SharedPtr robot_state_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::LocalPlanShare>::SharedPtr>
    neighbor_plan_subs_;

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::EscalationReport>::SharedPtr escalation_report_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::LocalPlanShare>::SharedPtr local_plan_share_pub_;

  rclcpp::TimerBase::SharedPtr startup_check_timer_; ///< R-05 지연 검사용 1 회성 **벽시계** 타이머

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
