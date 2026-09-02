// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_provenance.h"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'capacity_rule_id'
// Member 'generator_version'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RoadmapProvenance in the package mrs_msgs.
typedef struct mrs_msgs__msg__RoadmapProvenance
{
  double min_separation_m;
  double wall_inflation_m;
  /// 350 §3-2 V8 (트랩): enum in JSON but x-ros-type is "string" — do NOT render
  /// as uint8. No consuming module branches on this string (audit-only).
  rosidl_runtime_c__String capacity_rule_id;
  rosidl_runtime_c__String generator_version;
} mrs_msgs__msg__RoadmapProvenance;

// Struct for a sequence of mrs_msgs__msg__RoadmapProvenance.
typedef struct mrs_msgs__msg__RoadmapProvenance__Sequence
{
  mrs_msgs__msg__RoadmapProvenance * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__RoadmapProvenance__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_PROVENANCE__STRUCT_H_
