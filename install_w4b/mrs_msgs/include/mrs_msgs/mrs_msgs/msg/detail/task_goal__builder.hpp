// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/TaskGoal.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/task_goal.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__TASK_GOAL__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__TASK_GOAL__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/task_goal__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_TaskGoal_location
{
public:
  explicit Init_TaskGoal_location(::mrs_msgs::msg::TaskGoal & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::TaskGoal location(::mrs_msgs::msg::TaskGoal::_location_type arg)
  {
    msg_.location = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::TaskGoal msg_;
};

class Init_TaskGoal_goal_id
{
public:
  Init_TaskGoal_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TaskGoal_location goal_id(::mrs_msgs::msg::TaskGoal::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_TaskGoal_location(msg_);
  }

private:
  ::mrs_msgs::msg::TaskGoal msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::TaskGoal>()
{
  return mrs_msgs::msg::builder::Init_TaskGoal_goal_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__TASK_GOAL__BUILDER_HPP_
