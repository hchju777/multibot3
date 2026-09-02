// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/StopReason.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_reason.h"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'REASON_UNKNOWN'.
enum
{
  mrs_msgs__msg__StopReason__REASON_UNKNOWN = 0
};

/// Constant 'REASON_EXOGENOUS_BLOCK'.
enum
{
  mrs_msgs__msg__StopReason__REASON_EXOGENOUS_BLOCK = 1
};

/// Constant 'REASON_CTRL_STOP_DECLARED'.
enum
{
  mrs_msgs__msg__StopReason__REASON_CTRL_STOP_DECLARED = 2
};

/// Constant 'REASON_INFEASIBLE_SUBGOAL'.
enum
{
  mrs_msgs__msg__StopReason__REASON_INFEASIBLE_SUBGOAL = 3
};

/// Constant 'REASON_ORIENTATION_FLIP'.
enum
{
  mrs_msgs__msg__StopReason__REASON_ORIENTATION_FLIP = 4
};

/// Constant 'REASON_UNRESOLVABLE_LOCALLY'.
enum
{
  mrs_msgs__msg__StopReason__REASON_UNRESOLVABLE_LOCALLY = 5
};

/// Constant 'REASON_HORIZON_EXHAUSTING'.
enum
{
  mrs_msgs__msg__StopReason__REASON_HORIZON_EXHAUSTING = 6
};

/// Struct defined in msg/StopReason in the package mrs_msgs.
/**
  * Shared closed vocabulary for mrs.stop_declaration.declarations[].reason
  * (legal subset {1,3,5}) and mrs.replan_request.reason (legal subset
  * {1,2,3,4,5,6}) — 350 §3-2 V1, D1-c. ONE numbering, not two.
  * 🔴 whether rosidl accepts a constants-only, zero-field .msg is
  * UNCONFIRMED before the first colcon build (350 §3-4). If rejected, the
  * documented fallback is to declare these seven constants inside
  * StopDeclaration.msg instead (350 §3-4) — not applied here pre-emptively.
 */
typedef struct mrs_msgs__msg__StopReason
{
  uint8_t structure_needs_at_least_one_member;
} mrs_msgs__msg__StopReason;

// Struct for a sequence of mrs_msgs__msg__StopReason.
typedef struct mrs_msgs__msg__StopReason__Sequence
{
  mrs_msgs__msg__StopReason * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__StopReason__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__STOP_REASON__STRUCT_H_
