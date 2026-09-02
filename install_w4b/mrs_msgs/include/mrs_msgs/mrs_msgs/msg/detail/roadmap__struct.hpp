// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'nodes'
#include "mrs_msgs/msg/detail/roadmap_node__struct.hpp"
// Member 'edges'
#include "mrs_msgs/msg/detail/roadmap_edge__struct.hpp"
// Member 'corridors'
#include "mrs_msgs/msg/detail/corridor__struct.hpp"
// Member 'provenance'
#include "mrs_msgs/msg/detail/roadmap_provenance__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__msg__Roadmap __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__msg__Roadmap __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Roadmap_
{
  using Type = Roadmap_<ContainerAllocator>;

  explicit Roadmap_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : provenance(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
    }
  }

  explicit Roadmap_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : schema(_alloc),
    schema_version(_alloc),
    instance_id(_alloc),
    provenance(_alloc, _init)
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
  using _nodes_type =
    std::vector<mrs_msgs::msg::RoadmapNode_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RoadmapNode_<ContainerAllocator>>>;
  _nodes_type nodes;
  using _edges_type =
    std::vector<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>>;
  _edges_type edges;
  using _endpoints_type =
    std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>>;
  _endpoints_type endpoints;
  using _corridors_type =
    std::vector<mrs_msgs::msg::Corridor_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Corridor_<ContainerAllocator>>>;
  _corridors_type corridors;
  using _provenance_type =
    mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator>;
  _provenance_type provenance;

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
  Type & set__nodes(
    const std::vector<mrs_msgs::msg::RoadmapNode_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RoadmapNode_<ContainerAllocator>>> & _arg)
  {
    this->nodes = _arg;
    return *this;
  }
  Type & set__edges(
    const std::vector<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RoadmapEdge_<ContainerAllocator>>> & _arg)
  {
    this->edges = _arg;
    return *this;
  }
  Type & set__endpoints(
    const std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>> & _arg)
  {
    this->endpoints = _arg;
    return *this;
  }
  Type & set__corridors(
    const std::vector<mrs_msgs::msg::Corridor_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::Corridor_<ContainerAllocator>>> & _arg)
  {
    this->corridors = _arg;
    return *this;
  }
  Type & set__provenance(
    const mrs_msgs::msg::RoadmapProvenance_<ContainerAllocator> & _arg)
  {
    this->provenance = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::msg::Roadmap_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::msg::Roadmap_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Roadmap_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::msg::Roadmap_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__msg__Roadmap
    std::shared_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__msg__Roadmap
    std::shared_ptr<mrs_msgs::msg::Roadmap_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Roadmap_ & other) const
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
    if (this->nodes != other.nodes) {
      return false;
    }
    if (this->edges != other.edges) {
      return false;
    }
    if (this->endpoints != other.endpoints) {
      return false;
    }
    if (this->corridors != other.corridors) {
      return false;
    }
    if (this->provenance != other.provenance) {
      return false;
    }
    return true;
  }
  bool operator!=(const Roadmap_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Roadmap_

// alias to use template instance with default allocator
using Roadmap =
  mrs_msgs::msg::Roadmap_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_HPP_
