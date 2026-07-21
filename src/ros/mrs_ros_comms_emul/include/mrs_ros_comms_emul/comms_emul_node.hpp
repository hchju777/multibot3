#pragma once

/**
 * @file comms_emul_node.hpp
 * @brief comms_emul — 결함주입 인터포지션 릴레이 (D-10). Proxy/Decorator, 코어 노드 무변경.
 *
 * architecture §1.8-D: `local_plan_share` 는 D-10 v4 의 **주 표적**이다(R-09). 발행 측을
 * launch remap 으로 `.../local_plan_share_src` 로 옮기고 이 노드가 계약 이름
 * `.../local_plan_share` 로 중계한다. N<=8 에서는 수신자별 팬아웃 중계로 독립 손실을
 * 구현할 수 있으나(§1.8-D), 스캐폴딩 단계는 단순 1:1 중계 자리만 둔다 — 결함주입 로직
 * (지연·손실 적용)은 Phase 5(ros-builder) 대상이다.
 *
 * ROS 무의존 원칙(C1)은 이 패키지에 적용되지 않는다 — `ros/` 패키지이며 도메인 알고리즘을
 * 두지 않는 것이 대신 적용되는 규율이다(architecture §2.4-3 CI 검사).
 */

#include <cstdint>
#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "mrs_interfaces/msg/local_plan_share.hpp"

namespace mrs
{

/**
 * @brief 로봇↔로봇 `local_plan_share` 중계 노드 (D-10 v4 주 표적).
 */
class CommsEmulNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 로봇 수만큼 (source 구독, 중계 발행) 쌍을 구성한다.
   */
  CommsEmulNode();
  ~CommsEmulNode() override = default;

private:
  /**
   * @brief 로봇 i 의 `.../local_plan_share_src` 수신 콜백 — 결함 적용 후 중계 발행.
   * @param[in] robot_index 0-base 로봇 인덱스.
   * @param[in] msg 원본 메시지.
   * @return void
   */
  void on_local_plan_share_src(
    std::size_t robot_index, const mrs_interfaces::msg::LocalPlanShare::SharedPtr msg);

  std::vector<rclcpp::Subscription<mrs_interfaces::msg::LocalPlanShare>::SharedPtr> source_subs_;
  std::vector<rclcpp::Publisher<mrs_interfaces::msg::LocalPlanShare>::SharedPtr> relay_pubs_;

  int robot_count_{2}; ///< 파라미터 robot_count 기본값
};

} // namespace mrs
