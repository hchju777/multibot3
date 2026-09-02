// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_edge.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/roadmap_edge__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RoadmapEdge_corridor
{
public:
  explicit Init_RoadmapEdge_corridor(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RoadmapEdge corridor(::mrs_msgs::msg::RoadmapEdge::_corridor_type arg)
  {
    msg_.corridor = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_capacity_robots
{
public:
  explicit Init_RoadmapEdge_capacity_robots(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  Init_RoadmapEdge_corridor capacity_robots(::mrs_msgs::msg::RoadmapEdge::_capacity_robots_type arg)
  {
    msg_.capacity_robots = std::move(arg);
    return Init_RoadmapEdge_corridor(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_width_m
{
public:
  explicit Init_RoadmapEdge_width_m(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  Init_RoadmapEdge_capacity_robots width_m(::mrs_msgs::msg::RoadmapEdge::_width_m_type arg)
  {
    msg_.width_m = std::move(arg);
    return Init_RoadmapEdge_capacity_robots(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_length_m
{
public:
  explicit Init_RoadmapEdge_length_m(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  Init_RoadmapEdge_width_m length_m(::mrs_msgs::msg::RoadmapEdge::_length_m_type arg)
  {
    msg_.length_m = std::move(arg);
    return Init_RoadmapEdge_width_m(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_traversal
{
public:
  explicit Init_RoadmapEdge_traversal(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  Init_RoadmapEdge_length_m traversal(::mrs_msgs::msg::RoadmapEdge::_traversal_type arg)
  {
    msg_.traversal = std::move(arg);
    return Init_RoadmapEdge_length_m(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_to_id
{
public:
  explicit Init_RoadmapEdge_to_id(::mrs_msgs::msg::RoadmapEdge & msg)
  : msg_(msg)
  {}
  Init_RoadmapEdge_traversal to_id(::mrs_msgs::msg::RoadmapEdge::_to_id_type arg)
  {
    msg_.to_id = std::move(arg);
    return Init_RoadmapEdge_traversal(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

class Init_RoadmapEdge_from_id
{
public:
  Init_RoadmapEdge_from_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RoadmapEdge_to_id from_id(::mrs_msgs::msg::RoadmapEdge::_from_id_type arg)
  {
    msg_.from_id = std::move(arg);
    return Init_RoadmapEdge_to_id(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapEdge msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RoadmapEdge>()
{
  return mrs_msgs::msg::builder::Init_RoadmapEdge_from_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__BUILDER_HPP_
