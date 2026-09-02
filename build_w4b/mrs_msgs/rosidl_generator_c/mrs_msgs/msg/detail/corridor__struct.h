// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/corridor.h"


#ifndef MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_H_

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
// Member 'edges'
#include "mrs_msgs/msg/detail/edge_ref__struct.h"

/// Struct defined in msg/Corridor in the package mrs_msgs.
typedef struct mrs_msgs__msg__Corridor
{
  rosidl_runtime_c__String id;
  mrs_msgs__msg__EdgeRef__Sequence edges;
  double length_m;
} mrs_msgs__msg__Corridor;

// Struct for a sequence of mrs_msgs__msg__Corridor.
typedef struct mrs_msgs__msg__Corridor__Sequence
{
  mrs_msgs__msg__Corridor * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Corridor__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__CORRIDOR__STRUCT_H_
