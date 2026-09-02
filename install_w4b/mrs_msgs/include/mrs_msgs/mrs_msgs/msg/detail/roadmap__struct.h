// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_H_

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
// Member 'endpoints'
#include "rosidl_runtime_c/string.h"
// Member 'nodes'
#include "mrs_msgs/msg/detail/roadmap_node__struct.h"
// Member 'edges'
#include "mrs_msgs/msg/detail/roadmap_edge__struct.h"
// Member 'corridors'
#include "mrs_msgs/msg/detail/corridor__struct.h"
// Member 'provenance'
#include "mrs_msgs/msg/detail/roadmap_provenance__struct.h"

/// Struct defined in msg/Roadmap in the package mrs_msgs.
typedef struct mrs_msgs__msg__Roadmap
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  mrs_msgs__msg__RoadmapNode__Sequence nodes;
  mrs_msgs__msg__RoadmapEdge__Sequence edges;
  rosidl_runtime_c__String__Sequence endpoints;
  mrs_msgs__msg__Corridor__Sequence corridors;
  mrs_msgs__msg__RoadmapProvenance provenance;
} mrs_msgs__msg__Roadmap;

// Struct for a sequence of mrs_msgs__msg__Roadmap.
typedef struct mrs_msgs__msg__Roadmap__Sequence
{
  mrs_msgs__msg__Roadmap * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Roadmap__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP__STRUCT_H_
