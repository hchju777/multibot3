// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/SegmentReleaseItem.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/segment_release_item__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__SegmentReleaseItem__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xbd, 0x9a, 0x60, 0xc8, 0xab, 0x23, 0x9f, 0x79,
      0xf5, 0x49, 0xee, 0x4c, 0x15, 0xe5, 0x9b, 0x7b,
      0x2d, 0x49, 0x99, 0x0a, 0xbe, 0x65, 0x40, 0x34,
      0x06, 0xc1, 0x59, 0x65, 0x48, 0x41, 0x78, 0x32,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME[] = "mrs_msgs/msg/SegmentReleaseItem";

// Define type names, field names, and default values
static char mrs_msgs__msg__SegmentReleaseItem__FIELD_NAME__id[] = "id";
static char mrs_msgs__msg__SegmentReleaseItem__FIELD_NAME__released[] = "released";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__SegmentReleaseItem__FIELDS[] = {
  {
    {mrs_msgs__msg__SegmentReleaseItem__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentReleaseItem__FIELD_NAME__released, 8, 8},
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
mrs_msgs__msg__SegmentReleaseItem__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME, 31, 31},
      {mrs_msgs__msg__SegmentReleaseItem__FIELDS, 2, 2},
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
  "bool released";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__SegmentReleaseItem__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME, 31, 31},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 24, 24},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__SegmentReleaseItem__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__SegmentReleaseItem__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
