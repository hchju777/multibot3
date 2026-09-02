// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RobotSpecs.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_specs.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__STRUCT_H_

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
// Member 'robots'
#include "mrs_msgs/msg/detail/robot_spec__struct.h"

/// Struct defined in msg/RobotSpecs in the package mrs_msgs.
typedef struct mrs_msgs__msg__RobotSpecs
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  mrs_msgs__msg__RobotSpec__Sequence robots;
} mrs_msgs__msg__RobotSpecs;

// Struct for a sequence of mrs_msgs__msg__RobotSpecs.
typedef struct mrs_msgs__msg__RobotSpecs__Sequence
{
  mrs_msgs__msg__RobotSpecs * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RobotSpecs__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__STRUCT_H_
