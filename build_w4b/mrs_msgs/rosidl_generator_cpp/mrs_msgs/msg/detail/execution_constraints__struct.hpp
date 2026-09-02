// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/ExecutionConstraints.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/execution_constraints.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'segments'
#include "mrs_msgs/msg/detail/segment__struct.hpp"
// Member 'dependencies'
#include "mrs_msgs/msg/detail/dependency__struct.hpp"
// Member 'switch_groups'
#include "mrs_msgs/msg/detail/switch_group__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__ExecutionConstraints __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__ExecutionConstraints __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ExecutionConstraints_
{
  using Type = ExecutionConstraints_<ContainerAllocator>;

  explicit ExecutionConstraints_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->commit_seq = 0ull;
    }
  }

  explicit ExecutionConstraints_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : schema(_alloc),
    schema_version(_alloc),
    instance_id(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->commit_seq = 0ull;
    }
  }

  // field types and members
  using _schema_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_type schema;
  using _schema_version_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_version_type schema_version;
  using _instance_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _instance_id_type instance_id;
  using _commit_seq_type =
    uint64_t;
  _commit_seq_type commit_seq;
  using _segments_type =
    std::vector<mrs_msgs::msg::Segment_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Segment_<ContainerAllocator>>>;
  _segments_type segments;
  using _dependencies_type =
    std::vector<mrs_msgs::msg::Dependency_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Dependency_<ContainerAllocator>>>;
  _dependencies_type dependencies;
  using _switch_groups_type =
    std::vector<mrs_msgs::msg::SwitchGroup_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::SwitchGroup_<ContainerAllocator>>>;
  _switch_groups_type switch_groups;

  // setters for named parameter idiom
  Type & set__schema(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema = _arg;
    return *this;
  }
  Type & set__schema_version(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema_version = _arg;
    return *this;
  }
  Type & set__instance_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->instance_id = _arg;
    return *this;
  }
  Type & set__commit_seq(
    const uint64_t & _arg)
  {
    this->commit_seq = _arg;
    return *this;
  }
  Type & set__segments(
    const std::vector<mrs_msgs::msg::Segment_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Segment_<ContainerAllocator>>> & _arg)
  {
    this->segments = _arg;
    return *this;
  }
  Type & set__dependencies(
    const std::vector<mrs_msgs::msg::Dependency_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Dependency_<ContainerAllocator>>> & _arg)
  {
    this->dependencies = _arg;
    return *this;
  }
  Type & set__switch_groups(
    const std::vector<mrs_msgs::msg::SwitchGroup_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::SwitchGroup_<ContainerAllocator>>> & _arg)
  {
    this->switch_groups = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__ExecutionConstraints
    std::shared_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__ExecutionConstraints
    std::shared_ptr<mrs_msgs::msg::ExecutionConstraints_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ExecutionConstraints_ & other) const
  {
    if (this->schema != other.schema) {
      return false;
    }
    if (this->schema_version != other.schema_version) {
      return false;
    }
    if (this->instance_id != other.instance_id) {
      return false;
    }
    if (this->commit_seq != other.commit_seq) {
      return false;
    }
    if (this->segments != other.segments) {
      return false;
    }
    if (this->dependencies != other.dependencies) {
      return false;
    }
    if (this->switch_groups != other.switch_groups) {
      return false;
    }
    return true;
  }
  bool operator!=(const ExecutionConstraints_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ExecutionConstraints_

// alias to use template instance with default allocator
using ExecutionConstraints =
  mrs_msgs::msg::ExecutionConstraints_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_HPP_
