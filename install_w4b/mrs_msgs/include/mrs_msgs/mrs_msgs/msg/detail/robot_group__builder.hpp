// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotGroup.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_group.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_group__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotGroup_robots
{
public:
  Init_RobotGroup_robots()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::mrs_msgs::msg::RobotGroup robots(::mrs_msgs::msg::RobotGroup::_robots_type arg)
  {
    msg_.robots = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotGroup msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotGroup>()
{
  return mrs_msgs::msg::builder::Init_RobotGroup_robots();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__BUILDER_HPP_
