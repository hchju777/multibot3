// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/robot_spec__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "mrs_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "mrs_msgs/msg/detail/robot_spec__struct.h"
#include "mrs_msgs/msg/detail/robot_spec__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rosidl_runtime_c/string.h"  // robot
#include "rosidl_runtime_c/string_functions.h"  // robot

// forward declare type support functions


using _RobotSpec__ros_msg_type = mrs_msgs__msg__RobotSpec;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_mrs_msgs__msg__RobotSpec(
  const mrs_msgs__msg__RobotSpec * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: robot
  {
    const rosidl_runtime_c__String * str = &ros_message->robot;
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
  }

  // Field name: avg_traversal_speed_mps
  {
    cdr << ros_message->avg_traversal_speed_mps;
  }

  // Field name: v_max
  {
    cdr << ros_message->v_max;
  }

  // Field name: brake_decel_min_mps2
  {
    cdr << ros_message->brake_decel_min_mps2;
  }

  // Field name: curvature_max_invm
  {
    cdr << ros_message->curvature_max_invm;
  }

  // Field name: reverse_motion_allowed
  {
    cdr << (ros_message->reverse_motion_allowed ? true : false);
  }

  // Field name: circumradius_m
  {
    cdr << ros_message->circumradius_m;
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_deserialize_mrs_msgs__msg__RobotSpec(
  eprosima::fastcdr::Cdr & cdr,
  mrs_msgs__msg__RobotSpec * ros_message)
{
  // Field name: robot
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->robot.data) {
      rosidl_runtime_c__String__init(&ros_message->robot);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->robot,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'robot'\n");
      return false;
    }
  }

  // Field name: avg_traversal_speed_mps
  {
    cdr >> ros_message->avg_traversal_speed_mps;
  }

  // Field name: v_max
  {
    cdr >> ros_message->v_max;
  }

  // Field name: brake_decel_min_mps2
  {
    cdr >> ros_message->brake_decel_min_mps2;
  }

  // Field name: curvature_max_invm
  {
    cdr >> ros_message->curvature_max_invm;
  }

  // Field name: reverse_motion_allowed
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message->reverse_motion_allowed = tmp ? true : false;
  }

  // Field name: circumradius_m
  {
    cdr >> ros_message->circumradius_m;
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_mrs_msgs__msg__RobotSpec(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotSpec__ros_msg_type * ros_message = static_cast<const _RobotSpec__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: robot
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->robot.size + 1);

  // Field name: avg_traversal_speed_mps
  {
    size_t item_size = sizeof(ros_message->avg_traversal_speed_mps);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: v_max
  {
    size_t item_size = sizeof(ros_message->v_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: brake_decel_min_mps2
  {
    size_t item_size = sizeof(ros_message->brake_decel_min_mps2);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: curvature_max_invm
  {
    size_t item_size = sizeof(ros_message->curvature_max_invm);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: reverse_motion_allowed
  {
    size_t item_size = sizeof(ros_message->reverse_motion_allowed);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: circumradius_m
  {
    size_t item_size = sizeof(ros_message->circumradius_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_mrs_msgs__msg__RobotSpec(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // Field name: robot
  {
    size_t array_size = 1;
    full_bounded = false;
    is_plain = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }

  // Field name: avg_traversal_speed_mps
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: v_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: brake_decel_min_mps2
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: curvature_max_invm
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: reverse_motion_allowed
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  // Field name: circumradius_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }


  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RobotSpec;
    is_plain =
      (
      offsetof(DataType, circumradius_m) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_key_mrs_msgs__msg__RobotSpec(
  const mrs_msgs__msg__RobotSpec * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: robot
  {
    const rosidl_runtime_c__String * str = &ros_message->robot;
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
  }

  // Field name: avg_traversal_speed_mps
  {
    cdr << ros_message->avg_traversal_speed_mps;
  }

  // Field name: v_max
  {
    cdr << ros_message->v_max;
  }

  // Field name: brake_decel_min_mps2
  {
    cdr << ros_message->brake_decel_min_mps2;
  }

  // Field name: curvature_max_invm
  {
    cdr << ros_message->curvature_max_invm;
  }

  // Field name: reverse_motion_allowed
  {
    cdr << (ros_message->reverse_motion_allowed ? true : false);
  }

  // Field name: circumradius_m
  {
    cdr << ros_message->circumradius_m;
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_key_mrs_msgs__msg__RobotSpec(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotSpec__ros_msg_type * ros_message = static_cast<const _RobotSpec__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: robot
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->robot.size + 1);

  // Field name: avg_traversal_speed_mps
  {
    size_t item_size = sizeof(ros_message->avg_traversal_speed_mps);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: v_max
  {
    size_t item_size = sizeof(ros_message->v_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: brake_decel_min_mps2
  {
    size_t item_size = sizeof(ros_message->brake_decel_min_mps2);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: curvature_max_invm
  {
    size_t item_size = sizeof(ros_message->curvature_max_invm);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: reverse_motion_allowed
  {
    size_t item_size = sizeof(ros_message->reverse_motion_allowed);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: circumradius_m
  {
    size_t item_size = sizeof(ros_message->circumradius_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_key_mrs_msgs__msg__RobotSpec(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;
  // Field name: robot
  {
    size_t array_size = 1;
    full_bounded = false;
    is_plain = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }

  // Field name: avg_traversal_speed_mps
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: v_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: brake_decel_min_mps2
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: curvature_max_invm
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: reverse_motion_allowed
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  // Field name: circumradius_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RobotSpec;
    is_plain =
      (
      offsetof(DataType, circumradius_m) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _RobotSpec__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const mrs_msgs__msg__RobotSpec * ros_message = static_cast<const mrs_msgs__msg__RobotSpec *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_mrs_msgs__msg__RobotSpec(ros_message, cdr);
}

static bool _RobotSpec__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  mrs_msgs__msg__RobotSpec * ros_message = static_cast<mrs_msgs__msg__RobotSpec *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_mrs_msgs__msg__RobotSpec(cdr, ros_message);
}

static uint32_t _RobotSpec__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_mrs_msgs__msg__RobotSpec(
      untyped_ros_message, 0));
}

static size_t _RobotSpec__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_mrs_msgs__msg__RobotSpec(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_RobotSpec = {
  "mrs_msgs::msg",
  "RobotSpec",
  _RobotSpec__cdr_serialize,
  _RobotSpec__cdr_deserialize,
  _RobotSpec__get_serialized_size,
  _RobotSpec__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _RobotSpec__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_RobotSpec,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__RobotSpec__get_type_hash,
  &mrs_msgs__msg__RobotSpec__get_type_description,
  &mrs_msgs__msg__RobotSpec__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, mrs_msgs, msg, RobotSpec)() {
  return &_RobotSpec__type_support;
}

#if defined(__cplusplus)
}
#endif
