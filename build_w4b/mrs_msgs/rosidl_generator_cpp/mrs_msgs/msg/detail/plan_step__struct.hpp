// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/PlanStep.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/plan_step.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__PlanStep __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__PlanStep __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct PlanStep_
{
  using Type = PlanStep_<ContainerAllocator>;

  explicit PlanStep_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->index = 0ul;
      this->location = "";
    }
  }

  explicit PlanStep_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : location(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->index = 0ul;
      this->location = "";
    }
  }

  // field types and members
  using _index_type =
    uint32_t;
  _index_type index;
  using _location_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _location_type location;

  // setters for named parameter idiom
  Type & set__index(
    const uint32_t & _arg)
  {
    this->index = _arg;
    return *this;
  }
  Type & set__location(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->location = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::PlanStep_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::PlanStep_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::PlanStep_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::PlanStep_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__PlanStep
    std::shared_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__PlanStep
    std::shared_ptr<mrs_msgs::msg::PlanStep_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const PlanStep_ & other) const
  {
    if (this->index != other.index) {
      return false;
    }
    if (this->location != other.location) {
      return false;
    }
    return true;
  }
  bool operator!=(const PlanStep_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct PlanStep_

// alias to use template instance with default allocator
using PlanStep =
  mrs_msgs::msg::PlanStep_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_HPP_
