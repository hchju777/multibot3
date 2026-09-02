// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/VisitItem.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/visit_item.h"


#ifndef MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'robot'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/VisitItem in the package mrs_msgs.
typedef struct mrs_msgs__msg__VisitItem
{
  rosidl_runtime_c__String robot;
  uint32_t index;
} mrs_msgs__msg__VisitItem;

// Struct for a sequence of mrs_msgs__msg__VisitItem.
typedef struct mrs_msgs__msg__VisitItem__Sequence
{
  mrs_msgs__msg__VisitItem * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__VisitItem__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__VISIT_ITEM__STRUCT_H_
