// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotBaseline.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_baseline.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'robot'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RobotBaseline in the package mrs_msgs.
typedef struct mrs_msgs__msg__RobotBaseline
{
  rosidl_runtime_c__String robot;
  uint32_t committed_segment_count;
} mrs_msgs__msg__RobotBaseline;

// Struct for a sequence of mrs_msgs__msg__RobotBaseline.
typedef struct mrs_msgs__msg__RobotBaseline__Sequence
{
  mrs_msgs__msg__RobotBaseline * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotBaseline__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__STRUCT_H_
