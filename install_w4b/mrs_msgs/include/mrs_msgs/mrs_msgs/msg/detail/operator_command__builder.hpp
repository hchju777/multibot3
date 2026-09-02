// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/OperatorCommand.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_command.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/operator_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_OperatorCommand_goal_location
{
public:
  explicit Init_OperatorCommand_goal_location(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::OperatorCommand goal_location(::mrs_msgs::msg::OperatorCommand::_goal_location_type arg)
  {
    msg_.goal_location = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_has_speed_cap_mps
{
public:
  explicit Init_OperatorCommand_has_speed_cap_mps(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  Init_OperatorCommand_goal_location has_speed_cap_mps(::mrs_msgs::msg::OperatorCommand::_has_speed_cap_mps_type arg)
  {
    msg_.has_speed_cap_mps = std::move(arg);
    return Init_OperatorCommand_goal_location(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_speed_cap_mps
{
public:
  explicit Init_OperatorCommand_speed_cap_mps(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  Init_OperatorCommand_has_speed_cap_mps speed_cap_mps(::mrs_msgs::msg::OperatorCommand::_speed_cap_mps_type arg)
  {
    msg_.speed_cap_mps = std::move(arg);
    return Init_OperatorCommand_has_speed_cap_mps(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_active
{
public:
  explicit Init_OperatorCommand_active(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  Init_OperatorCommand_speed_cap_mps active(::mrs_msgs::msg::OperatorCommand::_active_type arg)
  {
    msg_.active = std::move(arg);
    return Init_OperatorCommand_speed_cap_mps(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_verb
{
public:
  explicit Init_OperatorCommand_verb(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  Init_OperatorCommand_active verb(::mrs_msgs::msg::OperatorCommand::_verb_type arg)
  {
    msg_.verb = std::move(arg);
    return Init_OperatorCommand_active(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_robot
{
public:
  explicit Init_OperatorCommand_robot(::mrs_msgs::msg::OperatorCommand & msg)
  : msg_(msg)
  {}
  Init_OperatorCommand_verb robot(::mrs_msgs::msg::OperatorCommand::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_OperatorCommand_verb(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

class Init_OperatorCommand_seq
{
public:
  Init_OperatorCommand_seq()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_OperatorCommand_robot seq(::mrs_msgs::msg::OperatorCommand::_seq_type arg)
  {
    msg_.seq = std::move(arg);
    return Init_OperatorCommand_robot(msg_);
  }

private:
  ::mrs_msgs::msg::OperatorCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::OperatorCommand>()
{
  return mrs_msgs::msg::builder::Init_OperatorCommand_seq();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__BUILDER_HPP_
