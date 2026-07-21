/**
 * @file main.cpp
 * @brief plan_tick_node 실행 파일 진입점 — mrs_bringup 이 인스턴스 1개만 기동함을 보장한다(R-05).
 */

#include <exception>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_plan_tick/plan_tick_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::PlanTickNode>();
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("PlanTickNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
