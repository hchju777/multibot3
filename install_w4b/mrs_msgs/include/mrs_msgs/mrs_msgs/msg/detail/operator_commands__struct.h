// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/OperatorCommands.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_commands.h"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'schema'
// Member 'schema_version'
// Member 'instance_id'
#include "rosidl_runtime_c/string.h"
// Member 'commands'
#include "mrs_msgs/msg/detail/operator_command__struct.h"

/// Struct defined in msg/OperatorCommands in the package mrs_msgs.
typedef struct mrs_msgs__msg__OperatorCommands
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  mrs_msgs__msg__OperatorCommand__Sequence commands;
} mrs_msgs__msg__OperatorCommands;

// Struct for a sequence of mrs_msgs__msg__OperatorCommands.
typedef struct mrs_msgs__msg__OperatorCommands__Sequence
{
  mrs_msgs__msg__OperatorCommands * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__OperatorCommands__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMANDS__STRUCT_H_
