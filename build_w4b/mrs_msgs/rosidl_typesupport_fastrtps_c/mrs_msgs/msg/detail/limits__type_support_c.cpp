// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/limits__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "mrs_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "mrs_msgs/msg/detail/limits__struct.h"
#include "mrs_msgs/msg/detail/limits__functions.h"
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


// forward declare type support functions


using _Limits__ros_msg_type = mrs_msgs__msg__Limits;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_mrs_msgs__msg__Limits(
  const mrs_msgs__msg__Limits * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: a_max
  {
    cdr << ros_message->a_max;
  }

  // Field name: j_max
  {
    cdr << ros_message->j_max;
  }

  // Field name: min_separation_m
  {
    cdr << ros_message->min_separation_m;
  }

  // Field name: omega_max_radps
  {
    cdr << ros_message->omega_max_radps;
  }

  // Field name: yaw_accel_max_radps2
  {
    cdr << ros_message->yaw_accel_max_radps2;
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_deserialize_mrs_msgs__msg__Limits(
  eprosima::fastcdr::Cdr & cdr,
  mrs_msgs__msg__Limits * ros_message)
{
  // Field name: a_max
  {
    cdr >> ros_message->a_max;
  }

  // Field name: j_max
  {
    cdr >> ros_message->j_max;
  }

  // Field name: min_separation_m
  {
    cdr >> ros_message->min_separation_m;
  }

  // Field name: omega_max_radps
  {
    cdr >> ros_message->omega_max_radps;
  }

  // Field name: yaw_accel_max_radps2
  {
    cdr >> ros_message->yaw_accel_max_radps2;
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_mrs_msgs__msg__Limits(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _Limits__ros_msg_type * ros_message = static_cast<const _Limits__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: a_max
  {
    size_t item_size = sizeof(ros_message->a_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: j_max
  {
    size_t item_size = sizeof(ros_message->j_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: min_separation_m
  {
    size_t item_size = sizeof(ros_message->min_separation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: omega_max_radps
  {
    size_t item_size = sizeof(ros_message->omega_max_radps);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: yaw_accel_max_radps2
  {
    size_t item_size = sizeof(ros_message->yaw_accel_max_radps2);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_mrs_msgs__msg__Limits(
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

  // Field name: a_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: j_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: min_separation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: omega_max_radps
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: yaw_accel_max_radps2
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
    using DataType = mrs_msgs__msg__Limits;
    is_plain =
      (
      offsetof(DataType, yaw_accel_max_radps2) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_key_mrs_msgs__msg__Limits(
  const mrs_msgs__msg__Limits * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: a_max
  {
    cdr << ros_message->a_max;
  }

  // Field name: j_max
  {
    cdr << ros_message->j_max;
  }

  // Field name: min_separation_m
  {
    cdr << ros_message->min_separation_m;
  }

  // Field name: omega_max_radps
  {
    cdr << ros_message->omega_max_radps;
  }

  // Field name: yaw_accel_max_radps2
  {
    cdr << ros_message->yaw_accel_max_radps2;
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_key_mrs_msgs__msg__Limits(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _Limits__ros_msg_type * ros_message = static_cast<const _Limits__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: a_max
  {
    size_t item_size = sizeof(ros_message->a_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: j_max
  {
    size_t item_size = sizeof(ros_message->j_max);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: min_separation_m
  {
    size_t item_size = sizeof(ros_message->min_separation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: omega_max_radps
  {
    size_t item_size = sizeof(ros_message->omega_max_radps);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: yaw_accel_max_radps2
  {
    size_t item_size = sizeof(ros_message->yaw_accel_max_radps2);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_key_mrs_msgs__msg__Limits(
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
  // Field name: a_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: j_max
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: min_separation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: omega_max_radps
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: yaw_accel_max_radps2
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
    using DataType = mrs_msgs__msg__Limits;
    is_plain =
      (
      offsetof(DataType, yaw_accel_max_radps2) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _Limits__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const mrs_msgs__msg__Limits * ros_message = static_cast<const mrs_msgs__msg__Limits *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_mrs_msgs__msg__Limits(ros_message, cdr);
}

static bool _Limits__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  mrs_msgs__msg__Limits * ros_message = static_cast<mrs_msgs__msg__Limits *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_mrs_msgs__msg__Limits(cdr, ros_message);
}

static uint32_t _Limits__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_mrs_msgs__msg__Limits(
      untyped_ros_message, 0));
}

static size_t _Limits__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_mrs_msgs__msg__Limits(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_Limits = {
  "mrs_msgs::msg",
  "Limits",
  _Limits__cdr_serialize,
  _Limits__cdr_deserialize,
  _Limits__get_serialized_size,
  _Limits__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _Limits__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_Limits,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Limits__get_type_hash,
  &mrs_msgs__msg__Limits__get_type_description,
  &mrs_msgs__msg__Limits__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, mrs_msgs, msg, Limits)() {
  return &_Limits__type_support;
}

#if defined(__cplusplus)
}
#endif
