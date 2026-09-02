// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/EdgeRef.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/edge_ref.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__EDGE_REF__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__EDGE_REF__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/edge_ref__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_EdgeRef_to_id
{
public:
  explicit Init_EdgeRef_to_id(::mrs_msgs::msg::EdgeRef & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::EdgeRef to_id(::mrs_msgs::msg::EdgeRef::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::EdgeRef msg_;
};

class Init_EdgeRef_from_id
{
public:
  Init_EdgeRef_from_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EdgeRef_to_id from_id(::mrs_msgs::msg::EdgeRef::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_EdgeRef_to_id(msg_);
  }

private:
  ::mrs_msgs::msg::EdgeRef msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::EdgeRef>()
{
  return mrs_msgs::msg::builder::Init_EdgeRef_from_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__EDGE_REF__BUILDER_HPP_
