// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/VisitItem.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/visit_item.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__VisitItem __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__VisitItem __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct VisitItem_
{
  using Type = VisitItem_<ContainerAllocator>;

  explicit VisitItem_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->index = 0ul;
    }
  }

  explicit VisitItem_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : robot(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->robot = "";
      this->index = 0ul;
    }
  }

  // field types and members
  using _robot_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _robot_type robot;
  using _index_type =
    uint32_t;
  _index_type index;

  // setters for named parameter idiom
  Type & set__robot(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->robot = _arg;
    return *this;
  }
  Type & set__index(
    const uint32_t & _arg)
  {
    this->index = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::VisitItem_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::VisitItem_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::VisitItem_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::VisitItem_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__VisitItem
    std::shared_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__VisitItem
    std::shared_ptr<mrs_msgs::msg::VisitItem_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const VisitItem_ & other) const
  {
    if (this->robot != other.robot) {
      return false;
    }
    if (this->index != other.index) {
      return false;
    }
    return true;
  }
  bool operator!=(const VisitItem_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct VisitItem_

// alias to use template instance with default allocator
using VisitItem =
  mrs_msgs::msg::VisitItem_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_HPP_
