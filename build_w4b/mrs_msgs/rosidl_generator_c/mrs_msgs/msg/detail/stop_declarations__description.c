// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/StopDeclarations.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/stop_declarations__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__StopDeclarations__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x85, 0x28, 0x54, 0x32, 0x90, 0xaa, 0x43, 0x56,
      0x37, 0x32, 0x92, 0x7e, 0x70, 0x5e, 0x3f, 0x96,
      0xde, 0x57, 0x20, 0x9a, 0x36, 0x3a, 0x2d, 0xeb,
      0x6d, 0x62, 0x3a, 0x2d, 0x90, 0x66, 0xf1, 0x2a,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/stop_declaration__functions.h"
#include "mrs_msgs/msg/detail/edge_ref__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__EdgeRef__EXPECTED_HASH = {1, {
    0x3d, 0x64, 0x47, 0xdf, 0x6c, 0x61, 0x15, 0xee,
    0xf5, 0x86, 0xb8, 0xd5, 0x52, 0x81, 0xc7, 0xac,
    0x5b, 0x0b, 0x43, 0x0d, 0xd5, 0xad, 0xd7, 0x31,
    0x6e, 0xf7, 0x12, 0x39, 0x0a, 0x5f, 0x48, 0x9c,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__StopDeclaration__EXPECTED_HASH = {1, {
    0x67, 0xfe, 0xe2, 0xfb, 0x3b, 0xd3, 0xce, 0xdd,
    0x3f, 0x4c, 0x52, 0xc1, 0xff, 0x03, 0x31, 0xd9,
    0x72, 0x8d, 0x08, 0xd4, 0x97, 0x60, 0x55, 0x04,
    0xd4, 0x5d, 0x68, 0xce, 0x00, 0x8f, 0x03, 0xb0,
  }};
#endif

static char mrs_msgs__msg__StopDeclarations__TYPE_NAME[] = "mrs_msgs/msg/StopDeclarations";
static char mrs_msgs__msg__EdgeRef__TYPE_NAME[] = "mrs_msgs/msg/EdgeRef";
static char mrs_msgs__msg__StopDeclaration__TYPE_NAME[] = "mrs_msgs/msg/StopDeclaration";

// Define type names, field names, and default values
static char mrs_msgs__msg__StopDeclarations__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__StopDeclarations__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__StopDeclarations__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__StopDeclarations__FIELD_NAME__declarations[] = "declarations";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__StopDeclarations__FIELDS[] = {
  {
    {mrs_msgs__msg__StopDeclarations__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclarations__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclarations__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclarations__FIELD_NAME__declarations, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__StopDeclaration__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__StopDeclarations__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclaration__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__StopDeclarations__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__StopDeclarations__TYPE_NAME, 29, 29},
      {mrs_msgs__msg__StopDeclarations__FIELDS, 4, 4},
    },
    {mrs_msgs__msg__StopDeclarations__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__EdgeRef__EXPECTED_HASH, mrs_msgs__msg__EdgeRef__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__EdgeRef__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__StopDeclaration__EXPECTED_HASH, mrs_msgs__msg__StopDeclaration__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = mrs_msgs__msg__StopDeclaration__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "StopDeclaration[] declarations";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__StopDeclarations__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__StopDeclarations__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 86, 86},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__StopDeclarations__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__StopDeclarations__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__EdgeRef__get_individual_type_description_source(NULL);
    sources[2] = *mrs_msgs__msg__StopDeclaration__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
