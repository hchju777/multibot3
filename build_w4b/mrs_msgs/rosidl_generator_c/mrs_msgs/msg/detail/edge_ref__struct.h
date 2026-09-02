// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/EdgeRef.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/edge_ref.h"


#ifndef MRS_MSGS__MSG__DETAIL__EDGE_REF__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__EDGE_REF__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'from_id'
// Member 'to_id'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/EdgeRef in the package mrs_msgs.
/**
  * Directed pair of roadmap-node identifiers reused across three JSON shapes
  * (350 §6 "재사용 결정 둘"): roadmap.corridors[].edges (2-element tuple),
  * stop_declaration.blocked_edge (object {from,to}), replan_request.blocked[]
  * (object {from,to}). The io/ adapter absorbs both JSON shapes into this one
  * type (350 §8 D6 item 8) — that adapter code is outside this round's scope.
 */
typedef struct mrs_msgs__msg__EdgeRef
{
  rosidl_runtime_c__String from_id;
  rosidl_runtime_c__String to_id;
} mrs_msgs__msg__EdgeRef;

// Struct for a sequence of mrs_msgs__msg__EdgeRef.
typedef struct mrs_msgs__msg__EdgeRef__Sequence
{
  mrs_msgs__msg__EdgeRef * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__EdgeRef__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__EDGE_REF__STRUCT_H_
