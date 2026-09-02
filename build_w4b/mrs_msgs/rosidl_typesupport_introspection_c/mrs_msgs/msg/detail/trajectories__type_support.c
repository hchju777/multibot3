// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mrs_msgs:msg/Trajectories.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mrs_msgs/msg/detail/trajectories__rosidl_typesupport_introspection_c.h"
#include "mrs_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mrs_msgs/msg/detail/trajectories__functions.h"
#include "mrs_msgs/msg/detail/trajectories__struct.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `limits`
#include "mrs_msgs/msg/limits.h"
// Member `limits`
#include "mrs_msgs/msg/detail/limits__rosidl_typesupport_introspection_c.h"
// Member `trajectories`
#include "mrs_msgs/msg/robot_trajectory.h"
// Member `trajectories`
#include "mrs_msgs/msg/detail/robot_trajectory__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mrs_msgs__msg__Trajectories__init(message_memory);
}

void mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_fini_function(void * message_memory)
{
  mrs_msgs__msg__Trajectories__fini(message_memory);
}

size_t mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__size_function__Trajectories__trajectories(
  const void * untyped_member)
{
  const mrs_msgs__msg__RobotTrajectory__Sequence * member =
    (const mrs_msgs__msg__RobotTrajectory__Sequence *)(untyped_member);
  return member->size;
}

const void * mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_const_function__Trajectories__trajectories(
  const void * untyped_member, size_t index)
{
  const mrs_msgs__msg__RobotTrajectory__Sequence * member =
    (const mrs_msgs__msg__RobotTrajectory__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_function__Trajectories__trajectories(
  void * untyped_member, size_t index)
{
  mrs_msgs__msg__RobotTrajectory__Sequence * member =
    (mrs_msgs__msg__RobotTrajectory__Sequence *)(untyped_member);
  return &member->data[index];
}

void mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__fetch_function__Trajectories__trajectories(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mrs_msgs__msg__RobotTrajectory * item =
    ((const mrs_msgs__msg__RobotTrajectory *)
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_const_function__Trajectories__trajectories(untyped_member, index));
  mrs_msgs__msg__RobotTrajectory * value =
    (mrs_msgs__msg__RobotTrajectory *)(untyped_value);
  *value = *item;
}

void mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__assign_function__Trajectories__trajectories(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mrs_msgs__msg__RobotTrajectory * item =
    ((mrs_msgs__msg__RobotTrajectory *)
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_function__Trajectories__trajectories(untyped_member, index));
  const mrs_msgs__msg__RobotTrajectory * value =
    (const mrs_msgs__msg__RobotTrajectory *)(untyped_value);
  *item = *value;
}

bool mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__resize_function__Trajectories__trajectories(
  void * untyped_member, size_t size)
{
  mrs_msgs__msg__RobotTrajectory__Sequence * member =
    (mrs_msgs__msg__RobotTrajectory__Sequence *)(untyped_member);
  mrs_msgs__msg__RobotTrajectory__Sequence__fini(member);
  return mrs_msgs__msg__RobotTrajectory__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_member_array[5] = {
  {
    "schema",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Trajectories, schema),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Trajectories, schema_version),  // bytes offset in struct
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
    offsetof(mrs_msgs__msg__Trajectories, instance_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "limits",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Trajectories, limits),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "trajectories",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs__msg__Trajectories, trajectories),  // bytes offset in struct
    NULL,  // default value
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__size_function__Trajectories__trajectories,  // size() function pointer
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_const_function__Trajectories__trajectories,  // get_const(index) function pointer
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__get_function__Trajectories__trajectories,  // get(index) function pointer
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__fetch_function__Trajectories__trajectories,  // fetch(index, &value) function pointer
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__assign_function__Trajectories__trajectories,  // assign(index, value) function pointer
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__resize_function__Trajectories__trajectories  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_members = {
  "mrs_msgs__msg",  // message namespace
  "Trajectories",  // message name
  5,  // number of fields
  sizeof(mrs_msgs__msg__Trajectories),
  false,  // has_any_key_member_
  mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_member_array,  // message members
  mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_init_function,  // function to initialize message memory (memory has to be allocated)
  mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_type_support_handle = {
  0,
  &mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Trajectories__get_type_hash,
  &mrs_msgs__msg__Trajectories__get_type_description,
  &mrs_msgs__msg__Trajectories__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mrs_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Trajectories)() {
  mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, Limits)();
  mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mrs_msgs, msg, RobotTrajectory)();
  if (!mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_type_support_handle.typesupport_identifier) {
    mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mrs_msgs__msg__Trajectories__rosidl_typesupport_introspection_c__Trajectories_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
