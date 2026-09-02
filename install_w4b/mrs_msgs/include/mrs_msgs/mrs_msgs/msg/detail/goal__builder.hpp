// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/goal.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/goal__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Goal_location
{
public:
  explicit Init_Goal_location(::mrs_msgs::msg::Goal & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Goal location(::mrs_msgs::msg::Goal::_location_type arg)
  {
    msg_.location = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Goal msg_;
};

class Init_Goal_task
{
public:
  explicit Init_Goal_task(::mrs_msgs::msg::Goal & msg)
  : msg_(msg)
  {}
  Init_Goal_location task(::mrs_msgs::msg::Goal::_task_type arg)
  {
    msg_.task = std::move(arg);
    return Init_Goal_location(msg_);
  }

private:
  ::mrs_msgs::msg::Goal msg_;
};

class Init_Goal_goal_id
{
public:
  Init_Goal_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Goal_task goal_id(::mrs_msgs::msg::Goal::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_Goal_task(msg_);
  }

private:
  ::mrs_msgs::msg::Goal msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Goal>()
{
  return mrs_msgs::msg::builder::Init_Goal_goal_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_
