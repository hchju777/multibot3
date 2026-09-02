// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RoadmapNode.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/roadmap_node__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RoadmapNode__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x2e, 0x93, 0x33, 0xfa, 0xea, 0x63, 0x9b, 0x18,
      0x44, 0x11, 0xd9, 0x5a, 0x96, 0x03, 0xcd, 0xa9,
      0x89, 0xf0, 0xf3, 0xd0, 0x3c, 0x13, 0x21, 0x4d,
      0x3f, 0xe9, 0x67, 0x37, 0x90, 0x8b, 0x49, 0x33,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RoadmapNode__TYPE_NAME[] = "mrs_msgs/msg/RoadmapNode";

// Define type names, field names, and default values
static char mrs_msgs__msg__RoadmapNode__FIELD_NAME__id[] = "id";
static char mrs_msgs__msg__RoadmapNode__FIELD_NAME__x[] = "x";
static char mrs_msgs__msg__RoadmapNode__FIELD_NAME__y[] = "y";
static char mrs_msgs__msg__RoadmapNode__FIELD_NAME__clearance_m[] = "clearance_m";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RoadmapNode__FIELDS[] = {
  {
    {mrs_msgs__msg__RoadmapNode__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapNode__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapNode__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapNode__FIELD_NAME__clearance_m, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RoadmapNode__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RoadmapNode__TYPE_NAME, 24, 24},
      {mrs_msgs__msg__RoadmapNode__FIELDS, 4, 4},
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
  "float64 x\n"
  "float64 y\n"
  "float64 clearance_m";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RoadmapNode__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RoadmapNode__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 50, 50},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RoadmapNode__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RoadmapNode__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
