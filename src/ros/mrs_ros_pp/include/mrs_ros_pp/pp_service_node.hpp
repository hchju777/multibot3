#pragma once

/**
 * @file pp_service_node.hpp
 * @brief pp_service — L2 경로계획 노드 (D-03). seam a(IPathSolver)는 mrs_planning 이 소유.
 */

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_planning/windowed_path_planner.hpp"
#include "mrs_interfaces/msg/task_assignment.hpp"
#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/srv/plan_paths.hpp"
#include "mrs_interfaces/srv/partial_replan.hpp"

namespace mrs
{

/**
 * @brief L2 경로계획 서비스 노드.
 */
class PpServiceNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 구독·발행·서비스 2종을 구성한다. 솔버는 아직 주입하지 않는다
   * (nullptr — seam a 구체 구현은 Phase 5, bringup 이 결정).
   */
  PpServiceNode();
  ~PpServiceNode() override = default;

private:
  /** @brief `/task_assignment` 수신 콜백. @return void */
  void on_task_assignment(const mrs_interfaces::msg::TaskAssignment::SharedPtr msg);

  /** @brief `/pp_service/plan_paths` 핸들러 (D-03). @return void */
  void on_plan_paths(
    const std::shared_ptr<mrs_interfaces::srv::PlanPaths::Request> request,
    std::shared_ptr<mrs_interfaces::srv::PlanPaths::Response> response);

  /** @brief `/pp_service/partial_replan` 핸들러 (D-03 v3, R3). @return void */
  void on_partial_replan(
    const std::shared_ptr<mrs_interfaces::srv::PartialReplan::Request> request,
    std::shared_ptr<mrs_interfaces::srv::PartialReplan::Response> response);

  mrs::WindowedPathPlanner planner_; ///< seam a(IPathSolver) 소비자

  rclcpp::Subscription<mrs_interfaces::msg::TaskAssignment>::SharedPtr task_assignment_sub_;
  rclcpp::Publisher<mrs_interfaces::msg::PlannedPaths>::SharedPtr planned_paths_pub_;
  rclcpp::Service<mrs_interfaces::srv::PlanPaths>::SharedPtr plan_paths_srv_;
  rclcpp::Service<mrs_interfaces::srv::PartialReplan>::SharedPtr partial_replan_srv_;
};

} // namespace mrs
