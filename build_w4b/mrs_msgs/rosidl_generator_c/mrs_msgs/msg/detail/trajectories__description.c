// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Trajectories.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/trajectories__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Trajectories__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x13, 0x53, 0x61, 0x7c, 0x89, 0x65, 0x6f, 0xa7,
      0x42, 0xa0, 0x11, 0x81, 0xc5, 0x95, 0x9e, 0x66,
      0x1f, 0xae, 0x1b, 0x5a, 0x0e, 0xc9, 0x66, 0x6b,
      0x34, 0xa7, 0x44, 0x76, 0x06, 0xda, 0x89, 0x8c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/trajectory_point__functions.h"
#include "mrs_msgs/msg/detail/robot_trajectory__functions.h"
#include "mrs_msgs/msg/detail/limits__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__Limits__EXPECTED_HASH = {1, {
    0xe9, 0x1b, 0x6f, 0x32, 0xe6, 0x06, 0x0e, 0x85,
    0x56, 0x7c, 0xe8, 0xc8, 0xdd, 0x30, 0xc2, 0x9f,
    0x5e, 0x18, 0x12, 0x10, 0x73, 0xe6, 0xc7, 0x79,
    0xbe, 0xe3, 0xc4, 0xa8, 0x87, 0xc1, 0x51, 0x64,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__RobotTrajectory__EXPECTED_HASH = {1, {
    0x88, 0x29, 0x8c, 0xf9, 0x8b, 0x3c, 0xff, 0xee,
    0x06, 0x8d, 0xff, 0x84, 0xa2, 0xcb, 0x9d, 0xaf,
    0x93, 0x68, 0xb5, 0x03, 0x73, 0xa4, 0x28, 0xa7,
    0x12, 0x36, 0x39, 0x87, 0x44, 0xf5, 0x44, 0x14,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__TrajectoryPoint__EXPECTED_HASH = {1, {
    0x28, 0x09, 0x9c, 0x89, 0xe9, 0x35, 0x77, 0x2d,
    0x5f, 0x0c, 0xd0, 0x39, 0x73, 0x3b, 0xa7, 0xd1,
    0x80, 0x75, 0x26, 0x08, 0xed, 0x65, 0x1b, 0x43,
    0x44, 0xce, 0xe2, 0x86, 0x3b, 0xf7, 0xe9, 0xa2,
  }};
#endif

static char mrs_msgs__msg__Trajectories__TYPE_NAME[] = "mrs_msgs/msg/Trajectories";
static char mrs_msgs__msg__Limits__TYPE_NAME[] = "mrs_msgs/msg/Limits";
static char mrs_msgs__msg__RobotTrajectory__TYPE_NAME[] = "mrs_msgs/msg/RobotTrajectory";
static char mrs_msgs__msg__TrajectoryPoint__TYPE_NAME[] = "mrs_msgs/msg/TrajectoryPoint";

// Define type names, field names, and default values
static char mrs_msgs__msg__Trajectories__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__Trajectories__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__Trajectories__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__Trajectories__FIELD_NAME__limits[] = "limits";
static char mrs_msgs__msg__Trajectories__FIELD_NAME__trajectories[] = "trajectories";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Trajectories__FIELDS[] = {
  {
    {mrs_msgs__msg__Trajectories__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Trajectories__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Trajectories__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Trajectories__FIELD_NAME__limits, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {mrs_msgs__msg__Limits__TYPE_NAME, 19, 19},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Trajectories__FIELD_NAME__trajectories, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RobotTrajectory__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__Trajectories__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__Limits__TYPE_NAME, 19, 19},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotTrajectory__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__Trajectories__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Trajectories__TYPE_NAME, 25, 25},
      {mrs_msgs__msg__Trajectories__FIELDS, 5, 5},
    },
    {mrs_msgs__msg__Trajectories__REFERENCED_TYPE_DESCRIPTIONS, 3, 3},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__Limits__EXPECTED_HASH, mrs_msgs__msg__Limits__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__Limits__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__RobotTrajectory__EXPECTED_HASH, mrs_msgs__msg__RobotTrajectory__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = mrs_msgs__msg__RobotTrajectory__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__TrajectoryPoint__EXPECTED_HASH, mrs_msgs__msg__TrajectoryPoint__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = mrs_msgs__msg__TrajectoryPoint__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "Limits limits\n"
  "RobotTrajectory[] trajectories";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Trajectories__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Trajectories__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 100, 100},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Trajectories__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[4];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 4, 4};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Trajectories__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__Limits__get_individual_type_description_source(NULL);
    sources[2] = *mrs_msgs__msg__RobotTrajectory__get_individual_type_description_source(NULL);
    sources[3] = *mrs_msgs__msg__TrajectoryPoint__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
