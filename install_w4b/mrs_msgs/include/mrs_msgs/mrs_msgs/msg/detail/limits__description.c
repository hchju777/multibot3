// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/limits__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__Limits__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xe9, 0x1b, 0x6f, 0x32, 0xe6, 0x06, 0x0e, 0x85,
      0x56, 0x7c, 0xe8, 0xc8, 0xdd, 0x30, 0xc2, 0x9f,
      0x5e, 0x18, 0x12, 0x10, 0x73, 0xe6, 0xc7, 0x79,
      0xbe, 0xe3, 0xc4, 0xa8, 0x87, 0xc1, 0x51, 0x64,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__Limits__TYPE_NAME[] = "mrs_msgs/msg/Limits";

// Define type names, field names, and default values
static char mrs_msgs__msg__Limits__FIELD_NAME__a_max[] = "a_max";
static char mrs_msgs__msg__Limits__FIELD_NAME__j_max[] = "j_max";
static char mrs_msgs__msg__Limits__FIELD_NAME__min_separation_m[] = "min_separation_m";
static char mrs_msgs__msg__Limits__FIELD_NAME__omega_max_radps[] = "omega_max_radps";
static char mrs_msgs__msg__Limits__FIELD_NAME__yaw_accel_max_radps2[] = "yaw_accel_max_radps2";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__Limits__FIELDS[] = {
  {
    {mrs_msgs__msg__Limits__FIELD_NAME__a_max, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Limits__FIELD_NAME__j_max, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Limits__FIELD_NAME__min_separation_m, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Limits__FIELD_NAME__omega_max_radps, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Limits__FIELD_NAME__yaw_accel_max_radps2, 20, 20},
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
mrs_msgs__msg__Limits__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__Limits__TYPE_NAME, 19, 19},
      {mrs_msgs__msg__Limits__FIELDS, 5, 5},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Fleet-folded physical limits only (mrs.robot_specs carries the per-robot\n"
  "# four: v_max, brake_decel_min_mps2, curvature_max_invm,\n"
  "# reverse_motion_allowed \\xe2\\x80\\x94 do NOT duplicate them here, 350/roadmap-style\n"
  "# single-source-of-truth rule).\n"
  "float64 a_max\n"
  "float64 j_max\n"
  "float64 min_separation_m\n"
  "float64 omega_max_radps\n"
  "float64 yaw_accel_max_radps2";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__Limits__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__Limits__TYPE_NAME, 19, 19},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 343, 343},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__Limits__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__Limits__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
