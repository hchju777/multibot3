// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotPlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_plan.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'TERMINAL_UNKNOWN'.
enum
{
  mrs_msgs__msg__RobotPlan__TERMINAL_UNKNOWN = 0
};

/// Constant 'TERMINAL_GOAL_REACHED'.
enum
{
  mrs_msgs__msg__RobotPlan__TERMINAL_GOAL_REACHED = 1
};

/// Constant 'TERMINAL_PLAN_TRUNCATED'.
enum
{
  mrs_msgs__msg__RobotPlan__TERMINAL_PLAN_TRUNCATED = 2
};

/// Constant 'TERMINAL_NO_PROGRESS'.
enum
{
  mrs_msgs__msg__RobotPlan__TERMINAL_NO_PROGRESS = 3
};

// Include directives for member types
// Member 'robot'
#include "rosidl_runtime_c/string.h"
// Member 'steps'
#include "mrs_msgs/msg/detail/plan_step__struct.h"

/// Struct defined in msg/RobotPlan in the package mrs_msgs.
/**
  * PlanTerminal vocabulary (350 §3-2 V2~V6).
 */
typedef struct mrs_msgs__msg__RobotPlan
{
  rosidl_runtime_c__String robot;
  uint8_t terminal;
  mrs_msgs__msg__PlanStep__Sequence steps;
} mrs_msgs__msg__RobotPlan;

// Struct for a sequence of mrs_msgs__msg__RobotPlan.
typedef struct mrs_msgs__msg__RobotPlan__Sequence
{
  mrs_msgs__msg__RobotPlan * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotPlan__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__STRUCT_H_
