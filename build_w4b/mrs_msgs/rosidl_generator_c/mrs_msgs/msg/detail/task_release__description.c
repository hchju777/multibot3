// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/TaskRelease.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/task_release__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__TaskRelease__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5c, 0x3c, 0x4e, 0xa6, 0xf6, 0xe3, 0x1d, 0xad,
      0x69, 0xaa, 0x3f, 0xac, 0x0b, 0xec, 0x9d, 0xc1,
      0x0c, 0x95, 0x10, 0x88, 0x69, 0x1d, 0x21, 0x02,
      0x95, 0x8c, 0x45, 0x59, 0xcb, 0x56, 0x5d, 0xdd,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/task_goal__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__TaskGoal__EXPECTED_HASH = {1, {
    0x83, 0xd1, 0x08, 0x6b, 0x9e, 0x72, 0x03, 0x78,
    0xae, 0x8c, 0xc3, 0xbe, 0xc0, 0xf7, 0x95, 0x1f,
    0x82, 0xff, 0x93, 0x24, 0xf4, 0x10, 0x73, 0x40,
    0x65, 0xda, 0xe1, 0xbe, 0x60, 0xf5, 0x52, 0xfe,
  }};
#endif

static char mrs_msgs__msg__TaskRelease__TYPE_NAME[] = "mrs_msgs/msg/TaskRelease";
static char mrs_msgs__msg__TaskGoal__TYPE_NAME[] = "mrs_msgs/msg/TaskGoal";

// Define type names, field names, and default values
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__release_index[] = "release_index";
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__task[] = "task";
static char mrs_msgs__msg__TaskRelease__FIELD_NAME__goals[] = "goals";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__TaskRelease__FIELDS[] = {
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__release_index, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__task, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskRelease__FIELD_NAME__goals, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__TaskGoal__TYPE_NAME, 21, 21},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__TaskRelease__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__TaskGoal__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__TaskRelease__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__TaskRelease__TYPE_NAME, 24, 24},
      {mrs_msgs__msg__TaskRelease__FIELDS, 6, 6},
    },
    {mrs_msgs__msg__TaskRelease__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__TaskGoal__EXPECTED_HASH, mrs_msgs__msg__TaskGoal__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__TaskGoal__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "uint64 release_index\n"
  "string task\n"
  "TaskGoal[] goals";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__TaskRelease__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__TaskRelease__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 105, 105},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__TaskRelease__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__TaskRelease__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__TaskGoal__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
