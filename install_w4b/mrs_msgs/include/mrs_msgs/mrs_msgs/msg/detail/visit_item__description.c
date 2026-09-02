// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/VisitItem.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/visit_item__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__VisitItem__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x50, 0xe2, 0x7f, 0xc4, 0xbd, 0xea, 0xfa, 0xfa,
      0xe6, 0x8d, 0x55, 0x3e, 0xf5, 0xa7, 0x41, 0x5b,
      0x63, 0xad, 0x73, 0xad, 0x0d, 0xb4, 0x1f, 0x60,
      0x3a, 0x2e, 0xdb, 0xe1, 0x8a, 0x8a, 0xf7, 0x13,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__VisitItem__TYPE_NAME[] = "mrs_msgs/msg/VisitItem";

// Define type names, field names, and default values
static char mrs_msgs__msg__VisitItem__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__VisitItem__FIELD_NAME__index[] = "index";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__VisitItem__FIELDS[] = {
  {
    {mrs_msgs__msg__VisitItem__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__VisitItem__FIELD_NAME__index, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__VisitItem__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__VisitItem__TYPE_NAME, 22, 22},
      {mrs_msgs__msg__VisitItem__FIELDS, 2, 2},
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
  "uint32 index";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__VisitItem__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__VisitItem__TYPE_NAME, 22, 22},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 26, 26},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__VisitItem__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__VisitItem__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
