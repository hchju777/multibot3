// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/roadmap_edge__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RoadmapEdge__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x2e, 0x57, 0x0c, 0xef, 0x4c, 0x3f, 0x3f, 0xa2,
      0xf2, 0xa1, 0x14, 0x51, 0xba, 0xf4, 0x42, 0xac,
      0x0a, 0x9c, 0x94, 0xbd, 0x99, 0x01, 0xf1, 0x77,
      0x88, 0x1d, 0xef, 0x70, 0x9f, 0x33, 0xbb, 0xef,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RoadmapEdge__TYPE_NAME[] = "mrs_msgs/msg/RoadmapEdge";

// Define type names, field names, and default values
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__from_id[] = "from_id";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__to_id[] = "to_id";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__traversal[] = "traversal";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__length_m[] = "length_m";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__width_m[] = "width_m";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__capacity_robots[] = "capacity_robots";
static char mrs_msgs__msg__RoadmapEdge__FIELD_NAME__corridor[] = "corridor";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RoadmapEdge__FIELDS[] = {
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__from_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__to_id, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__traversal, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__length_m, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__width_m, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__capacity_robots, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__FIELD_NAME__corridor, 8, 8},
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
mrs_msgs__msg__RoadmapEdge__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RoadmapEdge__TYPE_NAME, 24, 24},
      {mrs_msgs__msg__RoadmapEdge__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# EdgeTraversal vocabulary (350 \\xc2\\xa73-2 V2~V6). 0 is reserved *_UNKNOWN across all\n"
  "# vocabularies (350 D1-a) \\xe2\\x80\\x94 do not hand it to the first legal value.\n"
  "uint8 TRAVERSAL_UNKNOWN=0\n"
  "uint8 TRAVERSAL_BIDIRECTIONAL=1\n"
  "uint8 TRAVERSAL_FORWARD_ONLY=2\n"
  "uint8 TRAVERSAL_BACKWARD_ONLY=3\n"
  "\n"
  "string from_id\n"
  "string to_id\n"
  "uint8 traversal\n"
  "float64 length_m\n"
  "float64 width_m\n"
  "# 350 \\xc2\\xa73-2 V7: uint8 but NOT a vocabulary \\xe2\\x80\\x94 no constants, value carried as-is.\n"
  "uint8 capacity_robots\n"
  "# \"\" sentinel = no corridor reference (roadmap.schema.json:88, 350 \\xc2\\xa74-3).\n"
  "string corridor";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RoadmapEdge__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RoadmapEdge__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 539, 539},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RoadmapEdge__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RoadmapEdge__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
