/**
 * @file main.cpp
 * @brief l4_traj_node 실행 파일 진입점 — 로봇 1 대분, per-robot 프로세스(architecture §1.1).
 *
 * per-robot 프로세스로 두는 이유는 로봇 1 대의 실패가 함대를 죽이지 않게 하기 위함이다.
 * 그 취지가 실제로 성립하려면 **콜백이 예외로 프로세스를 끝내지 않아야** 하며, 그 방어선은
 * `LocalTrajNode` 의 콜백별 try/catch 다. 여기 있는 catch 는 **기동 경로 전용**의 최후 방어선이다.
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
    RCLCPP_FATAL(rclcpp::get_logger("LocalTrajNode"), "기동 중 잡히지 않은 예외: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
