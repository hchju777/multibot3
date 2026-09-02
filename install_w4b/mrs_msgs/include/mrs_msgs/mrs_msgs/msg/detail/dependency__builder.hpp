// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Dependency.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/dependency.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__DEPENDENCY__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__DEPENDENCY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/dependency__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Dependency_type
{
public:
  explicit Init_Dependency_type(::mrs_msgs::msg::Dependency & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Dependency type(::mrs_msgs::msg::Dependency::_type_type arg)
  {
    msg_.type = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Dependency msg_;
};

class Init_Dependency_to_id
{
public:
  explicit Init_Dependency_to_id(::mrs_msgs::msg::Dependency & msg)
  : msg_(msg)
  {}
  Init_Dependency_type to_id(::mrs_msgs::msg::Dependency::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return Init_Dependency_type(msg_);
  }

private:
  ::mrs_msgs::msg::Dependency msg_;
};

class Init_Dependency_from_id
{
public:
  Init_Dependency_from_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Dependency_to_id from_id(::mrs_msgs::msg::Dependency::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_Dependency_to_id(msg_);
  }

private:
  ::mrs_msgs::msg::Dependency msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Dependency>()
{
  return mrs_msgs::msg::builder::Init_Dependency_from_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__DEPENDENCY__BUILDER_HPP_
