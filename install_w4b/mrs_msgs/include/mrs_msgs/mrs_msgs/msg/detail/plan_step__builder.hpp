// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/PlanStep.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/plan_step.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__PLAN_STEP__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__PLAN_STEP__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/plan_step__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_PlanStep_location
{
public:
  explicit Init_PlanStep_location(::mrs_msgs::msg::PlanStep & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::PlanStep location(::mrs_msgs::msg::PlanStep::_location_type arg)
  {
    msg_.location = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::PlanStep msg_;
};

class Init_PlanStep_index
{
public:
  Init_PlanStep_index()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PlanStep_location index(::mrs_msgs::msg::PlanStep::_index_type arg)
  {
    msg_.index = std::move(arg);
    return Init_PlanStep_location(msg_);
  }

private:
  ::mrs_msgs::msg::PlanStep msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::PlanStep>()
{
  return mrs_msgs::msg::builder::Init_PlanStep_index();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__PLAN_STEP__BUILDER_HPP_
