#pragma once

/**
 * @file sadg_service_node.hpp
 * @brief sadg_service — L3 SADG 노드 (architecture §7-3). **로드맵 [2] Split B: 실 depgraph 배선.**
 *
 * ## 이 노드가 [2] 에서 하는 일 / 하지 않는 일
 *  - 한다: `/planned_paths` 를 받아 **실 `mrs::DependencyGraph` 를 구축**하고(ADG — Type-1
 * 로봇순서· Type-2 통행순서), 의존성이 충족된 세그먼트를 `graph_.release_next_window()` 로 릴리스해
 *    `/robot_{i}/execution_window` 에 발행한다(D-05). `/robot_{i}/robot_state` 를 구독해 로봇이
 *    노드를 **이탈(클리어)**하면 `graph_.on_progress_event()` 로 Type-2 선행제약을 해소한다 —
 *    이것이 창 릴리스를 앞으로 굴리는 진행 이벤트원이다. `/robot_{i}/escalation_report` 는
 *    수신해 **로그만** 남긴다.
 *  - **하지 않는다**: R1(BTPG)·R2(MILP)·judge·접합(splice)·좀비 창 실효(staleness repair, W5).
 *    전부 로드맵 [1]/[3] 및 pending-[0b](v_max 미실측)의 일이다. 사다리 라우팅은
 *    `ladder_orchestrator` 소관이다(D-08).
 *
 * ## 정직성 경계 (pysim 티어)
 * 이 배선이 검증하는 것은 **pysim 티어 seam 통합 로직**이다 — 더미 L4 의 합성 진행(RobotState
 * 관측)이 depgraph 의 릴리스/클리어 프론티어를 실제로 굴리는가. **QP feasibility·무livelock·
 * F1∧F5 는 isaac 필수(pending-isaac)** 이며 이 노드의 pysim 흐름 통과를 "연구 검증"으로
 * 선언하지 않는다.
 *
 * ## 실행자 모델 (계약 Q-3)
 * 콜백그룹을 **둘**로 나눈다.
 *  - `release_cb_group_` : 릴리스 타이머. 노드 기본 실행자에서 돈다.
 *  - `ingest_cb_group_`  : 계획 수신·**진행(RobotState) 수신**·에스컬레이션 수신·MapRegistry 조회.
 *    **전용 스레드 + 전용 단일 실행자**(@ref mrs::ros_sadg::CallbackGroupThread)에서 돈다.
 * 이유는 계약이 명시한다 — "릴리스 콜백이 MILP 에 막히면 **D-05 창 소비율 지표가 오염**된다".
 * [3] 에서 배선을 바꾸면 그때의 릴리스 지연 수치는 이미 오염된 뒤다. 두 그룹이 같은 상태
 * (특히 @ref graph_)를 읽고 쓰므로 @ref state_mutex_ 가 그 경계다.
 *
 * ## 창 릴리스가 타이머 구동인 이유 (의도적 이탈, 실행 규약 변경 아님)
 * architecture §1.6 은 `sadg_service` 를 **타이머 없는 순수 이벤트 구동**으로 규정한다(진행
 * 이벤트가 릴리스를 굴린다 — `adg-2019`). [2] 에는 진행 이벤트원(RobotState)이 붙었지만,
 * 릴리스 트리거는 **여전히 주기 타이머**로 둔다: 진행 이벤트로 선행제약이 해소돼도 그 시점에
 * 곧바로 릴리스를 밀지 않고, 다음 타이머 틱에서 `release_next_window` 가 재시도해 막힌 로봇을
 * 전진시킨다(막힘=false 반환, 정상). 순수 이벤트 구동(진행 이벤트가 직접 릴리스를 호출)은
 * `CommitStatus` 경로가 붙는 [3] 대상이다. 타이머는 릴리스 **트리거**이지 릴리스 **판정**이
 * 아니다 — 판정(의존성 충족)은 depgraph 가 단독으로 한다.
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
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/srv/get_uniform_view.hpp"

namespace mrs
{

/**
 * @brief L3 SADG 서비스 노드 — 실 `mrs::DependencyGraph` 를 소유·구동한다 (로드맵 [2] Split B).
 *
 * 릴리스 상태(릴리스/클리어 프론티어·로봇별 window_seq)는 전부 @ref graph_ 가 소유한다 — 이 노드는
 * 배관(구독·타이머·발행·변환)만 든다. 더미 세그먼트 열 상태는 제거됐다.
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
   * @brief `/planned_paths` 수신 콜백 — 계획을 도메인으로 변환해 depgraph 구축에 넘긴다.
   *
   * `ingest_cb_group_`(전용 스레드)에서 실행된다.
   *
   * @param[in] msg 수신한 계획. 자료형 `mrs_interfaces::msg::PlannedPaths::SharedPtr`.
   * @return void
   */
  void on_planned_paths(const mrs_interfaces::msg::PlannedPaths::SharedPtr msg);

  /**
   * @brief 로봇 i 의 `/robot_i/robot_state` 수신 콜백 — 진행(노드 이탈) 이벤트를 depgraph 에 반영.
   *
   * `ingest_cb_group_`(전용 스레드)에서 실행된다. 관측을 도메인으로 변환(스코프 대조 포함)한 뒤
   * 로봇이 유효 노드에서 **이탈**했는지 판정해 @ref advance_progress_locked 로 넘긴다.
   *
   * @param[in] robot_index 0-base 로봇 인덱스. 자료형 `std::size_t`.
   * @param[in] msg 수신한 로봇 상태. 자료형 `mrs_interfaces::msg::RobotState::SharedPtr`.
   * @return void
   */
  void on_robot_state(
    std::size_t robot_index, const mrs_interfaces::msg::RobotState::SharedPtr msg);

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
   * @brief 변환된 계획으로 depgraph 를 (재)구축한다 (@ref state_mutex_ 를 내부에서 잡는다).
   *
   * **build-once 판정**: `pp_service` 는 `auto_plan_period_s` 마다 `plan_epoch` 만 올려 **내용이
   * 동일한** canned 계획을 재발행한다. 매번 `build_from_paths` 로 재구축하면 depgraph 의 릴리스/
   * 클리어 프론티어가 리셋돼 로봇이 영원히 처음부터 릴리스된다. 그래서 **마지막 구축한 계획과
   * 노드열이 같으면 재구축을 건너뛴다**(진행 보존). 다르면 재구축한다. `plan_epoch` 은 창 스탬프용
   * 으로 매번 갱신한다.
   *
   * @param[in] paths 도메인 경로 목록. 자료형 `std::vector<mrs::RobotPath>`.
   * @param[in] plan_epoch 계획 세대 번호. 자료형 `std::uint32_t`. 메시지 봉투에서 읽은 값이다.
   * @return void
   */
  void apply_planned_paths(const std::vector<RobotPath> & paths, std::uint32_t plan_epoch);

  /**
   * @brief 로봇 1대의 다음 창을 depgraph 에서 릴리스해 발행한다.
   *
   * @warning 호출자가 @ref state_mutex_ 를 **이미 잡고** 있어야 한다(`_locked` 접미의 뜻).
   *
   * depgraph 가 릴리스한 창에 봉투 2건(`plan_epoch`·`window_valid_until_s`)만 노드가 스탬프하고
   * 변환·발행한다. `window_seq`·`segments`·`predecessor_constraints`·스코프는 depgraph 소유값이다.
   *
   * @param[in] robot_id 대상 로봇 id. 자료형 `mrs::RobotId`.
   * @param[in] now_s 현재 시각 [s], 시뮬 시계 절대시각. 자료형 `double`.
   * @return `bool` — 창을 실제로 발행했으면 true. 릴리스할 창이 없거나(막힘/소진) 변환에 실패하면
   *         false.
   */
  bool release_robot_window_locked(RobotId robot_id, double now_s);

  /**
   * @brief 발행한 창 1건을 사람이 읽을 수 있게 기록한다 (막힘 지점 포함, seam 흐름 실측용).
   * @param[in] window 발행한 도메인 창. 자료형 `mrs::ExecutionWindow`. 봉투 2건은 이미 스탬프됨.
   * @param[in] now_s 릴리스 시각 [s], 시뮬 시계 절대시각. 자료형 `double`.
   * @return void
   */
  void log_released_window(const ExecutionWindow & window, double now_s);

  /**
   * @brief 로봇이 이전 관측의 유효 노드를 이탈했으면 depgraph 에 진행 이벤트를 낸다.
   *
   * @warning 호출자가 @ref state_mutex_ 를 이미 잡고 있어야 한다.
   *
   * 진행 semantics: 로봇별 마지막 유효 `occupied_node` 를 추적하고, 그 노드 X 가 **다른 값**
   * (센티넬 또는 다른 노드)으로 바뀌면 로봇이 X 를 **클리어(이탈)**한 것이다("도착"이 아니라
   * "이탈" — 선행 로봇이 공유 노드를 비워야 후속이 진입한다). 매 관측마다 마지막 노드를 갱신한다.
   *
   * @param[in] robot_index 0-base 로봇 인덱스(추적 배열 첨자). 자료형 `std::size_t`.
   * @param[in] robot_id 대상 로봇 id. 자료형 `mrs::RobotId`.
   * @param[in] occupied_node 이번 관측의 점유 노드. 자료형 `mrs::UniformNodeId`. 센티넬은 "엣지
   * 위".
   * @param[in] time_s 이벤트 시각 [s](RobotState.header.stamp). 자료형 `double`. 시뮬 시계
   * 절대시각.
   * @return void
   */
  void advance_progress_locked(
    std::size_t robot_index, RobotId robot_id, UniformNodeId occupied_node, double time_s);

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

  // ── 도메인 소유물 ─────────────────────────────────────────────────────
  mrs::DependencyGraph graph_; ///< ADG/SADG 단일 소유 (D-05). [2] 릴리스/진행의 단일 소유자
  mrs::BtpgAttemptTracker btpg_tracker_; ///< R1 (seam e). [2] 미사용
  mrs::MilpJobTracker milp_tracker_;     ///< R2 (seam d). [2] 미사용
  mrs::SlackRecursionEstimator judge_;   ///< judge (seam f). [2] 미사용

  // ── 릴리스·진행 상태 (두 콜백그룹이 공유 — state_mutex_ 가 경계) ─────────────
  mutable std::mutex state_mutex_; ///< 아래 상태 전부(특히 graph_)를 보호한다
  ViewScope view_scope_{};         ///< 균일 뷰 스코프 (MapRegistry 가 유일 출처)
  bool scope_ready_{false}; ///< 스코프 확보 여부. false 면 아무것도 릴리스하지 않는다

  bool graph_built_{false}; ///< depgraph 구축 여부. false 면 릴리스하지 않는다
  std::vector<RobotPath> last_built_paths_; ///< 마지막으로 구축한 계획 (build-once 판정용)
  std::uint32_t current_plan_epoch_{0};     ///< 마지막 수신 계획 세대 (창 스탬프용)
  std::vector<UniformNodeId>
    last_occupied_node_; ///< 로봇별 마지막 유효 점유 노드 (진행 이탈 판정용)

  // ── 콜백그룹·실행자 (계약 Q-3) ──────────────────────────────────────────
  rclcpp::CallbackGroup::SharedPtr release_cb_group_; ///< 릴리스 경로 (노드 기본 실행자)
  rclcpp::CallbackGroup::SharedPtr ingest_cb_group_;  ///< 수신·조회 경로 (전용 실행자)

  // ── ROS 배선 ────────────────────────────────────────────────────────────
  rclcpp::Subscription<mrs_interfaces::msg::PlannedPaths>::SharedPtr planned_paths_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::RobotState>::SharedPtr> robot_state_subs_;
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
