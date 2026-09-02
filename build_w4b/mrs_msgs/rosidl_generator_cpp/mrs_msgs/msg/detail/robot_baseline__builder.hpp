// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotBaseline.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_baseline.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_baseline__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotBaseline_committed_segment_count
{
public:
  explicit Init_RobotBaseline_committed_segment_count(::mrs_msgs::msg::RobotBaseline & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotBaseline committed_segment_count(::mrs_msgs::msg::RobotBaseline::_committed_segment_count_type arg)
  {
    msg_.committed_segment_count = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotBaseline msg_;
};

class Init_RobotBaseline_robot
{
public:
  Init_RobotBaseline_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotBaseline_committed_segment_count robot(::mrs_msgs::msg::RobotBaseline::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_RobotBaseline_committed_segment_count(msg_);
  }

private:
  ::mrs_msgs::msg::RobotBaseline msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotBaseline>()
{
  return mrs_msgs::msg::builder::Init_RobotBaseline_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__BUILDER_HPP_
