// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/action/detail/replan__functions.h"
#include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `affected_robots`
// Member `blocked_hash`
#include "rosidl_runtime_c/string_functions.h"
// Member `blocked`
#include "mrs_msgs/msg/edge_ref.h"
// Member `blocked`
#include "mrs_msgs/msg/detail/edge_ref__rosidl_typesupport_introspection_c.h"
// Member `ordering_constraints`
#include "mrs_msgs/msg/visit_order.h"
// Member `ordering_constraints`
#include "mrs_msgs/msg/detail/visit_order__rosidl_typesupport_introspection_c.h"
// Member `baseline`
#include "mrs_msgs/msg/robot_baseline.h"
// Member `baseline`
#include "mrs_msgs/msg/detail/robot_baseline__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_Goal__init(message_memory);
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_Goal__fini(message_memory);
}

size_t mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__affected_robots(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__affected_robots(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__affected_robots(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__affected_robots(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__affected_robots(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__affected_robots(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__affected_robots(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__affected_robots(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__blocked(
  const void * untyped_member)
{
  const mrs_msgs__msg__EdgeRef__Sequence * member =
    (const mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__blocked(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__EdgeRef__Sequence * member =
    (const mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__blocked(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__EdgeRef__Sequence * member =
    (mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__blocked(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__EdgeRef * item =
    ((const mrs_msgs__msg__EdgeRef *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__blocked(untyped_member, index));
  mrs_msgs__msg__EdgeRef * value =
    (mrs_msgs__msg__EdgeRef *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__blocked(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__EdgeRef * item =
    ((mrs_msgs__msg__EdgeRef *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__blocked(untyped_member, index));
  const mrs_msgs__msg__EdgeRef * value =
    (const mrs_msgs__msg__EdgeRef *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__blocked(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__EdgeRef__Sequence * member =
    (mrs_msgs__msg__EdgeRef__Sequence *)(untyped_member);
  mrs_msgs__msg__EdgeRef__Sequence__fini(member);
  return mrs_msgs__msg__EdgeRef__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__ordering_constraints(
  const void * untyped_member)
{
  const mrs_msgs__msg__VisitOrder__Sequence * member =
    (const mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__ordering_constraints(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__VisitOrder__Sequence * member =
    (const mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__ordering_constraints(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__VisitOrder__Sequence * member =
    (mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__ordering_constraints(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__VisitOrder * item =
    ((const mrs_msgs__msg__VisitOrder *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__ordering_constraints(untyped_member, index));
  mrs_msgs__msg__VisitOrder * value =
    (mrs_msgs__msg__VisitOrder *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__ordering_constraints(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__VisitOrder * item =
    ((mrs_msgs__msg__VisitOrder *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__ordering_constraints(untyped_member, index));
  const mrs_msgs__msg__VisitOrder * value =
    (const mrs_msgs__msg__VisitOrder *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__ordering_constraints(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__VisitOrder__Sequence * member =
    (mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  mrs_msgs__msg__VisitOrder__Sequence__fini(member);
  return mrs_msgs__msg__VisitOrder__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__baseline(
  const void * untyped_member)
{
  const mrs_msgs__msg__RobotBaseline__Sequence * member =
    (const mrs_msgs__msg__RobotBaseline__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__baseline(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RobotBaseline__Sequence * member =
    (const mrs_msgs__msg__RobotBaseline__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__baseline(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RobotBaseline__Sequence * member =
    (mrs_msgs__msg__RobotBaseline__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__baseline(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RobotBaseline * item =
    ((const mrs_msgs__msg__RobotBaseline *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__baseline(untyped_member, index));
  mrs_msgs__msg__RobotBaseline * value =
    (mrs_msgs__msg__RobotBaseline *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__baseline(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RobotBaseline * item =
    ((mrs_msgs__msg__RobotBaseline *)
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__baseline(untyped_member, index));
  const mrs_msgs__msg__RobotBaseline * value =
    (const mrs_msgs__msg__RobotBaseline *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__baseline(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RobotBaseline__Sequence * member =
    (mrs_msgs__msg__RobotBaseline__Sequence *)(untyped_member);
  mrs_msgs__msg__RobotBaseline__Sequence__fini(member);
  return mrs_msgs__msg__RobotBaseline__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_member_array[10] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, schema),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "schema_version",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, schema_version),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "instance_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "reason",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, reason),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "affected_robots",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, affected_robots),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__affected_robots,  // size() function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__affected_robots,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__affected_robots,  // get(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__affected_robots,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__affected_robots,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__affected_robots  // resize(index) function pointer
  },
  {
    "blocked",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, blocked),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__blocked,  // size() function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__blocked,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__blocked,  // get(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__blocked,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__blocked,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__blocked  // resize(index) function pointer
  },
  {
    "blocked_hash",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, blocked_hash),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "ordering_constraints",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, ordering_constraints),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__ordering_constraints,  // size() function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__ordering_constraints,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__ordering_constraints,  // get(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__ordering_constraints,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__ordering_constraints,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__ordering_constraints  // resize(index) function pointer
  },
  {
    "baseline",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, baseline),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__size_function__Replan_Goal__baseline,  // size() function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_const_function__Replan_Goal__baseline,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__get_function__Replan_Goal__baseline,  // get(index) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__fetch_function__Replan_Goal__baseline,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__assign_function__Replan_Goal__baseline,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__resize_function__Replan_Goal__baseline  // resize(index) function pointer
  },
  {
    "plan_revision",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Goal, plan_revision),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_Goal",  // message name
  10,  // number of fields
  sizeof(mrs_msgs__action__Replan_Goal),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_member_array,  // message members
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Goal__get_type_hash,
  &mrs_msgs__action__Replan_Goal__get_type_description,
  &mrs_msgs__action__Replan_Goal__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Goal)() {
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, EdgeRef)();
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, VisitOrder)();
  mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_member_array[8].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RobotBaseline)();
  if (!mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_Goal__rosidl_typesupport_introspection_c__Replan_Goal_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `blocked_hash`
// Member `unreachable_robots`
// already included above
// #include "rosidl_runtime_c/string_functions.h"
// Member `components`
#include "mrs_msgs/msg/robot_group.h"
// Member `components`
#include "mrs_msgs/msg/detail/robot_group__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_Result__init(message_memory);
}

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_Result__fini(message_memory);
}

size_t mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__size_function__Replan_Result__unreachable_robots(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__unreachable_robots(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__unreachable_robots(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__fetch_function__Replan_Result__unreachable_robots(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__unreachable_robots(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__assign_function__Replan_Result__unreachable_robots(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__unreachable_robots(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__resize_function__Replan_Result__unreachable_robots(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__size_function__Replan_Result__components(
  const void * untyped_member)
{
  const mrs_msgs__msg__RobotGroup__Sequence * member =
    (const mrs_msgs__msg__RobotGroup__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__components(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RobotGroup__Sequence * member =
    (const mrs_msgs__msg__RobotGroup__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__components(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RobotGroup__Sequence * member =
    (mrs_msgs__msg__RobotGroup__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__fetch_function__Replan_Result__components(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RobotGroup * item =
    ((const mrs_msgs__msg__RobotGroup *)
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__components(untyped_member, index));
  mrs_msgs__msg__RobotGroup * value =
    (mrs_msgs__msg__RobotGroup *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__assign_function__Replan_Result__components(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RobotGroup * item =
    ((mrs_msgs__msg__RobotGroup *)
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__components(untyped_member, index));
  const mrs_msgs__msg__RobotGroup * value =
    (const mrs_msgs__msg__RobotGroup *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__resize_function__Replan_Result__components(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RobotGroup__Sequence * member =
    (mrs_msgs__msg__RobotGroup__Sequence *)(untyped_member);
  mrs_msgs__msg__RobotGroup__Sequence__fini(member);
  return mrs_msgs__msg__RobotGroup__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_member_array[8] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, schema),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "schema_version",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, schema_version),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "instance_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "outcome",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, outcome),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "plan_revision",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, plan_revision),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "blocked_hash",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, blocked_hash),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "unreachable_robots",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, unreachable_robots),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__size_function__Replan_Result__unreachable_robots,  // size() function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__unreachable_robots,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__unreachable_robots,  // get(index) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__fetch_function__Replan_Result__unreachable_robots,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__assign_function__Replan_Result__unreachable_robots,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__resize_function__Replan_Result__unreachable_robots  // resize(index) function pointer
  },
  {
    "components",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Result, components),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__size_function__Replan_Result__components,  // size() function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_const_function__Replan_Result__components,  // get_const(index) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__get_function__Replan_Result__components,  // get(index) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__fetch_function__Replan_Result__components,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__assign_function__Replan_Result__components,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__resize_function__Replan_Result__components  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_Result",  // message name
  8,  // number of fields
  sizeof(mrs_msgs__action__Replan_Result),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_member_array,  // message members
  mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Result__get_type_hash,
  &mrs_msgs__action__Replan_Result__get_type_description,
  &mrs_msgs__action__Replan_Result__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Result)() {
  mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RobotGroup)();
  if (!mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_Result__rosidl_typesupport_introspection_c__Replan_Result_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_Feedback__init(message_memory);
}

void mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_Feedback__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_member_array[1] = {
  {
    "structure_needs_at_least_one_member",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_Feedback, structure_needs_at_least_one_member),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_Feedback",  // message name
  1,  // number of fields
  sizeof(mrs_msgs__action__Replan_Feedback),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_member_array,  // message members
  mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Feedback__get_type_hash,
  &mrs_msgs__action__Replan_Feedback__get_type_description,
  &mrs_msgs__action__Replan_Feedback__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Feedback)() {
  if (!mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_Feedback__rosidl_typesupport_introspection_c__Replan_Feedback_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `goal_id`
#include "unique_identifier_msgs/msg/uuid.h"
// Member `goal_id`
#include "unique_identifier_msgs/msg/detail/uuid__rosidl_typesupport_introspection_c.h"
// Member `goal`
#include "mrs_msgs/action/replan.h"
// Member `goal`
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_SendGoal_Request__init(message_memory);
}

void mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_SendGoal_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_member_array[2] = {
  {
    "goal_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Request, goal_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "goal",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Request, goal),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_SendGoal_Request",  // message name
  2,  // number of fields
  sizeof(mrs_msgs__action__Replan_SendGoal_Request),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_member_array,  // message members
  mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Request)() {
  mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, unique_identifier_msgs, msg, UUID)();
  mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Goal)();
  if (!mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/time.h"
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_SendGoal_Response__init(message_memory);
}

void mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_SendGoal_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_member_array[2] = {
  {
    "accepted",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Response, accepted),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "stamp",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Response, stamp),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_SendGoal_Response",  // message name
  2,  // number of fields
  sizeof(mrs_msgs__action__Replan_SendGoal_Response),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_member_array,  // message members
  mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Response)() {
  mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Time)();
  if (!mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/replan.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_SendGoal_Event__init(message_memory);
}

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_SendGoal_Event__fini(message_memory);
}

size_t mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__size_function__Replan_SendGoal_Event__request(
  const void * untyped_member)
{
  const mrs_msgs__action__Replan_SendGoal_Request__Sequence * member =
    (const mrs_msgs__action__Replan_SendGoal_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__request(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__action__Replan_SendGoal_Request__Sequence * member =
    (const mrs_msgs__action__Replan_SendGoal_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__request(
  void * untyped_member, size_t index)
{
  mrs_msgs__action__Replan_SendGoal_Request__Sequence * member =
    (mrs_msgs__action__Replan_SendGoal_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_SendGoal_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__action__Replan_SendGoal_Request * item =
    ((const mrs_msgs__action__Replan_SendGoal_Request *)
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__request(untyped_member, index));
  mrs_msgs__action__Replan_SendGoal_Request * value =
    (mrs_msgs__action__Replan_SendGoal_Request *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__assign_function__Replan_SendGoal_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__action__Replan_SendGoal_Request * item =
    ((mrs_msgs__action__Replan_SendGoal_Request *)
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__request(untyped_member, index));
  const mrs_msgs__action__Replan_SendGoal_Request * value =
    (const mrs_msgs__action__Replan_SendGoal_Request *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__resize_function__Replan_SendGoal_Event__request(
  void * untyped_member, size_t size)
{
  mrs_msgs__action__Replan_SendGoal_Request__Sequence * member =
    (mrs_msgs__action__Replan_SendGoal_Request__Sequence *)(untyped_member);
  mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(member);
  return mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__size_function__Replan_SendGoal_Event__response(
  const void * untyped_member)
{
  const mrs_msgs__action__Replan_SendGoal_Response__Sequence * member =
    (const mrs_msgs__action__Replan_SendGoal_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__response(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__action__Replan_SendGoal_Response__Sequence * member =
    (const mrs_msgs__action__Replan_SendGoal_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__response(
  void * untyped_member, size_t index)
{
  mrs_msgs__action__Replan_SendGoal_Response__Sequence * member =
    (mrs_msgs__action__Replan_SendGoal_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_SendGoal_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__action__Replan_SendGoal_Response * item =
    ((const mrs_msgs__action__Replan_SendGoal_Response *)
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__response(untyped_member, index));
  mrs_msgs__action__Replan_SendGoal_Response * value =
    (mrs_msgs__action__Replan_SendGoal_Response *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__assign_function__Replan_SendGoal_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__action__Replan_SendGoal_Response * item =
    ((mrs_msgs__action__Replan_SendGoal_Response *)
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__response(untyped_member, index));
  const mrs_msgs__action__Replan_SendGoal_Response * value =
    (const mrs_msgs__action__Replan_SendGoal_Response *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__resize_function__Replan_SendGoal_Event__response(
  void * untyped_member, size_t size)
{
  mrs_msgs__action__Replan_SendGoal_Response__Sequence * member =
    (mrs_msgs__action__Replan_SendGoal_Response__Sequence *)(untyped_member);
  mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(member);
  return mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Event, request),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__size_function__Replan_SendGoal_Event__request,  // size() function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__request,  // get_const(index) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__request,  // get(index) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_SendGoal_Event__request,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__assign_function__Replan_SendGoal_Event__request,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__resize_function__Replan_SendGoal_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(mrs_msgs__action__Replan_SendGoal_Event, response),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__size_function__Replan_SendGoal_Event__response,  // size() function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_SendGoal_Event__response,  // get_const(index) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__get_function__Replan_SendGoal_Event__response,  // get(index) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_SendGoal_Event__response,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__assign_function__Replan_SendGoal_Event__response,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__resize_function__Replan_SendGoal_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_SendGoal_Event",  // message name
  3,  // number of fields
  sizeof(mrs_msgs__action__Replan_SendGoal_Event),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_member_array,  // message members
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Event)() {
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Request)();
  mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Response)();
  if (!mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_members = {
  "mrs_msgs__action",  // service namespace
  "Replan_SendGoal",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle,
  NULL,  // response message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle
  NULL  // event_message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle
};


static rosidl_service_type_support_t mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_type_support_handle = {
  0,
  &mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_members,
  get_service_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Request__rosidl_typesupport_introspection_c__Replan_SendGoal_Request_message_type_support_handle,
  &mrs_msgs__action__Replan_SendGoal_Response__rosidl_typesupport_introspection_c__Replan_SendGoal_Response_message_type_support_handle,
  &mrs_msgs__action__Replan_SendGoal_Event__rosidl_typesupport_introspection_c__Replan_SendGoal_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    mrs_msgs,
    action,
    Replan_SendGoal
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    mrs_msgs,
    action,
    Replan_SendGoal
  ),
  &mrs_msgs__action__Replan_SendGoal__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal__get_type_description,
  &mrs_msgs__action__Replan_SendGoal__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal)(void) {
  if (!mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_SendGoal_Event)()->data;
  }

  return &mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_SendGoal_service_type_support_handle;
}

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/uuid.h"
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_GetResult_Request__init(message_memory);
}

void mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_GetResult_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_member_array[1] = {
  {
    "goal_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Request, goal_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_GetResult_Request",  // message name
  1,  // number of fields
  sizeof(mrs_msgs__action__Replan_GetResult_Request),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_member_array,  // message members
  mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Request)() {
  mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, unique_identifier_msgs, msg, UUID)();
  if (!mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `result`
// already included above
// #include "mrs_msgs/action/replan.h"
// Member `result`
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_GetResult_Response__init(message_memory);
}

void mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_GetResult_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_member_array[2] = {
  {
    "status",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Response, status),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "result",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Response, result),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_GetResult_Response",  // message name
  2,  // number of fields
  sizeof(mrs_msgs__action__Replan_GetResult_Response),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_member_array,  // message members
  mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Response)() {
  mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Result)();
  if (!mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `info`
// already included above
// #include "service_msgs/msg/service_event_info.h"
// Member `info`
// already included above
// #include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/replan.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_GetResult_Event__init(message_memory);
}

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_GetResult_Event__fini(message_memory);
}

size_t mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__size_function__Replan_GetResult_Event__request(
  const void * untyped_member)
{
  const mrs_msgs__action__Replan_GetResult_Request__Sequence * member =
    (const mrs_msgs__action__Replan_GetResult_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__request(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__action__Replan_GetResult_Request__Sequence * member =
    (const mrs_msgs__action__Replan_GetResult_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__request(
  void * untyped_member, size_t index)
{
  mrs_msgs__action__Replan_GetResult_Request__Sequence * member =
    (mrs_msgs__action__Replan_GetResult_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_GetResult_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__action__Replan_GetResult_Request * item =
    ((const mrs_msgs__action__Replan_GetResult_Request *)
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__request(untyped_member, index));
  mrs_msgs__action__Replan_GetResult_Request * value =
    (mrs_msgs__action__Replan_GetResult_Request *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__assign_function__Replan_GetResult_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__action__Replan_GetResult_Request * item =
    ((mrs_msgs__action__Replan_GetResult_Request *)
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__request(untyped_member, index));
  const mrs_msgs__action__Replan_GetResult_Request * value =
    (const mrs_msgs__action__Replan_GetResult_Request *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__resize_function__Replan_GetResult_Event__request(
  void * untyped_member, size_t size)
{
  mrs_msgs__action__Replan_GetResult_Request__Sequence * member =
    (mrs_msgs__action__Replan_GetResult_Request__Sequence *)(untyped_member);
  mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(member);
  return mrs_msgs__action__Replan_GetResult_Request__Sequence__init(member, size);
}

size_t mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__size_function__Replan_GetResult_Event__response(
  const void * untyped_member)
{
  const mrs_msgs__action__Replan_GetResult_Response__Sequence * member =
    (const mrs_msgs__action__Replan_GetResult_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__response(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__action__Replan_GetResult_Response__Sequence * member =
    (const mrs_msgs__action__Replan_GetResult_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__response(
  void * untyped_member, size_t index)
{
  mrs_msgs__action__Replan_GetResult_Response__Sequence * member =
    (mrs_msgs__action__Replan_GetResult_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_GetResult_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__action__Replan_GetResult_Response * item =
    ((const mrs_msgs__action__Replan_GetResult_Response *)
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__response(untyped_member, index));
  mrs_msgs__action__Replan_GetResult_Response * value =
    (mrs_msgs__action__Replan_GetResult_Response *)(untyped_value);
  *value = *item;
}

void mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__assign_function__Replan_GetResult_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__action__Replan_GetResult_Response * item =
    ((mrs_msgs__action__Replan_GetResult_Response *)
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__response(untyped_member, index));
  const mrs_msgs__action__Replan_GetResult_Response * value =
    (const mrs_msgs__action__Replan_GetResult_Response *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__resize_function__Replan_GetResult_Event__response(
  void * untyped_member, size_t size)
{
  mrs_msgs__action__Replan_GetResult_Response__Sequence * member =
    (mrs_msgs__action__Replan_GetResult_Response__Sequence *)(untyped_member);
  mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(member);
  return mrs_msgs__action__Replan_GetResult_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Event, request),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__size_function__Replan_GetResult_Event__request,  // size() function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__request,  // get_const(index) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__request,  // get(index) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_GetResult_Event__request,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__assign_function__Replan_GetResult_Event__request,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__resize_function__Replan_GetResult_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(mrs_msgs__action__Replan_GetResult_Event, response),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__size_function__Replan_GetResult_Event__response,  // size() function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_const_function__Replan_GetResult_Event__response,  // get_const(index) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__get_function__Replan_GetResult_Event__response,  // get(index) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__fetch_function__Replan_GetResult_Event__response,  // fetch(index, &value) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__assign_function__Replan_GetResult_Event__response,  // assign(index, value) function pointer
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__resize_function__Replan_GetResult_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_GetResult_Event",  // message name
  3,  // number of fields
  sizeof(mrs_msgs__action__Replan_GetResult_Event),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_member_array,  // message members
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Event)() {
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Request)();
  mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Response)();
  if (!mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_members = {
  "mrs_msgs__action",  // service namespace
  "Replan_GetResult",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle,
  NULL,  // response message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle
  NULL  // event_message
  // mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle
};


static rosidl_service_type_support_t mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_type_support_handle = {
  0,
  &mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_members,
  get_service_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Request__rosidl_typesupport_introspection_c__Replan_GetResult_Request_message_type_support_handle,
  &mrs_msgs__action__Replan_GetResult_Response__rosidl_typesupport_introspection_c__Replan_GetResult_Response_message_type_support_handle,
  &mrs_msgs__action__Replan_GetResult_Event__rosidl_typesupport_introspection_c__Replan_GetResult_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    mrs_msgs,
    action,
    Replan_GetResult
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    mrs_msgs,
    action,
    Replan_GetResult
  ),
  &mrs_msgs__action__Replan_GetResult__get_type_hash,
  &mrs_msgs__action__Replan_GetResult__get_type_description,
  &mrs_msgs__action__Replan_GetResult__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult)(void) {
  if (!mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_GetResult_Event)()->data;
  }

  return &mrs_msgs__action__detail__replan__rosidl_typesupport_introspection_c__Replan_GetResult_service_type_support_handle;
}

// already included above
// #include <stddef.h>
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.h"


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/uuid.h"
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__rosidl_typesupport_introspection_c.h"
// Member `feedback`
// already included above
// #include "mrs_msgs/action/replan.h"
// Member `feedback`
// already included above
// #include "mrs_msgs/action/detail/replan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__action__Replan_FeedbackMessage__init(message_memory);
}

void mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_fini_function(void * message_memory)
{
  mrs_msgs__action__Replan_FeedbackMessage__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_member_array[2] = {
  {
    "goal_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_FeedbackMessage, goal_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "feedback",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__action__Replan_FeedbackMessage, feedback),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_members = {
  "mrs_msgs__action",  // message namespace
  "Replan_FeedbackMessage",  // message name
  2,  // number of fields
  sizeof(mrs_msgs__action__Replan_FeedbackMessage),
  false,  // has_any_key_member_
  mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_member_array,  // message members
  mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_type_support_handle = {
  0,
  &mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_hash,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_description,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_FeedbackMessage)() {
  mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, unique_identifier_msgs, msg, UUID)();
  mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, action, Replan_Feedback)();
  if (!mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__action__Replan_FeedbackMessage__rosidl_typesupport_introspection_c__Replan_FeedbackMessage_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
