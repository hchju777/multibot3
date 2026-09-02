// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotAssignment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_assignment.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__STRUCT_H_

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
// Member 'start'
#include "rosidl_runtime_c/string.h"
// Member 'goals'
#include "mrs_msgs/msg/detail/goal__struct.h"

/// Struct defined in msg/RobotAssignment in the package mrs_msgs.
typedef struct mrs_msgs__msg__RobotAssignment
{
  rosidl_runtime_c__String robot;
  rosidl_runtime_c__String start;
  mrs_msgs__msg__Goal__Sequence goals;
} mrs_msgs__msg__RobotAssignment;

// Struct for a sequence of mrs_msgs__msg__RobotAssignment.
typedef struct mrs_msgs__msg__RobotAssignment__Sequence
{
  mrs_msgs__msg__RobotAssignment * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotAssignment__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_ASSIGNMENT__STRUCT_H_
