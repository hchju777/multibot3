// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mrs_msgs:msg/TaskGoal.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/task_goal.h"


#ifndef MRS_MSGS__MSG__DETAIL__TASK_GOAL__STRUCT_H_
#define MRS_MSGS__MSG__DETAIL__TASK_GOAL__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// Member 'location'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/TaskGoal in the package mrs_msgs.
/**
  * 350 §7 개정 안건 ① — task_release.schema.json:31의 "Goal (공유)"는 성립하지
  * 않는다(assignment.Goal과 필드 집합이 다르다). 이 판은 개정을 적용하지 않고
  * TaskGoal로 렌더링만 한다(350 D6, 오늘의 임시 처분). 스키마 텍스트는 미개정.
 */
typedef struct mrs_msgs__msg__TaskGoal
{
  rosidl_runtime_c__String goal_id;
  rosidl_runtime_c__String location;
} mrs_msgs__msg__TaskGoal;

// Struct for a sequence of mrs_msgs__msg__TaskGoal.
typedef struct mrs_msgs__msg__TaskGoal__Sequence
{
  mrs_msgs__msg__TaskGoal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mrs_msgs__msg__TaskGoal__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__TASK_GOAL__STRUCT_H_
