// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_edge.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

/// Constant 'TRAVERSAL_UNKNOWN'.
enum
{
  mrs_msgs__msg__RoadmapEdge__TRAVERSAL_UNKNOWN = 0
};

/// Constant 'TRAVERSAL_BIDIRECTIONAL'.
enum
{
  mrs_msgs__msg__RoadmapEdge__TRAVERSAL_BIDIRECTIONAL = 1
};

/// Constant 'TRAVERSAL_FORWARD_ONLY'.
enum
{
  mrs_msgs__msg__RoadmapEdge__TRAVERSAL_FORWARD_ONLY = 2
};

/// Constant 'TRAVERSAL_BACKWARD_ONLY'.
enum
{
  mrs_msgs__msg__RoadmapEdge__TRAVERSAL_BACKWARD_ONLY = 3
};

// Include directives for member types
// Member 'from_id'
// Member 'to_id'
// Member 'corridor'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RoadmapEdge in the package mrs_msgs.
/**
  * EdgeTraversal vocabulary (350 §3-2 V2~V6). 0 is reserved *_UNKNOWN across all
  * vocabularies (350 D1-a) — do not hand it to the first legal value.
 */
typedef struct mrs_msgs__msg__RoadmapEdge
{
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
  uint8_t traversal;
  double length_m;
  double width_m;
  /// 350 §3-2 V7: uint8 but NOT a vocabulary — no constants, value carried as-is.
  uint8_t capacity_robots;
  /// "" sentinel = no corridor reference (roadmap.schema.json:88, 350 §4-3).
  rosidl_runtime_c__String corridor;
} mrs_msgs__msg__RoadmapEdge;

// Struct for a sequence of mrs_msgs__msg__RoadmapEdge.
typedef struct mrs_msgs__msg__RoadmapEdge__Sequence
{
  mrs_msgs__msg__RoadmapEdge * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RoadmapEdge__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__STRUCT_H_
