/**
 * @file main.cpp
 * @brief comms_emul 실행 파일 진입점 — D-10 실험에서만 기동(선택적).
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_comms_emul/comms_emul_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::CommsEmulNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(
      rclcpp::get_logger("CommsEmulNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
