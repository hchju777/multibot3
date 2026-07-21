/**
 * @file main.cpp
 * @brief bringup_startup_check 실행 파일 — R-A2 위반 시 0이 아닌 종료코드를 반환한다.
 *
 * launch 파일은 이 실행 파일을 다른 노드보다 먼저 실행하고, 종료코드가 0이 아니면
 * 전체 기동을 중단하도록 구성한다(Phase 5, ros-builder — launch 액션 배선).
 */

#include <rclcpp/rclcpp.hpp>

#include "mrs_bringup/startup_check_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<mrs::StartupCheckNode>();
  const bool ok = node->passed();
  rclcpp::shutdown();
  return ok ? 0 : 1;
}
