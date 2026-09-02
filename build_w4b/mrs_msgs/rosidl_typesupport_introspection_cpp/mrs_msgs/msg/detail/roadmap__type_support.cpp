// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "mrs_msgs/msg/detail/roadmap__functions.h"
#include "mrs_msgs/msg/detail/roadmap__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace mrs_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void Roadmap_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) mrs_msgs::msg::Roadmap(_init);
}

void Roadmap_fini_function(void * message_memory)
{
  auto typed_message = static_cast<mrs_msgs::msg::Roadmap *>(message_memory);
  typed_message->~Roadmap();
}

size_t size_function__Roadmap__nodes(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::RoadmapNode> *>(untyped_member);
  return member->size();
}

const void * get_const_function__Roadmap__nodes(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::RoadmapNode> *>(untyped_member);
  return &member[index];
}

void * get_function__Roadmap__nodes(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::RoadmapNode> *>(untyped_member);
  return &member[index];
}

void fetch_function__Roadmap__nodes(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::RoadmapNode *>(
    get_const_function__Roadmap__nodes(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::RoadmapNode *>(untyped_value);
  value = item;
}

void assign_function__Roadmap__nodes(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::RoadmapNode *>(
    get_function__Roadmap__nodes(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::RoadmapNode *>(untyped_value);
  item = value;
}

void resize_function__Roadmap__nodes(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::RoadmapNode> *>(untyped_member);
  member->resize(size);
}

size_t size_function__Roadmap__edges(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::RoadmapEdge> *>(untyped_member);
  return member->size();
}

const void * get_const_function__Roadmap__edges(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::RoadmapEdge> *>(untyped_member);
  return &member[index];
}

void * get_function__Roadmap__edges(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::RoadmapEdge> *>(untyped_member);
  return &member[index];
}

void fetch_function__Roadmap__edges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::RoadmapEdge *>(
    get_const_function__Roadmap__edges(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::RoadmapEdge *>(untyped_value);
  value = item;
}

void assign_function__Roadmap__edges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::RoadmapEdge *>(
    get_function__Roadmap__edges(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::RoadmapEdge *>(untyped_value);
  item = value;
}

void resize_function__Roadmap__edges(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::RoadmapEdge> *>(untyped_member);
  member->resize(size);
}

size_t size_function__Roadmap__endpoints(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<std::string> *>(untyped_member);
  return member->size();
}

const void * get_const_function__Roadmap__endpoints(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<std::string> *>(untyped_member);
  return &member[index];
}

void * get_function__Roadmap__endpoints(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<std::string> *>(untyped_member);
  return &member[index];
}

void fetch_function__Roadmap__endpoints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const std::string *>(
    get_const_function__Roadmap__endpoints(untyped_member, index));
  auto & value = *reinterpret_cast<std::string *>(untyped_value);
  value = item;
}

void assign_function__Roadmap__endpoints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<std::string *>(
    get_function__Roadmap__endpoints(untyped_member, index));
  const auto & value = *reinterpret_cast<const std::string *>(untyped_value);
  item = value;
}

void resize_function__Roadmap__endpoints(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<std::string> *>(untyped_member);
  member->resize(size);
}

size_t size_function__Roadmap__corridors(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::Corridor> *>(untyped_member);
  return member->size();
}

const void * get_const_function__Roadmap__corridors(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::Corridor> *>(untyped_member);
  return &member[index];
}

void * get_function__Roadmap__corridors(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::Corridor> *>(untyped_member);
  return &member[index];
}

void fetch_function__Roadmap__corridors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::Corridor *>(
    get_const_function__Roadmap__corridors(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::Corridor *>(untyped_value);
  value = item;
}

void assign_function__Roadmap__corridors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::Corridor *>(
    get_function__Roadmap__corridors(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::Corridor *>(untyped_value);
  item = value;
}

void resize_function__Roadmap__corridors(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::Corridor> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember Roadmap_message_member_array[8] = {
  {
    "schema",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, schema),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "schema_version",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, schema_version),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "instance_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, instance_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "nodes",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::RoadmapNode>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, nodes),  // bytes offset in struct
    nullptr,  // default value
    size_function__Roadmap__nodes,  // size() function pointer
    get_const_function__Roadmap__nodes,  // get_const(index) function pointer
    get_function__Roadmap__nodes,  // get(index) function pointer
    fetch_function__Roadmap__nodes,  // fetch(index, &value) function pointer
    assign_function__Roadmap__nodes,  // assign(index, value) function pointer
    resize_function__Roadmap__nodes  // resize(index) function pointer
  },
  {
    "edges",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::RoadmapEdge>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, edges),  // bytes offset in struct
    nullptr,  // default value
    size_function__Roadmap__edges,  // size() function pointer
    get_const_function__Roadmap__edges,  // get_const(index) function pointer
    get_function__Roadmap__edges,  // get(index) function pointer
    fetch_function__Roadmap__edges,  // fetch(index, &value) function pointer
    assign_function__Roadmap__edges,  // assign(index, value) function pointer
    resize_function__Roadmap__edges  // resize(index) function pointer
  },
  {
    "endpoints",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, endpoints),  // bytes offset in struct
    nullptr,  // default value
    size_function__Roadmap__endpoints,  // size() function pointer
    get_const_function__Roadmap__endpoints,  // get_const(index) function pointer
    get_function__Roadmap__endpoints,  // get(index) function pointer
    fetch_function__Roadmap__endpoints,  // fetch(index, &value) function pointer
    assign_function__Roadmap__endpoints,  // assign(index, value) function pointer
    resize_function__Roadmap__endpoints  // resize(index) function pointer
  },
  {
    "corridors",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::Corridor>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, corridors),  // bytes offset in struct
    nullptr,  // default value
    size_function__Roadmap__corridors,  // size() function pointer
    get_const_function__Roadmap__corridors,  // get_const(index) function pointer
    get_function__Roadmap__corridors,  // get(index) function pointer
    fetch_function__Roadmap__corridors,  // fetch(index, &value) function pointer
    assign_function__Roadmap__corridors,  // assign(index, value) function pointer
    resize_function__Roadmap__corridors  // resize(index) function pointer
  },
  {
    "provenance",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::RoadmapProvenance>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Roadmap, provenance),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers Roadmap_message_members = {
  "mrs_msgs::msg",  // message namespace
  "Roadmap",  // message name
  8,  // number of fields
  sizeof(mrs_msgs::msg::Roadmap),
  false,  // has_any_key_member_
  Roadmap_message_member_array,  // message members
  Roadmap_init_function,  // function to initialize message memory (memory has to be allocated)
  Roadmap_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t Roadmap_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &Roadmap_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Roadmap__get_type_hash,
  &mrs_msgs__msg__Roadmap__get_type_description,
  &mrs_msgs__msg__Roadmap__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace mrs_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::msg::Roadmap>()
{
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::Roadmap_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, msg, Roadmap)() {
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::Roadmap_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
