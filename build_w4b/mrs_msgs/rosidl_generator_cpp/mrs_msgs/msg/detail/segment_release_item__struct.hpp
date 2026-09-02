// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/SegmentReleaseItem.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment_release_item.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__SegmentReleaseItem __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__SegmentReleaseItem __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct SegmentReleaseItem_
{
  using Type = SegmentReleaseItem_<ContainerAllocator>;

  explicit SegmentReleaseItem_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->released = false;
    }
  }

  explicit SegmentReleaseItem_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : id(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->released = false;
    }
  }

  // field types and members
  using _id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _id_type id;
  using _released_type =
    bool;
  _released_type released;

  // setters for named parameter idiom
  Type & set__id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__released(
    const bool & _arg)
  {
    this->released = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__SegmentReleaseItem
    std::shared_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__SegmentReleaseItem
    std::shared_ptr<mrs_msgs::msg::SegmentReleaseItem_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SegmentReleaseItem_ & other) const
  {
    if (this->id != other.id) {
      return false;
    }
    if (this->released != other.released) {
      return false;
    }
    return true;
  }
  bool operator!=(const SegmentReleaseItem_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SegmentReleaseItem_

// alias to use template instance with default allocator
using SegmentReleaseItem =
  mrs_msgs::msg::SegmentReleaseItem_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE_ITEM__STRUCT_HPP_
