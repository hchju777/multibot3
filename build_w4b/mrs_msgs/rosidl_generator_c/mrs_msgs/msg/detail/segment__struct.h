// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Segment.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/segment.h"


#ifndef MRS_MSGS__MSG__DETAIL__SEGMENT__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__SEGMENT__STRUCT_H_

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
// Member 'robot'
// Member 'from_id'
// Member 'to_id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/Segment in the package mrs_msgs.
typedef struct mrs_msgs__msg__Segment
{
  rosidl_runtime_c__String id;
  rosidl_runtime_c__String robot;
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
} mrs_msgs__msg__Segment;

// Struct for a sequence of mrs_msgs__msg__Segment.
typedef struct mrs_msgs__msg__Segment__Sequence
{
  mrs_msgs__msg__Segment * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Segment__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__SEGMENT__STRUCT_H_
