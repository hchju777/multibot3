// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/OperatorCommand.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/operator_command__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__OperatorCommand__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf7, 0x5f, 0xec, 0x20, 0xdd, 0xbb, 0x45, 0x0e,
      0x7f, 0x43, 0x1b, 0x49, 0xca, 0xc4, 0x29, 0x1e,
      0x84, 0x28, 0x6a, 0x3a, 0x82, 0xe1, 0x85, 0x5a,
      0xa6, 0x28, 0x4f, 0x73, 0x4b, 0x65, 0x36, 0x4a,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__OperatorCommand__TYPE_NAME[] = "mrs_msgs/msg/OperatorCommand";

// Define type names, field names, and default values
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__seq[] = "seq";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__verb[] = "verb";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__active[] = "active";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__speed_cap_mps[] = "speed_cap_mps";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__has_speed_cap_mps[] = "has_speed_cap_mps";
static char mrs_msgs__msg__OperatorCommand__FIELD_NAME__goal_location[] = "goal_location";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__OperatorCommand__FIELDS[] = {
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__seq, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__verb, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__active, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__speed_cap_mps, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__has_speed_cap_mps, 17, 17},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__OperatorCommand__FIELD_NAME__goal_location, 13, 13},
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
mrs_msgs__msg__OperatorCommand__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__OperatorCommand__TYPE_NAME, 28, 28},
      {mrs_msgs__msg__OperatorCommand__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# OperatorVerb vocabulary (350 \\xc2\\xa73-2 V2~V6).\n"
  "uint8 VERB_UNKNOWN=0\n"
  "uint8 VERB_HOLD=1\n"
  "uint8 VERB_SPEED_CAP=2\n"
  "uint8 VERB_GOAL_OVERRIDE=3\n"
  "\n"
  "uint64 seq\n"
  "string robot\n"
  "uint8 verb\n"
  "bool active\n"
  "# Optional numeric field: has_* companion (350 \\xc2\\xa74-3). Required only when\n"
  "# verb==VERB_SPEED_CAP and active==true (layer-2 condition, not layer-1).\n"
  "float64 speed_cap_mps\n"
  "bool has_speed_cap_mps\n"
  "# Optional STRING field: \"\" sentinel, NOT a has_* flag (350 \\xc2\\xa74-3 rule \\xe2\\x80\\x94 this\n"
  "# message is the one place both conventions coexist). Required only when\n"
  "# verb==VERB_GOAL_OVERRIDE and active==true.\n"
  "string goal_location";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__OperatorCommand__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__OperatorCommand__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 589, 589},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__OperatorCommand__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__OperatorCommand__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
