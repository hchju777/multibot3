// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Segment.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/segment__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Segment__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe5, 0x30, 0xc1, 0x49, 0x19, 0x91, 0x3e, 0xa3,
      0xa7, 0x6e, 0x86, 0xab, 0x8d, 0x2c, 0xb4, 0x99,
      0x45, 0xb1, 0x0e, 0x11, 0xe9, 0x9f, 0x92, 0x34,
      0xa1, 0x21, 0xcb, 0x98, 0x87, 0x06, 0x37, 0x7c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__Segment__TYPE_NAME[] = "mrs_msgs/msg/Segment";

// Define type names, field names, and default values
static char mrs_msgs__msg__Segment__FIELD_NAME__id[] = "id";
static char mrs_msgs__msg__Segment__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__Segment__FIELD_NAME__from_id[] = "from_id";
static char mrs_msgs__msg__Segment__FIELD_NAME__to_id[] = "to_id";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Segment__FIELDS[] = {
  {
    {mrs_msgs__msg__Segment__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Segment__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Segment__FIELD_NAME__from_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Segment__FIELD_NAME__to_id, 5, 5},
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
mrs_msgs__msg__Segment__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Segment__TYPE_NAME, 20, 20},
      {mrs_msgs__msg__Segment__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string id\n"
  "string robot\n"
  "string from_id\n"
  "string to_id";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Segment__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Segment__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 51, 51},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Segment__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Segment__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
