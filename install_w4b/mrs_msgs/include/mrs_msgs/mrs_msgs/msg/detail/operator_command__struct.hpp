// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/OperatorCommand.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_command.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__OperatorCommand __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__OperatorCommand __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct OperatorCommand_
{
  using Type = OperatorCommand_<ContainerAllocator>;

  explicit OperatorCommand_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->verb = 0;
      this->active = false;
      this->speed_cap_mps = 0.0;
      this->has_speed_cap_mps = false;
      this->goal_location = "";
    }
  }

  explicit OperatorCommand_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc),
    goal_location(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->verb = 0;
      this->active = false;
      this->speed_cap_mps = 0.0;
      this->has_speed_cap_mps = false;
      this->goal_location = "";
    }
  }

  // field types and members
  using _seq_type =
    uint64_t;
  _seq_type seq;
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _verb_type =
    uint8_t;
  _verb_type verb;
  using _active_type =
    bool;
  _active_type active;
  using _speed_cap_mps_type =
    double;
  _speed_cap_mps_type speed_cap_mps;
  using _has_speed_cap_mps_type =
    bool;
  _has_speed_cap_mps_type has_speed_cap_mps;
  using _goal_location_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _goal_location_type goal_location;

  // setters for named parameter idiom
  Type & set__seq(
    const uint64_t & _arg)
  {
    this->seq = _arg;
    return *this;
  }
  Type & set__robot(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->robot = _arg;
    return *this;
  }
  Type & set__verb(
    const uint8_t & _arg)
  {
    this->verb = _arg;
    return *this;
  }
  Type & set__active(
    const bool & _arg)
  {
    this->active = _arg;
    return *this;
  }
  Type & set__speed_cap_mps(
    const double & _arg)
  {
    this->speed_cap_mps = _arg;
    return *this;
  }
  Type & set__has_speed_cap_mps(
    const bool & _arg)
  {
    this->has_speed_cap_mps = _arg;
    return *this;
  }
  Type & set__goal_location(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->goal_location = _arg;
    return *this;
  }

  // constant declarations
  static constexpr uint8_t VERB_UNKNOWN =
    0u;
  static constexpr uint8_t VERB_HOLD =
    1u;
  static constexpr uint8_t VERB_SPEED_CAP =
    2u;
  static constexpr uint8_t VERB_GOAL_OVERRIDE =
    3u;

  // pointer types
  using RawPtr =
    mrs_msgs::msg::OperatorCommand_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::OperatorCommand_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::OperatorCommand_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::OperatorCommand_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__OperatorCommand
    std::shared_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__OperatorCommand
    std::shared_ptr<mrs_msgs::msg::OperatorCommand_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const OperatorCommand_ & other) const
  {
    if (this->seq != other.seq) {
      return false;
    }
    if (this->robot != other.robot) {
      return false;
    }
    if (this->verb != other.verb) {
      return false;
    }
    if (this->active != other.active) {
      return false;
    }
    if (this->speed_cap_mps != other.speed_cap_mps) {
      return false;
    }
    if (this->has_speed_cap_mps != other.has_speed_cap_mps) {
      return false;
    }
    if (this->goal_location != other.goal_location) {
      return false;
    }
    return true;
  }
  bool operator!=(const OperatorCommand_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct OperatorCommand_

// alias to use template instance with default allocator
using OperatorCommand =
  mrs_msgs::msg::OperatorCommand_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t OperatorCommand_<ContainerAllocator>::VERB_UNKNOWN;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t OperatorCommand_<ContainerAllocator>::VERB_HOLD;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t OperatorCommand_<ContainerAllocator>::VERB_SPEED_CAP;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t OperatorCommand_<ContainerAllocator>::VERB_GOAL_OVERRIDE;
#endif  // __cplusplus < 201703L

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_HPP_
