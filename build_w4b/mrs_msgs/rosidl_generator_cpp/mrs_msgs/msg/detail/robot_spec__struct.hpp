// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_spec.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RobotSpec __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RobotSpec __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotSpec_
{
  using Type = RobotSpec_<ContainerAllocator>;

  explicit RobotSpec_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->avg_traversal_speed_mps = 0.0;
      this->v_max = 0.0;
      this->brake_decel_min_mps2 = 0.0;
      this->curvature_max_invm = 0.0;
      this->reverse_motion_allowed = false;
      this->circumradius_m = 0.0;
    }
  }

  explicit RobotSpec_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->avg_traversal_speed_mps = 0.0;
      this->v_max = 0.0;
      this->brake_decel_min_mps2 = 0.0;
      this->curvature_max_invm = 0.0;
      this->reverse_motion_allowed = false;
      this->circumradius_m = 0.0;
    }
  }

  // field types and members
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _avg_traversal_speed_mps_type =
    double;
  _avg_traversal_speed_mps_type avg_traversal_speed_mps;
  using _v_max_type =
    double;
  _v_max_type v_max;
  using _brake_decel_min_mps2_type =
    double;
  _brake_decel_min_mps2_type brake_decel_min_mps2;
  using _curvature_max_invm_type =
    double;
  _curvature_max_invm_type curvature_max_invm;
  using _reverse_motion_allowed_type =
    bool;
  _reverse_motion_allowed_type reverse_motion_allowed;
  using _circumradius_m_type =
    double;
  _circumradius_m_type circumradius_m;

  // setters for named parameter idiom
  Type & set__robot(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->robot = _arg;
    return *this;
  }
  Type & set__avg_traversal_speed_mps(
    const double & _arg)
  {
    this->avg_traversal_speed_mps = _arg;
    return *this;
  }
  Type & set__v_max(
    const double & _arg)
  {
    this->v_max = _arg;
    return *this;
  }
  Type & set__brake_decel_min_mps2(
    const double & _arg)
  {
    this->brake_decel_min_mps2 = _arg;
    return *this;
  }
  Type & set__curvature_max_invm(
    const double & _arg)
  {
    this->curvature_max_invm = _arg;
    return *this;
  }
  Type & set__reverse_motion_allowed(
    const bool & _arg)
  {
    this->reverse_motion_allowed = _arg;
    return *this;
  }
  Type & set__circumradius_m(
    const double & _arg)
  {
    this->circumradius_m = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RobotSpec_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RobotSpec_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotSpec_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotSpec_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RobotSpec
    std::shared_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RobotSpec
    std::shared_ptr<mrs_msgs::msg::RobotSpec_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotSpec_ & other) const
  {
    if (this->robot != other.robot) {
      return false;
    }
    if (this->avg_traversal_speed_mps != other.avg_traversal_speed_mps) {
      return false;
    }
    if (this->v_max != other.v_max) {
      return false;
    }
    if (this->brake_decel_min_mps2 != other.brake_decel_min_mps2) {
      return false;
    }
    if (this->curvature_max_invm != other.curvature_max_invm) {
      return false;
    }
    if (this->reverse_motion_allowed != other.reverse_motion_allowed) {
      return false;
    }
    if (this->circumradius_m != other.circumradius_m) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotSpec_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotSpec_

// alias to use template instance with default allocator
using RobotSpec =
  mrs_msgs::msg::RobotSpec_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_HPP_
