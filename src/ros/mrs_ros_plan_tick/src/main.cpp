/**
 * @file main.cpp
 * @brief plan_tick_node 실행 파일 진입점 — mrs_bringup 이 인스턴스 1개만 기동함을 보장한다(R-05).
 *
 * 전용 `SingleThreadedExecutor` 를 명시적으로 쓴다(nav2-reference §2-A1: 클론 전체에
 * `MultiThreadedExecutor` 사용 0건). `rclcpp::spin` 과 동작은 같으나, 실행자 모델이 코드에
 * 드러나 있어야 이후 누가 콜백을 추가할 때 병렬 실행을 무심코 도입하지 않는다.
 */

#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_plan_tick/plan_tick_node.hpp"

/**
 * @brief plan_tick_node 프로세스를 기동하고 종료까지 spin 한다.
 * @param[in] argc 명령행 인자 개수. 자료형 `int`.
 * @param[in] argv 명령행 인자 배열. 자료형 `char **`. ROS 인자(`--ros-args`)를 포함한다.
 * @return `int` — 정상 종료 0, 기동·실행 중 미포착 예외로 중단되면 1.
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::PlanTickNode>();
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("PlanTickNode"), "uncaught exception: %s", e.what());
    rclcpp::shutdown();
    return 1;
  }
  catch (...)
  {
    RCLCPP_FATAL(rclcpp::get_logger("PlanTickNode"), "uncaught unknown exception");
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return 0;
}
