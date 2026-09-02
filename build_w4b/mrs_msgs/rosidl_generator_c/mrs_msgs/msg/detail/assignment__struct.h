// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Assignment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/assignment.h"


#ifndef MRS_MSGS__MSG__DETAIL__ASSIGNMENT__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ASSIGNMENT__STRUCT_H_

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
// Member 'unassigned_tasks'
#include "rosidl_runtime_c/string.h"
// Member 'assignments'
#include "mrs_msgs/msg/detail/robot_assignment__struct.h"

/// Struct defined in msg/Assignment in the package mrs_msgs.
typedef struct mrs_msgs__msg__Assignment
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint64_t revision;
  mrs_msgs__msg__RobotAssignment__Sequence assignments;
  rosidl_runtime_c__String__Sequence unassigned_tasks;
} mrs_msgs__msg__Assignment;

// Struct for a sequence of mrs_msgs__msg__Assignment.
typedef struct mrs_msgs__msg__Assignment__Sequence
{
  mrs_msgs__msg__Assignment * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Assignment__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ASSIGNMENT__STRUCT_H_
