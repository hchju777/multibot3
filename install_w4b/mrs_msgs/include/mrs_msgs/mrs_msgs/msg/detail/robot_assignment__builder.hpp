// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotAssignment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_assignment.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_assignment__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotAssignment_goals
{
public:
  explicit Init_RobotAssignment_goals(::mrs_msgs::msg::RobotAssignment & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotAssignment goals(::mrs_msgs::msg::RobotAssignment::_goals_type arg)
  {
    msg_.goals = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotAssignment msg_;
};

class Init_RobotAssignment_start
{
public:
  explicit Init_RobotAssignment_start(::mrs_msgs::msg::RobotAssignment & msg)
  : msg_(msg)
  {}
  Init_RobotAssignment_goals start(::mrs_msgs::msg::RobotAssignment::_start_type arg)
  {
    msg_.start = std::move(arg);
    return Init_RobotAssignment_goals(msg_);
  }

private:
  ::mrs_msgs::msg::RobotAssignment msg_;
};

class Init_RobotAssignment_robot
{
public:
  Init_RobotAssignment_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotAssignment_start robot(::mrs_msgs::msg::RobotAssignment::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_RobotAssignment_start(msg_);
  }

private:
  ::mrs_msgs::msg::RobotAssignment msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotAssignment>()
{
  return mrs_msgs::msg::builder::Init_RobotAssignment_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__BUILDER_HPP_
