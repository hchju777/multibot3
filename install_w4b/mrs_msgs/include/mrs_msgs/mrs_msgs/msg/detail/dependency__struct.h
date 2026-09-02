// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Dependency.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/dependency.h"


#ifndef MRS_MSGS__MSG__DETAIL__DEPENDENCY__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__DEPENDENCY__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'DEP_UNKNOWN'.
enum
{
  mrs_msgs__msg__Dependency__DEP_UNKNOWN = 0
};

/// Constant 'DEP_SEQUENTIAL'.
enum
{
  mrs_msgs__msg__Dependency__DEP_SEQUENTIAL = 1
};

/// Constant 'DEP_ORDERING'.
enum
{
  mrs_msgs__msg__Dependency__DEP_ORDERING = 2
};

// Include directives for member types
// Member 'from_id'
// Member 'to_id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Dependency in the package mrs_msgs.
/**
  * DependencyType vocabulary (350 §3-2 V2~V6).
 */
typedef struct mrs_msgs__msg__Dependency
{
  /// 🔴 from_id/to_id here are SEGMENT identifiers ({robot}#{index}), NOT roadmap
  /// node ids (350 §8 D6 item 1). Do NOT nest an EdgeRef into this message — the
  /// name space differs from RoadmapEdge/Segment's from_id/to_id even though the
  /// JSON source field names ("from"/"to") are identical.
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
  uint8_t type;
} mrs_msgs__msg__Dependency;

// Struct for a sequence of mrs_msgs__msg__Dependency.
typedef struct mrs_msgs__msg__Dependency__Sequence
{
  mrs_msgs__msg__Dependency * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Dependency__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__DEPENDENCY__STRUCT_H_
