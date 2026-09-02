// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/action/replan.h"


#ifndef MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_H_
#define MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_H_

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
// Member 'affected_robots'
// Member 'blocked_hash'
#include "rosidl_runtime_c/string.h"
// Member 'blocked'
#include "mrs_msgs/msg/detail/edge_ref__struct.h"
// Member 'ordering_constraints'
#include "mrs_msgs/msg/detail/visit_order__struct.h"
// Member 'baseline'
#include "mrs_msgs/msg/detail/robot_baseline__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_Goal
{
  /// --- goal: mrs.replan_request 7.0.0 ---
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  /// ReplanReason == shared StopReason vocabulary (mrs_msgs/msg/StopReason.msg).
  uint8_t reason;
  rosidl_runtime_c__String__Sequence affected_robots;
  mrs_msgs__msg__EdgeRef__Sequence blocked;
  rosidl_runtime_c__String blocked_hash;
  mrs_msgs__msg__VisitOrder__Sequence ordering_constraints;
  mrs_msgs__msg__RobotBaseline__Sequence baseline;
  uint64_t plan_revision;
} mrs_msgs__action__Replan_Goal;

// Struct for a sequence of mrs_msgs__action__Replan_Goal.
typedef struct mrs_msgs__action__Replan_Goal__Sequence
{
  mrs_msgs__action__Replan_Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_Goal__Sequence;

// Constants defined in the message

/// Constant 'OUTCOME_UNKNOWN'.
enum
{
  mrs_msgs__action__Replan_Result__OUTCOME_UNKNOWN = 0
};

/// Constant 'OUTCOME_PLANNED'.
enum
{
  mrs_msgs__action__Replan_Result__OUTCOME_PLANNED = 1
};

/// Constant 'OUTCOME_FALLBACK'.
enum
{
  mrs_msgs__action__Replan_Result__OUTCOME_FALLBACK = 2
};

/// Constant 'OUTCOME_PREEMPTED'.
enum
{
  mrs_msgs__action__Replan_Result__OUTCOME_PREEMPTED = 3
};

/// Constant 'OUTCOME_REJECTED'.
enum
{
  mrs_msgs__action__Replan_Result__OUTCOME_REJECTED = 4
};

// Include directives for member types
// Member 'schema'
// Member 'schema_version'
// Member 'instance_id'
// Member 'blocked_hash'
// Member 'unreachable_robots'
// already included above
// #include "rosidl_runtime_c/string.h"
// Member 'components'
#include "mrs_msgs/msg/detail/robot_group__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_Result
{
  rosidl_runtime_c__String schema;
  rosidl_runtime_c__String schema_version;
  rosidl_runtime_c__String instance_id;
  uint8_t outcome;
  uint64_t plan_revision;
  rosidl_runtime_c__String blocked_hash;
  rosidl_runtime_c__String__Sequence unreachable_robots;
  mrs_msgs__msg__RobotGroup__Sequence components;
} mrs_msgs__action__Replan_Result;

// Struct for a sequence of mrs_msgs__action__Replan_Result.
typedef struct mrs_msgs__action__Replan_Result__Sequence
{
  mrs_msgs__action__Replan_Result * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_Result__Sequence;

// Constants defined in the message

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_Feedback
{
  uint8_t structure_needs_at_least_one_member;
} mrs_msgs__action__Replan_Feedback;

// Struct for a sequence of mrs_msgs__action__Replan_Feedback.
typedef struct mrs_msgs__action__Replan_Feedback__Sequence
{
  mrs_msgs__action__Replan_Feedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_Feedback__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'goal'
#include "mrs_msgs/action/detail/replan__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_SendGoal_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
  mrs_msgs__action__Replan_Goal goal;
} mrs_msgs__action__Replan_SendGoal_Request;

// Struct for a sequence of mrs_msgs__action__Replan_SendGoal_Request.
typedef struct mrs_msgs__action__Replan_SendGoal_Request__Sequence
{
  mrs_msgs__action__Replan_SendGoal_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_SendGoal_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_SendGoal_Response
{
  bool accepted;
  builtin_interfaces__msg__Time stamp;
} mrs_msgs__action__Replan_SendGoal_Response;

// Struct for a sequence of mrs_msgs__action__Replan_SendGoal_Response.
typedef struct mrs_msgs__action__Replan_SendGoal_Response__Sequence
{
  mrs_msgs__action__Replan_SendGoal_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_SendGoal_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  mrs_msgs__action__Replan_SendGoal_Event__request__MAX_SIZE = 1
};
// response
enum
{
  mrs_msgs__action__Replan_SendGoal_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_SendGoal_Event
{
  service_msgs__msg__ServiceEventInfo info;
  mrs_msgs__action__Replan_SendGoal_Request__Sequence request;
  mrs_msgs__action__Replan_SendGoal_Response__Sequence response;
} mrs_msgs__action__Replan_SendGoal_Event;

// Struct for a sequence of mrs_msgs__action__Replan_SendGoal_Event.
typedef struct mrs_msgs__action__Replan_SendGoal_Event__Sequence
{
  mrs_msgs__action__Replan_SendGoal_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_SendGoal_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_GetResult_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
} mrs_msgs__action__Replan_GetResult_Request;

// Struct for a sequence of mrs_msgs__action__Replan_GetResult_Request.
typedef struct mrs_msgs__action__Replan_GetResult_Request__Sequence
{
  mrs_msgs__action__Replan_GetResult_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_GetResult_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'result'
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_GetResult_Response
{
  int8_t status;
  mrs_msgs__action__Replan_Result result;
} mrs_msgs__action__Replan_GetResult_Response;

// Struct for a sequence of mrs_msgs__action__Replan_GetResult_Response.
typedef struct mrs_msgs__action__Replan_GetResult_Response__Sequence
{
  mrs_msgs__action__Replan_GetResult_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_GetResult_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  mrs_msgs__action__Replan_GetResult_Event__request__MAX_SIZE = 1
};
// response
enum
{
  mrs_msgs__action__Replan_GetResult_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_GetResult_Event
{
  service_msgs__msg__ServiceEventInfo info;
  mrs_msgs__action__Replan_GetResult_Request__Sequence request;
  mrs_msgs__action__Replan_GetResult_Response__Sequence response;
} mrs_msgs__action__Replan_GetResult_Event;

// Struct for a sequence of mrs_msgs__action__Replan_GetResult_Event.
typedef struct mrs_msgs__action__Replan_GetResult_Event__Sequence
{
  mrs_msgs__action__Replan_GetResult_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_GetResult_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'feedback'
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"

/// Struct defined in action/Replan in the package mrs_msgs.
typedef struct mrs_msgs__action__Replan_FeedbackMessage
{
  unique_identifier_msgs__msg__UUID goal_id;
  mrs_msgs__action__Replan_Feedback feedback;
} mrs_msgs__action__Replan_FeedbackMessage;

// Struct for a sequence of mrs_msgs__action__Replan_FeedbackMessage.
typedef struct mrs_msgs__action__Replan_FeedbackMessage__Sequence
{
  mrs_msgs__action__Replan_FeedbackMessage * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__action__Replan_FeedbackMessage__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_H_
