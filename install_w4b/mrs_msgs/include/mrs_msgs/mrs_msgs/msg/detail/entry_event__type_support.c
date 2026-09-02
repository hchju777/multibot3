// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/EntryEvent.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/entry_event__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/entry_event__functions.h"
#include "mrs_msgs/msg/detail/entry_event__struct.h"


// Include directives for member types
// Member `robot`
// Member `segment`
// Member `location`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__EntryEvent__init(message_memory);
}

void mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_fini_function(void * message_memory)
{
  mrs_msgs__msg__EntryEvent__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_member_array[5] = {
  {
    "seq",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__EntryEvent, seq),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "robot",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__EntryEvent, robot),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "segment",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__EntryEvent, segment),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "location",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__EntryEvent, location),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "entered",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__EntryEvent, entered),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_members = {
  "mrs_msgs__msg",  // message namespace
  "EntryEvent",  // message name
  5,  // number of fields
  sizeof(mrs_msgs__msg__EntryEvent),
  false,  // has_any_key_member_
  mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_member_array,  // message members
  mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_type_support_handle = {
  0,
  &mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__EntryEvent__get_type_hash,
  &mrs_msgs__msg__EntryEvent__get_type_description,
  &mrs_msgs__msg__EntryEvent__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, EntryEvent)() {
  if (!mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__EntryEvent__rosidl_typesupport_introspection_c__EntryEvent_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
