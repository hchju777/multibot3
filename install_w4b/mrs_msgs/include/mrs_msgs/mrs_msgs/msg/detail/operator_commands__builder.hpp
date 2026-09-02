// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/OperatorCommands.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_commands.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/operator_commands__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_OperatorCommands_commands
{
public:
  explicit Init_OperatorCommands_commands(::mrs_msgs::msg::OperatorCommands & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::OperatorCommands commands(::mrs_msgs::msg::OperatorCommands::_commands_type arg)
  {
    msg_.commands = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommands msg_;
};

class Init_OperatorCommands_instance_id
{
public:
  explicit Init_OperatorCommands_instance_id(::mrs_msgs::msg::OperatorCommands & msg)
  : msg_(msg)
  {}
  Init_OperatorCommands_commands instance_id(::mrs_msgs::msg::OperatorCommands::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_OperatorCommands_commands(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommands msg_;
};

class Init_OperatorCommands_schema_version
{
public:
  explicit Init_OperatorCommands_schema_version(::mrs_msgs::msg::OperatorCommands & msg)
  : msg_(msg)
  {}
  Init_OperatorCommands_instance_id schema_version(::mrs_msgs::msg::OperatorCommands::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_OperatorCommands_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommands msg_;
};

class Init_OperatorCommands_schema
{
public:
  Init_OperatorCommands_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_OperatorCommands_schema_version schema(::mrs_msgs::msg::OperatorCommands::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_OperatorCommands_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommands msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::OperatorCommands>()
{
  return mrs_msgs::msg::builder::Init_OperatorCommands_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__BUILDER_HPP_
