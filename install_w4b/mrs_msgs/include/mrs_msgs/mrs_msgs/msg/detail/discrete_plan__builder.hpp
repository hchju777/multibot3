// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/discrete_plan.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/discrete_plan__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_DiscretePlan_visit_order
{
public:
  explicit Init_DiscretePlan_visit_order(::mrs_msgs::msg::DiscretePlan & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::DiscretePlan visit_order(::mrs_msgs::msg::DiscretePlan::_visit_order_type arg)
  {
    msg_.visit_order = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

class Init_DiscretePlan_plans
{
public:
  explicit Init_DiscretePlan_plans(::mrs_msgs::msg::DiscretePlan & msg)
  : msg_(msg)
  {}
  Init_DiscretePlan_visit_order plans(::mrs_msgs::msg::DiscretePlan::_plans_type arg)
  {
    msg_.plans = std::move(arg);
    return Init_DiscretePlan_visit_order(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

class Init_DiscretePlan_plan_revision
{
public:
  explicit Init_DiscretePlan_plan_revision(::mrs_msgs::msg::DiscretePlan & msg)
  : msg_(msg)
  {}
  Init_DiscretePlan_plans plan_revision(::mrs_msgs::msg::DiscretePlan::_plan_revision_type arg)
  {
    msg_.plan_revision = std::move(arg);
    return Init_DiscretePlan_plans(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

class Init_DiscretePlan_instance_id
{
public:
  explicit Init_DiscretePlan_instance_id(::mrs_msgs::msg::DiscretePlan & msg)
  : msg_(msg)
  {}
  Init_DiscretePlan_plan_revision instance_id(::mrs_msgs::msg::DiscretePlan::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_DiscretePlan_plan_revision(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

class Init_DiscretePlan_schema_version
{
public:
  explicit Init_DiscretePlan_schema_version(::mrs_msgs::msg::DiscretePlan & msg)
  : msg_(msg)
  {}
  Init_DiscretePlan_instance_id schema_version(::mrs_msgs::msg::DiscretePlan::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_DiscretePlan_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

class Init_DiscretePlan_schema
{
public:
  Init_DiscretePlan_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_DiscretePlan_schema_version schema(::mrs_msgs::msg::DiscretePlan::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_DiscretePlan_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::DiscretePlan msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::DiscretePlan>()
{
  return mrs_msgs::msg::builder::Init_DiscretePlan_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__BUILDER_HPP_
