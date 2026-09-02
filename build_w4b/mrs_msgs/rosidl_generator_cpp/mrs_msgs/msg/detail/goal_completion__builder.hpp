// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/GoalCompletion.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/goal_completion.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__GOAL_COMPLETION__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__GOAL_COMPLETION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/goal_completion__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_GoalCompletion_revision
{
public:
  explicit Init_GoalCompletion_revision(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::GoalCompletion revision(::mrs_msgs::msg::GoalCompletion::_revision_type arg)
  {
    msg_.revision = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_goal_id
{
public:
  explicit Init_GoalCompletion_goal_id(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  Init_GoalCompletion_revision goal_id(::mrs_msgs::msg::GoalCompletion::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_GoalCompletion_revision(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_robot
{
public:
  explicit Init_GoalCompletion_robot(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  Init_GoalCompletion_goal_id robot(::mrs_msgs::msg::GoalCompletion::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_GoalCompletion_goal_id(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_seq
{
public:
  explicit Init_GoalCompletion_seq(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  Init_GoalCompletion_robot seq(::mrs_msgs::msg::GoalCompletion::_seq_type arg)
  {
    msg_.seq = std::move(arg);
    return Init_GoalCompletion_robot(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_instance_id
{
public:
  explicit Init_GoalCompletion_instance_id(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  Init_GoalCompletion_seq instance_id(::mrs_msgs::msg::GoalCompletion::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_GoalCompletion_seq(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_schema_version
{
public:
  explicit Init_GoalCompletion_schema_version(::mrs_msgs::msg::GoalCompletion & msg)
  : msg_(msg)
  {}
  Init_GoalCompletion_instance_id schema_version(::mrs_msgs::msg::GoalCompletion::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_GoalCompletion_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

class Init_GoalCompletion_schema
{
public:
  Init_GoalCompletion_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GoalCompletion_schema_version schema(::mrs_msgs::msg::GoalCompletion::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_GoalCompletion_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::GoalCompletion msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::GoalCompletion>()
{
  return mrs_msgs::msg::builder::Init_GoalCompletion_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__GOAL_COMPLETION__BUILDER_HPP_
