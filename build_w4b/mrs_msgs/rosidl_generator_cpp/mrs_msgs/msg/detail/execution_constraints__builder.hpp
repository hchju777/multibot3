// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/ExecutionConstraints.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/execution_constraints.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/execution_constraints__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_ExecutionConstraints_switch_groups
{
public:
  explicit Init_ExecutionConstraints_switch_groups(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::ExecutionConstraints switch_groups(::mrs_msgs::msg::ExecutionConstraints::_switch_groups_type arg)
  {
    msg_.switch_groups = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_dependencies
{
public:
  explicit Init_ExecutionConstraints_dependencies(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  Init_ExecutionConstraints_switch_groups dependencies(::mrs_msgs::msg::ExecutionConstraints::_dependencies_type arg)
  {
    msg_.dependencies = std::move(arg);
    return Init_ExecutionConstraints_switch_groups(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_segments
{
public:
  explicit Init_ExecutionConstraints_segments(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  Init_ExecutionConstraints_dependencies segments(::mrs_msgs::msg::ExecutionConstraints::_segments_type arg)
  {
    msg_.segments = std::move(arg);
    return Init_ExecutionConstraints_dependencies(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_commit_seq
{
public:
  explicit Init_ExecutionConstraints_commit_seq(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  Init_ExecutionConstraints_segments commit_seq(::mrs_msgs::msg::ExecutionConstraints::_commit_seq_type arg)
  {
    msg_.commit_seq = std::move(arg);
    return Init_ExecutionConstraints_segments(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_instance_id
{
public:
  explicit Init_ExecutionConstraints_instance_id(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  Init_ExecutionConstraints_commit_seq instance_id(::mrs_msgs::msg::ExecutionConstraints::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_ExecutionConstraints_commit_seq(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_schema_version
{
public:
  explicit Init_ExecutionConstraints_schema_version(::mrs_msgs::msg::ExecutionConstraints & msg)
  : msg_(msg)
  {}
  Init_ExecutionConstraints_instance_id schema_version(::mrs_msgs::msg::ExecutionConstraints::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_ExecutionConstraints_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

class Init_ExecutionConstraints_schema
{
public:
  Init_ExecutionConstraints_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ExecutionConstraints_schema_version schema(::mrs_msgs::msg::ExecutionConstraints::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_ExecutionConstraints_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::ExecutionConstraints msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::ExecutionConstraints>()
{
  return mrs_msgs::msg::builder::Init_ExecutionConstraints_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__BUILDER_HPP_
