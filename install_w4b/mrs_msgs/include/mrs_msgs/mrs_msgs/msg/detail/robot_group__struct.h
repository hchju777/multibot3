// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotGroup.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_group.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'robots'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RobotGroup in the package mrs_msgs.
/**
  * string[][] has no IDL representation (350 §6 replan_result.components) —
  * one extra level of nesting via this wrapper instead (350 §7-6 "3중 이상
  * 중첩은 만들지 않는다").
 */
typedef struct mrs_msgs__msg__RobotGroup
{
  rosidl_runtime_c__String__Sequence robots;
} mrs_msgs__msg__RobotGroup;

// Struct for a sequence of mrs_msgs__msg__RobotGroup.
typedef struct mrs_msgs__msg__RobotGroup__Sequence
{
  mrs_msgs__msg__RobotGroup * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotGroup__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_GROUP__STRUCT_H_
