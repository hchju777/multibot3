/**
 * @file main.cpp
 * @brief sim_bridge 실행 파일 진입점 — backend=pysim|isaac|fake config 한 줄 전환(R-01).
 *
 * 백엔드 구성에 실패하면 **spin 하지 않고 0 이 아닌 종료코드로 빠져나간다**. 살아 있으나
 * 아무 일도 못 하는 노드는 "시뮬이 도는 것처럼 보이지만 세계가 멈춰 있는" 상태를 만들고,
 * 그 상태에서 나온 배관 실측치는 전부 거짓이다.
 */

#include <exception>
#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "mrs_ros_sim_bridge/sim_bridge_node.hpp"

/**
 * @brief sim_bridge 노드를 기동한다.
 * @param[in] argc 인자 개수. 자료형 `int`.
 * @param[in] argv 인자 배열. 자료형 `char **`.
 * @return `int` — 정상 종료 0, 백엔드 구성 실패(기동 거부) 1, 미포착 예외 2.
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  try
  {
    auto node = std::make_shared<mrs::SimBridgeNode>();
    if (!node->backend_ready())
    {
      RCLCPP_FATAL(
        node->get_logger(),
        "sim_bridge 기동 거부 — 백엔드가 준비되지 않았습니다. 위의 FATAL 로그를 확인하십시오.");
      rclcpp::shutdown();
      return 1;
    }
    rclcpp::spin(node);
  }
  catch (const std::exception & e)
  {
    RCLCPP_FATAL(
      rclcpp::get_logger("SimBridgeNode"), "uncaught exception at startup: %s", e.what());
    rclcpp::shutdown();
    return 2;
  }
  rclcpp::shutdown();
  return 0;
}
