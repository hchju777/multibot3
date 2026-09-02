// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/PlanStep.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/plan_step__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__PlanStep__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa1, 0x67, 0x52, 0xe3, 0xcc, 0x54, 0x20, 0xc4,
      0x98, 0x1b, 0x60, 0x63, 0xc5, 0x26, 0xb1, 0x26,
      0x8f, 0x93, 0xdb, 0x45, 0xd3, 0xd3, 0x6b, 0xd6,
      0x4f, 0x7b, 0x21, 0x0c, 0x0a, 0xe5, 0x44, 0x00,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__PlanStep__TYPE_NAME[] = "mrs_msgs/msg/PlanStep";

// Define type names, field names, and default values
static char mrs_msgs__msg__PlanStep__FIELD_NAME__index[] = "index";
static char mrs_msgs__msg__PlanStep__FIELD_NAME__location[] = "location";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__PlanStep__FIELDS[] = {
  {
    {mrs_msgs__msg__PlanStep__FIELD_NAME__index, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__PlanStep__FIELD_NAME__location, 8, 8},
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
mrs_msgs__msg__PlanStep__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__PlanStep__TYPE_NAME, 21, 21},
      {mrs_msgs__msg__PlanStep__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "uint32 index\n"
  "string location";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__PlanStep__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__PlanStep__TYPE_NAME, 21, 21},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 29, 29},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__PlanStep__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__PlanStep__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
