// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RealizabilityEntry.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability_entry.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/realizability_entry__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RealizabilityEntry_realizable
{
public:
  explicit Init_RealizabilityEntry_realizable(::mrs_msgs::msg::RealizabilityEntry & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RealizabilityEntry realizable(::mrs_msgs::msg::RealizabilityEntry::_realizable_type arg)
  {
    msg_.realizable = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RealizabilityEntry msg_;
};

class Init_RealizabilityEntry_alternative
{
public:
  explicit Init_RealizabilityEntry_alternative(::mrs_msgs::msg::RealizabilityEntry & msg)
  : msg_(msg)
  {}
  Init_RealizabilityEntry_realizable alternative(::mrs_msgs::msg::RealizabilityEntry::_alternative_type arg)
  {
    msg_.alternative = std::move(arg);
    return Init_RealizabilityEntry_realizable(msg_);
  }

private:
  ::mrs_msgs::msg::RealizabilityEntry msg_;
};

class Init_RealizabilityEntry_group
{
public:
  Init_RealizabilityEntry_group()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RealizabilityEntry_alternative group(::mrs_msgs::msg::RealizabilityEntry::_group_type arg)
  {
    msg_.group = std::move(arg);
    return Init_RealizabilityEntry_alternative(msg_);
  }

private:
  ::mrs_msgs::msg::RealizabilityEntry msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RealizabilityEntry>()
{
  return mrs_msgs::msg::builder::Init_RealizabilityEntry_group();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__BUILDER_HPP_
