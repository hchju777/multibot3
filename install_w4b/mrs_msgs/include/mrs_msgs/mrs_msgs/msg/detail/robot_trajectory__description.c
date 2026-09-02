// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotTrajectory.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_trajectory__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotTrajectory__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x88, 0x29, 0x8c, 0xf9, 0x8b, 0x3c, 0xff, 0xee,
      0x06, 0x8d, 0xff, 0x84, 0xa2, 0xcb, 0x9d, 0xaf,
      0x93, 0x68, 0xb5, 0x03, 0x73, 0xa4, 0x28, 0xa7,
      0x12, 0x36, 0x39, 0x87, 0x44, 0xf5, 0x44, 0x14,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/trajectory_point__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__TrajectoryPoint__EXPECTED_HASH = {1, {
    0x28, 0x09, 0x9c, 0x89, 0xe9, 0x35, 0x77, 0x2d,
    0x5f, 0x0c, 0xd0, 0x39, 0x73, 0x3b, 0xa7, 0xd1,
    0x80, 0x75, 0x26, 0x08, 0xed, 0x65, 0x1b, 0x43,
    0x44, 0xce, 0xe2, 0x86, 0x3b, 0xf7, 0xe9, 0xa2,
  }};
#endif

static char mrs_msgs__msg__RobotTrajectory__TYPE_NAME[] = "mrs_msgs/msg/RobotTrajectory";
static char mrs_msgs__msg__TrajectoryPoint__TYPE_NAME[] = "mrs_msgs/msg/TrajectoryPoint";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotTrajectory__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__RobotTrajectory__FIELD_NAME__points[] = "points";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotTrajectory__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotTrajectory__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotTrajectory__FIELD_NAME__points, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__TrajectoryPoint__TYPE_NAME, 28, 28},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__RobotTrajectory__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__TrajectoryPoint__TYPE_NAME, 28, 28},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotTrajectory__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotTrajectory__TYPE_NAME, 28, 28},
      {mrs_msgs__msg__RobotTrajectory__FIELDS, 2, 2},
    },
    {mrs_msgs__msg__RobotTrajectory__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__TrajectoryPoint__EXPECTED_HASH, mrs_msgs__msg__TrajectoryPoint__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__TrajectoryPoint__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string robot\n"
  "TrajectoryPoint[] points";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotTrajectory__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotTrajectory__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 38, 38},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotTrajectory__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotTrajectory__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__TrajectoryPoint__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
