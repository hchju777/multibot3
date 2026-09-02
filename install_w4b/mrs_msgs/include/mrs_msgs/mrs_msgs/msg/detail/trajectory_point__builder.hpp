// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/TrajectoryPoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/trajectory_point.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/trajectory_point__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_TrajectoryPoint_has_a
{
public:
  explicit Init_TrajectoryPoint_has_a(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::TrajectoryPoint has_a(::mrs_msgs::msg::TrajectoryPoint::_has_a_type arg)
  {
    msg_.has_a = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_a
{
public:
  explicit Init_TrajectoryPoint_a(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_has_a a(::mrs_msgs::msg::TrajectoryPoint::_a_type arg)
  {
    msg_.a = std::move(arg);
    return Init_TrajectoryPoint_has_a(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_has_v
{
public:
  explicit Init_TrajectoryPoint_has_v(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_a has_v(::mrs_msgs::msg::TrajectoryPoint::_has_v_type arg)
  {
    msg_.has_v = std::move(arg);
    return Init_TrajectoryPoint_a(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_v
{
public:
  explicit Init_TrajectoryPoint_v(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_has_v v(::mrs_msgs::msg::TrajectoryPoint::_v_type arg)
  {
    msg_.v = std::move(arg);
    return Init_TrajectoryPoint_has_v(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_yaw
{
public:
  explicit Init_TrajectoryPoint_yaw(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_v yaw(::mrs_msgs::msg::TrajectoryPoint::_yaw_type arg)
  {
    msg_.yaw = std::move(arg);
    return Init_TrajectoryPoint_v(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_y
{
public:
  explicit Init_TrajectoryPoint_y(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_yaw y(::mrs_msgs::msg::TrajectoryPoint::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_TrajectoryPoint_yaw(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_x
{
public:
  explicit Init_TrajectoryPoint_x(::mrs_msgs::msg::TrajectoryPoint & msg)
  : msg_(msg)
  {}
  Init_TrajectoryPoint_y x(::mrs_msgs::msg::TrajectoryPoint::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_TrajectoryPoint_y(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

class Init_TrajectoryPoint_t
{
public:
  Init_TrajectoryPoint_t()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_TrajectoryPoint_x t(::mrs_msgs::msg::TrajectoryPoint::_t_type arg)
  {
    msg_.t = std::move(arg);
    return Init_TrajectoryPoint_x(msg_);
  }

private:
  ::mrs_msgs::msg::TrajectoryPoint msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::TrajectoryPoint>()
{
  return mrs_msgs::msg::builder::Init_TrajectoryPoint_t();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__BUILDER_HPP_
