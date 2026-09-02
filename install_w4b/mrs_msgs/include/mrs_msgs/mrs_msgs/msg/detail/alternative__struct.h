// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/Alternative.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/alternative.h"


#ifndef MRS_MSGS__MSG__DETAIL__ALTERNATIVE__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ALTERNATIVE__STRUCT_H_

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
// Member 'dependencies'
#include "mrs_msgs/msg/detail/dependency__struct.h"

/// Struct defined in msg/Alternative in the package mrs_msgs.
typedef struct mrs_msgs__msg__Alternative
{
  rosidl_runtime_c__String id;
  mrs_msgs__msg__Dependency__Sequence dependencies;
} mrs_msgs__msg__Alternative;

// Struct for a sequence of mrs_msgs__msg__Alternative.
typedef struct mrs_msgs__msg__Alternative__Sequence
{
  mrs_msgs__msg__Alternative * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__Alternative__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ALTERNATIVE__STRUCT_H_
