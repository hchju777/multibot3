// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declaration.h"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_H_

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
// Member 'blocked_edge'
#include "mrs_msgs/msg/detail/edge_ref__struct.h"

/// Struct defined in msg/StopDeclaration in the package mrs_msgs.
/**
  * reason uses the SHARED StopReason vocabulary (mrs_msgs/msg/StopReason.msg,
  * 350 §3-2 V1). This channel's legal subset is {1,3,5} — NON-contiguous.
  * 🔴 Do not validate with a range check (350 §3-2, §4). Validate by set
  * membership against the {1,3,5} subset.
 */
typedef struct mrs_msgs__msg__StopDeclaration
{
  uint64_t seq;
  rosidl_runtime_c__String robot;
  mrs_msgs__msg__EdgeRef blocked_edge;
  bool declared;
  uint8_t reason;
} mrs_msgs__msg__StopDeclaration;

// Struct for a sequence of mrs_msgs__msg__StopDeclaration.
typedef struct mrs_msgs__msg__StopDeclaration__Sequence
{
  mrs_msgs__msg__StopDeclaration * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__StopDeclaration__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__STRUCT_H_
