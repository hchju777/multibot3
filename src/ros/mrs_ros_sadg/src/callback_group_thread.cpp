/**
 * @file callback_group_thread.cpp
 * @brief @ref mrs::ros_sadg::CallbackGroupThread 본문.
 */

#include "mrs_ros_sadg/callback_group_thread.hpp"

#include <exception>
#include <utility>

namespace mrs::ros_sadg
{

CallbackGroupThread::CallbackGroupThread(
  rclcpp::CallbackGroup::SharedPtr group,
  rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base, rclcpp::Logger logger)
: logger_(std::move(logger))
{
  if (group == nullptr || node_base == nullptr)
  {
    // 스레드를 띄우지 않는다. 이 상태에서도 노드는 살아 있어야 하므로(CLAUDE.md 규율 2)
    // 예외를 던지지 않고 로그만 남긴다 — 해당 그룹의 콜백은 실행되지 않는다.
    RCLCPP_ERROR(logger_, "CallbackGroupThread: group 또는 node_base 가 null 이다 — 스레드 미기동");
    return;
  }

  executor_.add_callback_group(group, node_base);

  thread_ = std::thread(
    [this]()
    {
      try
      {
        executor_.spin();
      }
      catch (const std::exception & e)
      {
        // spin 이 죽어도 프로세스를 내리지 않는다 — 나머지 콜백그룹(릴리스 경로)은 계속 돈다.
        RCLCPP_ERROR(logger_, "CallbackGroupThread: 실행자 spin 실패: %s", e.what());
      }
      catch (...)
      {
        RCLCPP_ERROR(logger_, "CallbackGroupThread: 실행자 spin 실패 (알 수 없는 예외)");
      }
    });
}

CallbackGroupThread::~CallbackGroupThread()
{
  stop();
}

void CallbackGroupThread::stop() noexcept
{
  try
  {
    executor_.cancel();
    if (thread_.joinable())
    {
      thread_.join();
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR(logger_, "CallbackGroupThread: 정리 실패: %s", e.what());
  }
  catch (...)
  {
    // 소멸자 경로이므로 어떤 예외도 밖으로 내지 않는다.
  }
}

} // namespace mrs::ros_sadg
