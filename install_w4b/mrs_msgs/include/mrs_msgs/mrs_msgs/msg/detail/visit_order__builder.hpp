// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/VisitOrder.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/visit_order.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__VISIT_ORDER__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__VISIT_ORDER__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/visit_order__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_VisitOrder_sequence
{
public:
  explicit Init_VisitOrder_sequence(::mrs_msgs::msg::VisitOrder & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::VisitOrder sequence(::mrs_msgs::msg::VisitOrder::_sequence_type arg)
  {
    msg_.sequence = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::VisitOrder msg_;
};

class Init_VisitOrder_location
{
public:
  Init_VisitOrder_location()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_VisitOrder_sequence location(::mrs_msgs::msg::VisitOrder::_location_type arg)
  {
    msg_.location = std::move(arg);
    return Init_VisitOrder_sequence(msg_);
  }

private:
  ::mrs_msgs::msg::VisitOrder msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::VisitOrder>()
{
  return mrs_msgs::msg::builder::Init_VisitOrder_location();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__VISIT_ORDER__BUILDER_HPP_
