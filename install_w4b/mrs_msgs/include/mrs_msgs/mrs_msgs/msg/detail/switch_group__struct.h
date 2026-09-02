// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/SwitchGroup.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/switch_group.h"


#ifndef MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__STRUCT_H_

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
// Member 'location'
#include "rosidl_runtime_c/string.h"
// Member 'alternatives'
#include "mrs_msgs/msg/detail/alternative__struct.h"

/// Struct defined in msg/SwitchGroup in the package mrs_msgs.
typedef struct mrs_msgs__msg__SwitchGroup
{
  rosidl_runtime_c__String id;
  rosidl_runtime_c__String location;
  mrs_msgs__msg__Alternative__Sequence alternatives;
} mrs_msgs__msg__SwitchGroup;

// Struct for a sequence of mrs_msgs__msg__SwitchGroup.
typedef struct mrs_msgs__msg__SwitchGroup__Sequence
{
  mrs_msgs__msg__SwitchGroup * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__SwitchGroup__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__SWITCH_GROUP__STRUCT_H_
