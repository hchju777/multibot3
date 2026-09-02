// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotBaseline.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_baseline__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotBaseline__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x56, 0xf1, 0x66, 0x69, 0x86, 0x36, 0x56, 0x7b,
      0x33, 0x7f, 0x13, 0x07, 0x9c, 0xd3, 0x57, 0xf7,
      0x75, 0x9c, 0x8d, 0x4d, 0xa9, 0x49, 0x04, 0x58,
      0xe3, 0x9a, 0xdd, 0x10, 0x3a, 0xd4, 0x73, 0x4c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RobotBaseline__TYPE_NAME[] = "mrs_msgs/msg/RobotBaseline";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotBaseline__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__RobotBaseline__FIELD_NAME__committed_segment_count[] = "committed_segment_count";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotBaseline__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotBaseline__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotBaseline__FIELD_NAME__committed_segment_count, 23, 23},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotBaseline__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotBaseline__TYPE_NAME, 26, 26},
      {mrs_msgs__msg__RobotBaseline__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string robot\n"
  "uint32 committed_segment_count";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotBaseline__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotBaseline__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 44, 44},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotBaseline__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotBaseline__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
