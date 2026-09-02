// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotGroup.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_group__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotGroup__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x51, 0xdb, 0x72, 0x9c, 0xd1, 0x31, 0x3b, 0x31,
      0xbd, 0x73, 0x27, 0x9b, 0xc0, 0x86, 0x50, 0xb0,
      0x87, 0xef, 0xd5, 0xf5, 0x3a, 0xd3, 0xd8, 0xc6,
      0xaa, 0x82, 0xfe, 0x87, 0x4b, 0xd6, 0x7b, 0xce,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RobotGroup__TYPE_NAME[] = "mrs_msgs/msg/RobotGroup";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotGroup__FIELD_NAME__robots[] = "robots";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotGroup__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotGroup__FIELD_NAME__robots, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotGroup__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotGroup__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__RobotGroup__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# string[][] has no IDL representation (350 \\xc2\\xa76 replan_result.components) \\xe2\\x80\\x94\n"
  "# one extra level of nesting via this wrapper instead (350 \\xc2\\xa77-6 \"3\\xec\\xa4\\x91 \\xec\\x9d\\xb4\\xec\\x83\\x81\n"
  "# \\xec\\xa4\\x91\\xec\\xb2\\xa9\\xec\\x9d\\x80 \\xeb\\xa7\\x8c\\xeb\\x93\\xa4\\xec\\xa7\\x80 \\xec\\x95\\x8a\\xeb\\x8a\\x94\\xeb\\x8b\\xa4\").\n"
  "string[] robots";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotGroup__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotGroup__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 179, 179},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotGroup__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotGroup__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
