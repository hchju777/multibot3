// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/TaskGoal.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/task_goal__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__TaskGoal__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x83, 0xd1, 0x08, 0x6b, 0x9e, 0x72, 0x03, 0x78,
      0xae, 0x8c, 0xc3, 0xbe, 0xc0, 0xf7, 0x95, 0x1f,
      0x82, 0xff, 0x93, 0x24, 0xf4, 0x10, 0x73, 0x40,
      0x65, 0xda, 0xe1, 0xbe, 0x60, 0xf5, 0x52, 0xfe,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__TaskGoal__TYPE_NAME[] = "mrs_msgs/msg/TaskGoal";

// Define type names, field names, and default values
static char mrs_msgs__msg__TaskGoal__FIELD_NAME__goal_id[] = "goal_id";
static char mrs_msgs__msg__TaskGoal__FIELD_NAME__location[] = "location";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__TaskGoal__FIELDS[] = {
  {
    {mrs_msgs__msg__TaskGoal__FIELD_NAME__goal_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TaskGoal__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__TaskGoal__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__TaskGoal__TYPE_NAME, 21, 21},
      {mrs_msgs__msg__TaskGoal__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# 350 \\xc2\\xa77 \\xea\\xb0\\x9c\\xec\\xa0\\x95 \\xec\\x95\\x88\\xea\\xb1\\xb4 \\xe2\\x91\\xa0 \\xe2\\x80\\x94 task_release.schema.json:31\\xec\\x9d\\x98 \"Goal (\\xea\\xb3\\xb5\\xec\\x9c\\xa0)\"\\xeb\\x8a\\x94 \\xec\\x84\\xb1\\xeb\\xa6\\xbd\\xed\\x95\\x98\\xec\\xa7\\x80\n"
  "# \\xec\\x95\\x8a\\xeb\\x8a\\x94\\xeb\\x8b\\xa4(assignment.Goal\\xea\\xb3\\xbc \\xed\\x95\\x84\\xeb\\x93\\x9c \\xec\\xa7\\x91\\xed\\x95\\xa9\\xec\\x9d\\xb4 \\xeb\\x8b\\xa4\\xeb\\xa5\\xb4\\xeb\\x8b\\xa4). \\xec\\x9d\\xb4 \\xed\\x8c\\x90\\xec\\x9d\\x80 \\xea\\xb0\\x9c\\xec\\xa0\\x95\\xec\\x9d\\x84 \\xec\\xa0\\x81\\xec\\x9a\\xa9\\xed\\x95\\x98\\xec\\xa7\\x80 \\xec\\x95\\x8a\\xea\\xb3\\xa0\n"
  "# TaskGoal\\xeb\\xa1\\x9c \\xeb\\xa0\\x8c\\xeb\\x8d\\x94\\xeb\\xa7\\x81\\xeb\\xa7\\x8c \\xed\\x95\\x9c\\xeb\\x8b\\xa4(350 D6, \\xec\\x98\\xa4\\xeb\\x8a\\x98\\xec\\x9d\\x98 \\xec\\x9e\\x84\\xec\\x8b\\x9c \\xec\\xb2\\x98\\xeb\\xb6\\x84). \\xec\\x8a\\xa4\\xed\\x82\\xa4\\xeb\\xa7\\x88 \\xed\\x85\\x8d\\xec\\x8a\\xa4\\xed\\x8a\\xb8\\xeb\\x8a\\x94 \\xeb\\xaf\\xb8\\xea\\xb0\\x9c\\xec\\xa0\\x95.\n"
  "string goal_id\n"
  "string location";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__TaskGoal__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__TaskGoal__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 204, 204},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__TaskGoal__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__TaskGoal__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
