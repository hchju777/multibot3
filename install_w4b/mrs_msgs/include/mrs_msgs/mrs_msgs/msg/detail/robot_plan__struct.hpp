// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RobotPlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_plan.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'steps'
#include "mrs_msgs/msg/detail/plan_step__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RobotPlan __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RobotPlan __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotPlan_
{
  using Type = RobotPlan_<ContainerAllocator>;

  explicit RobotPlan_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->terminal = 0;
    }
  }

  explicit RobotPlan_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->terminal = 0;
    }
  }

  // field types and members
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _terminal_type =
    uint8_t;
  _terminal_type terminal;
  using _steps_type =
    std::vector<mrs_msgs::msg::PlanStep_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::PlanStep_<ContainerAllocator>>>;
  _steps_type steps;

  // setters for named parameter idiom
  Type & set__robot(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->robot = _arg;
    return *this;
  }
  Type & set__terminal(
    const uint8_t & _arg)
  {
    this->terminal = _arg;
    return *this;
  }
  Type & set__steps(
    const std::vector<mrs_msgs::msg::PlanStep_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::PlanStep_<ContainerAllocator>>> & _arg)
  {
    this->steps = _arg;
    return *this;
  }

  // constant declarations
  static constexpr uint8_t TERMINAL_UNKNOWN =
    0u;
  static constexpr uint8_t TERMINAL_GOAL_REACHED =
    1u;
  static constexpr uint8_t TERMINAL_PLAN_TRUNCATED =
    2u;
  static constexpr uint8_t TERMINAL_NO_PROGRESS =
    3u;

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RobotPlan_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RobotPlan_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotPlan_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotPlan_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RobotPlan
    std::shared_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RobotPlan
    std::shared_ptr<mrs_msgs::msg::RobotPlan_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotPlan_ & other) const
  {
    if (this->robot != other.robot) {
      return false;
    }
    if (this->terminal != other.terminal) {
      return false;
    }
    if (this->steps != other.steps) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotPlan_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotPlan_

// alias to use template instance with default allocator
using RobotPlan =
  mrs_msgs::msg::RobotPlan_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RobotPlan_<ContainerAllocator>::TERMINAL_UNKNOWN;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RobotPlan_<ContainerAllocator>::TERMINAL_GOAL_REACHED;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RobotPlan_<ContainerAllocator>::TERMINAL_PLAN_TRUNCATED;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RobotPlan_<ContainerAllocator>::TERMINAL_NO_PROGRESS;
#endif  // __cplusplus < 201703L

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_HPP_
