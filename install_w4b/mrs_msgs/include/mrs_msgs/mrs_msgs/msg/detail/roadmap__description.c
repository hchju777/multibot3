// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/roadmap__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Roadmap__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xd3, 0x2f, 0x4c, 0x22, 0xab, 0x12, 0x69, 0x0a,
      0x7e, 0xa4, 0xca, 0x46, 0x6a, 0x81, 0x3a, 0x8f,
      0x60, 0x6c, 0x2e, 0xf7, 0x83, 0x90, 0xcf, 0xfa,
      0xb7, 0xe9, 0xb8, 0x13, 0xc7, 0x53, 0x52, 0x95,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/roadmap_node__functions.h"
#include "mrs_msgs/msg/detail/roadmap_provenance__functions.h"
#include "mrs_msgs/msg/detail/edge_ref__functions.h"
#include "mrs_msgs/msg/detail/roadmap_edge__functions.h"
#include "mrs_msgs/msg/detail/corridor__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__Corridor__EXPECTED_HASH = {1, {
    0x0c, 0x69, 0x4c, 0xb6, 0x33, 0x66, 0x05, 0xd5,
    0x5d, 0x91, 0xf9, 0x06, 0xa4, 0xb6, 0xc2, 0xca,
    0x83, 0xa7, 0x48, 0xa9, 0xf5, 0x8d, 0xd6, 0x0f,
    0x94, 0x09, 0xf9, 0x28, 0x8e, 0x1b, 0x98, 0x11,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__EdgeRef__EXPECTED_HASH = {1, {
    0x3d, 0x64, 0x47, 0xdf, 0x6c, 0x61, 0x15, 0xee,
    0xf5, 0x86, 0xb8, 0xd5, 0x52, 0x81, 0xc7, 0xac,
    0x5b, 0x0b, 0x43, 0x0d, 0xd5, 0xad, 0xd7, 0x31,
    0x6e, 0xf7, 0x12, 0x39, 0x0a, 0x5f, 0x48, 0x9c,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__RoadmapEdge__EXPECTED_HASH = {1, {
    0x2e, 0x57, 0x0c, 0xef, 0x4c, 0x3f, 0x3f, 0xa2,
    0xf2, 0xa1, 0x14, 0x51, 0xba, 0xf4, 0x42, 0xac,
    0x0a, 0x9c, 0x94, 0xbd, 0x99, 0x01, 0xf1, 0x77,
    0x88, 0x1d, 0xef, 0x70, 0x9f, 0x33, 0xbb, 0xef,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__RoadmapNode__EXPECTED_HASH = {1, {
    0x2e, 0x93, 0x33, 0xfa, 0xea, 0x63, 0x9b, 0x18,
    0x44, 0x11, 0xd9, 0x5a, 0x96, 0x03, 0xcd, 0xa9,
    0x89, 0xf0, 0xf3, 0xd0, 0x3c, 0x13, 0x21, 0x4d,
    0x3f, 0xe9, 0x67, 0x37, 0x90, 0x8b, 0x49, 0x33,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__RoadmapProvenance__EXPECTED_HASH = {1, {
    0x3e, 0xe5, 0x5c, 0xcd, 0x97, 0x3e, 0x43, 0x60,
    0x50, 0xbd, 0xf2, 0xdc, 0x6a, 0xf2, 0x4e, 0x6a,
    0x90, 0x07, 0x35, 0x6e, 0xbc, 0x4b, 0xa3, 0xf4,
    0x07, 0xa9, 0xc0, 0x1d, 0xd7, 0xe4, 0xfa, 0x1d,
  }};
#endif

static char mrs_msgs__msg__Roadmap__TYPE_NAME[] = "mrs_msgs/msg/Roadmap";
static char mrs_msgs__msg__Corridor__TYPE_NAME[] = "mrs_msgs/msg/Corridor";
static char mrs_msgs__msg__EdgeRef__TYPE_NAME[] = "mrs_msgs/msg/EdgeRef";
static char mrs_msgs__msg__RoadmapEdge__TYPE_NAME[] = "mrs_msgs/msg/RoadmapEdge";
static char mrs_msgs__msg__RoadmapNode__TYPE_NAME[] = "mrs_msgs/msg/RoadmapNode";
static char mrs_msgs__msg__RoadmapProvenance__TYPE_NAME[] = "mrs_msgs/msg/RoadmapProvenance";

// Define type names, field names, and default values
static char mrs_msgs__msg__Roadmap__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__nodes[] = "nodes";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__edges[] = "edges";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__endpoints[] = "endpoints";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__corridors[] = "corridors";
static char mrs_msgs__msg__Roadmap__FIELD_NAME__provenance[] = "provenance";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Roadmap__FIELDS[] = {
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__nodes, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RoadmapNode__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__edges, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RoadmapEdge__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__endpoints, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__corridors, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__Corridor__TYPE_NAME, 21, 21},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Roadmap__FIELD_NAME__provenance, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {mrs_msgs__msg__RoadmapProvenance__TYPE_NAME, 30, 30},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__Roadmap__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__Corridor__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapEdge__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapNode__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RoadmapProvenance__TYPE_NAME, 30, 30},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Roadmap__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Roadmap__TYPE_NAME, 20, 20},
      {mrs_msgs__msg__Roadmap__FIELDS, 8, 8},
    },
    {mrs_msgs__msg__Roadmap__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__Corridor__EXPECTED_HASH, mrs_msgs__msg__Corridor__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__Corridor__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__EdgeRef__EXPECTED_HASH, mrs_msgs__msg__EdgeRef__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = mrs_msgs__msg__EdgeRef__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__RoadmapEdge__EXPECTED_HASH, mrs_msgs__msg__RoadmapEdge__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = mrs_msgs__msg__RoadmapEdge__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__RoadmapNode__EXPECTED_HASH, mrs_msgs__msg__RoadmapNode__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = mrs_msgs__msg__RoadmapNode__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__RoadmapProvenance__EXPECTED_HASH, mrs_msgs__msg__RoadmapProvenance__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = mrs_msgs__msg__RoadmapProvenance__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "RoadmapNode[] nodes\n"
  "RoadmapEdge[] edges\n"
  "string[] endpoints\n"
  "Corridor[] corridors\n"
  "RoadmapProvenance provenance";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Roadmap__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Roadmap__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 164, 164},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Roadmap__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Roadmap__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__Corridor__get_individual_type_description_source(NULL);
    sources[2] = *mrs_msgs__msg__EdgeRef__get_individual_type_description_source(NULL);
    sources[3] = *mrs_msgs__msg__RoadmapEdge__get_individual_type_description_source(NULL);
    sources[4] = *mrs_msgs__msg__RoadmapNode__get_individual_type_description_source(NULL);
    sources[5] = *mrs_msgs__msg__RoadmapProvenance__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
