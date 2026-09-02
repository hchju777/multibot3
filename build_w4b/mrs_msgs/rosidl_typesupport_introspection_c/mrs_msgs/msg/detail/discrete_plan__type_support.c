// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/discrete_plan__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/discrete_plan__functions.h"
#include "mrs_msgs/msg/detail/discrete_plan__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `plans`
#include "mrs_msgs/msg/robot_plan.h"
// Member `plans`
#include "mrs_msgs/msg/detail/robot_plan__rosidl_typesupport_introspection_c.h"
// Member `visit_order`
#include "mrs_msgs/msg/visit_order.h"
// Member `visit_order`
#include "mrs_msgs/msg/detail/visit_order__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__DiscretePlan__init(message_memory);
}

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_fini_function(void * message_memory)
{
  mrs_msgs__msg__DiscretePlan__fini(message_memory);
}

size_t mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__size_function__DiscretePlan__plans(
  const void * untyped_member)
{
  const mrs_msgs__msg__RobotPlan__Sequence * member =
    (const mrs_msgs__msg__RobotPlan__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__plans(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RobotPlan__Sequence * member =
    (const mrs_msgs__msg__RobotPlan__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__plans(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RobotPlan__Sequence * member =
    (mrs_msgs__msg__RobotPlan__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__fetch_function__DiscretePlan__plans(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RobotPlan * item =
    ((const mrs_msgs__msg__RobotPlan *)
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__plans(untyped_member, index));
  mrs_msgs__msg__RobotPlan * value =
    (mrs_msgs__msg__RobotPlan *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__assign_function__DiscretePlan__plans(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RobotPlan * item =
    ((mrs_msgs__msg__RobotPlan *)
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__plans(untyped_member, index));
  const mrs_msgs__msg__RobotPlan * value =
    (const mrs_msgs__msg__RobotPlan *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__resize_function__DiscretePlan__plans(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RobotPlan__Sequence * member =
    (mrs_msgs__msg__RobotPlan__Sequence *)(untyped_member);
  mrs_msgs__msg__RobotPlan__Sequence__fini(member);
  return mrs_msgs__msg__RobotPlan__Sequence__init(member, size);
}

size_t mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__size_function__DiscretePlan__visit_order(
  const void * untyped_member)
{
  const mrs_msgs__msg__VisitOrder__Sequence * member =
    (const mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__visit_order(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__VisitOrder__Sequence * member =
    (const mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__visit_order(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__VisitOrder__Sequence * member =
    (mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__fetch_function__DiscretePlan__visit_order(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__VisitOrder * item =
    ((const mrs_msgs__msg__VisitOrder *)
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__visit_order(untyped_member, index));
  mrs_msgs__msg__VisitOrder * value =
    (mrs_msgs__msg__VisitOrder *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__assign_function__DiscretePlan__visit_order(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__VisitOrder * item =
    ((mrs_msgs__msg__VisitOrder *)
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__visit_order(untyped_member, index));
  const mrs_msgs__msg__VisitOrder * value =
    (const mrs_msgs__msg__VisitOrder *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__resize_function__DiscretePlan__visit_order(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__VisitOrder__Sequence * member =
    (mrs_msgs__msg__VisitOrder__Sequence *)(untyped_member);
  mrs_msgs__msg__VisitOrder__Sequence__fini(member);
  return mrs_msgs__msg__VisitOrder__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_member_array[6] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__DiscretePlan, schema),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__DiscretePlan, schema_version),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__DiscretePlan, instance_id),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__DiscretePlan, plan_revision),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "plans",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__DiscretePlan, plans),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__size_function__DiscretePlan__plans,  // size() function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__plans,  // get_const(index) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__plans,  // get(index) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__fetch_function__DiscretePlan__plans,  // fetch(index, &value) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__assign_function__DiscretePlan__plans,  // assign(index, value) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__resize_function__DiscretePlan__plans  // resize(index) function pointer
  },
  {
    "visit_order",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__DiscretePlan, visit_order),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__size_function__DiscretePlan__visit_order,  // size() function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_const_function__DiscretePlan__visit_order,  // get_const(index) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__get_function__DiscretePlan__visit_order,  // get(index) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__fetch_function__DiscretePlan__visit_order,  // fetch(index, &value) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__assign_function__DiscretePlan__visit_order,  // assign(index, value) function pointer
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__resize_function__DiscretePlan__visit_order  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_members = {
  "mrs_msgs__msg",  // message namespace
  "DiscretePlan",  // message name
  6,  // number of fields
  sizeof(mrs_msgs__msg__DiscretePlan),
  false,  // has_any_key_member_
  mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_member_array,  // message members
  mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_type_support_handle = {
  0,
  &mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__DiscretePlan__get_type_hash,
  &mrs_msgs__msg__DiscretePlan__get_type_description,
  &mrs_msgs__msg__DiscretePlan__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, DiscretePlan)() {
  mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RobotPlan)();
  mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, VisitOrder)();
  if (!mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__DiscretePlan__rosidl_typesupport_introspection_c__DiscretePlan_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
