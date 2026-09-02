// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RoadmapNode.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_node.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RoadmapNode in the package mrs_msgs.
typedef struct mrs_msgs__msg__RoadmapNode
{
  rosidl_runtime_c__String id;
  double x;
  double y;
  double clearance_m;
} mrs_msgs__msg__RoadmapNode;

// Struct for a sequence of mrs_msgs__msg__RoadmapNode.
typedef struct mrs_msgs__msg__RoadmapNode__Sequence
{
  mrs_msgs__msg__RoadmapNode * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RoadmapNode__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_NODE__STRUCT_H_
