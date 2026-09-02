// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/roadmap_provenance__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "mrs_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "mrs_msgs/msg/detail/roadmap_provenance__struct.h"
#include "mrs_msgs/msg/detail/roadmap_provenance__functions.h"
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

#include "rosidl_runtime_c/string.h"  // capacity_rule_id, generator_version
#include "rosidl_runtime_c/string_functions.h"  // capacity_rule_id, generator_version

// forward declare type support functions


using _RoadmapProvenance__ros_msg_type = mrs_msgs__msg__RoadmapProvenance;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_mrs_msgs__msg__RoadmapProvenance(
  const mrs_msgs__msg__RoadmapProvenance * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: min_separation_m
  {
    cdr << ros_message->min_separation_m;
  }

  // Field name: wall_inflation_m
  {
    cdr << ros_message->wall_inflation_m;
  }

  // Field name: capacity_rule_id
  {
    const rosidl_runtime_c__String * str = &ros_message->capacity_rule_id;
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

  // Field name: generator_version
  {
    const rosidl_runtime_c__String * str = &ros_message->generator_version;
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

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_deserialize_mrs_msgs__msg__RoadmapProvenance(
  eprosima::fastcdr::Cdr & cdr,
  mrs_msgs__msg__RoadmapProvenance * ros_message)
{
  // Field name: min_separation_m
  {
    cdr >> ros_message->min_separation_m;
  }

  // Field name: wall_inflation_m
  {
    cdr >> ros_message->wall_inflation_m;
  }

  // Field name: capacity_rule_id
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->capacity_rule_id.data) {
      rosidl_runtime_c__String__init(&ros_message->capacity_rule_id);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->capacity_rule_id,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'capacity_rule_id'\n");
      return false;
    }
  }

  // Field name: generator_version
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->generator_version.data) {
      rosidl_runtime_c__String__init(&ros_message->generator_version);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->generator_version,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'generator_version'\n");
      return false;
    }
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_mrs_msgs__msg__RoadmapProvenance(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RoadmapProvenance__ros_msg_type * ros_message = static_cast<const _RoadmapProvenance__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: min_separation_m
  {
    size_t item_size = sizeof(ros_message->min_separation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: wall_inflation_m
  {
    size_t item_size = sizeof(ros_message->wall_inflation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: capacity_rule_id
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->capacity_rule_id.size + 1);

  // Field name: generator_version
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->generator_version.size + 1);

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_mrs_msgs__msg__RoadmapProvenance(
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

  // Field name: min_separation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: wall_inflation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: capacity_rule_id
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

  // Field name: generator_version
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


  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RoadmapProvenance;
    is_plain =
      (
      offsetof(DataType, generator_version) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_key_mrs_msgs__msg__RoadmapProvenance(
  const mrs_msgs__msg__RoadmapProvenance * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: min_separation_m
  {
    cdr << ros_message->min_separation_m;
  }

  // Field name: wall_inflation_m
  {
    cdr << ros_message->wall_inflation_m;
  }

  // Field name: capacity_rule_id
  {
    const rosidl_runtime_c__String * str = &ros_message->capacity_rule_id;
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

  // Field name: generator_version
  {
    const rosidl_runtime_c__String * str = &ros_message->generator_version;
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

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_key_mrs_msgs__msg__RoadmapProvenance(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RoadmapProvenance__ros_msg_type * ros_message = static_cast<const _RoadmapProvenance__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: min_separation_m
  {
    size_t item_size = sizeof(ros_message->min_separation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: wall_inflation_m
  {
    size_t item_size = sizeof(ros_message->wall_inflation_m);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  // Field name: capacity_rule_id
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->capacity_rule_id.size + 1);

  // Field name: generator_version
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->generator_version.size + 1);

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_key_mrs_msgs__msg__RoadmapProvenance(
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
  // Field name: min_separation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: wall_inflation_m
  {
    size_t array_size = 1;
    last_member_size = array_size * sizeof(uint64_t);
    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Field name: capacity_rule_id
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

  // Field name: generator_version
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

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RoadmapProvenance;
    is_plain =
      (
      offsetof(DataType, generator_version) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _RoadmapProvenance__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const mrs_msgs__msg__RoadmapProvenance * ros_message = static_cast<const mrs_msgs__msg__RoadmapProvenance *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_mrs_msgs__msg__RoadmapProvenance(ros_message, cdr);
}

static bool _RoadmapProvenance__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  mrs_msgs__msg__RoadmapProvenance * ros_message = static_cast<mrs_msgs__msg__RoadmapProvenance *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_mrs_msgs__msg__RoadmapProvenance(cdr, ros_message);
}

static uint32_t _RoadmapProvenance__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_mrs_msgs__msg__RoadmapProvenance(
      untyped_ros_message, 0));
}

static size_t _RoadmapProvenance__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_mrs_msgs__msg__RoadmapProvenance(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_RoadmapProvenance = {
  "mrs_msgs::msg",
  "RoadmapProvenance",
  _RoadmapProvenance__cdr_serialize,
  _RoadmapProvenance__cdr_deserialize,
  _RoadmapProvenance__get_serialized_size,
  _RoadmapProvenance__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _RoadmapProvenance__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_RoadmapProvenance,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__RoadmapProvenance__get_type_hash,
  &mrs_msgs__msg__RoadmapProvenance__get_type_description,
  &mrs_msgs__msg__RoadmapProvenance__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, mrs_msgs, msg, RoadmapProvenance)() {
  return &_RoadmapProvenance__type_support;
}

#if defined(__cplusplus)
}
#endif
