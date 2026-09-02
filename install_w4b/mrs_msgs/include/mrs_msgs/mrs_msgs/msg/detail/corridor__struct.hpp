// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/corridor.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'edges'
#include "mrs_msgs/msg/detail/edge_ref__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__Corridor __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__Corridor __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Corridor_
{
  using Type = Corridor_<ContainerAllocator>;

  explicit Corridor_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->length_m = 0.0;
    }
  }

  explicit Corridor_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : id(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->id = "";
      this->length_m = 0.0;
    }
  }

  // field types and members
  using _id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _id_type id;
  using _edges_type =
    std::vector<mrs_msgs::msg::EdgeRef_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EdgeRef_<ContainerAllocator>>>;
  _edges_type edges;
  using _length_m_type =
    double;
  _length_m_type length_m;

  // setters for named parameter idiom
  Type & set__id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__edges(
    const std::vector<mrs_msgs::msg::EdgeRef_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EdgeRef_<ContainerAllocator>>> & _arg)
  {
    this->edges = _arg;
    return *this;
  }
  Type & set__length_m(
    const double & _arg)
  {
    this->length_m = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::Corridor_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::Corridor_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Corridor_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Corridor_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__Corridor
    std::shared_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__Corridor
    std::shared_ptr<mrs_msgs::msg::Corridor_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Corridor_ & other) const
  {
    if (this->id != other.id) {
      return false;
    }
    if (this->edges != other.edges) {
      return false;
    }
    if (this->length_m != other.length_m) {
      return false;
    }
    return true;
  }
  bool operator!=(const Corridor_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Corridor_

// alias to use template instance with default allocator
using Corridor =
  mrs_msgs::msg::Corridor_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_HPP_
