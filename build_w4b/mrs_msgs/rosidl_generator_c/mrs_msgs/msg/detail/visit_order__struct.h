// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/VisitOrder.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/visit_order.h"


#ifndef MRS_MSGS__MSG__DETAIL__VISIT_ORDER__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__VISIT_ORDER__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'location'
#include "rosidl_runtime_c/string.h"
// Member 'sequence'
#include "mrs_msgs/msg/detail/visit_item__struct.h"

/// Struct defined in msg/VisitOrder in the package mrs_msgs.
/**
  * Reused in two places with the SAME field set but DIFFERENT values (350 §6
  * "재사용 결정 둘" / §8 D6 item 7): discrete_plan.visit_order[] (full order)
  * and replan_request.ordering_constraints[] (realized prefix of the former).
  * Do not write code that assumes "same type => same value".
 */
typedef struct mrs_msgs__msg__VisitOrder
{
  rosidl_runtime_c__String location;
  mrs_msgs__msg__VisitItem__Sequence sequence;
} mrs_msgs__msg__VisitOrder;

// Struct for a sequence of mrs_msgs__msg__VisitOrder.
typedef struct mrs_msgs__msg__VisitOrder__Sequence
{
  mrs_msgs__msg__VisitOrder * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__VisitOrder__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__VISIT_ORDER__STRUCT_H_
