// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotAssignment.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_assignment__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotAssignment__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x1a, 0x08, 0xb6, 0xd6, 0xb1, 0x26, 0x7f, 0xb0,
      0x09, 0x80, 0xb3, 0x6e, 0xd3, 0xfc, 0xac, 0x41,
      0x97, 0x94, 0x27, 0x40, 0x49, 0xf8, 0xbd, 0x7c,
      0xc2, 0x80, 0x93, 0xa3, 0x26, 0xed, 0x0a, 0x50,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/goal__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__Goal__EXPECTED_HASH = {1, {
    0x38, 0x53, 0x88, 0xff, 0x4e, 0x42, 0x60, 0x20,
    0x36, 0x1c, 0x58, 0x8b, 0x6f, 0xd9, 0x86, 0x35,
    0xc6, 0x23, 0x88, 0xa2, 0xd7, 0x09, 0x84, 0xbc,
    0x29, 0xf4, 0xa9, 0x03, 0x0c, 0xe3, 0x66, 0x9a,
  }};
#endif

static char mrs_msgs__msg__RobotAssignment__TYPE_NAME[] = "mrs_msgs/msg/RobotAssignment";
static char mrs_msgs__msg__Goal__TYPE_NAME[] = "mrs_msgs/msg/Goal";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotAssignment__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__RobotAssignment__FIELD_NAME__start[] = "start";
static char mrs_msgs__msg__RobotAssignment__FIELD_NAME__goals[] = "goals";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotAssignment__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotAssignment__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotAssignment__FIELD_NAME__start, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotAssignment__FIELD_NAME__goals, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__Goal__TYPE_NAME, 17, 17},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__RobotAssignment__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__Goal__TYPE_NAME, 17, 17},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotAssignment__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotAssignment__TYPE_NAME, 28, 28},
      {mrs_msgs__msg__RobotAssignment__FIELDS, 3, 3},
    },
    {mrs_msgs__msg__RobotAssignment__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__Goal__EXPECTED_HASH, mrs_msgs__msg__Goal__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__Goal__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string robot\n"
  "string start\n"
  "Goal[] goals";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotAssignment__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotAssignment__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 39, 39},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotAssignment__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotAssignment__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__Goal__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
