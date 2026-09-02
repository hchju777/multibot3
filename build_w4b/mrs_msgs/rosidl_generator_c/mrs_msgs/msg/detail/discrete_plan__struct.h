// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/discrete_plan.h"


#ifndef MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__STRUCT_H_

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
// Member 'plans'
#include "mrs_msgs/msg/detail/robot_plan__struct.h"
// Member 'visit_order'
#include "mrs_msgs/msg/detail/visit_order__struct.h"

/// Struct defined in msg/DiscretePlan in the package mrs_msgs.
typedef struct mrs_msgs__msg__DiscretePlan
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint64_t plan_revision;
  mrs_msgs__msg__RobotPlan__Sequence plans;
  mrs_msgs__msg__VisitOrder__Sequence visit_order;
} mrs_msgs__msg__DiscretePlan;

// Struct for a sequence of mrs_msgs__msg__DiscretePlan.
typedef struct mrs_msgs__msg__DiscretePlan__Sequence
{
  mrs_msgs__msg__DiscretePlan * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__DiscretePlan__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__STRUCT_H_
