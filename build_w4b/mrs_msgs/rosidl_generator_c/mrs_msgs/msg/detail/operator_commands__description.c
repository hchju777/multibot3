// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/OperatorCommands.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/operator_commands__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__OperatorCommands__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xee, 0x10, 0x3c, 0x0d, 0xbe, 0x08, 0x65, 0xf3,
      0x96, 0x45, 0xb5, 0xb1, 0x1d, 0xe6, 0xfe, 0x50,
      0x30, 0xc4, 0x40, 0xd0, 0xae, 0x17, 0x39, 0xf0,
      0xd3, 0x7f, 0x13, 0x1c, 0xcf, 0x1c, 0x93, 0xf5,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/operator_command__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__OperatorCommand__EXPECTED_HASH = {1, {
    0xf7, 0x5f, 0xec, 0x20, 0xdd, 0xbb, 0x45, 0x0e,
    0x7f, 0x43, 0x1b, 0x49, 0xca, 0xc4, 0x29, 0x1e,
    0x84, 0x28, 0x6a, 0x3a, 0x82, 0xe1, 0x85, 0x5a,
    0xa6, 0x28, 0x4f, 0x73, 0x4b, 0x65, 0x36, 0x4a,
  }};
#endif

static char mrs_msgs__msg__OperatorCommands__TYPE_NAME[] = "mrs_msgs/msg/OperatorCommands";
static char mrs_msgs__msg__OperatorCommand__TYPE_NAME[] = "mrs_msgs/msg/OperatorCommand";

// Define type names, field names, and default values
static char mrs_msgs__msg__OperatorCommands__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__OperatorCommands__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__OperatorCommands__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__OperatorCommands__FIELD_NAME__commands[] = "commands";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__OperatorCommands__FIELDS[] = {
  {
    {mrs_msgs__msg__OperatorCommands__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommands__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommands__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommands__FIELD_NAME__commands, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__OperatorCommand__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__OperatorCommands__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__OperatorCommand__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__OperatorCommands__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__OperatorCommands__TYPE_NAME, 29, 29},
      {mrs_msgs__msg__OperatorCommands__FIELDS, 4, 4},
    },
    {mrs_msgs__msg__OperatorCommands__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__OperatorCommand__EXPECTED_HASH, mrs_msgs__msg__OperatorCommand__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__OperatorCommand__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "OperatorCommand[] commands";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__OperatorCommands__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__OperatorCommands__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 82, 82},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__OperatorCommands__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__OperatorCommands__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__OperatorCommand__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
