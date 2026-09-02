// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Trajectories.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/trajectories.h"


#ifndef MRS_MSGS__MSG__DETAIL__TRAJECTORIES__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__TRAJECTORIES__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'schema'
// Member 'schema_version'
// Member 'instance_id'
#include "rosidl_runtime_c/string.h"
// Member 'limits'
#include "mrs_msgs/msg/detail/limits__struct.h"
// Member 'trajectories'
#include "mrs_msgs/msg/detail/robot_trajectory__struct.h"

/// Struct defined in msg/Trajectories in the package mrs_msgs.
typedef struct mrs_msgs__msg__Trajectories
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  mrs_msgs__msg__Limits limits;
  mrs_msgs__msg__RobotTrajectory__Sequence trajectories;
} mrs_msgs__msg__Trajectories;

// Struct for a sequence of mrs_msgs__msg__Trajectories.
typedef struct mrs_msgs__msg__Trajectories__Sequence
{
  mrs_msgs__msg__Trajectories * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Trajectories__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__TRAJECTORIES__STRUCT_H_
