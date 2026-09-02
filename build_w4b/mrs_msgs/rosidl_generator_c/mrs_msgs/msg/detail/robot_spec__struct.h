// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_spec.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_H_

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

/// Struct defined in msg/RobotSpec in the package mrs_msgs.
typedef struct mrs_msgs__msg__RobotSpec
{
  rosidl_runtime_c__String robot;
  double avg_traversal_speed_mps;
  double v_max;
  double brake_decel_min_mps2;
  double curvature_max_invm;
  bool reverse_motion_allowed;
  double circumradius_m;
} mrs_msgs__msg__RobotSpec;

// Struct for a sequence of mrs_msgs__msg__RobotSpec.
typedef struct mrs_msgs__msg__RobotSpec__Sequence
{
  mrs_msgs__msg__RobotSpec * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotSpec__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__STRUCT_H_
