// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/TrajectoryPoint.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/trajectory_point__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__TrajectoryPoint__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x28, 0x09, 0x9c, 0x89, 0xe9, 0x35, 0x77, 0x2d,
      0x5f, 0x0c, 0xd0, 0x39, 0x73, 0x3b, 0xa7, 0xd1,
      0x80, 0x75, 0x26, 0x08, 0xed, 0x65, 0x1b, 0x43,
      0x44, 0xce, 0xe2, 0x86, 0x3b, 0xf7, 0xe9, 0xa2,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__TrajectoryPoint__TYPE_NAME[] = "mrs_msgs/msg/TrajectoryPoint";

// Define type names, field names, and default values
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__t[] = "t";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__x[] = "x";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__y[] = "y";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__yaw[] = "yaw";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__v[] = "v";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__has_v[] = "has_v";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__a[] = "a";
static char mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__has_a[] = "has_a";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__TrajectoryPoint__FIELDS[] = {
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__t, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__yaw, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__v, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__has_v, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__a, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__TrajectoryPoint__FIELD_NAME__has_a, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__TrajectoryPoint__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__TrajectoryPoint__TYPE_NAME, 28, 28},
      {mrs_msgs__msg__TrajectoryPoint__FIELDS, 8, 8},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float64 t\n"
  "float64 x\n"
  "float64 y\n"
  "float64 yaw\n"
  "# Optional numeric fields: has_* companion booleans (350 \\xc2\\xa74-3 rule). has_* is\n"
  "# NEVER carried in the JSON boundary dump \\xe2\\x80\\x94 absence there IS absence here.\n"
  "float64 v\n"
  "bool has_v\n"
  "float64 a\n"
  "bool has_a";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__TrajectoryPoint__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__TrajectoryPoint__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 237, 237},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__TrajectoryPoint__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__TrajectoryPoint__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
