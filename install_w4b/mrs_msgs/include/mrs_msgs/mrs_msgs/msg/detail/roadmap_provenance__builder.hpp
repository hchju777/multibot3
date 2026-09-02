// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_provenance.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/roadmap_provenance__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RoadmapProvenance_generator_version
{
public:
  explicit Init_RoadmapProvenance_generator_version(::mrs_msgs::msg::RoadmapProvenance & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RoadmapProvenance generator_version(::mrs_msgs::msg::RoadmapProvenance::_generator_version_type arg)
  {
    msg_.generator_version = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapProvenance msg_;
};

class Init_RoadmapProvenance_capacity_rule_id
{
public:
  explicit Init_RoadmapProvenance_capacity_rule_id(::mrs_msgs::msg::RoadmapProvenance & msg)
  : msg_(msg)
  {}
  Init_RoadmapProvenance_generator_version capacity_rule_id(::mrs_msgs::msg::RoadmapProvenance::_capacity_rule_id_type arg)
  {
    msg_.capacity_rule_id = std::move(arg);
    return Init_RoadmapProvenance_generator_version(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapProvenance msg_;
};

class Init_RoadmapProvenance_wall_inflation_m
{
public:
  explicit Init_RoadmapProvenance_wall_inflation_m(::mrs_msgs::msg::RoadmapProvenance & msg)
  : msg_(msg)
  {}
  Init_RoadmapProvenance_capacity_rule_id wall_inflation_m(::mrs_msgs::msg::RoadmapProvenance::_wall_inflation_m_type arg)
  {
    msg_.wall_inflation_m = std::move(arg);
    return Init_RoadmapProvenance_capacity_rule_id(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapProvenance msg_;
};

class Init_RoadmapProvenance_min_separation_m
{
public:
  Init_RoadmapProvenance_min_separation_m()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RoadmapProvenance_wall_inflation_m min_separation_m(::mrs_msgs::msg::RoadmapProvenance::_min_separation_m_type arg)
  {
    msg_.min_separation_m = std::move(arg);
    return Init_RoadmapProvenance_wall_inflation_m(msg_);
  }

private:
  ::mrs_msgs::msg::RoadmapProvenance msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RoadmapProvenance>()
{
  return mrs_msgs::msg::builder::Init_RoadmapProvenance_min_separation_m();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__BUILDER_HPP_
