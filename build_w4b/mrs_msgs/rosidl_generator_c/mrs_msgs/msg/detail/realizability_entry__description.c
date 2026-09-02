// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RealizabilityEntry.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/realizability_entry__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RealizabilityEntry__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xbb, 0xab, 0x2c, 0x2f, 0xe7, 0xc8, 0x58, 0xb3,
      0x6e, 0x88, 0x4d, 0xa3, 0x00, 0x0c, 0xa4, 0x74,
      0xea, 0x8d, 0x0f, 0xba, 0x3e, 0x69, 0x69, 0x80,
      0x38, 0x31, 0x67, 0xee, 0xa4, 0x9f, 0x30, 0x38,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RealizabilityEntry__TYPE_NAME[] = "mrs_msgs/msg/RealizabilityEntry";

// Define type names, field names, and default values
static char mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__group[] = "group";
static char mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__alternative[] = "alternative";
static char mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__realizable[] = "realizable";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RealizabilityEntry__FIELDS[] = {
  {
    {mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__group, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__alternative, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RealizabilityEntry__FIELD_NAME__realizable, 10, 10},
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
mrs_msgs__msg__RealizabilityEntry__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RealizabilityEntry__TYPE_NAME, 31, 31},
      {mrs_msgs__msg__RealizabilityEntry__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string group\n"
  "string alternative\n"
  "bool realizable";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RealizabilityEntry__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RealizabilityEntry__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 48, 48},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RealizabilityEntry__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RealizabilityEntry__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
