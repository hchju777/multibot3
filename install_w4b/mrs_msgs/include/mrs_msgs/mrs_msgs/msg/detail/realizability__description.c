// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Realizability.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/realizability__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Realizability__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x45, 0xf0, 0x48, 0x48, 0x31, 0x26, 0x0f, 0x17,
      0xf8, 0x31, 0xe3, 0x07, 0x43, 0xb0, 0xfd, 0xdc,
      0x84, 0x57, 0xc9, 0x61, 0x25, 0x00, 0xba, 0x50,
      0x62, 0x00, 0x43, 0x5f, 0x18, 0x7f, 0xbc, 0xdc,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/realizability_entry__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__RealizabilityEntry__EXPECTED_HASH = {1, {
    0xbb, 0xab, 0x2c, 0x2f, 0xe7, 0xc8, 0x58, 0xb3,
    0x6e, 0x88, 0x4d, 0xa3, 0x00, 0x0c, 0xa4, 0x74,
    0xea, 0x8d, 0x0f, 0xba, 0x3e, 0x69, 0x69, 0x80,
    0x38, 0x31, 0x67, 0xee, 0xa4, 0x9f, 0x30, 0x38,
  }};
#endif

static char mrs_msgs__msg__Realizability__TYPE_NAME[] = "mrs_msgs/msg/Realizability";
static char mrs_msgs__msg__RealizabilityEntry__TYPE_NAME[] = "mrs_msgs/msg/RealizabilityEntry";

// Define type names, field names, and default values
static char mrs_msgs__msg__Realizability__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__Realizability__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__Realizability__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__Realizability__FIELD_NAME__rz_seq[] = "rz_seq";
static char mrs_msgs__msg__Realizability__FIELD_NAME__entries[] = "entries";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Realizability__FIELDS[] = {
  {
    {mrs_msgs__msg__Realizability__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Realizability__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Realizability__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Realizability__FIELD_NAME__rz_seq, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Realizability__FIELD_NAME__entries, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RealizabilityEntry__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__Realizability__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__RealizabilityEntry__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Realizability__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Realizability__TYPE_NAME, 26, 26},
      {mrs_msgs__msg__Realizability__FIELDS, 5, 5},
    },
    {mrs_msgs__msg__Realizability__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__RealizabilityEntry__EXPECTED_HASH, mrs_msgs__msg__RealizabilityEntry__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__RealizabilityEntry__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "uint64 rz_seq\n"
  "RealizabilityEntry[] entries";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Realizability__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Realizability__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 98, 98},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Realizability__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Realizability__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__RealizabilityEntry__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
