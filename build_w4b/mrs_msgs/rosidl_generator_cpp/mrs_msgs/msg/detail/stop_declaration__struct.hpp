// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declaration.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'blocked_edge'
#include "mrs_msgs/msg/detail/edge_ref__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__StopDeclaration __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__StopDeclaration __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct StopDeclaration_
{
  using Type = StopDeclaration_<ContainerAllocator>;

  explicit StopDeclaration_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : blocked_edge(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->declared = false;
      this->reason = 0;
    }
  }

  explicit StopDeclaration_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc),
    blocked_edge(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->seq = 0ull;
      this->robot = "";
      this->declared = false;
      this->reason = 0;
    }
  }

  // field types and members
  using _seq_type =
    uint64_t;
  _seq_type seq;
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _blocked_edge_type =
    mrs_msgs::msg::EdgeRef_<ContainerAllocator>;
  _blocked_edge_type blocked_edge;
  using _declared_type =
    bool;
  _declared_type declared;
  using _reason_type =
    uint8_t;
  _reason_type reason;

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
  Type & set__blocked_edge(
    const mrs_msgs::msg::EdgeRef_<ContainerAllocator> & _arg)
  {
    this->blocked_edge = _arg;
    return *this;
  }
  Type & set__declared(
    const bool & _arg)
  {
    this->declared = _arg;
    return *this;
  }
  Type & set__reason(
    const uint8_t & _arg)
  {
    this->reason = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::StopDeclaration_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::StopDeclaration_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::StopDeclaration_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::StopDeclaration_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__StopDeclaration
    std::shared_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__StopDeclaration
    std::shared_ptr<mrs_msgs::msg::StopDeclaration_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const StopDeclaration_ & other) const
  {
    if (this->seq != other.seq) {
      return false;
    }
    if (this->robot != other.robot) {
      return false;
    }
    if (this->blocked_edge != other.blocked_edge) {
      return false;
    }
    if (this->declared != other.declared) {
      return false;
    }
    if (this->reason != other.reason) {
      return false;
    }
    return true;
  }
  bool operator!=(const StopDeclaration_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct StopDeclaration_

// alias to use template instance with default allocator
using StopDeclaration =
  mrs_msgs::msg::StopDeclaration_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_HPP_
