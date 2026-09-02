// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/roadmap__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/roadmap__functions.h"
#include "mrs_msgs/msg/detail/roadmap__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `endpoints`
#include "rosidl_runtime_c/string_functions.h"
// Member `nodes`
#include "mrs_msgs/msg/roadmap_node.h"
// Member `nodes`
#include "mrs_msgs/msg/detail/roadmap_node__rosidl_typesupport_introspection_c.h"
// Member `edges`
#include "mrs_msgs/msg/roadmap_edge.h"
// Member `edges`
#include "mrs_msgs/msg/detail/roadmap_edge__rosidl_typesupport_introspection_c.h"
// Member `corridors`
#include "mrs_msgs/msg/corridor.h"
// Member `corridors`
#include "mrs_msgs/msg/detail/corridor__rosidl_typesupport_introspection_c.h"
// Member `provenance`
#include "mrs_msgs/msg/roadmap_provenance.h"
// Member `provenance`
#include "mrs_msgs/msg/detail/roadmap_provenance__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__Roadmap__init(message_memory);
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_fini_function(void * message_memory)
{
  mrs_msgs__msg__Roadmap__fini(message_memory);
}

size_t mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__nodes(
  const void * untyped_member)
{
  const mrs_msgs__msg__RoadmapNode__Sequence * member =
    (const mrs_msgs__msg__RoadmapNode__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__nodes(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RoadmapNode__Sequence * member =
    (const mrs_msgs__msg__RoadmapNode__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__nodes(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RoadmapNode__Sequence * member =
    (mrs_msgs__msg__RoadmapNode__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__nodes(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RoadmapNode * item =
    ((const mrs_msgs__msg__RoadmapNode *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__nodes(untyped_member, index));
  mrs_msgs__msg__RoadmapNode * value =
    (mrs_msgs__msg__RoadmapNode *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__nodes(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RoadmapNode * item =
    ((mrs_msgs__msg__RoadmapNode *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__nodes(untyped_member, index));
  const mrs_msgs__msg__RoadmapNode * value =
    (const mrs_msgs__msg__RoadmapNode *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__nodes(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RoadmapNode__Sequence * member =
    (mrs_msgs__msg__RoadmapNode__Sequence *)(untyped_member);
  mrs_msgs__msg__RoadmapNode__Sequence__fini(member);
  return mrs_msgs__msg__RoadmapNode__Sequence__init(member, size);
}

size_t mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__edges(
  const void * untyped_member)
{
  const mrs_msgs__msg__RoadmapEdge__Sequence * member =
    (const mrs_msgs__msg__RoadmapEdge__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__edges(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RoadmapEdge__Sequence * member =
    (const mrs_msgs__msg__RoadmapEdge__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__edges(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RoadmapEdge__Sequence * member =
    (mrs_msgs__msg__RoadmapEdge__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__edges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RoadmapEdge * item =
    ((const mrs_msgs__msg__RoadmapEdge *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__edges(untyped_member, index));
  mrs_msgs__msg__RoadmapEdge * value =
    (mrs_msgs__msg__RoadmapEdge *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__edges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RoadmapEdge * item =
    ((mrs_msgs__msg__RoadmapEdge *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__edges(untyped_member, index));
  const mrs_msgs__msg__RoadmapEdge * value =
    (const mrs_msgs__msg__RoadmapEdge *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__edges(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RoadmapEdge__Sequence * member =
    (mrs_msgs__msg__RoadmapEdge__Sequence *)(untyped_member);
  mrs_msgs__msg__RoadmapEdge__Sequence__fini(member);
  return mrs_msgs__msg__RoadmapEdge__Sequence__init(member, size);
}

size_t mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__endpoints(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__endpoints(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__endpoints(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__endpoints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__endpoints(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__endpoints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__endpoints(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__endpoints(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__corridors(
  const void * untyped_member)
{
  const mrs_msgs__msg__Corridor__Sequence * member =
    (const mrs_msgs__msg__Corridor__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__corridors(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__Corridor__Sequence * member =
    (const mrs_msgs__msg__Corridor__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__corridors(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__Corridor__Sequence * member =
    (mrs_msgs__msg__Corridor__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__corridors(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__Corridor * item =
    ((const mrs_msgs__msg__Corridor *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__corridors(untyped_member, index));
  mrs_msgs__msg__Corridor * value =
    (mrs_msgs__msg__Corridor *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__corridors(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__Corridor * item =
    ((mrs_msgs__msg__Corridor *)
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__corridors(untyped_member, index));
  const mrs_msgs__msg__Corridor * value =
    (const mrs_msgs__msg__Corridor *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__corridors(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__Corridor__Sequence * member =
    (mrs_msgs__msg__Corridor__Sequence *)(untyped_member);
  mrs_msgs__msg__Corridor__Sequence__fini(member);
  return mrs_msgs__msg__Corridor__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array[8] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, schema),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "schema_version",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, schema_version),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "instance_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "nodes",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, nodes),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__nodes,  // size() function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__nodes,  // get_const(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__nodes,  // get(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__nodes,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__nodes,  // assign(index, value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__nodes  // resize(index) function pointer
  },
  {
    "edges",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, edges),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__edges,  // size() function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__edges,  // get_const(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__edges,  // get(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__edges,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__edges,  // assign(index, value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__edges  // resize(index) function pointer
  },
  {
    "endpoints",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, endpoints),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__endpoints,  // size() function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__endpoints,  // get_const(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__endpoints,  // get(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__endpoints,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__endpoints,  // assign(index, value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__endpoints  // resize(index) function pointer
  },
  {
    "corridors",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, corridors),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__size_function__Roadmap__corridors,  // size() function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_const_function__Roadmap__corridors,  // get_const(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__get_function__Roadmap__corridors,  // get(index) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__fetch_function__Roadmap__corridors,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__assign_function__Roadmap__corridors,  // assign(index, value) function pointer
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__resize_function__Roadmap__corridors  // resize(index) function pointer
  },
  {
    "provenance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Roadmap, provenance),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_members = {
  "mrs_msgs__msg",  // message namespace
  "Roadmap",  // message name
  8,  // number of fields
  sizeof(mrs_msgs__msg__Roadmap),
  false,  // has_any_key_member_
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array,  // message members
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_type_support_handle = {
  0,
  &mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Roadmap__get_type_hash,
  &mrs_msgs__msg__Roadmap__get_type_description,
  &mrs_msgs__msg__Roadmap__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Roadmap)() {
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RoadmapNode)();
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RoadmapEdge)();
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Corridor)();
  mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RoadmapProvenance)();
  if (!mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__Roadmap__rosidl_typesupport_introspection_c__Roadmap_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
