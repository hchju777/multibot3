// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/EntryEvent.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/entry_event.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/entry_event__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_EntryEvent_entered
{
public:
  explicit Init_EntryEvent_entered(::mrs_msgs::msg::EntryEvent & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::EntryEvent entered(::mrs_msgs::msg::EntryEvent::_entered_type arg)
  {
    msg_.entered = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvent msg_;
};

class Init_EntryEvent_location
{
public:
  explicit Init_EntryEvent_location(::mrs_msgs::msg::EntryEvent & msg)
  : msg_(msg)
  {}
  Init_EntryEvent_entered location(::mrs_msgs::msg::EntryEvent::_location_type arg)
  {
    msg_.location = std::move(arg);
    return Init_EntryEvent_entered(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvent msg_;
};

class Init_EntryEvent_segment
{
public:
  explicit Init_EntryEvent_segment(::mrs_msgs::msg::EntryEvent & msg)
  : msg_(msg)
  {}
  Init_EntryEvent_location segment(::mrs_msgs::msg::EntryEvent::_segment_type arg)
  {
    msg_.segment = std::move(arg);
    return Init_EntryEvent_location(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvent msg_;
};

class Init_EntryEvent_robot
{
public:
  explicit Init_EntryEvent_robot(::mrs_msgs::msg::EntryEvent & msg)
  : msg_(msg)
  {}
  Init_EntryEvent_segment robot(::mrs_msgs::msg::EntryEvent::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_EntryEvent_segment(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvent msg_;
};

class Init_EntryEvent_seq
{
public:
  Init_EntryEvent_seq()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EntryEvent_robot seq(::mrs_msgs::msg::EntryEvent::_seq_type arg)
  {
    msg_.seq = std::move(arg);
    return Init_EntryEvent_robot(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvent msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::EntryEvent>()
{
  return mrs_msgs::msg::builder::Init_EntryEvent_seq();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__BUILDER_HPP_
