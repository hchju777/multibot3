// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Assignment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/assignment.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ASSIGNMENT__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ASSIGNMENT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/assignment__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Assignment_unassigned_tasks
{
public:
  explicit Init_Assignment_unassigned_tasks(::mrs_msgs::msg::Assignment & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Assignment unassigned_tasks(::mrs_msgs::msg::Assignment::_unassigned_tasks_type arg)
  {
    msg_.unassigned_tasks = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

class Init_Assignment_assignments
{
public:
  explicit Init_Assignment_assignments(::mrs_msgs::msg::Assignment & msg)
  : msg_(msg)
  {}
  Init_Assignment_unassigned_tasks assignments(::mrs_msgs::msg::Assignment::_assignments_type arg)
  {
    msg_.assignments = std::move(arg);
    return Init_Assignment_unassigned_tasks(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

class Init_Assignment_revision
{
public:
  explicit Init_Assignment_revision(::mrs_msgs::msg::Assignment & msg)
  : msg_(msg)
  {}
  Init_Assignment_assignments revision(::mrs_msgs::msg::Assignment::_revision_type arg)
  {
    msg_.revision = std::move(arg);
    return Init_Assignment_assignments(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

class Init_Assignment_instance_id
{
public:
  explicit Init_Assignment_instance_id(::mrs_msgs::msg::Assignment & msg)
  : msg_(msg)
  {}
  Init_Assignment_revision instance_id(::mrs_msgs::msg::Assignment::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Assignment_revision(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

class Init_Assignment_schema_version
{
public:
  explicit Init_Assignment_schema_version(::mrs_msgs::msg::Assignment & msg)
  : msg_(msg)
  {}
  Init_Assignment_instance_id schema_version(::mrs_msgs::msg::Assignment::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Assignment_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

class Init_Assignment_schema
{
public:
  Init_Assignment_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Assignment_schema_version schema(::mrs_msgs::msg::Assignment::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Assignment_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::Assignment msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Assignment>()
{
  return mrs_msgs::msg::builder::Init_Assignment_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ASSIGNMENT__BUILDER_HPP_
