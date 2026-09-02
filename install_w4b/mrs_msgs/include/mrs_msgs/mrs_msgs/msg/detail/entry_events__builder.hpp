// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/EntryEvents.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/entry_events.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/entry_events__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_EntryEvents_events
{
public:
  explicit Init_EntryEvents_events(::mrs_msgs::msg::EntryEvents & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::EntryEvents events(::mrs_msgs::msg::EntryEvents::_events_type arg)
  {
    msg_.events = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvents msg_;
};

class Init_EntryEvents_instance_id
{
public:
  explicit Init_EntryEvents_instance_id(::mrs_msgs::msg::EntryEvents & msg)
  : msg_(msg)
  {}
  Init_EntryEvents_events instance_id(::mrs_msgs::msg::EntryEvents::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_EntryEvents_events(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvents msg_;
};

class Init_EntryEvents_schema_version
{
public:
  explicit Init_EntryEvents_schema_version(::mrs_msgs::msg::EntryEvents & msg)
  : msg_(msg)
  {}
  Init_EntryEvents_instance_id schema_version(::mrs_msgs::msg::EntryEvents::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_EntryEvents_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvents msg_;
};

class Init_EntryEvents_schema
{
public:
  Init_EntryEvents_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EntryEvents_schema_version schema(::mrs_msgs::msg::EntryEvents::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_EntryEvents_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::EntryEvents msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::EntryEvents>()
{
  return mrs_msgs::msg::builder::Init_EntryEvents_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__BUILDER_HPP_
