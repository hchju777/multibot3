// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Segment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__SEGMENT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/segment__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Segment_to_id
{
public:
  explicit Init_Segment_to_id(::mrs_msgs::msg::Segment & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Segment to_id(::mrs_msgs::msg::Segment::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Segment msg_;
};

class Init_Segment_from_id
{
public:
  explicit Init_Segment_from_id(::mrs_msgs::msg::Segment & msg)
  : msg_(msg)
  {}
  Init_Segment_to_id from_id(::mrs_msgs::msg::Segment::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_Segment_to_id(msg_);
  }

private:
  ::mrs_msgs::msg::Segment msg_;
};

class Init_Segment_robot
{
public:
  explicit Init_Segment_robot(::mrs_msgs::msg::Segment & msg)
  : msg_(msg)
  {}
  Init_Segment_from_id robot(::mrs_msgs::msg::Segment::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_Segment_from_id(msg_);
  }

private:
  ::mrs_msgs::msg::Segment msg_;
};

class Init_Segment_id
{
public:
  Init_Segment_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Segment_robot id(::mrs_msgs::msg::Segment::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Segment_robot(msg_);
  }

private:
  ::mrs_msgs::msg::Segment msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Segment>()
{
  return mrs_msgs::msg::builder::Init_Segment_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT__BUILDER_HPP_
