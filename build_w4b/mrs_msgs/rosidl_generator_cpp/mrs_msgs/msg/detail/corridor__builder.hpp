// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/corridor.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__CORRIDOR__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__CORRIDOR__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/corridor__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Corridor_length_m
{
public:
  explicit Init_Corridor_length_m(::mrs_msgs::msg::Corridor & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Corridor length_m(::mrs_msgs::msg::Corridor::_length_m_type arg)
  {
    msg_.length_m = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Corridor msg_;
};

class Init_Corridor_edges
{
public:
  explicit Init_Corridor_edges(::mrs_msgs::msg::Corridor & msg)
  : msg_(msg)
  {}
  Init_Corridor_length_m edges(::mrs_msgs::msg::Corridor::_edges_type arg)
  {
    msg_.edges = std::move(arg);
    return Init_Corridor_length_m(msg_);
  }

private:
  ::mrs_msgs::msg::Corridor msg_;
};

class Init_Corridor_id
{
public:
  Init_Corridor_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Corridor_edges id(::mrs_msgs::msg::Corridor::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Corridor_edges(msg_);
  }

private:
  ::mrs_msgs::msg::Corridor msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Corridor>()
{
  return mrs_msgs::msg::builder::Init_Corridor_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__CORRIDOR__BUILDER_HPP_
