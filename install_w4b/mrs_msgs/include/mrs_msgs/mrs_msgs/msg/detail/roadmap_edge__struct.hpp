// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_edge.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RoadmapEdge __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RoadmapEdge __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RoadmapEdge_
{
  using Type = RoadmapEdge_<ContainerAllocator>;

  explicit RoadmapEdge_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->from_id = "";
      this->to_id = "";
      this->traversal = 0;
      this->length_m = 0.0;
      this->width_m = 0.0;
      this->capacity_robots = 0;
      this->corridor = "";
    }
  }

  explicit RoadmapEdge_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : from_id(_alloc),
    to_id(_alloc),
    corridor(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->from_id = "";
      this->to_id = "";
      this->traversal = 0;
      this->length_m = 0.0;
      this->width_m = 0.0;
      this->capacity_robots = 0;
      this->corridor = "";
    }
  }

  // field types and members
  using _from_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _from_id_type from_id;
  using _to_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _to_id_type to_id;
  using _traversal_type =
    uint8_t;
  _traversal_type traversal;
  using _length_m_type =
    double;
  _length_m_type length_m;
  using _width_m_type =
    double;
  _width_m_type width_m;
  using _capacity_robots_type =
    uint8_t;
  _capacity_robots_type capacity_robots;
  using _corridor_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _corridor_type corridor;

  // setters for named parameter idiom
  Type & set__from_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->from_id = _arg;
    return *this;
  }
  Type & set__to_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->to_id = _arg;
    return *this;
  }
  Type & set__traversal(
    const uint8_t & _arg)
  {
    this->traversal = _arg;
    return *this;
  }
  Type & set__length_m(
    const double & _arg)
  {
    this->length_m = _arg;
    return *this;
  }
  Type & set__width_m(
    const double & _arg)
  {
    this->width_m = _arg;
    return *this;
  }
  Type & set__capacity_robots(
    const uint8_t & _arg)
  {
    this->capacity_robots = _arg;
    return *this;
  }
  Type & set__corridor(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->corridor = _arg;
    return *this;
  }

  // constant declarations
  static constexpr uint8_t TRAVERSAL_UNKNOWN =
    0u;
  static constexpr uint8_t TRAVERSAL_BIDIRECTIONAL =
    1u;
  static constexpr uint8_t TRAVERSAL_FORWARD_ONLY =
    2u;
  static constexpr uint8_t TRAVERSAL_BACKWARD_ONLY =
    3u;

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RoadmapEdge
    std::shared_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RoadmapEdge
    std::shared_ptr<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RoadmapEdge_ & other) const
  {
    if (this->from_id != other.from_id) {
      return false;
    }
    if (this->to_id != other.to_id) {
      return false;
    }
    if (this->traversal != other.traversal) {
      return false;
    }
    if (this->length_m != other.length_m) {
      return false;
    }
    if (this->width_m != other.width_m) {
      return false;
    }
    if (this->capacity_robots != other.capacity_robots) {
      return false;
    }
    if (this->corridor != other.corridor) {
      return false;
    }
    return true;
  }
  bool operator!=(const RoadmapEdge_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RoadmapEdge_

// alias to use template instance with default allocator
using RoadmapEdge =
  mrs_msgs::msg::RoadmapEdge_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RoadmapEdge_<ContainerAllocator>::TRAVERSAL_UNKNOWN;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RoadmapEdge_<ContainerAllocator>::TRAVERSAL_BIDIRECTIONAL;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RoadmapEdge_<ContainerAllocator>::TRAVERSAL_FORWARD_ONLY;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t RoadmapEdge_<ContainerAllocator>::TRAVERSAL_BACKWARD_ONLY;
#endif  // __cplusplus < 201703L

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_HPP_
