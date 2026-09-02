// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/goal.h"


#ifndef MRS_MSGS__MSG__DETAIL__GOAL__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__GOAL__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// Member 'task'
// Member 'location'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Goal in the package mrs_msgs.
typedef struct mrs_msgs__msg__Goal
{
  rosidl_runtime_c__String goal_id;
  rosidl_runtime_c__String task;
  rosidl_runtime_c__String location;
} mrs_msgs__msg__Goal;

// Struct for a sequence of mrs_msgs__msg__Goal.
typedef struct mrs_msgs__msg__Goal__Sequence
{
  mrs_msgs__msg__Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Goal__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__GOAL__STRUCT_H_
