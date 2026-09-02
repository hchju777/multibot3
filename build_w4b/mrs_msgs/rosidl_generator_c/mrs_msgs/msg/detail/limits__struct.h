// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/limits.h"


#ifndef MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/Limits in the package mrs_msgs.
/**
  * Fleet-folded physical limits only (mrs.robot_specs carries the per-robot
  * four: v_max, brake_decel_min_mps2, curvature_max_invm,
  * reverse_motion_allowed — do NOT duplicate them here, 350/roadmap-style
  * single-source-of-truth rule).
 */
typedef struct mrs_msgs__msg__Limits
{
  double a_max;
  double j_max;
  double min_separation_m;
  double omega_max_radps;
  double yaw_accel_max_radps2;
} mrs_msgs__msg__Limits;

// Struct for a sequence of mrs_msgs__msg__Limits.
typedef struct mrs_msgs__msg__Limits__Sequence
{
  mrs_msgs__msg__Limits * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Limits__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__LIMITS__STRUCT_H_
