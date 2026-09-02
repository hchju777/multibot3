// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/limits.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__Limits __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__Limits __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Limits_
{
  using Type = Limits_<ContainerAllocator>;

  explicit Limits_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->a_max = 0.0;
      this->j_max = 0.0;
      this->min_separation_m = 0.0;
      this->omega_max_radps = 0.0;
      this->yaw_accel_max_radps2 = 0.0;
    }
  }

  explicit Limits_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->a_max = 0.0;
      this->j_max = 0.0;
      this->min_separation_m = 0.0;
      this->omega_max_radps = 0.0;
      this->yaw_accel_max_radps2 = 0.0;
    }
  }

  // field types and members
  using _a_max_type =
    double;
  _a_max_type a_max;
  using _j_max_type =
    double;
  _j_max_type j_max;
  using _min_separation_m_type =
    double;
  _min_separation_m_type min_separation_m;
  using _omega_max_radps_type =
    double;
  _omega_max_radps_type omega_max_radps;
  using _yaw_accel_max_radps2_type =
    double;
  _yaw_accel_max_radps2_type yaw_accel_max_radps2;

  // setters for named parameter idiom
  Type & set__a_max(
    const double & _arg)
  {
    this->a_max = _arg;
    return *this;
  }
  Type & set__j_max(
    const double & _arg)
  {
    this->j_max = _arg;
    return *this;
  }
  Type & set__min_separation_m(
    const double & _arg)
  {
    this->min_separation_m = _arg;
    return *this;
  }
  Type & set__omega_max_radps(
    const double & _arg)
  {
    this->omega_max_radps = _arg;
    return *this;
  }
  Type & set__yaw_accel_max_radps2(
    const double & _arg)
  {
    this->yaw_accel_max_radps2 = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::Limits_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::Limits_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::Limits_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::Limits_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Limits_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Limits_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Limits_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Limits_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::Limits_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::Limits_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__Limits
    std::shared_ptr<mrs_msgs::msg::Limits_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__Limits
    std::shared_ptr<mrs_msgs::msg::Limits_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Limits_ & other) const
  {
    if (this->a_max != other.a_max) {
      return false;
    }
    if (this->j_max != other.j_max) {
      return false;
    }
    if (this->min_separation_m != other.min_separation_m) {
      return false;
    }
    if (this->omega_max_radps != other.omega_max_radps) {
      return false;
    }
    if (this->yaw_accel_max_radps2 != other.yaw_accel_max_radps2) {
      return false;
    }
    return true;
  }
  bool operator!=(const Limits_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Limits_

// alias to use template instance with default allocator
using Limits =
  mrs_msgs::msg::Limits_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_HPP_
