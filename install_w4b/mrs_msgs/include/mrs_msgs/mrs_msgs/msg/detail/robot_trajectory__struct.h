// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotTrajectory.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_trajectory.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__STRUCT_H_

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
// Member 'points'
#include "mrs_msgs/msg/detail/trajectory_point__struct.h"

/// Struct defined in msg/RobotTrajectory in the package mrs_msgs.
typedef struct mrs_msgs__msg__RobotTrajectory
{
  rosidl_runtime_c__String robot;
  mrs_msgs__msg__TrajectoryPoint__Sequence points;
} mrs_msgs__msg__RobotTrajectory;

// Struct for a sequence of mrs_msgs__msg__RobotTrajectory.
typedef struct mrs_msgs__msg__RobotTrajectory__Sequence
{
  mrs_msgs__msg__RobotTrajectory * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotTrajectory__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_TRAJECTORY__STRUCT_H_
