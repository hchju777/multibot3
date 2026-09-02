// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/StopReason.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_reason.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__StopReason __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__StopReason __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct StopReason_
{
  using Type = StopReason_<ContainerAllocator>;

  explicit StopReason_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit StopReason_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  // field types and members
  using _structure_needs_at_least_one_member_type =
    uint8_t;
  _structure_needs_at_least_one_member_type structure_needs_at_least_one_member;


  // constant declarations
  static constexpr uint8_t REASON_UNKNOWN =
    0u;
  static constexpr uint8_t REASON_EXOGENOUS_BLOCK =
    1u;
  static constexpr uint8_t REASON_CTRL_STOP_DECLARED =
    2u;
  static constexpr uint8_t REASON_INFEASIBLE_SUBGOAL =
    3u;
  static constexpr uint8_t REASON_ORIENTATION_FLIP =
    4u;
  static constexpr uint8_t REASON_UNRESOLVABLE_LOCALLY =
    5u;
  static constexpr uint8_t REASON_HORIZON_EXHAUSTING =
    6u;

  // pointer types
  using RawPtr =
    mrs_msgs::msg::StopReason_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::StopReason_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::StopReason_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::StopReason_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__StopReason
    std::shared_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__StopReason
    std::shared_ptr<mrs_msgs::msg::StopReason_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const StopReason_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const StopReason_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct StopReason_

// alias to use template instance with default allocator
using StopReason =
  mrs_msgs::msg::StopReason_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_UNKNOWN;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_EXOGENOUS_BLOCK;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_CTRL_STOP_DECLARED;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_INFEASIBLE_SUBGOAL;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_ORIENTATION_FLIP;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_UNRESOLVABLE_LOCALLY;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t StopReason_<ContainerAllocator>::REASON_HORIZON_EXHAUSTING;
#endif  // __cplusplus < 201703L

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_HPP_
