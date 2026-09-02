// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/limits.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__LIMITS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__LIMITS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/limits__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Limits_yaw_accel_max_radps2
{
public:
  explicit Init_Limits_yaw_accel_max_radps2(::mrs_msgs::msg::Limits & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Limits yaw_accel_max_radps2(::mrs_msgs::msg::Limits::_yaw_accel_max_radps2_type arg)
  {
    msg_.yaw_accel_max_radps2 = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Limits msg_;
};

class Init_Limits_omega_max_radps
{
public:
  explicit Init_Limits_omega_max_radps(::mrs_msgs::msg::Limits & msg)
  : msg_(msg)
  {}
  Init_Limits_yaw_accel_max_radps2 omega_max_radps(::mrs_msgs::msg::Limits::_omega_max_radps_type arg)
  {
    msg_.omega_max_radps = std::move(arg);
    return Init_Limits_yaw_accel_max_radps2(msg_);
  }

private:
  ::mrs_msgs::msg::Limits msg_;
};

class Init_Limits_min_separation_m
{
public:
  explicit Init_Limits_min_separation_m(::mrs_msgs::msg::Limits & msg)
  : msg_(msg)
  {}
  Init_Limits_omega_max_radps min_separation_m(::mrs_msgs::msg::Limits::_min_separation_m_type arg)
  {
    msg_.min_separation_m = std::move(arg);
    return Init_Limits_omega_max_radps(msg_);
  }

private:
  ::mrs_msgs::msg::Limits msg_;
};

class Init_Limits_j_max
{
public:
  explicit Init_Limits_j_max(::mrs_msgs::msg::Limits & msg)
  : msg_(msg)
  {}
  Init_Limits_min_separation_m j_max(::mrs_msgs::msg::Limits::_j_max_type arg)
  {
    msg_.j_max = std::move(arg);
    return Init_Limits_min_separation_m(msg_);
  }

private:
  ::mrs_msgs::msg::Limits msg_;
};

class Init_Limits_a_max
{
public:
  Init_Limits_a_max()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Limits_j_max a_max(::mrs_msgs::msg::Limits::_a_max_type arg)
  {
    msg_.a_max = std::move(arg);
    return Init_Limits_j_max(msg_);
  }

private:
  ::mrs_msgs::msg::Limits msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Limits>()
{
  return mrs_msgs::msg::builder::Init_Limits_a_max();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__LIMITS__BUILDER_HPP_
