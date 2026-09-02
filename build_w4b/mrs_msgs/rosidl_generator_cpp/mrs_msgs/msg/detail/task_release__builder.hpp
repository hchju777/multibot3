// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/TaskRelease.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/task_release.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__TASK_RELEASE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__TASK_RELEASE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/task_release__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_TaskRelease_goals
{
public:
  explicit Init_TaskRelease_goals(::mrs_msgs::msg::TaskRelease & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::TaskRelease goals(::mrs_msgs::msg::TaskRelease::_goals_type arg)
  {
    msg_.goals = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

class Init_TaskRelease_task
{
public:
  explicit Init_TaskRelease_task(::mrs_msgs::msg::TaskRelease & msg)
  : msg_(msg)
  {}
  Init_TaskRelease_goals task(::mrs_msgs::msg::TaskRelease::_task_type arg)
  {
    msg_.task = std::move(arg);
    return Init_TaskRelease_goals(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

class Init_TaskRelease_release_index
{
public:
  explicit Init_TaskRelease_release_index(::mrs_msgs::msg::TaskRelease & msg)
  : msg_(msg)
  {}
  Init_TaskRelease_task release_index(::mrs_msgs::msg::TaskRelease::_release_index_type arg)
  {
    msg_.release_index = std::move(arg);
    return Init_TaskRelease_task(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

class Init_TaskRelease_instance_id
{
public:
  explicit Init_TaskRelease_instance_id(::mrs_msgs::msg::TaskRelease & msg)
  : msg_(msg)
  {}
  Init_TaskRelease_release_index instance_id(::mrs_msgs::msg::TaskRelease::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_TaskRelease_release_index(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

class Init_TaskRelease_schema_version
{
public:
  explicit Init_TaskRelease_schema_version(::mrs_msgs::msg::TaskRelease & msg)
  : msg_(msg)
  {}
  Init_TaskRelease_instance_id schema_version(::mrs_msgs::msg::TaskRelease::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_TaskRelease_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

class Init_TaskRelease_schema
{
public:
  Init_TaskRelease_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TaskRelease_schema_version schema(::mrs_msgs::msg::TaskRelease::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_TaskRelease_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::TaskRelease msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::TaskRelease>()
{
  return mrs_msgs::msg::builder::Init_TaskRelease_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__TASK_RELEASE__BUILDER_HPP_
