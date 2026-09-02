// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/GoalCompletion.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/goal_completion__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__GoalCompletion__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x4b, 0x2d, 0x82, 0x54, 0x63, 0x89, 0xcf, 0xf8,
      0x60, 0x51, 0x33, 0x40, 0x8c, 0x3c, 0x62, 0xca,
      0x49, 0x1f, 0xae, 0x9c, 0x3a, 0xf7, 0xa2, 0x55,
      0x4d, 0x7e, 0xc3, 0x84, 0x68, 0xf7, 0x87, 0x94,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__GoalCompletion__TYPE_NAME[] = "mrs_msgs/msg/GoalCompletion";

// Define type names, field names, and default values
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__seq[] = "seq";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__goal_id[] = "goal_id";
static char mrs_msgs__msg__GoalCompletion__FIELD_NAME__revision[] = "revision";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__GoalCompletion__FIELDS[] = {
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__seq, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__goal_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__GoalCompletion__FIELD_NAME__revision, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__GoalCompletion__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__GoalCompletion__TYPE_NAME, 27, 27},
      {mrs_msgs__msg__GoalCompletion__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "uint64 seq\n"
  "string robot\n"
  "string goal_id\n"
  "uint64 revision";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__GoalCompletion__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__GoalCompletion__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 110, 110},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__GoalCompletion__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__GoalCompletion__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
