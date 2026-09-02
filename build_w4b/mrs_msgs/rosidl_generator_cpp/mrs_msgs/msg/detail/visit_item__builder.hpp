// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/VisitItem.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/visit_item.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__VISIT_ITEM__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__VISIT_ITEM__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/visit_item__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_VisitItem_index
{
public:
  explicit Init_VisitItem_index(::mrs_msgs::msg::VisitItem & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::VisitItem index(::mrs_msgs::msg::VisitItem::_index_type arg)
  {
    msg_.index = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::VisitItem msg_;
};

class Init_VisitItem_robot
{
public:
  Init_VisitItem_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_VisitItem_index robot(::mrs_msgs::msg::VisitItem::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_VisitItem_index(msg_);
  }

private:
  ::mrs_msgs::msg::VisitItem msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::VisitItem>()
{
  return mrs_msgs::msg::builder::Init_VisitItem_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__VISIT_ITEM__BUILDER_HPP_
