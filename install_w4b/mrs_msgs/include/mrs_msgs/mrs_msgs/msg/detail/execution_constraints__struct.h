// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/ExecutionConstraints.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/execution_constraints.h"


#ifndef MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_H_

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
// Member 'segments'
#include "mrs_msgs/msg/detail/segment__struct.h"
// Member 'dependencies'
#include "mrs_msgs/msg/detail/dependency__struct.h"
// Member 'switch_groups'
#include "mrs_msgs/msg/detail/switch_group__struct.h"

/// Struct defined in msg/ExecutionConstraints in the package mrs_msgs.
typedef struct mrs_msgs__msg__ExecutionConstraints
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint64_t commit_seq;
  mrs_msgs__msg__Segment__Sequence segments;
  mrs_msgs__msg__Dependency__Sequence dependencies;
  mrs_msgs__msg__SwitchGroup__Sequence switch_groups;
} mrs_msgs__msg__ExecutionConstraints;

// Struct for a sequence of mrs_msgs__msg__ExecutionConstraints.
typedef struct mrs_msgs__msg__ExecutionConstraints__Sequence
{
  mrs_msgs__msg__ExecutionConstraints * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__ExecutionConstraints__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__STRUCT_H_
