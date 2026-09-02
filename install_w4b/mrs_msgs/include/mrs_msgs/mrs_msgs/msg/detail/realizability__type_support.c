// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/Realizability.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/realizability__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/realizability__functions.h"
#include "mrs_msgs/msg/detail/realizability__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `entries`
#include "mrs_msgs/msg/realizability_entry.h"
// Member `entries`
#include "mrs_msgs/msg/detail/realizability_entry__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__Realizability__init(message_memory);
}

void mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_fini_function(void * message_memory)
{
  mrs_msgs__msg__Realizability__fini(message_memory);
}

size_t mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__size_function__Realizability__entries(
  const void * untyped_member)
{
  const mrs_msgs__msg__RealizabilityEntry__Sequence * member =
    (const mrs_msgs__msg__RealizabilityEntry__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_const_function__Realizability__entries(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RealizabilityEntry__Sequence * member =
    (const mrs_msgs__msg__RealizabilityEntry__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_function__Realizability__entries(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RealizabilityEntry__Sequence * member =
    (mrs_msgs__msg__RealizabilityEntry__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__fetch_function__Realizability__entries(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RealizabilityEntry * item =
    ((const mrs_msgs__msg__RealizabilityEntry *)
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_const_function__Realizability__entries(untyped_member, index));
  mrs_msgs__msg__RealizabilityEntry * value =
    (mrs_msgs__msg__RealizabilityEntry *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__assign_function__Realizability__entries(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RealizabilityEntry * item =
    ((mrs_msgs__msg__RealizabilityEntry *)
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_function__Realizability__entries(untyped_member, index));
  const mrs_msgs__msg__RealizabilityEntry * value =
    (const mrs_msgs__msg__RealizabilityEntry *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__resize_function__Realizability__entries(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RealizabilityEntry__Sequence * member =
    (mrs_msgs__msg__RealizabilityEntry__Sequence *)(untyped_member);
  mrs_msgs__msg__RealizabilityEntry__Sequence__fini(member);
  return mrs_msgs__msg__RealizabilityEntry__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_member_array[5] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Realizability, schema),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Realizability, schema_version),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Realizability, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "rz_seq",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Realizability, rz_seq),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "entries",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Realizability, entries),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__size_function__Realizability__entries,  // size() function pointer
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_const_function__Realizability__entries,  // get_const(index) function pointer
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__get_function__Realizability__entries,  // get(index) function pointer
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__fetch_function__Realizability__entries,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__assign_function__Realizability__entries,  // assign(index, value) function pointer
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__resize_function__Realizability__entries  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_members = {
  "mrs_msgs__msg",  // message namespace
  "Realizability",  // message name
  5,  // number of fields
  sizeof(mrs_msgs__msg__Realizability),
  false,  // has_any_key_member_
  mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_member_array,  // message members
  mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_type_support_handle = {
  0,
  &mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Realizability__get_type_hash,
  &mrs_msgs__msg__Realizability__get_type_description,
  &mrs_msgs__msg__Realizability__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Realizability)() {
  mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RealizabilityEntry)();
  if (!mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__Realizability__rosidl_typesupport_introspection_c__Realizability_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
