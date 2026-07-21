/**
 * @file main.cpp
 * @brief pp_service 실행 파일 진입점.
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_pp/pp_service_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::PpServiceNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("PpServiceNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
