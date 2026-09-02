// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/EntryEvent.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/entry_event__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__EntryEvent__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x94, 0x84, 0xb4, 0x4c, 0x32, 0x4d, 0x82, 0xea,
      0x1e, 0x27, 0xf2, 0x1a, 0x40, 0x7f, 0x1d, 0xf0,
      0x29, 0x6f, 0xa7, 0xcb, 0x5a, 0x50, 0x61, 0x9b,
      0x00, 0x9d, 0x80, 0x58, 0x11, 0x7f, 0xd7, 0xad,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__EntryEvent__TYPE_NAME[] = "mrs_msgs/msg/EntryEvent";

// Define type names, field names, and default values
static char mrs_msgs__msg__EntryEvent__FIELD_NAME__seq[] = "seq";
static char mrs_msgs__msg__EntryEvent__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__EntryEvent__FIELD_NAME__segment[] = "segment";
static char mrs_msgs__msg__EntryEvent__FIELD_NAME__location[] = "location";
static char mrs_msgs__msg__EntryEvent__FIELD_NAME__entered[] = "entered";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__EntryEvent__FIELDS[] = {
  {
    {mrs_msgs__msg__EntryEvent__FIELD_NAME__seq, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvent__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvent__FIELD_NAME__segment, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvent__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvent__FIELD_NAME__entered, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__EntryEvent__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__EntryEvent__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__EntryEvent__FIELDS, 5, 5},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "uint64 seq\n"
  "string robot\n"
  "string segment\n"
  "string location\n"
  "bool entered";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__EntryEvent__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__EntryEvent__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 68, 68},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__EntryEvent__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__EntryEvent__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
