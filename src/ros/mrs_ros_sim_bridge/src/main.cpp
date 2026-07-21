/**
 * @file main.cpp
 * @brief sim_bridge 실행 파일 진입점 — backend=pysim|isaac|fake config 한 줄 전환(R-01).
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_sim_bridge/sim_bridge_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::SimBridgeNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("SimBridgeNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
