// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Alternative.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/alternative.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ALTERNATIVE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ALTERNATIVE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/alternative__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Alternative_dependencies
{
public:
  explicit Init_Alternative_dependencies(::mrs_msgs::msg::Alternative & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Alternative dependencies(::mrs_msgs::msg::Alternative::_dependencies_type arg)
  {
    msg_.dependencies = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Alternative msg_;
};

class Init_Alternative_id
{
public:
  Init_Alternative_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Alternative_dependencies id(::mrs_msgs::msg::Alternative::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Alternative_dependencies(msg_);
  }

private:
  ::mrs_msgs::msg::Alternative msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Alternative>()
{
  return mrs_msgs::msg::builder::Init_Alternative_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ALTERNATIVE__BUILDER_HPP_
