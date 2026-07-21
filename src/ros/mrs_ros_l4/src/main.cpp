/**
 * @file main.cpp
 * @brief l4_traj_node 실행 파일 진입점 — 로봇 1대분, per-robot 프로세스(architecture §1.1).
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_l4/local_traj_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::LocalTrajNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("LocalTrajNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
