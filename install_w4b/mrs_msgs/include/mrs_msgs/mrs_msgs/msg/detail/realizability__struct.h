// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Realizability.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability.h"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY__STRUCT_H_

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
// Member 'entries'
#include "mrs_msgs/msg/detail/realizability_entry__struct.h"

/// Struct defined in msg/Realizability in the package mrs_msgs.
typedef struct mrs_msgs__msg__Realizability
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint64_t rz_seq;
  mrs_msgs__msg__RealizabilityEntry__Sequence entries;
} mrs_msgs__msg__Realizability;

// Struct for a sequence of mrs_msgs__msg__Realizability.
typedef struct mrs_msgs__msg__Realizability__Sequence
{
  mrs_msgs__msg__Realizability * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Realizability__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY__STRUCT_H_
