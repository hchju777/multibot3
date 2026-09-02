// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/corridor__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Corridor__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x0c, 0x69, 0x4c, 0xb6, 0x33, 0x66, 0x05, 0xd5,
      0x5d, 0x91, 0xf9, 0x06, 0xa4, 0xb6, 0xc2, 0xca,
      0x83, 0xa7, 0x48, 0xa9, 0xf5, 0x8d, 0xd6, 0x0f,
      0x94, 0x09, 0xf9, 0x28, 0x8e, 0x1b, 0x98, 0x11,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/edge_ref__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__EdgeRef__EXPECTED_HASH = {1, {
    0x3d, 0x64, 0x47, 0xdf, 0x6c, 0x61, 0x15, 0xee,
    0xf5, 0x86, 0xb8, 0xd5, 0x52, 0x81, 0xc7, 0xac,
    0x5b, 0x0b, 0x43, 0x0d, 0xd5, 0xad, 0xd7, 0x31,
    0x6e, 0xf7, 0x12, 0x39, 0x0a, 0x5f, 0x48, 0x9c,
  }};
#endif

static char mrs_msgs__msg__Corridor__TYPE_NAME[] = "mrs_msgs/msg/Corridor";
static char mrs_msgs__msg__EdgeRef__TYPE_NAME[] = "mrs_msgs/msg/EdgeRef";

// Define type names, field names, and default values
static char mrs_msgs__msg__Corridor__FIELD_NAME__id[] = "id";
static char mrs_msgs__msg__Corridor__FIELD_NAME__edges[] = "edges";
static char mrs_msgs__msg__Corridor__FIELD_NAME__length_m[] = "length_m";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Corridor__FIELDS[] = {
  {
    {mrs_msgs__msg__Corridor__FIELD_NAME__id, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Corridor__FIELD_NAME__edges, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Corridor__FIELD_NAME__length_m, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__Corridor__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Corridor__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Corridor__TYPE_NAME, 21, 21},
      {mrs_msgs__msg__Corridor__FIELDS, 3, 3},
    },
    {mrs_msgs__msg__Corridor__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__EdgeRef__EXPECTED_HASH, mrs_msgs__msg__EdgeRef__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__EdgeRef__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string id\n"
  "EdgeRef[] edges\n"
  "float64 length_m";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Corridor__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Corridor__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 43, 43},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Corridor__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Corridor__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__EdgeRef__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
