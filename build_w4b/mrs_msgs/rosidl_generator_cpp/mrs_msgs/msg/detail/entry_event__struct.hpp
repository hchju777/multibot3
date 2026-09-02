// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/EntryEvent.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/entry_event.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__EntryEvent __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__EntryEvent __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct EntryEvent_
{
  using Type = EntryEvent_<ContainerAllocator>;

  explicit EntryEvent_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->segment = "";
      this->location = "";
      this->entered = false;
    }
  }

  explicit EntryEvent_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc),
    segment(_alloc),
    location(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->segment = "";
      this->location = "";
      this->entered = false;
    }
  }

  // field types and members
  using _seq_type =
    uint64_t;
  _seq_type seq;
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _segment_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _segment_type segment;
  using _location_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _location_type location;
  using _entered_type =
    bool;
  _entered_type entered;

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
  Type & set__segment(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->segment = _arg;
    return *this;
  }
  Type & set__location(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->location = _arg;
    return *this;
  }
  Type & set__entered(
    const bool & _arg)
  {
    this->entered = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::EntryEvent_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::EntryEvent_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::EntryEvent_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::EntryEvent_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__EntryEvent
    std::shared_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__EntryEvent
    std::shared_ptr<mrs_msgs::msg::EntryEvent_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const EntryEvent_ & other) const
  {
    if (this->seq != other.seq) {
      return false;
    }
    if (this->robot != other.robot) {
      return false;
    }
    if (this->segment != other.segment) {
      return false;
    }
    if (this->location != other.location) {
      return false;
    }
    if (this->entered != other.entered) {
      return false;
    }
    return true;
  }
  bool operator!=(const EntryEvent_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct EntryEvent_

// alias to use template instance with default allocator
using EntryEvent =
  mrs_msgs::msg::EntryEvent_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ENTRY_EVENT__STRUCT_HPP_
