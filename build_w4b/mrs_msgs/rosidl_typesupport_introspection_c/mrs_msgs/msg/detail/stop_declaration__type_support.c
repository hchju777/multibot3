// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/stop_declaration__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/stop_declaration__functions.h"
#include "mrs_msgs/msg/detail/stop_declaration__struct.h"


// Include directives for member types
// Member `robot`
#include "rosidl_runtime_c/string_functions.h"
// Member `blocked_edge`
#include "mrs_msgs/msg/edge_ref.h"
// Member `blocked_edge`
#include "mrs_msgs/msg/detail/edge_ref__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__StopDeclaration__init(message_memory);
}

void mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_fini_function(void * message_memory)
{
  mrs_msgs__msg__StopDeclaration__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_member_array[5] = {
  {
    "seq",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__StopDeclaration, seq),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__StopDeclaration, robot),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "blocked_edge",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__StopDeclaration, blocked_edge),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "declared",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__StopDeclaration, declared),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "reason",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__StopDeclaration, reason),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_members = {
  "mrs_msgs__msg",  // message namespace
  "StopDeclaration",  // message name
  5,  // number of fields
  sizeof(mrs_msgs__msg__StopDeclaration),
  false,  // has_any_key_member_
  mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_member_array,  // message members
  mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_type_support_handle = {
  0,
  &mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__StopDeclaration__get_type_hash,
  &mrs_msgs__msg__StopDeclaration__get_type_description,
  &mrs_msgs__msg__StopDeclaration__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, StopDeclaration)() {
  mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, EdgeRef)();
  if (!mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__StopDeclaration__rosidl_typesupport_introspection_c__StopDeclaration_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
