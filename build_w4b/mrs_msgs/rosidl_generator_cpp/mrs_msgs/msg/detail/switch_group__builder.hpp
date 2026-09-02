// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/SwitchGroup.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/switch_group.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/switch_group__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_SwitchGroup_alternatives
{
public:
  explicit Init_SwitchGroup_alternatives(::mrs_msgs::msg::SwitchGroup & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::SwitchGroup alternatives(::mrs_msgs::msg::SwitchGroup::_alternatives_type arg)
  {
    msg_.alternatives = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::SwitchGroup msg_;
};

class Init_SwitchGroup_location
{
public:
  explicit Init_SwitchGroup_location(::mrs_msgs::msg::SwitchGroup & msg)
  : msg_(msg)
  {}
  Init_SwitchGroup_alternatives location(::mrs_msgs::msg::SwitchGroup::_location_type arg)
  {
    msg_.location = std::move(arg);
    return Init_SwitchGroup_alternatives(msg_);
  }

private:
  ::mrs_msgs::msg::SwitchGroup msg_;
};

class Init_SwitchGroup_id
{
public:
  Init_SwitchGroup_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SwitchGroup_location id(::mrs_msgs::msg::SwitchGroup::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_SwitchGroup_location(msg_);
  }

private:
  ::mrs_msgs::msg::SwitchGroup msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::SwitchGroup>()
{
  return mrs_msgs::msg::builder::Init_SwitchGroup_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__BUILDER_HPP_
