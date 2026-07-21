/**
 * @file main.cpp
 * @brief ladder_orchestrator 실행 파일 진입점.
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_ladder/ladder_orchestrator_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::LadderOrchestratorNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(
      rclcpp::get_logger("LadderOrchestratorNode"), "기동 중 잡히지 않은 예외: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
