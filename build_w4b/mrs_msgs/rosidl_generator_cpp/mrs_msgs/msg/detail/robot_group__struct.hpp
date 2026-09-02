// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RobotGroup.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_group.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RobotGroup __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RobotGroup __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotGroup_
{
  using Type = RobotGroup_<ContainerAllocator>;

  explicit RobotGroup_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit RobotGroup_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _robots_type =
    std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>>;
  _robots_type robots;

  // setters for named parameter idiom
  Type & set__robots(
    const std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>> & _arg)
  {
    this->robots = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RobotGroup_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RobotGroup_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotGroup_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RobotGroup_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RobotGroup
    std::shared_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RobotGroup
    std::shared_ptr<mrs_msgs::msg::RobotGroup_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotGroup_ & other) const
  {
    if (this->robots != other.robots) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotGroup_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotGroup_

// alias to use template instance with default allocator
using RobotGroup =
  mrs_msgs::msg::RobotGroup_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_HPP_
