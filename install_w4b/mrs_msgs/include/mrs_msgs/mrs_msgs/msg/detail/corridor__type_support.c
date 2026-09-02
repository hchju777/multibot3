// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/corridor__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/corridor__functions.h"
#include "mrs_msgs/msg/detail/corridor__struct.h"


// Include directives for member types
// Member `id`
#include "rosidl_runtime_c/string_functions.h"
// Member `edges`
#include "mrs_msgs/msg/edge_ref.h"
// Member `edges`
#include "mrs_msgs/msg/detail/edge_ref__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__Corridor__init(message_memory);
}

void mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_fini_function(void * message_memory)
{
  mrs_msgs__msg__Corridor__fini(message_memory);
}

size_t mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__size_function__Corridor__edges(
  const void * untyped_member)
{
  const mrs_msgs__msg__EdgeRef__Sequence * member =
    (const mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_const_function__Corridor__edges(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__EdgeRef__Sequence * member =
    (const mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_function__Corridor__edges(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__EdgeRef__Sequence * member =
    (mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__fetch_function__Corridor__edges(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__EdgeRef * item =
    ((const mrs_msgs__msg__EdgeRef *)
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_const_function__Corridor__edges(untyped_member, index));
  mrs_msgs__msg__EdgeRef * value =
    (mrs_msgs__msg__EdgeRef *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__assign_function__Corridor__edges(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__EdgeRef * item =
    ((mrs_msgs__msg__EdgeRef *)
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_function__Corridor__edges(untyped_member, index));
  const mrs_msgs__msg__EdgeRef * value =
    (const mrs_msgs__msg__EdgeRef *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__resize_function__Corridor__edges(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__EdgeRef__Sequence * member =
    (mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  mrs_msgs__msg__EdgeRef__Sequence__fini(member);
  return mrs_msgs__msg__EdgeRef__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_member_array[3] = {
  {
    "id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Corridor, id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
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
    offsetof(mrs_msgs__msg__Corridor, edges),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__size_function__Corridor__edges,  // size() function pointer
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_const_function__Corridor__edges,  // get_const(index) function pointer
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__get_function__Corridor__edges,  // get(index) function pointer
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__fetch_function__Corridor__edges,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__assign_function__Corridor__edges,  // assign(index, value) function pointer
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__resize_function__Corridor__edges  // resize(index) function pointer
  },
  {
    "length_m",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Corridor, length_m),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_members = {
  "mrs_msgs__msg",  // message namespace
  "Corridor",  // message name
  3,  // number of fields
  sizeof(mrs_msgs__msg__Corridor),
  false,  // has_any_key_member_
  mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_member_array,  // message members
  mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_type_support_handle = {
  0,
  &mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Corridor__get_type_hash,
  &mrs_msgs__msg__Corridor__get_type_description,
  &mrs_msgs__msg__Corridor__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Corridor)() {
  mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, EdgeRef)();
  if (!mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__Corridor__rosidl_typesupport_introspection_c__Corridor_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
