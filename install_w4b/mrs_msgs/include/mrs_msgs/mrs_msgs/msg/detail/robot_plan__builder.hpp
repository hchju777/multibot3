// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotPlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_plan.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_plan__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotPlan_steps
{
public:
  explicit Init_RobotPlan_steps(::mrs_msgs::msg::RobotPlan & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotPlan steps(::mrs_msgs::msg::RobotPlan::_steps_type arg)
  {
    msg_.steps = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotPlan msg_;
};

class Init_RobotPlan_terminal
{
public:
  explicit Init_RobotPlan_terminal(::mrs_msgs::msg::RobotPlan & msg)
  : msg_(msg)
  {}
  Init_RobotPlan_steps terminal(::mrs_msgs::msg::RobotPlan::_terminal_type arg)
  {
    msg_.terminal = std::move(arg);
    return Init_RobotPlan_steps(msg_);
  }

private:
  ::mrs_msgs::msg::RobotPlan msg_;
};

class Init_RobotPlan_robot
{
public:
  Init_RobotPlan_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotPlan_terminal robot(::mrs_msgs::msg::RobotPlan::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_RobotPlan_terminal(msg_);
  }

private:
  ::mrs_msgs::msg::RobotPlan msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotPlan>()
{
  return mrs_msgs::msg::builder::Init_RobotPlan_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__BUILDER_HPP_
