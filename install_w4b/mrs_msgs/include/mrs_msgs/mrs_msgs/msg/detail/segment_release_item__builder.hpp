// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/SegmentReleaseItem.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment_release_item.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/segment_release_item__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_SegmentReleaseItem_released
{
public:
  explicit Init_SegmentReleaseItem_released(::mrs_msgs::msg::SegmentReleaseItem & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::SegmentReleaseItem released(::mrs_msgs::msg::SegmentReleaseItem::_released_type arg)
  {
    msg_.released = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentReleaseItem msg_;
};

class Init_SegmentReleaseItem_id
{
public:
  Init_SegmentReleaseItem_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SegmentReleaseItem_released id(::mrs_msgs::msg::SegmentReleaseItem::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_SegmentReleaseItem_released(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentReleaseItem msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::SegmentReleaseItem>()
{
  return mrs_msgs::msg::builder::Init_SegmentReleaseItem_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__BUILDER_HPP_
