// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/Assignment.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/assignment__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/assignment__functions.h"
#include "mrs_msgs/msg/detail/assignment__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `unassigned_tasks`
#include "rosidl_runtime_c/string_functions.h"
// Member `assignments`
#include "mrs_msgs/msg/robot_assignment.h"
// Member `assignments`
#include "mrs_msgs/msg/detail/robot_assignment__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__Assignment__init(message_memory);
}

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_fini_function(void * message_memory)
{
  mrs_msgs__msg__Assignment__fini(message_memory);
}

size_t mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__size_function__Assignment__assignments(
  const void * untyped_member)
{
  const mrs_msgs__msg__RobotAssignment__Sequence * member =
    (const mrs_msgs__msg__RobotAssignment__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__assignments(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RobotAssignment__Sequence * member =
    (const mrs_msgs__msg__RobotAssignment__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__assignments(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RobotAssignment__Sequence * member =
    (mrs_msgs__msg__RobotAssignment__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__fetch_function__Assignment__assignments(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RobotAssignment * item =
    ((const mrs_msgs__msg__RobotAssignment *)
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__assignments(untyped_member, index));
  mrs_msgs__msg__RobotAssignment * value =
    (mrs_msgs__msg__RobotAssignment *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__assign_function__Assignment__assignments(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RobotAssignment * item =
    ((mrs_msgs__msg__RobotAssignment *)
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__assignments(untyped_member, index));
  const mrs_msgs__msg__RobotAssignment * value =
    (const mrs_msgs__msg__RobotAssignment *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__resize_function__Assignment__assignments(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RobotAssignment__Sequence * member =
    (mrs_msgs__msg__RobotAssignment__Sequence *)(untyped_member);
  mrs_msgs__msg__RobotAssignment__Sequence__fini(member);
  return mrs_msgs__msg__RobotAssignment__Sequence__init(member, size);
}

size_t mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__size_function__Assignment__unassigned_tasks(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__unassigned_tasks(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__unassigned_tasks(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__fetch_function__Assignment__unassigned_tasks(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__unassigned_tasks(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__assign_function__Assignment__unassigned_tasks(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__unassigned_tasks(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__resize_function__Assignment__unassigned_tasks(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_member_array[6] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Assignment, schema),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Assignment, schema_version),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Assignment, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "revision",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Assignment, revision),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "assignments",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Assignment, assignments),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__size_function__Assignment__assignments,  // size() function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__assignments,  // get_const(index) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__assignments,  // get(index) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__fetch_function__Assignment__assignments,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__assign_function__Assignment__assignments,  // assign(index, value) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__resize_function__Assignment__assignments  // resize(index) function pointer
  },
  {
    "unassigned_tasks",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Assignment, unassigned_tasks),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__size_function__Assignment__unassigned_tasks,  // size() function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_const_function__Assignment__unassigned_tasks,  // get_const(index) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__get_function__Assignment__unassigned_tasks,  // get(index) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__fetch_function__Assignment__unassigned_tasks,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__assign_function__Assignment__unassigned_tasks,  // assign(index, value) function pointer
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__resize_function__Assignment__unassigned_tasks  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_members = {
  "mrs_msgs__msg",  // message namespace
  "Assignment",  // message name
  6,  // number of fields
  sizeof(mrs_msgs__msg__Assignment),
  false,  // has_any_key_member_
  mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_member_array,  // message members
  mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_type_support_handle = {
  0,
  &mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Assignment__get_type_hash,
  &mrs_msgs__msg__Assignment__get_type_description,
  &mrs_msgs__msg__Assignment__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Assignment)() {
  mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RobotAssignment)();
  if (!mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__Assignment__rosidl_typesupport_introspection_c__Assignment_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
