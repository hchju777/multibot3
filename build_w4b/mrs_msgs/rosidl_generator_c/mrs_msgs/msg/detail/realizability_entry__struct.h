// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RealizabilityEntry.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability_entry.h"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'group'
// Member 'alternative'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RealizabilityEntry in the package mrs_msgs.
typedef struct mrs_msgs__msg__RealizabilityEntry
{
  rosidl_runtime_c__String group;
  rosidl_runtime_c__String alternative;
  bool realizable;
} mrs_msgs__msg__RealizabilityEntry;

// Struct for a sequence of mrs_msgs__msg__RealizabilityEntry.
typedef struct mrs_msgs__msg__RealizabilityEntry__Sequence
{
  mrs_msgs__msg__RealizabilityEntry * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RealizabilityEntry__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__STRUCT_H_
