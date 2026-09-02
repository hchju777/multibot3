// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_provenance.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__RoadmapProvenance __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__RoadmapProvenance __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RoadmapProvenance_
{
  using Type = RoadmapProvenance_<ContainerAllocator>;

  explicit RoadmapProvenance_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->min_separation_m = 0.0;
      this->wall_inflation_m = 0.0;
      this->capacity_rule_id = "";
      this->generator_version = "";
    }
  }

  explicit RoadmapProvenance_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : capacity_rule_id(_alloc),
    generator_version(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->min_separation_m = 0.0;
      this->wall_inflation_m = 0.0;
      this->capacity_rule_id = "";
      this->generator_version = "";
    }
  }

  // field types and members
  using _min_separation_m_type =
    double;
  _min_separation_m_type min_separation_m;
  using _wall_inflation_m_type =
    double;
  _wall_inflation_m_type wall_inflation_m;
  using _capacity_rule_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _capacity_rule_id_type capacity_rule_id;
  using _generator_version_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _generator_version_type generator_version;

  // setters for named parameter idiom
  Type & set__min_separation_m(
    const double & _arg)
  {
    this->min_separation_m = _arg;
    return *this;
  }
  Type & set__wall_inflation_m(
    const double & _arg)
  {
    this->wall_inflation_m = _arg;
    return *this;
  }
  Type & set__capacity_rule_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->capacity_rule_id = _arg;
    return *this;
  }
  Type & set__generator_version(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->generator_version = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__RoadmapProvenance
    std::shared_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__RoadmapProvenance
    std::shared_ptr<mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RoadmapProvenance_ & other) const
  {
    if (this->min_separation_m != other.min_separation_m) {
      return false;
    }
    if (this->wall_inflation_m != other.wall_inflation_m) {
      return false;
    }
    if (this->capacity_rule_id != other.capacity_rule_id) {
      return false;
    }
    if (this->generator_version != other.generator_version) {
      return false;
    }
    return true;
  }
  bool operator!=(const RoadmapProvenance_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RoadmapProvenance_

// alias to use template instance with default allocator
using RoadmapProvenance =
  mrs_msgs::msg::RoadmapProvenance_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_HPP_
