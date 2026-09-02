// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/SegmentRelease.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment_release.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/segment_release__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_SegmentRelease_segments
{
public:
  explicit Init_SegmentRelease_segments(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::SegmentRelease segments(::mrs_msgs::msg::SegmentRelease::_segments_type arg)
  {
    msg_.segments = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_entry_seq_ack
{
public:
  explicit Init_SegmentRelease_entry_seq_ack(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_segments entry_seq_ack(::mrs_msgs::msg::SegmentRelease::_entry_seq_ack_type arg)
  {
    msg_.entry_seq_ack = std::move(arg);
    return Init_SegmentRelease_segments(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_commit_seq
{
public:
  explicit Init_SegmentRelease_commit_seq(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_entry_seq_ack commit_seq(::mrs_msgs::msg::SegmentRelease::_commit_seq_type arg)
  {
    msg_.commit_seq = std::move(arg);
    return Init_SegmentRelease_entry_seq_ack(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_release_seq
{
public:
  explicit Init_SegmentRelease_release_seq(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_commit_seq release_seq(::mrs_msgs::msg::SegmentRelease::_release_seq_type arg)
  {
    msg_.release_seq = std::move(arg);
    return Init_SegmentRelease_commit_seq(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_robot
{
public:
  explicit Init_SegmentRelease_robot(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_release_seq robot(::mrs_msgs::msg::SegmentRelease::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_SegmentRelease_release_seq(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_instance_id
{
public:
  explicit Init_SegmentRelease_instance_id(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_robot instance_id(::mrs_msgs::msg::SegmentRelease::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_SegmentRelease_robot(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_schema_version
{
public:
  explicit Init_SegmentRelease_schema_version(::mrs_msgs::msg::SegmentRelease & msg)
  : msg_(msg)
  {}
  Init_SegmentRelease_instance_id schema_version(::mrs_msgs::msg::SegmentRelease::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_SegmentRelease_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

class Init_SegmentRelease_schema
{
public:
  Init_SegmentRelease_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SegmentRelease_schema_version schema(::mrs_msgs::msg::SegmentRelease::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_SegmentRelease_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::SegmentRelease msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::SegmentRelease>()
{
  return mrs_msgs::msg::builder::Init_SegmentRelease_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__BUILDER_HPP_
