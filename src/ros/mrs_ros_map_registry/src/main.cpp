/**
 * @file main.cpp
 * @brief map_registry 실행 파일 진입점.
 *
 * 전용 `SingleThreadedExecutor` 를 명시적으로 쓴다(nav2-reference §2-A1: 클론 전체에
 * `MultiThreadedExecutor` 사용 0건). 서비스 5종이 한 MutuallyExclusive 그룹에 묶여 있으므로
 * 콜백은 직렬로 실행된다.
 */

#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_map_registry/map_registry_node.hpp"

/**
 * @brief map_registry 프로세스를 기동하고 종료까지 spin 한다.
 * @param[in] argc 명령행 인자 개수. 자료형 `int`.
 * @param[in] argv 명령행 인자 배열. 자료형 `char **`. ROS 인자(`--ros-args`)를 포함한다.
 * @return `int` — 정상 종료 0, 기동·실행 중 미포착 예외로 중단되면 1.
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::MapRegistryNode>();
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("MapRegistryNode"), "uncaught exception: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  catch (...)
  {
    RCLCPP_FATAL(rclcpp::get_logger("MapRegistryNode"), "uncaught unknown exception");
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
