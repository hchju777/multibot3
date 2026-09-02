// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotPlan.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_plan__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotPlan__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x62, 0xea, 0x63, 0x50, 0x01, 0x41, 0x89, 0x9f,
      0x7b, 0x3a, 0x58, 0x6e, 0xa8, 0x89, 0x3a, 0xf7,
      0xed, 0x0c, 0x4f, 0xcb, 0x8b, 0xf1, 0x7c, 0x5e,
      0x56, 0x8f, 0xc6, 0x23, 0x0e, 0xa3, 0x9a, 0x83,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/plan_step__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__PlanStep__EXPECTED_HASH = {1, {
    0xa1, 0x67, 0x52, 0xe3, 0xcc, 0x54, 0x20, 0xc4,
    0x98, 0x1b, 0x60, 0x63, 0xc5, 0x26, 0xb1, 0x26,
    0x8f, 0x93, 0xdb, 0x45, 0xd3, 0xd3, 0x6b, 0xd6,
    0x4f, 0x7b, 0x21, 0x0c, 0x0a, 0xe5, 0x44, 0x00,
  }};
#endif

static char mrs_msgs__msg__RobotPlan__TYPE_NAME[] = "mrs_msgs/msg/RobotPlan";
static char mrs_msgs__msg__PlanStep__TYPE_NAME[] = "mrs_msgs/msg/PlanStep";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotPlan__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__RobotPlan__FIELD_NAME__terminal[] = "terminal";
static char mrs_msgs__msg__RobotPlan__FIELD_NAME__steps[] = "steps";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotPlan__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotPlan__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotPlan__FIELD_NAME__terminal, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotPlan__FIELD_NAME__steps, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__PlanStep__TYPE_NAME, 21, 21},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__RobotPlan__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__PlanStep__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotPlan__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotPlan__TYPE_NAME, 22, 22},
      {mrs_msgs__msg__RobotPlan__FIELDS, 3, 3},
    },
    {mrs_msgs__msg__RobotPlan__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__PlanStep__EXPECTED_HASH, mrs_msgs__msg__PlanStep__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__PlanStep__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# PlanTerminal vocabulary (350 \\xc2\\xa73-2 V2~V6).\n"
  "uint8 TERMINAL_UNKNOWN=0\n"
  "uint8 TERMINAL_GOAL_REACHED=1\n"
  "uint8 TERMINAL_PLAN_TRUNCATED=2\n"
  "uint8 TERMINAL_NO_PROGRESS=3\n"
  "\n"
  "string robot\n"
  "uint8 terminal\n"
  "PlanStep[] steps";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotPlan__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotPlan__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 206, 206},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotPlan__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotPlan__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__PlanStep__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
