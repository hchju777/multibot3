// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RealizabilityEntry.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability_entry.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RealizabilityEntry __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RealizabilityEntry __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RealizabilityEntry_
{
  using Type = RealizabilityEntry_<ContainerAllocator>;

  explicit RealizabilityEntry_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->group = "";
      this->alternative = "";
      this->realizable = false;
    }
  }

  explicit RealizabilityEntry_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : group(_alloc),
    alternative(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->group = "";
      this->alternative = "";
      this->realizable = false;
    }
  }

  // field types and members
  using _group_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _group_type group;
  using _alternative_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _alternative_type alternative;
  using _realizable_type =
    bool;
  _realizable_type realizable;

  // setters for named parameter idiom
  Type & set__group(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->group = _arg;
    return *this;
  }
  Type & set__alternative(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->alternative = _arg;
    return *this;
  }
  Type & set__realizable(
    const bool & _arg)
  {
    this->realizable = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RealizabilityEntry
    std::shared_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RealizabilityEntry
    std::shared_ptr<mrs_msgs::msg::RealizabilityEntry_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RealizabilityEntry_ & other) const
  {
    if (this->group != other.group) {
      return false;
    }
    if (this->alternative != other.alternative) {
      return false;
    }
    if (this->realizable != other.realizable) {
      return false;
    }
    return true;
  }
  bool operator!=(const RealizabilityEntry_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RealizabilityEntry_

// alias to use template instance with default allocator
using RealizabilityEntry =
  mrs_msgs::msg::RealizabilityEntry_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_HPP_
