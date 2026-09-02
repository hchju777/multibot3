// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/robot_spec__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__RobotSpec__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x79, 0x80, 0x24, 0x02, 0xf0, 0x8b, 0xd1, 0xa6,
      0x84, 0x48, 0x1c, 0x9c, 0xe1, 0xf9, 0x88, 0xc6,
      0x26, 0x59, 0xa4, 0xca, 0x42, 0x4a, 0x89, 0xce,
      0xeb, 0x9b, 0x1a, 0x7f, 0xa1, 0x7b, 0x36, 0xbe,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__RobotSpec__TYPE_NAME[] = "mrs_msgs/msg/RobotSpec";

// Define type names, field names, and default values
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__avg_traversal_speed_mps[] = "avg_traversal_speed_mps";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__v_max[] = "v_max";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__brake_decel_min_mps2[] = "brake_decel_min_mps2";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__curvature_max_invm[] = "curvature_max_invm";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__reverse_motion_allowed[] = "reverse_motion_allowed";
static char mrs_msgs__msg__RobotSpec__FIELD_NAME__circumradius_m[] = "circumradius_m";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__RobotSpec__FIELDS[] = {
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__avg_traversal_speed_mps, 23, 23},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__v_max, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__brake_decel_min_mps2, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__curvature_max_invm, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__reverse_motion_allowed, 22, 22},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotSpec__FIELD_NAME__circumradius_m, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__RobotSpec__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__RobotSpec__TYPE_NAME, 22, 22},
      {mrs_msgs__msg__RobotSpec__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string robot\n"
  "float64 avg_traversal_speed_mps\n"
  "float64 v_max\n"
  "float64 brake_decel_min_mps2\n"
  "float64 curvature_max_invm\n"
  "bool reverse_motion_allowed\n"
  "float64 circumradius_m";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__RobotSpec__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__RobotSpec__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 166, 166},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__RobotSpec__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__RobotSpec__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
