// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotTrajectory.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_trajectory.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_trajectory__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotTrajectory_points
{
public:
  explicit Init_RobotTrajectory_points(::mrs_msgs::msg::RobotTrajectory & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotTrajectory points(::mrs_msgs::msg::RobotTrajectory::_points_type arg)
  {
    msg_.points = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotTrajectory msg_;
};

class Init_RobotTrajectory_robot
{
public:
  Init_RobotTrajectory_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotTrajectory_points robot(::mrs_msgs::msg::RobotTrajectory::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_RobotTrajectory_points(msg_);
  }

private:
  ::mrs_msgs::msg::RobotTrajectory msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotTrajectory>()
{
  return mrs_msgs::msg::builder::Init_RobotTrajectory_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__BUILDER_HPP_
