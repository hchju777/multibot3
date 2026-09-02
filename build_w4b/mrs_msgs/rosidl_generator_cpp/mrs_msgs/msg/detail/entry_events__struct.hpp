// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/EntryEvents.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/entry_events.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'events'
#include "mrs_msgs/msg/detail/entry_event__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__EntryEvents __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__EntryEvents __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct EntryEvents_
{
  using Type = EntryEvents_<ContainerAllocator>;

  explicit EntryEvents_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
    }
  }

  explicit EntryEvents_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
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
  using _events_type =
    std::vector<mrs_msgs::msg::EntryEvent_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EntryEvent_<ContainerAllocator>>>;
  _events_type events;

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
  Type & set__events(
    const std::vector<mrs_msgs::msg::EntryEvent_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EntryEvent_<ContainerAllocator>>> & _arg)
  {
    this->events = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::EntryEvents_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::EntryEvents_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::EntryEvents_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::EntryEvents_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__EntryEvents
    std::shared_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__EntryEvents
    std::shared_ptr<mrs_msgs::msg::EntryEvents_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const EntryEvents_ & other) const
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
    if (this->events != other.events) {
      return false;
    }
    return true;
  }
  bool operator!=(const EntryEvents_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct EntryEvents_

// alias to use template instance with default allocator
using EntryEvents =
  mrs_msgs::msg::EntryEvents_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_HPP_
