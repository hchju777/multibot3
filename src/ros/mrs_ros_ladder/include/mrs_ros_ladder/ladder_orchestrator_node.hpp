#pragma once

/**
 * @file ladder_orchestrator_node.hpp
 * @brief ladder_orchestrator — **[0a] tracer bullet 판: 로그 전용, 라우팅 없음**(architecture
 * §7-6).
 *
 * ## 이 파일이 지금 무엇인가 (정직 고지)
 * 확정서 D-08 의 사다리는 R0→R5 상태기계 + 이원 트리거 라우팅이다. **이 구현에는 그것이 없다.**
 * [0a] 에서 이 노드가 하는 일은 두 가지뿐이다:
 *  1. per-robot `/robot_{i}/escalation_report` 를 실제 스키마·실제 변환으로 수신하고
 *     **수신 시각·전달 지연을 기록**한다 — [0a] 가 재려는 상향 왕복의 종점이다.
 *  2. 관측한 R0 상향을 `/ladder/rung_event` 로 **발행**한다(§ 발행 판단 참조).
 *
 * ⛔ `PartialReplan`(R3)·`ReassignRequest`(R4) 클라이언트를 **만들지 않는다.** 라우팅은 D-08 의
 * 일이고 로드맵 [3] 의 몫이다. [0a] 에 라우팅을 넣으면 배관 실측 런이 사다리 실험처럼 보이고,
 * 그 로그가 F2 발동률·F6 스래싱 카운터의 오염원이 된다.
 * ⛔ `/ladder/judge_verdict` 도 구독하지 않는다 — [0a] 에 judge 발행자가 없어 검증되는 것이 없다.
 *
 * ## `/ladder/rung_event` 발행 판단 (지시받은 판단 사항 — 근거를 남긴다)
 * **발행한다.** 근거 3건과, 그 대가로 감수한 결손 1건을 함께 적는다.
 *  - (근거 1) architecture §7 의 [0a] 최소 토픽 목록이 `/ladder/rung_event` 를 **명시**한다.
 *  - (근거 2) 계약 L-13 은 이 스키마에 대해 "**스키마 드리프트 = 실험 무효**"를 선언하고, F2 의
 *    신규성 판정(R2 발동률 하한)과 F6 의 반례 실증(M6 순환 증인)이 여기 걸려 있다. 이 경로를
 *    [3] 까지 미검증으로 두면 **실험이 걸린 계측 배선**을 가장 비싼 시점에 처음 밟게 된다.
 *  - (근거 3) `to_msg(RungEventRecord, ...)` 는 이미 구현돼 있다. 발행은 **실제 계약 경로**를
 *    밟는 것이지 새 스키마를 만드는 일이 아니다.
 *  - (결손, 감수) `RungEventRecord` 의 M2·M6 필드(`phi_hat_*`·`edge_set_fingerprint`·
 *    `attempt_count`·`duration_s`)를 [0a] 는 **정직하게 산출할 수 없다**(judge·재조정·roadmap 이
 *    없다). 지어내지 않고 계약이 허용하는 "미산출" 표현으로 내보낸다 —
 *    `phi_hat_after_s = NaN`(계약이 명시한 미산출 값), 나머지는 0. **[0a] 로그의 이 필드들을
 *    F2·F6 분석에 넣지 말 것.** 리포트에 OPEN 으로 올린다.
 *
 * ## 시각·변환 규율
 *  - `Time` ↔ `double` 은 **반드시** `mrs::convert::time_to_seconds` 경유(R-18). `1e9` 산술 금지.
 *  - 메시지 ↔ 도메인은 전부 `mrs_msg_convert` 경유(계약 §0.1 V3).
 *  - 판정은 convert, **보고는 노드**(R-15 (b)=b3) — 사유별 폐기 카운터·THROTTLE 로그가 여기 있다.
 *
 * ⛔ 이 노드는 `mrs_judge`·`mrs_reorder_btpg` 를 include 하지 않는다(architecture §2.3, C3).
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include <builtin_interfaces/msg/time.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/u_int64.hpp>

#include "mrs/contract_types.hpp"
#include "mrs/view_ids.hpp"
#include "mrs_ladder/ladder_state_machine.hpp"
#include "mrs_msg_convert/convert_result.hpp"

#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"

namespace mrs
{

/**
 * @brief 사다리 오케스트레이션 노드 — [0a] 로그 전용 판.
 */
class LadderOrchestratorNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 로봇 수만큼 `escalation_report` 를 구독한다(라우팅 클라이언트 없음).
   */
  LadderOrchestratorNode();
  ~LadderOrchestratorNode() override = default;

private:
  /**
   * @brief `/map_registry/roadmap_version`(transient_local) 수신 — 기대 뷰 스코프를 학습한다.
   * @param[in] msg 지도 버전. 자료형 `std_msgs::msg::UInt64::SharedPtr`. 0 은 런타임 금지값이다.
   * @return void
   */
  void on_roadmap_version(const std_msgs::msg::UInt64::SharedPtr msg);

  /**
   * @brief 로봇 i 의 `/robot_{i}/escalation_report` 수신 콜백 (수신·계측만, 라우팅 없음).
   * @param[in] robot_index 구독을 만든 0-base 로봇 인덱스. 자료형 `std::size_t`.
   *            메시지의 `robot_id` 와 다를 수 있으며(오배선 탐지), 그 경우 경고를 남긴다.
   * @param[in] msg 에스컬레이션 보고. 자료형 `mrs_interfaces::msg::EscalationReport::SharedPtr`.
   * @return void
   */
  void on_escalation_report(
    std::size_t robot_index, const mrs_interfaces::msg::EscalationReport::SharedPtr msg);

  /**
   * @brief 기대 스코프 미확정 상태에서 상향 보고가 들어왔음을 THROTTLE 로 경고한다.
   * @return void
   */
  void warn_scope_unknown();

  /**
   * @brief 변환에 성공한 상향 보고를 계측에 적립하고(필요하면) `RungEvent` 를 발행한다.
   * @param[in] robot_index 구독을 만든 0-base 로봇 인덱스. 자료형 `std::size_t`. 오배선 대조용.
   * @param[in] msg 수신 메시지. 자료형 `mrs_interfaces::msg::EscalationReport`.
   * @param[in] reason 변환된 도메인 사유. 자료형 `mrs::EscalationReason`.
   * @param[in] now_s 수신 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void record_escalation(
    std::size_t robot_index, const mrs_interfaces::msg::EscalationReport & msg,
    mrs::EscalationReason reason, double now_s);

  /**
   * @brief `/ladder/rung_event` 수신 콜백 (계측 관측 — [0a] 에서는 자기 발행분의 왕복 확인).
   * @param[in] msg 가로대 사건. 자료형 `mrs_interfaces::msg::RungEvent::SharedPtr`.
   * @return void
   */
  void on_rung_event(const mrs_interfaces::msg::RungEvent::SharedPtr msg);

  /**
   * @brief 상향 보고의 발행→수신 지연을 기록한다 ([0a] 핵심 실측량: 상향 왕복).
   * @param[in] stamp 보고 메시지의 `header.stamp`. 자료형 `builtin_interfaces::msg::Time`.
   * @param[in] robot_id 보고 로봇 id. 자료형 `std::uint16_t`.
   * @param[in] event_id 상관 키. 자료형 `std::uint64_t`.
   * @param[in] reason 변환된 도메인 사유. 자료형 `mrs::EscalationReason`.
   * @param[in] now_s 수신 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각.
   * @return void
   */
  void log_escalation_latency(
    const builtin_interfaces::msg::Time & stamp, std::uint16_t robot_id, std::uint64_t event_id,
    mrs::EscalationReason reason, double now_s);

  /**
   * @brief 관측한 R0 상향을 `/ladder/rung_event` 로 발행한다 (계측 경로 검증 — 파일 머리말 참조).
   * @param[in] robot_id 보고 로봇 id. 자료형 `std::uint16_t`. `affected_robots` 의 유일 원소가
   * 된다.
   * @param[in] event_id 상관 키. 자료형 `std::uint64_t`. 0 이면 convert 가 E1 위반으로 거부한다.
   * @param[in] now_s 발행 시각 [s]. 자료형 `double`. 시뮬 시계 절대시각 → `header.stamp`.
   * @return void
   * @warning M2·M6 필드는 [0a] 가 산출할 수 없어 "미산출" 표현으로 나간다 — 파일 머리말의
   *          결손 항목. 이 로그를 F2·F6 분석에 넣지 말 것.
   */
  void publish_rung_event(std::uint16_t robot_id, std::uint64_t event_id, double now_s);

  /**
   * @brief 현재 시뮬 시각을 초로 얻는다 (R-18 구속 — 가드 헬퍼 경유).
   * @param[out] out_seconds 시뮬 시계 기준 절대시각 [s]. 자료형 `double`.
   * @return `bool` — 변환 성공이면 true. false 이면 호출자는 그 사건의 처리를 건너뛴다.
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
   * @brief 누적 계측·폐기 카운터를 로그에 덤프한다 (사후 감사용).
   * @return void
   */
  void log_summary();

  // ── 파라미터 ────────────────────────────────────────────────────────────────
  int robot_count_{2};            ///< param `robot_count` — [0a] 는 2 대
  bool publish_rung_event_{true}; ///< param — `/ladder/rung_event` 발행 여부 (머리말 판단)
  int summary_log_interval_{20}; ///< param — 요약 로그 주기 [수신 건수]. 0 이면 비활성

  // ── 상태 ────────────────────────────────────────────────────────────────────
  /**
   * @brief D-08 상태기계.
   * @note **[0a] 에서는 호출하지 않는다** — 이 노드는 로그 전용이고(architecture §7-6),
   *       도메인 구현이 아직 스텁(예외 발생)이라 호출하면 노드가 죽는다. 배선 의도를 잃지 않도록
   *       멤버는 남긴다. 실제 라우팅은 [3] 에서 붙는다.
   */
  mrs::LadderStateMachine ladder_;

  mrs::ViewScope expected_scope_; ///< 기대 뷰 스코프 (UNIFORM 고정)
  bool scope_known_{false};       ///< 기대 스코프 확정 여부

  std::uint64_t escalation_received_{0};  ///< 변환까지 성공한 상향 보고 건수
  std::uint64_t rung_event_published_{0}; ///< 발행한 `RungEvent` 건수
  std::uint64_t rung_event_received_{0}; ///< 수신한 `RungEvent` 건수 (자기 발행분 포함)
  double escalation_latency_max_s_{0.0}; ///< 관측된 최대 상향 전달 지연 [s]
  std::map<std::uint8_t, std::uint64_t> reason_counts_; ///< 사유별 수신 건수 (뭉개지 않기 위함)

  /** @brief `mrs::convert::ConvertStatus` 값별 폐기 카운터 (인덱스 = 열거값, 0..6). */
  std::array<std::uint64_t, 7> discard_by_status_{};

  // ── ROS 엔티티 ──────────────────────────────────────────────────────────────
  rclcpp::CallbackGroup::SharedPtr io_group_; ///< 전 콜백을 담는 단일 MutuallyExclusive 그룹

  rclcpp::Subscription<std_msgs::msg::UInt64>::SharedPtr roadmap_version_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::EscalationReport>::SharedPtr>
    escalation_report_subs_;
  rclcpp::Subscription<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_sub_;
  rclcpp::Publisher<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_pub_;

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
