// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declaration.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/stop_declaration__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_StopDeclaration_reason
{
public:
  explicit Init_StopDeclaration_reason(::mrs_msgs::msg::StopDeclaration & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::StopDeclaration reason(::mrs_msgs::msg::StopDeclaration::_reason_type arg)
  {
    msg_.reason = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclaration msg_;
};

class Init_StopDeclaration_declared
{
public:
  explicit Init_StopDeclaration_declared(::mrs_msgs::msg::StopDeclaration & msg)
  : msg_(msg)
  {}
  Init_StopDeclaration_reason declared(::mrs_msgs::msg::StopDeclaration::_declared_type arg)
  {
    msg_.declared = std::move(arg);
    return Init_StopDeclaration_reason(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclaration msg_;
};

class Init_StopDeclaration_blocked_edge
{
public:
  explicit Init_StopDeclaration_blocked_edge(::mrs_msgs::msg::StopDeclaration & msg)
  : msg_(msg)
  {}
  Init_StopDeclaration_declared blocked_edge(::mrs_msgs::msg::StopDeclaration::_blocked_edge_type arg)
  {
    msg_.blocked_edge = std::move(arg);
    return Init_StopDeclaration_declared(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclaration msg_;
};

class Init_StopDeclaration_robot
{
public:
  explicit Init_StopDeclaration_robot(::mrs_msgs::msg::StopDeclaration & msg)
  : msg_(msg)
  {}
  Init_StopDeclaration_blocked_edge robot(::mrs_msgs::msg::StopDeclaration::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_StopDeclaration_blocked_edge(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclaration msg_;
};

class Init_StopDeclaration_seq
{
public:
  Init_StopDeclaration_seq()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_StopDeclaration_robot seq(::mrs_msgs::msg::StopDeclaration::_seq_type arg)
  {
    msg_.seq = std::move(arg);
    return Init_StopDeclaration_robot(msg_);
  }

private:
  ::mrs_msgs::msg::StopDeclaration msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::StopDeclaration>()
{
  return mrs_msgs::msg::builder::Init_StopDeclaration_seq();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__BUILDER_HPP_
