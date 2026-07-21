#pragma once

/**
 * @file sadg_service_node.hpp
 * @brief sadg_service — **[0a] tracer bullet 더미** L3 노드 (architecture §7-3).
 *
 * ## 이 노드가 [0a] 에서 하는 일 / 하지 않는 일
 *  - 한다: `/planned_paths` 를 받아 경로를 세그먼트로 자르고, **실제 `ExecutionWindow` 스키마로**
 *    `/robot_{i}/execution_window` 에 순서대로 릴리스한다. 이것이 [0a] 의 핵심 계측 경로다
 *    (창 발행 → 로봇 수신 지연). `/robot_{i}/escalation_report` 를 받아 **로그만** 남긴다.
 *  - **하지 않는다**: 의존성 판정·R1(BTPG)·R2(MILP)·judge·접합(splice)·좀비 창 실효 판정.
 *    전부 로드맵 [1]~[3] 의 일이다. 사다리 라우팅은 `ladder_orchestrator` 소관이다.
 *
 * ## 실행자 모델 (계약 Q-3 — [0a] 에 MILP 가 없어도 지금 구조를 잡는다)
 * 콜백그룹을 **둘**로 나눈다.
 *  - `release_cb_group_` : 릴리스 타이머. 노드 기본 실행자에서 돈다.
 *  - `ingest_cb_group_`  : 계획 수신·에스컬레이션 수신·MapRegistry 조회. **전용 스레드 + 전용
 *    단일 실행자**(@ref mrs::ros_sadg::CallbackGroupThread)에서 돈다.
 * 이유는 계약이 명시한다 — "릴리스 콜백이 MILP 에 막히면 **D-05 창 소비율 지표가 오염**된다".
 * [3] 에서 배선을 바꾸면 그때의 릴리스 지연 수치는 이미 오염된 뒤다. 두 그룹이 같은 상태를
 * 읽고 쓰므로 @ref state_mutex_ 가 그 경계다.
 *
 * ## 창 릴리스가 [0a] 에서 타이머 구동인 이유 (의도적 이탈, 실행 규약 변경 아님)
 * architecture §1.6 은 `sadg_service` 를 **타이머 없는 순수 이벤트 구동**으로 규정한다(진행
 * 이벤트가 릴리스를 굴린다 — `adg-2019`). 그러나 [0a] 에는 진행 이벤트의 출처가 없다:
 * 계약 §0.1.2 승인 기록이 "**[0a] 는 `CommitStatus` 를 발행하지 않는다**"를 명시했고,
 * `CommitStatus` → 도메인 변환 함수도 `mrs_msg_convert` 에 없다. 이벤트원이 없는 상태에서
 * 창을 하나만 내보내면 배관 실측이 표본 1개로 끝난다.
 * ⇒ [0a] 더미에 한해 **릴리스를 주기 타이머로 굴린다**. [1]/[3] 에서 `CommitStatus`·진행
 * 이벤트가 붙으면 이 타이머는 제거된다. 이것은 측정 장치이지 설계 결정이 아니다.
 *
 * ## 뷰 규약 (계약 §0.1)
 * 창의 노드 id 는 전부 **UNIFORM** 이고 인스턴스 스코프는 `roadmap_version` + `view_id` 평면
 * 쌍이다. 두 값의 유일한 출처는 **MapRegistry 의 `GetUniformView` 응답**이며 파라미터로 지어내지
 * 않는다. 수신 `PlannedPaths` 도 같은 스코프로 대조한다(V2).
 */

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "mrs/contract_types.hpp"
#include "mrs/view_ids.hpp"
#include "mrs_depgraph/dependency_graph.hpp"
#include "mrs_judge/slack_recursion_estimator.hpp"
#include "mrs_msg_convert/convert_result.hpp"
#include "mrs_reorder_btpg/btpg_attempt_tracker.hpp"
#include "mrs_reorder_milp/milp_job_tracker.hpp"
#include "mrs_ros_sadg/callback_group_thread.hpp"
#include "mrs_ros_sadg/convert_diagnostics.hpp"

#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/judge_verdict.hpp"
#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/srv/get_uniform_view.hpp"

namespace mrs
{

/**
 * @brief 로봇 1대분 릴리스 상태 — [0a] 더미의 "의존성 그래프" 대용.
 *
 * 실제 시스템에서 이 상태는 `mrs_depgraph::DependencyGraph` 안에 있고 릴리스는 의존성 충족
 * 판정의 결과다(D-05). [0a] 는 판정이 없으므로 **세그먼트 열 + 다음 인덱스**만 든다.
 */
struct RobotReleaseState
{
  RobotId robot_id{ROBOT_ID_NONE};     ///< 로봇 id
  std::vector<WindowSegment> segments; ///< 계획에서 잘라 낸 세그먼트 열 (뷰 UNIFORM)
  std::size_t next_segment_index{0};   ///< 다음에 릴리스할 세그먼트 인덱스
  std::uint32_t window_seq{0};         ///< 이 로봇에 마지막으로 실린 창 seq
  std::uint32_t plan_epoch{0};         ///< 세그먼트가 유래한 계획 세대
  bool exhausted_logged{false}; ///< 경로 소진 로그를 이미 남겼는지 (로그 폭주 방지)
};

/**
 * @brief L3 SADG 서비스 노드의 [0a] 더미 구현.
 */
class SadgServiceNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·콜백그룹 2종·per-robot 토픽·MapRegistry 조회를 구성한다.
   *
   * 생성자는 블로킹하지 않는다. 전용 실행자 스레드는 **가장 마지막에** 띄운다 — 그 전에 띄우면
   * 아직 초기화되지 않은 멤버를 콜백이 건드릴 수 있다.
   */
  SadgServiceNode();
  ~SadgServiceNode() override = default;

private:
  /**
   * @brief `/planned_paths` 수신 콜백 — 계획을 세그먼트 열로 잘라 릴리스 대기열에 넣는다.
   *
   * `ingest_cb_group_`(전용 스레드)에서 실행된다.
   *
   * @param[in] msg 수신한 계획. 자료형 `mrs_interfaces::msg::PlannedPaths::SharedPtr`.
   * @return void
   */
  void on_planned_paths(const mrs_interfaces::msg::PlannedPaths::SharedPtr msg);

  /**
   * @brief 로봇 i 의 `/robot_i/escalation_report` 수신 콜백 — **로그만** 남긴다.
   *
   * 라우팅(어느 가로대로 보낼지)은 `ladder_orchestrator` 소관이다(D-08). 이 노드는 [0a] 상향
   * 왕복 지연의 관측점 하나를 제공할 뿐이다.
   *
   * @param[in] robot_index 0-base 로봇 인덱스. 자료형 `std::size_t`.
   * @param[in] msg 수신한 에스컬레이션 보고. 자료형
   *            `mrs_interfaces::msg::EscalationReport::SharedPtr`.
   * @return void
   */
  void on_escalation_report(
    std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg);

  /**
   * @brief 수신한 에스컬레이션 1건을 사람이 읽을 수 있게 기록한다 ([0a] 상향 왕복 관측).
   * @param[in] robot_index 0-base 로봇 인덱스. 자료형 `std::size_t`.
   * @param[in] reason 변환된 에스컬레이션 사유. 자료형 `mrs::EscalationReason`.
   * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::EscalationReport`.
   * @param[in] latency_s 발행→수신 지연 [s]. 자료형 `double`. 측정 실패 시 0.
   * @return void
   */
  void log_escalation_report(
    std::size_t robot_index, EscalationReason reason,
    const mrs_interfaces::msg::EscalationReport & msg, double latency_s);

  /**
   * @brief 릴리스 타이머 콜백 — 로봇별로 다음 창을 하나씩 발행한다.
   *
   * `release_cb_group_`(노드 기본 실행자)에서 실행된다. 무거운 작업을 여기에 두지 않는 것이
   * 계약 Q-3 의 요구다.
   *
   * @return void
   */
  void on_release_timer();

  /**
   * @brief MapRegistry 균일 뷰 조회 재시도 타이머 콜백. `ingest_cb_group_` 에서 실행된다.
   * @return void
   */
  void on_map_query_timer();

  /**
   * @brief `GetUniformView` 비동기 응답 콜백 — 뷰 스코프를 확정한다.
   * @param[in] future 응답 future. 자료형
   *            `rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture`.
   * @return void
   */
  void on_uniform_view_response(
    rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture future);

  /**
   * @brief 응답 없는 조회 요청을 시한 초과 시 회수한다 (nav2-reference §A2 pending 누수 방지).
   * @return `bool` — 새 요청을 보내도 되면 true, 아직 응답을 기다리는 중이면 false.
   */
  bool reclaim_stale_map_query();

  /**
   * @brief `GetUniformView` 요청을 비동기로 보낸다 (블로킹하지 않는다).
   * @return void
   */
  void send_uniform_view_request();

  /**
   * @brief 균일 뷰 응답을 채택해 뷰 스코프를 확정한다.
   * @param[in] response 균일 뷰 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @return `bool` — 채택했으면 true, 스코프가 사용 불가면 false(사유는 이 함수가 로그로 남긴다).
   */
  bool adopt_uniform_view(const mrs_interfaces::srv::GetUniformView::Response & response);

  /**
   * @brief 현재 확보한 뷰 스코프를 잠금 하에 복사한다.
   * @param[out] out 복사받을 스코프. 자료형 `mrs::ViewScope`. false 반환 시 내용은 변경되지 않는다.
   * @return `bool` — 스코프를 확보한 상태면 true, 아직 미확보면 false.
   */
  [[nodiscard]] bool current_scope(ViewScope & out) const;

  /**
   * @brief 수신 메시지의 발행 시각과 현재 시각의 차이를 계산한다 ([0a] 왕복 지연 실측).
   *
   * 시각 변환은 반드시 `mrs_msg_convert` 헬퍼를 거친다(R-18 구속) — 가드가 그 헬퍼에만 있다.
   *
   * @param[in] label 실패 보고용 지점 이름. 자료형 `const char *`.
   * @param[in] stamp 메시지 헤더의 발행 시각. 자료형 `builtin_interfaces::msg::Time`.
   * @param[out] out_latency_s 계산된 지연 [s]. 자료형 `double`. 변환 실패 시 0 이 들어간다.
   * @return void
   */
  void log_receive_latency(
    const char * label, const builtin_interfaces::msg::Time & stamp, double & out_latency_s);

  /**
   * @brief 변환된 계획을 로봇별 릴리스 대기열에 반영한다 (@ref state_mutex_ 를 내부에서 잡는다).
   * @param[in] paths 도메인 경로 목록. 자료형 `std::vector<mrs::RobotPath>`.
   * @param[in] plan_epoch 계획 세대 번호. 자료형 `std::uint32_t`. 메시지 봉투에서 읽은 값이다.
   * @return void
   */
  void apply_planned_paths(const std::vector<RobotPath> & paths, std::uint32_t plan_epoch);

  /**
   * @brief 로봇 1대의 다음 창을 만들어 발행한다.
   *
   * @warning 호출자가 @ref state_mutex_ 를 **이미 잡고** 있어야 한다(이름의 `_locked` 접미가 그
   * 뜻).
   *
   * @param[in,out] state 대상 로봇의 릴리스 상태. 자료형 `mrs::RobotReleaseState`.
   *                성공 시 `next_segment_index` 와 `window_seq` 가 전진한다.
   * @param[in] now_s 현재 시각 [s], 시뮬 시계 절대시각. 자료형 `double`.
   * @return `bool` — 창을 실제로 발행했으면 true. 남은 세그먼트가 없거나 변환에 실패하면 false.
   */
  bool release_next_window_locked(RobotReleaseState & state, double now_s);

  /**
   * @brief 아직 릴리스할 세그먼트가 남았는지 판정한다 (소진 시 1회만 로그).
   * @warning 호출자가 @ref state_mutex_ 를 이미 잡고 있어야 한다.
   * @param[in,out] state 대상 로봇의 릴리스 상태. 자료형 `mrs::RobotReleaseState`.
   *                소진 로그를 남기면 `exhausted_logged` 가 true 가 된다.
   * @param[in] robot_index 0-base 로봇 인덱스(로그용). 자료형 `std::size_t`.
   * @return `bool` — 남은 세그먼트가 있으면 true.
   */
  bool has_pending_segments(RobotReleaseState & state, std::size_t robot_index);

  /**
   * @brief 변환 실패 1건을 사유별로 적립하고 스로틀 로그를 남긴다 (R-15 (b) = b3).
   * @param[in] context 실패 지점 이름(로그용). 자료형 `const char *`.
   * @param[in] result 변환 결과. 자료형 `mrs::convert::ConvertResult`. `ok == true` 이면 무시된다.
   * @return void
   */
  void report_convert_failure(const char * context, const mrs::convert::ConvertResult & result);

  /**
   * @brief 현재 시각을 시뮬 시계 기준 절대 초로 얻는다 (계약 §0 시각 규약).
   * @return `double` — 시뮬 시계 기준 절대시각 [s].
   */
  [[nodiscard]] double now_seconds() const;

  // ── 도메인 소유물 ([0a] 미사용 — [1]/[3] 에서 이 노드가 단일 소유자가 된다) ─────
  mrs::DependencyGraph graph_; ///< ADG/SADG 단일 소유 (D-05). [0a] 는 호출하지 않는다
  mrs::BtpgAttemptTracker btpg_tracker_; ///< R1 (seam e). [0a] 미사용
  mrs::MilpJobTracker milp_tracker_;     ///< R2 (seam d). [0a] 미사용
  mrs::SlackRecursionEstimator judge_;   ///< judge (seam f). [0a] 미사용

  // ── 릴리스 상태 (두 콜백그룹이 공유 — state_mutex_ 가 경계) ─────────────
  mutable std::mutex state_mutex_;              ///< 아래 상태 전부를 보호한다
  std::vector<RobotReleaseState> robot_states_; ///< 인덱스 = 0-base 로봇 인덱스
  ViewScope view_scope_{}; ///< 균일 뷰 스코프 (MapRegistry 가 유일 출처)
  bool scope_ready_{false}; ///< 스코프 확보 여부. false 면 아무것도 릴리스하지 않는다

  // ── 콜백그룹·실행자 (계약 Q-3) ──────────────────────────────────────────
  rclcpp::CallbackGroup::SharedPtr release_cb_group_; ///< 릴리스 경로 (노드 기본 실행자)
  rclcpp::CallbackGroup::SharedPtr ingest_cb_group_;  ///< 수신·조회 경로 (전용 실행자)

  // ── ROS 배선 ────────────────────────────────────────────────────────────
  rclcpp::Subscription<mrs_interfaces::msg::PlannedPaths>::SharedPtr planned_paths_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::EscalationReport>::SharedPtr>
    escalation_report_subs_;
  std::vector<rclcpp::Publisher<mrs_interfaces::msg::ExecutionWindow>::SharedPtr>
    execution_window_pubs_;
  rclcpp::Publisher<mrs_interfaces::msg::JudgeVerdict>::SharedPtr judge_verdict_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_pub_;
  rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedPtr uniform_view_client_;
  rclcpp::TimerBase::SharedPtr release_timer_;
  rclcpp::TimerBase::SharedPtr map_query_timer_;

  // ── 파라미터 (값의 성격은 생성자 주석 참조) ─────────────────────────────
  int robot_count_{2};                  ///< 로봇 수. [0a] = 2대 (architecture §7)
  double window_release_period_s_{0.5}; ///< 창 릴리스 주기 [s] (문헌치 Q-11 세그먼트 Δt)
  int segments_per_window_{1};          ///< 창 1개에 싣는 세그먼트 수
  double window_validity_horizon_s_{2.0}; ///< `window_valid_until` 여유 [s] — **[0a] 실측 대상**
  double map_query_retry_period_s_{1.0}; ///< MapRegistry 조회 재시도 주기 [s] (배관 상수)
  double map_query_timeout_s_{2.0}; ///< 조회 응답 대기 시한 [s] (pending 누수 방지)

  // ── MapRegistry 조회 진행 상태 ──────────────────────────────────────────
  bool map_query_pending_{false};        ///< 응답 대기 중 여부 (중복 요청 방지)
  std::int64_t map_query_request_id_{0}; ///< 대기 중 요청 id (`remove_pending_request` 인자)
  double map_query_sent_at_s_{0.0};      ///< 요청 발신 시각 [s], 시뮬 시계 절대시각

  // ── 계측 ────────────────────────────────────────────────────────────────
  std::uint64_t released_window_count_{0}; ///< 발행 성공 누적 창 수 (릴리스 스레드 전용)
  std::uint64_t escalation_report_count_{0}; ///< 수신 에스컬레이션 누적 건수 (수신 스레드 전용)

  /**
   * @brief 변환 실패 카운터 전용 잠금.
   * @note 두 콜백그룹이 서로 다른 스레드에서 카운터를 만진다. @ref state_mutex_ 를 재사용하지
   *       않는 이유는 이 잠금이 **항상 가장 안쪽**이어야 잠금 순서가 단순해지기 때문이다
   *       (릴리스 경로는 `state_mutex_` 를 잡은 채 변환 실패를 보고한다).
   */
  mutable std::mutex diag_mutex_;
  mrs::ros_sadg::ConvertDiagnostics convert_diag_; ///< 변환 실패 사유별 폐기 카운터

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

  /**
   * @brief `ingest_cb_group_` 전용 실행자 스레드.
   * @warning **멤버 선언 순서상 마지막**이어야 한다 — 생성 즉시 콜백이 돌기 때문이다.
   */
  std::unique_ptr<mrs::ros_sadg::CallbackGroupThread> ingest_thread_;
};

} // namespace mrs
