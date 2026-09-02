// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/TaskRelease.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/task_release.h"


#ifndef MRS_MSGS__MSG__DETAIL__TASK_RELEASE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__TASK_RELEASE__STRUCT_H_

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
// Member 'task'
#include "rosidl_runtime_c/string.h"
// Member 'goals'
#include "mrs_msgs/msg/detail/task_goal__struct.h"

/// Struct defined in msg/TaskRelease in the package mrs_msgs.
typedef struct mrs_msgs__msg__TaskRelease
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint64_t release_index;
  rosidl_runtime_c__String task;
  mrs_msgs__msg__TaskGoal__Sequence goals;
} mrs_msgs__msg__TaskRelease;

// Struct for a sequence of mrs_msgs__msg__TaskRelease.
typedef struct mrs_msgs__msg__TaskRelease__Sequence
{
  mrs_msgs__msg__TaskRelease * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__TaskRelease__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__TASK_RELEASE__STRUCT_H_
