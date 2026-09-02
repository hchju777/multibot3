// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_spec.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_spec__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotSpec_circumradius_m
{
public:
  explicit Init_RobotSpec_circumradius_m(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotSpec circumradius_m(::mrs_msgs::msg::RobotSpec::_circumradius_m_type arg)
  {
    msg_.circumradius_m = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_reverse_motion_allowed
{
public:
  explicit Init_RobotSpec_reverse_motion_allowed(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  Init_RobotSpec_circumradius_m reverse_motion_allowed(::mrs_msgs::msg::RobotSpec::_reverse_motion_allowed_type arg)
  {
    msg_.reverse_motion_allowed = std::move(arg);
    return Init_RobotSpec_circumradius_m(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_curvature_max_invm
{
public:
  explicit Init_RobotSpec_curvature_max_invm(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  Init_RobotSpec_reverse_motion_allowed curvature_max_invm(::mrs_msgs::msg::RobotSpec::_curvature_max_invm_type arg)
  {
    msg_.curvature_max_invm = std::move(arg);
    return Init_RobotSpec_reverse_motion_allowed(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_brake_decel_min_mps2
{
public:
  explicit Init_RobotSpec_brake_decel_min_mps2(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  Init_RobotSpec_curvature_max_invm brake_decel_min_mps2(::mrs_msgs::msg::RobotSpec::_brake_decel_min_mps2_type arg)
  {
    msg_.brake_decel_min_mps2 = std::move(arg);
    return Init_RobotSpec_curvature_max_invm(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_v_max
{
public:
  explicit Init_RobotSpec_v_max(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  Init_RobotSpec_brake_decel_min_mps2 v_max(::mrs_msgs::msg::RobotSpec::_v_max_type arg)
  {
    msg_.v_max = std::move(arg);
    return Init_RobotSpec_brake_decel_min_mps2(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_avg_traversal_speed_mps
{
public:
  explicit Init_RobotSpec_avg_traversal_speed_mps(::mrs_msgs::msg::RobotSpec & msg)
  : msg_(msg)
  {}
  Init_RobotSpec_v_max avg_traversal_speed_mps(::mrs_msgs::msg::RobotSpec::_avg_traversal_speed_mps_type arg)
  {
    msg_.avg_traversal_speed_mps = std::move(arg);
    return Init_RobotSpec_v_max(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

class Init_RobotSpec_robot
{
public:
  Init_RobotSpec_robot()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotSpec_avg_traversal_speed_mps robot(::mrs_msgs::msg::RobotSpec::_robot_type arg)
  {
    msg_.robot = std::move(arg);
    return Init_RobotSpec_avg_traversal_speed_mps(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpec msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotSpec>()
{
  return mrs_msgs::msg::builder::Init_RobotSpec_robot();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__BUILDER_HPP_
