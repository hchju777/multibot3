// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Realizability.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/realizability__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Realizability_entries
{
public:
  explicit Init_Realizability_entries(::mrs_msgs::msg::Realizability & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Realizability entries(::mrs_msgs::msg::Realizability::_entries_type arg)
  {
    msg_.entries = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Realizability msg_;
};

class Init_Realizability_rz_seq
{
public:
  explicit Init_Realizability_rz_seq(::mrs_msgs::msg::Realizability & msg)
  : msg_(msg)
  {}
  Init_Realizability_entries rz_seq(::mrs_msgs::msg::Realizability::_rz_seq_type arg)
  {
    msg_.rz_seq = std::move(arg);
    return Init_Realizability_entries(msg_);
  }

private:
  ::mrs_msgs::msg::Realizability msg_;
};

class Init_Realizability_instance_id
{
public:
  explicit Init_Realizability_instance_id(::mrs_msgs::msg::Realizability & msg)
  : msg_(msg)
  {}
  Init_Realizability_rz_seq instance_id(::mrs_msgs::msg::Realizability::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Realizability_rz_seq(msg_);
  }

private:
  ::mrs_msgs::msg::Realizability msg_;
};

class Init_Realizability_schema_version
{
public:
  explicit Init_Realizability_schema_version(::mrs_msgs::msg::Realizability & msg)
  : msg_(msg)
  {}
  Init_Realizability_instance_id schema_version(::mrs_msgs::msg::Realizability::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Realizability_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::Realizability msg_;
};

class Init_Realizability_schema
{
public:
  Init_Realizability_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Realizability_schema_version schema(::mrs_msgs::msg::Realizability::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Realizability_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::Realizability msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Realizability>()
{
  return mrs_msgs::msg::builder::Init_Realizability_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY__BUILDER_HPP_
