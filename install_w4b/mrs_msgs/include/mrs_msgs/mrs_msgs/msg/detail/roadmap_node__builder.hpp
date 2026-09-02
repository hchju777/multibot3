// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RoadmapNode.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_node.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/roadmap_node__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RoadmapNode_clearance_m
{
public:
  explicit Init_RoadmapNode_clearance_m(::mrs_msgs::msg::RoadmapNode & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RoadmapNode clearance_m(::mrs_msgs::msg::RoadmapNode::_clearance_m_type arg)
  {
    msg_.clearance_m = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapNode msg_;
};

class Init_RoadmapNode_y
{
public:
  explicit Init_RoadmapNode_y(::mrs_msgs::msg::RoadmapNode & msg)
  : msg_(msg)
  {}
  Init_RoadmapNode_clearance_m y(::mrs_msgs::msg::RoadmapNode::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_RoadmapNode_clearance_m(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapNode msg_;
};

class Init_RoadmapNode_x
{
public:
  explicit Init_RoadmapNode_x(::mrs_msgs::msg::RoadmapNode & msg)
  : msg_(msg)
  {}
  Init_RoadmapNode_y x(::mrs_msgs::msg::RoadmapNode::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_RoadmapNode_y(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapNode msg_;
};

class Init_RoadmapNode_id
{
public:
  Init_RoadmapNode_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RoadmapNode_x id(::mrs_msgs::msg::RoadmapNode::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_RoadmapNode_x(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapNode msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RoadmapNode>()
{
  return mrs_msgs::msg::builder::Init_RoadmapNode_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__BUILDER_HPP_
