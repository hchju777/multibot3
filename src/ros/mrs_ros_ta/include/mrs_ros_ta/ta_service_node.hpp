#pragma once

/**
 * @file ta_service_node.hpp
 * @brief ta_service — L1 작업할당 노드 (D-02). 사다리 최상단 R4 담당.
 */

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/task_assignment.hpp"
#include "mrs_interfaces/srv/reassign_request.hpp"
#include "mrs_task_alloc/lifelong_task_allocator.hpp"

namespace mrs
{

/**
 * @brief L1 작업할당 서비스 노드.
 */
class TaServiceNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 `/task_assignment` 발행자와 `/ta_service/reassign_request` 서비스를
   * 구성한다.
   */
  TaServiceNode();
  ~TaServiceNode() override = default;

private:
  /**
   * @brief `/ta_service/reassign_request` 핸들러 — R4 재할당 요청 처리 (D-02 v2 혼합 트리거).
   * @return void
   */
  void on_reassign_request(
    const std::shared_ptr<mrs_interfaces::srv::ReassignRequest::Request> request,
    std::shared_ptr<mrs_interfaces::srv::ReassignRequest::Response> response);

  mrs::LifelongTaskAllocator allocator_; ///< 분리형 lifelong TA (D-02)

  rclcpp::Publisher<mrs_interfaces::msg::TaskAssignment>::SharedPtr task_assignment_pub_;
  rclcpp::Service<mrs_interfaces::srv::ReassignRequest>::SharedPtr reassign_request_srv_;
};

} // namespace mrs
