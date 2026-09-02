// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/TrajectoryPoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/trajectory_point.h"


#ifndef MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Struct defined in msg/TrajectoryPoint in the package mrs_msgs.
typedef struct mrs_msgs__msg__TrajectoryPoint
{
  double t;
  double x;
  double y;
  double yaw;
  /// Optional numeric fields: has_* companion booleans (350 §4-3 rule). has_* is
  /// NEVER carried in the JSON boundary dump — absence there IS absence here.
  double v;
  bool has_v;
  double a;
  bool has_a;
} mrs_msgs__msg__TrajectoryPoint;

// Struct for a sequence of mrs_msgs__msg__TrajectoryPoint.
typedef struct mrs_msgs__msg__TrajectoryPoint__Sequence
{
  mrs_msgs__msg__TrajectoryPoint * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__TrajectoryPoint__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__TRAJECTORY_POINT__STRUCT_H_
