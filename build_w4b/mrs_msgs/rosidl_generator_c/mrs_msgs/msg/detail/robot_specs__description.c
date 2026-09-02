// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotSpecs.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_specs__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotSpecs__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xea, 0xc6, 0xf0, 0x0b, 0xf6, 0x39, 0xef, 0x1c,
      0xfd, 0x75, 0x6a, 0xcc, 0x8c, 0x08, 0xde, 0x14,
      0x54, 0x53, 0xbc, 0x0b, 0x4c, 0xc5, 0xf9, 0xf9,
      0x45, 0xc8, 0xf6, 0xd1, 0x7c, 0xb3, 0xf5, 0xd6,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/robot_spec__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__RobotSpec__EXPECTED_HASH = {1, {
    0x79, 0x80, 0x24, 0x02, 0xf0, 0x8b, 0xd1, 0xa6,
    0x84, 0x48, 0x1c, 0x9c, 0xe1, 0xf9, 0x88, 0xc6,
    0x26, 0x59, 0xa4, 0xca, 0x42, 0x4a, 0x89, 0xce,
    0xeb, 0x9b, 0x1a, 0x7f, 0xa1, 0x7b, 0x36, 0xbe,
  }};
#endif

static char mrs_msgs__msg__RobotSpecs__TYPE_NAME[] = "mrs_msgs/msg/RobotSpecs";
static char mrs_msgs__msg__RobotSpec__TYPE_NAME[] = "mrs_msgs/msg/RobotSpec";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotSpecs__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__RobotSpecs__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__RobotSpecs__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__RobotSpecs__FIELD_NAME__robots[] = "robots";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotSpecs__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotSpecs__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpecs__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpecs__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpecs__FIELD_NAME__robots, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RobotSpec__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__RobotSpecs__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__RobotSpec__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotSpecs__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotSpecs__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__RobotSpecs__FIELDS, 4, 4},
    },
    {mrs_msgs__msg__RobotSpecs__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__RobotSpec__EXPECTED_HASH, mrs_msgs__msg__RobotSpec__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__RobotSpec__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "RobotSpec[] robots";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotSpecs__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotSpecs__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 74, 74},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotSpecs__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotSpecs__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__RobotSpec__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
