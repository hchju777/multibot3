// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/OperatorCommand.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_command.h"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'VERB_UNKNOWN'.
enum
{
  mrs_msgs__msg__OperatorCommand__VERB_UNKNOWN = 0
};

/// Constant 'VERB_HOLD'.
enum
{
  mrs_msgs__msg__OperatorCommand__VERB_HOLD = 1
};

/// Constant 'VERB_SPEED_CAP'.
enum
{
  mrs_msgs__msg__OperatorCommand__VERB_SPEED_CAP = 2
};

/// Constant 'VERB_GOAL_OVERRIDE'.
enum
{
  mrs_msgs__msg__OperatorCommand__VERB_GOAL_OVERRIDE = 3
};

// Include directives for member types
// Member 'robot'
// Member 'goal_location'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/OperatorCommand in the package mrs_msgs.
/**
  * OperatorVerb vocabulary (350 §3-2 V2~V6).
 */
typedef struct mrs_msgs__msg__OperatorCommand
{
  uint64_t seq;
  rosidl_runtime_c__String robot;
  uint8_t verb;
  bool active;
  /// Optional numeric field: has_* companion (350 §4-3). Required only when
  /// verb==VERB_SPEED_CAP and active==true (layer-2 condition, not layer-1).
  double speed_cap_mps;
  bool has_speed_cap_mps;
  /// Optional STRING field: "" sentinel, NOT a has_* flag (350 §4-3 rule — this
  /// message is the one place both conventions coexist). Required only when
  /// verb==VERB_GOAL_OVERRIDE and active==true.
  rosidl_runtime_c__String goal_location;
} mrs_msgs__msg__OperatorCommand;

// Struct for a sequence of mrs_msgs__msg__OperatorCommand.
typedef struct mrs_msgs__msg__OperatorCommand__Sequence
{
  mrs_msgs__msg__OperatorCommand * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__OperatorCommand__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__STRUCT_H_
