// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from mrs_msgs:msg/RobotSpecs.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/robot_specs__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <cstddef>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/serialization_helpers.hpp"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "mrs_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "mrs_msgs/msg/detail/robot_specs__struct.h"
#include "mrs_msgs/msg/detail/robot_specs__functions.h"
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

#include "mrs_msgs/msg/detail/robot_spec__functions.h"  // robots
#include "rosidl_runtime_c/string.h"  // instance_id, schema, schema_version
#include "rosidl_runtime_c/string_functions.h"  // instance_id, schema, schema_version

// forward declare type support functions

bool cdr_serialize_mrs_msgs__msg__RobotSpec(
  const mrs_msgs__msg__RobotSpec * ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool cdr_deserialize_mrs_msgs__msg__RobotSpec(
  eprosima::fastcdr::Cdr & cdr,
  mrs_msgs__msg__RobotSpec * ros_message);

size_t get_serialized_size_mrs_msgs__msg__RobotSpec(
  const void * untyped_ros_message,
  size_t current_alignment);

size_t max_serialized_size_mrs_msgs__msg__RobotSpec(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

bool cdr_serialize_key_mrs_msgs__msg__RobotSpec(
  const mrs_msgs__msg__RobotSpec * ros_message,
  eprosima::fastcdr::Cdr & cdr);

size_t get_serialized_size_key_mrs_msgs__msg__RobotSpec(
  const void * untyped_ros_message,
  size_t current_alignment);

size_t max_serialized_size_key_mrs_msgs__msg__RobotSpec(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, mrs_msgs, msg, RobotSpec)();


using _RobotSpecs__ros_msg_type = mrs_msgs__msg__RobotSpecs;


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_mrs_msgs__msg__RobotSpecs(
  const mrs_msgs__msg__RobotSpecs * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: schema
  {
    const rosidl_runtime_c__String * str = &ros_message->schema;
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

  // Field name: schema_version
  {
    const rosidl_runtime_c__String * str = &ros_message->schema_version;
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

  // Field name: instance_id
  {
    const rosidl_runtime_c__String * str = &ros_message->instance_id;
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

  // Field name: robots
  {
    size_t size = ros_message->robots.size;
    auto array_ptr = ros_message->robots.data;
    cdr << static_cast<uint32_t>(size);
    for (size_t i = 0; i < size; ++i) {
      cdr_serialize_mrs_msgs__msg__RobotSpec(
        &array_ptr[i], cdr);
    }
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_deserialize_mrs_msgs__msg__RobotSpecs(
  eprosima::fastcdr::Cdr & cdr,
  mrs_msgs__msg__RobotSpecs * ros_message)
{
  // Field name: schema
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->schema.data) {
      rosidl_runtime_c__String__init(&ros_message->schema);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->schema,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'schema'\n");
      return false;
    }
  }

  // Field name: schema_version
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->schema_version.data) {
      rosidl_runtime_c__String__init(&ros_message->schema_version);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->schema_version,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'schema_version'\n");
      return false;
    }
  }

  // Field name: instance_id
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->instance_id.data) {
      rosidl_runtime_c__String__init(&ros_message->instance_id);
    }
    bool succeeded = rosidl_runtime_c__String__assign(
      &ros_message->instance_id,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'instance_id'\n");
      return false;
    }
  }

  // Field name: robots
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);

    // Check there are at least 'size' remaining bytes in the CDR stream before resizing
    auto old_state = cdr.get_state();
    bool correct_size = cdr.jump(size);
    cdr.set_state(old_state);
    if (!correct_size) {
      fprintf(stderr, "sequence size exceeds remaining buffer\n");
      return false;
    }

    if (ros_message->robots.data) {
      mrs_msgs__msg__RobotSpec__Sequence__fini(&ros_message->robots);
    }
    if (!mrs_msgs__msg__RobotSpec__Sequence__init(&ros_message->robots, size)) {
      fprintf(stderr, "failed to create array for field 'robots'");
      return false;
    }
    auto array_ptr = ros_message->robots.data;
    for (size_t i = 0; i < size; ++i) {
      cdr_deserialize_mrs_msgs__msg__RobotSpec(cdr, &array_ptr[i]);
    }
  }

  return true;
}  // NOLINT(readability/fn_size)


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_mrs_msgs__msg__RobotSpecs(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotSpecs__ros_msg_type * ros_message = static_cast<const _RobotSpecs__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: schema
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->schema.size + 1);

  // Field name: schema_version
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->schema_version.size + 1);

  // Field name: instance_id
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->instance_id.size + 1);

  // Field name: robots
  {
    size_t array_size = ros_message->robots.size;
    auto array_ptr = ros_message->robots.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += get_serialized_size_mrs_msgs__msg__RobotSpec(
        &array_ptr[index], current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}


ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_mrs_msgs__msg__RobotSpecs(
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

  // Field name: schema
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

  // Field name: schema_version
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

  // Field name: instance_id
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

  // Field name: robots
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_mrs_msgs__msg__RobotSpec(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }


  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RobotSpecs;
    is_plain =
      (
      offsetof(DataType, robots) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
bool cdr_serialize_key_mrs_msgs__msg__RobotSpecs(
  const mrs_msgs__msg__RobotSpecs * ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Field name: schema
  {
    const rosidl_runtime_c__String * str = &ros_message->schema;
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

  // Field name: schema_version
  {
    const rosidl_runtime_c__String * str = &ros_message->schema_version;
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

  // Field name: instance_id
  {
    const rosidl_runtime_c__String * str = &ros_message->instance_id;
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

  // Field name: robots
  {
    size_t size = ros_message->robots.size;
    auto array_ptr = ros_message->robots.data;
    cdr << static_cast<uint32_t>(size);
    for (size_t i = 0; i < size; ++i) {
      cdr_serialize_key_mrs_msgs__msg__RobotSpec(
        &array_ptr[i], cdr);
    }
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t get_serialized_size_key_mrs_msgs__msg__RobotSpecs(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _RobotSpecs__ros_msg_type * ros_message = static_cast<const _RobotSpecs__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;

  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Field name: schema
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->schema.size + 1);

  // Field name: schema_version
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->schema_version.size + 1);

  // Field name: instance_id
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->instance_id.size + 1);

  // Field name: robots
  {
    size_t array_size = ros_message->robots.size;
    auto array_ptr = ros_message->robots.data;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += get_serialized_size_key_mrs_msgs__msg__RobotSpec(
        &array_ptr[index], current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_mrs_msgs
size_t max_serialized_size_key_mrs_msgs__msg__RobotSpecs(
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
  // Field name: schema
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

  // Field name: schema_version
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

  // Field name: instance_id
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

  // Field name: robots
  {
    size_t array_size = 0;
    full_bounded = false;
    is_plain = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);
    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_key_mrs_msgs__msg__RobotSpec(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = mrs_msgs__msg__RobotSpecs;
    is_plain =
      (
      offsetof(DataType, robots) +
      last_member_size
      ) == ret_val;
  }
  return ret_val;
}


static bool _RobotSpecs__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const mrs_msgs__msg__RobotSpecs * ros_message = static_cast<const mrs_msgs__msg__RobotSpecs *>(untyped_ros_message);
  (void)ros_message;
  return cdr_serialize_mrs_msgs__msg__RobotSpecs(ros_message, cdr);
}

static bool _RobotSpecs__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  mrs_msgs__msg__RobotSpecs * ros_message = static_cast<mrs_msgs__msg__RobotSpecs *>(untyped_ros_message);
  (void)ros_message;
  return cdr_deserialize_mrs_msgs__msg__RobotSpecs(cdr, ros_message);
}

static uint32_t _RobotSpecs__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_mrs_msgs__msg__RobotSpecs(
      untyped_ros_message, 0));
}

static size_t _RobotSpecs__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_mrs_msgs__msg__RobotSpecs(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_RobotSpecs = {
  "mrs_msgs::msg",
  "RobotSpecs",
  _RobotSpecs__cdr_serialize,
  _RobotSpecs__cdr_deserialize,
  _RobotSpecs__get_serialized_size,
  _RobotSpecs__max_serialized_size,
  nullptr
};

static rosidl_message_type_support_t _RobotSpecs__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_RobotSpecs,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__RobotSpecs__get_type_hash,
  &mrs_msgs__msg__RobotSpecs__get_type_description,
  &mrs_msgs__msg__RobotSpecs__get_type_description_sources,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, mrs_msgs, msg, RobotSpecs)() {
  return &_RobotSpecs__type_support;
}

#if defined(__cplusplus)
}
#endif
