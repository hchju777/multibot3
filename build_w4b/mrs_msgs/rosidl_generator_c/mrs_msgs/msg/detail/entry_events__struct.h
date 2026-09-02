// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/EntryEvents.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/entry_events.h"


#ifndef MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_H_

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
#include "rosidl_runtime_c/string.h"
// Member 'events'
#include "mrs_msgs/msg/detail/entry_event__struct.h"

/// Struct defined in msg/EntryEvents in the package mrs_msgs.
typedef struct mrs_msgs__msg__EntryEvents
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  mrs_msgs__msg__EntryEvent__Sequence events;
} mrs_msgs__msg__EntryEvents;

// Struct for a sequence of mrs_msgs__msg__EntryEvents.
typedef struct mrs_msgs__msg__EntryEvents__Sequence
{
  mrs_msgs__msg__EntryEvents * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__EntryEvents__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__ENTRY_EVENTS__STRUCT_H_
