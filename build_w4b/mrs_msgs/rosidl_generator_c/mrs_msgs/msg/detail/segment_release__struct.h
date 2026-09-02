// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/SegmentRelease.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment_release.h"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__STRUCT_H_

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
// Member 'robot'
#include "rosidl_runtime_c/string.h"
// Member 'segments'
#include "mrs_msgs/msg/detail/segment_release_item__struct.h"

/// Struct defined in msg/SegmentRelease in the package mrs_msgs.
typedef struct mrs_msgs__msg__SegmentRelease
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  rosidl_runtime_c__String robot;
  uint64_t release_seq;
  uint64_t commit_seq;
  uint64_t entry_seq_ack;
  mrs_msgs__msg__SegmentReleaseItem__Sequence segments;
} mrs_msgs__msg__SegmentRelease;

// Struct for a sequence of mrs_msgs__msg__SegmentRelease.
typedef struct mrs_msgs__msg__SegmentRelease__Sequence
{
  mrs_msgs__msg__SegmentRelease * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__SegmentRelease__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT_RELEASE__STRUCT_H_
