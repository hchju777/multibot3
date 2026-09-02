// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Dependency.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/dependency__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Dependency__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x95, 0xaa, 0x3e, 0xba, 0xce, 0x79, 0x3c, 0xa7,
      0x7c, 0x07, 0xb5, 0x68, 0x38, 0x85, 0xe8, 0x18,
      0x11, 0xef, 0xed, 0x45, 0x8f, 0xff, 0x8b, 0x00,
      0x98, 0xf0, 0x0d, 0x66, 0x64, 0xa0, 0x3e, 0x37,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__Dependency__TYPE_NAME[] = "mrs_msgs/msg/Dependency";

// Define type names, field names, and default values
static char mrs_msgs__msg__Dependency__FIELD_NAME__from_id[] = "from_id";
static char mrs_msgs__msg__Dependency__FIELD_NAME__to_id[] = "to_id";
static char mrs_msgs__msg__Dependency__FIELD_NAME__type[] = "type";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Dependency__FIELDS[] = {
  {
    {mrs_msgs__msg__Dependency__FIELD_NAME__from_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Dependency__FIELD_NAME__to_id, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Dependency__FIELD_NAME__type, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Dependency__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Dependency__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__Dependency__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# DependencyType vocabulary (350 \\xc2\\xa73-2 V2~V6).\n"
  "uint8 DEP_UNKNOWN=0\n"
  "uint8 DEP_SEQUENTIAL=1\n"
  "uint8 DEP_ORDERING=2\n"
  "\n"
  "# \\xf0\\x9f\\x94\\xb4 from_id/to_id here are SEGMENT identifiers ({robot}#{index}), NOT roadmap\n"
  "# node ids (350 \\xc2\\xa78 D6 item 1). Do NOT nest an EdgeRef into this message \\xe2\\x80\\x94 the\n"
  "# name space differs from RoadmapEdge/Segment's from_id/to_id even though the\n"
  "# JSON source field names (\"from\"/\"to\") are identical.\n"
  "string from_id\n"
  "string to_id\n"
  "uint8 type";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Dependency__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Dependency__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 439, 439},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Dependency__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Dependency__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
