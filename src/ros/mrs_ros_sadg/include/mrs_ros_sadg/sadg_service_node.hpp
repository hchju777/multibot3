#pragma once

/**
 * @file sadg_service_node.hpp
 * @brief sadg_service — L3 SADG 서비스 노드. R1(BTPG)/R2(MILP)/judge/릴리스의 단일 소유자.
 *
 * architecture §1.1: btpg_module/sadg_milp_module/judge_module/릴리스는 노드로 쪼개지 않는다 —
 * 넷 다 같은 의존성 그래프 인스턴스를 읽고 쓴다. C3(judge↔btpg 독립)은 노드 분리가 아니라
 * **패키지 의존 그래프**로 강제한다(`mrs_reorder_btpg` 는 `mrs_judge` 를 include 하지 않는다,
 * CI 검사 §2.4-1) — 이 노드가 둘 다 의존하는 것은 위반이 아니다(조립 지점의 정상적 형태).
 */

#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "mrs_depgraph/dependency_graph.hpp"
#include "mrs_reorder_btpg/btpg_attempt_tracker.hpp"
#include "mrs_reorder_milp/milp_job_tracker.hpp"
#include "mrs_judge/slack_recursion_estimator.hpp"

#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_interfaces/msg/judge_verdict.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/srv/reorder_passing.hpp"

namespace mrs
{

/**
 * @brief L3 SADG 서비스 노드 — R1/R2/judge/릴리스 단일 소유.
 */
class SadgServiceNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 로봇 수만큼 per-robot 토픽 + 전역 토픽·서비스를 구성한다.
   */
  SadgServiceNode();
  ~SadgServiceNode() override = default;

private:
  /** @brief `/planned_paths` 수신 콜백 (L2 산출물). @return void */
  void on_planned_paths(const mrs_interfaces::msg::PlannedPaths::SharedPtr msg);

  /**
   * @brief 로봇 i 의 `/robot_i/commit_status` 수신 콜백 (T2-R6).
   * @param[in] robot_index 0-base 로봇 인덱스.
   * @param[in] msg 커밋 상태.
   * @return void
   */
  void on_commit_status(std::size_t robot_index, const mrs_interfaces::msg::CommitStatus::SharedPtr msg);

  /**
   * @brief `/sadg_service/reorder_passing` 핸들러 — Q-2 서비스 바인딩(reorder_transport=service 시).
   * @return void
   */
  void on_reorder_passing(
    const std::shared_ptr<mrs_interfaces::srv::ReorderPassing::Request> request,
    std::shared_ptr<mrs_interfaces::srv::ReorderPassing::Response> response);

  mrs::DependencyGraph graph_;                        ///< ADG/SADG 단일 소유 (D-05)
  mrs::BtpgAttemptTracker btpg_tracker_;               ///< R1 (seam e 소비자)
  mrs::MilpJobTracker milp_tracker_;                   ///< R2 (seam d 소비자)
  mrs::SlackRecursionEstimator judge_;                 ///< R2↔R3 판정자 (seam f 소비자)

  rclcpp::Subscription<mrs_interfaces::msg::PlannedPaths>::SharedPtr planned_paths_sub_;
  std::vector<rclcpp::Subscription<mrs_interfaces::msg::CommitStatus>::SharedPtr> commit_status_subs_;
  std::vector<rclcpp::Publisher<mrs_interfaces::msg::ExecutionWindow>::SharedPtr> execution_window_pubs_;
  rclcpp::Publisher<mrs_interfaces::msg::JudgeVerdict>::SharedPtr judge_verdict_pub_;
  rclcpp::Publisher<mrs_interfaces::msg::RungEvent>::SharedPtr rung_event_pub_;
  rclcpp::Service<mrs_interfaces::srv::ReorderPassing>::SharedPtr reorder_passing_srv_;

  int robot_count_{2}; ///< 파라미터 robot_count 기본값 (tracer bullet [0] = 2대)
};

} // namespace mrs
