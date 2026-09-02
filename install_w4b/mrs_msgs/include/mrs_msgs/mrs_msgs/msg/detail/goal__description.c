// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/goal__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Goal__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x38, 0x53, 0x88, 0xff, 0x4e, 0x42, 0x60, 0x20,
      0x36, 0x1c, 0x58, 0x8b, 0x6f, 0xd9, 0x86, 0x35,
      0xc6, 0x23, 0x88, 0xa2, 0xd7, 0x09, 0x84, 0xbc,
      0x29, 0xf4, 0xa9, 0x03, 0x0c, 0xe3, 0x66, 0x9a,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__Goal__TYPE_NAME[] = "mrs_msgs/msg/Goal";

// Define type names, field names, and default values
static char mrs_msgs__msg__Goal__FIELD_NAME__goal_id[] = "goal_id";
static char mrs_msgs__msg__Goal__FIELD_NAME__task[] = "task";
static char mrs_msgs__msg__Goal__FIELD_NAME__location[] = "location";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Goal__FIELDS[] = {
  {
    {mrs_msgs__msg__Goal__FIELD_NAME__goal_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Goal__FIELD_NAME__task, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Goal__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Goal__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Goal__TYPE_NAME, 17, 17},
      {mrs_msgs__msg__Goal__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string goal_id\n"
  "string task\n"
  "string location";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Goal__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Goal__TYPE_NAME, 17, 17},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 43, 43},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Goal__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Goal__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
