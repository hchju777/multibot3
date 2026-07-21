#pragma once

/**
 * @file map_registry_node.hpp
 * @brief map_registry 서비스 노드 — GetRoadmap/GetUniformView/GetDependencySkeleton/
 * TransformNode/ValidateMap 5종 + roadmap_version latched 통지 (D-11, contract-registry L-16).
 *
 * 콜백은 전부 `mrs::MapRegistry` 로 위임한다 — 이 패키지에 3뷰 변환 알고리즘을 두지 않는다
 * (architecture C4, CI 검사 §2.4-3).
 */

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/u_int64.hpp>

#include "mrs_map_registry/map_registry.hpp"
#include "mrs_interfaces/srv/get_roadmap.hpp"
#include "mrs_interfaces/srv/get_uniform_view.hpp"
#include "mrs_interfaces/srv/get_dependency_skeleton.hpp"
#include "mrs_interfaces/srv/transform_node.hpp"
#include "mrs_interfaces/srv/validate_map.hpp"

namespace mrs
{

/**
 * @brief MapRegistry 조회 서비스 5종을 노출하는 노드.
 */
class MapRegistryNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 서비스 5종과 roadmap_version 통지 토픽을 구성한다.
   */
  MapRegistryNode();
  ~MapRegistryNode() override = default;

private:
  /** @brief `/map_registry/get_roadmap` 핸들러. @return void */
  void on_get_roadmap(
    const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response);

  /** @brief `/map_registry/get_uniform_view` 핸들러. @return void */
  void on_get_uniform_view(
    const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response);

  /** @brief `/map_registry/get_dependency_skeleton` 핸들러. @return void */
  void on_get_dependency_skeleton(
    const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response);

  /** @brief `/map_registry/transform_node` 핸들러. @return void */
  void on_transform_node(
    const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
    std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response);

  /** @brief `/map_registry/validate_map` 핸들러. @return void */
  void on_validate_map(
    const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response);

  mrs::MapRegistry registry_; ///< 3뷰 정합 단일 소유자 (C4)

  rclcpp::Service<mrs_interfaces::srv::GetRoadmap>::SharedPtr get_roadmap_srv_;
  rclcpp::Service<mrs_interfaces::srv::GetUniformView>::SharedPtr get_uniform_view_srv_;
  rclcpp::Service<mrs_interfaces::srv::GetDependencySkeleton>::SharedPtr get_dependency_skeleton_srv_;
  rclcpp::Service<mrs_interfaces::srv::TransformNode>::SharedPtr transform_node_srv_;
  rclcpp::Service<mrs_interfaces::srv::ValidateMap>::SharedPtr validate_map_srv_;
  rclcpp::Publisher<std_msgs::msg::UInt64>::SharedPtr roadmap_version_pub_; ///< transient_local (Q-6)
};

} // namespace mrs
