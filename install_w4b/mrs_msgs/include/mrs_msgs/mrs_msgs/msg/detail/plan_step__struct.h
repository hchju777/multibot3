// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/PlanStep.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/plan_step.h"


#ifndef MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'location'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/PlanStep in the package mrs_msgs.
typedef struct mrs_msgs__msg__PlanStep
{
  uint32_t index;
  rosidl_runtime_c__String location;
} mrs_msgs__msg__PlanStep;

// Struct for a sequence of mrs_msgs__msg__PlanStep.
typedef struct mrs_msgs__msg__PlanStep__Sequence
{
  mrs_msgs__msg__PlanStep * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__PlanStep__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__PLAN_STEP__STRUCT_H_
