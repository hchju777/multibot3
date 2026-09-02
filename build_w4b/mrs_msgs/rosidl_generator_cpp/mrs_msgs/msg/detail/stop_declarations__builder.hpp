// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/StopDeclarations.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declarations.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/stop_declarations__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_StopDeclarations_declarations
{
public:
  explicit Init_StopDeclarations_declarations(::mrs_msgs::msg::StopDeclarations & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::StopDeclarations declarations(::mrs_msgs::msg::StopDeclarations::_declarations_type arg)
  {
    msg_.declarations = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclarations msg_;
};

class Init_StopDeclarations_instance_id
{
public:
  explicit Init_StopDeclarations_instance_id(::mrs_msgs::msg::StopDeclarations & msg)
  : msg_(msg)
  {}
  Init_StopDeclarations_declarations instance_id(::mrs_msgs::msg::StopDeclarations::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_StopDeclarations_declarations(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclarations msg_;
};

class Init_StopDeclarations_schema_version
{
public:
  explicit Init_StopDeclarations_schema_version(::mrs_msgs::msg::StopDeclarations & msg)
  : msg_(msg)
  {}
  Init_StopDeclarations_instance_id schema_version(::mrs_msgs::msg::StopDeclarations::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_StopDeclarations_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclarations msg_;
};

class Init_StopDeclarations_schema
{
public:
  Init_StopDeclarations_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_StopDeclarations_schema_version schema(::mrs_msgs::msg::StopDeclarations::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_StopDeclarations_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclarations msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::StopDeclarations>()
{
  return mrs_msgs::msg::builder::Init_StopDeclarations_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__BUILDER_HPP_
