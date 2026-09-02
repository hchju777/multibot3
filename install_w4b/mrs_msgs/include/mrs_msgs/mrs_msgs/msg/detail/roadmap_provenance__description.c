// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/roadmap_provenance__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RoadmapProvenance__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3e, 0xe5, 0x5c, 0xcd, 0x97, 0x3e, 0x43, 0x60,
      0x50, 0xbd, 0xf2, 0xdc, 0x6a, 0xf2, 0x4e, 0x6a,
      0x90, 0x07, 0x35, 0x6e, 0xbc, 0x4b, 0xa3, 0xf4,
      0x07, 0xa9, 0xc0, 0x1d, 0xd7, 0xe4, 0xfa, 0x1d,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RoadmapProvenance__TYPE_NAME[] = "mrs_msgs/msg/RoadmapProvenance";

// Define type names, field names, and default values
static char mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__min_separation_m[] = "min_separation_m";
static char mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__wall_inflation_m[] = "wall_inflation_m";
static char mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__capacity_rule_id[] = "capacity_rule_id";
static char mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__generator_version[] = "generator_version";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RoadmapProvenance__FIELDS[] = {
  {
    {mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__min_separation_m, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__wall_inflation_m, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__capacity_rule_id, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapProvenance__FIELD_NAME__generator_version, 17, 17},
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
mrs_msgs__msg__RoadmapProvenance__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RoadmapProvenance__TYPE_NAME, 30, 30},
      {mrs_msgs__msg__RoadmapProvenance__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float64 min_separation_m\n"
  "float64 wall_inflation_m\n"
  "# 350 \\xc2\\xa73-2 V8 (\\xed\\x8a\\xb8\\xeb\\x9e\\xa9): enum in JSON but x-ros-type is \"string\" \\xe2\\x80\\x94 do NOT render\n"
  "# as uint8. No consuming module branches on this string (audit-only).\n"
  "string capacity_rule_id\n"
  "string generator_version";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RoadmapProvenance__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RoadmapProvenance__TYPE_NAME, 30, 30},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 245, 245},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RoadmapProvenance__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RoadmapProvenance__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
