// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/EdgeRef.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/edge_ref__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__EdgeRef__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x3d, 0x64, 0x47, 0xdf, 0x6c, 0x61, 0x15, 0xee,
      0xf5, 0x86, 0xb8, 0xd5, 0x52, 0x81, 0xc7, 0xac,
      0x5b, 0x0b, 0x43, 0x0d, 0xd5, 0xad, 0xd7, 0x31,
      0x6e, 0xf7, 0x12, 0x39, 0x0a, 0x5f, 0x48, 0x9c,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__EdgeRef__TYPE_NAME[] = "mrs_msgs/msg/EdgeRef";

// Define type names, field names, and default values
static char mrs_msgs__msg__EdgeRef__FIELD_NAME__from_id[] = "from_id";
static char mrs_msgs__msg__EdgeRef__FIELD_NAME__to_id[] = "to_id";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__EdgeRef__FIELDS[] = {
  {
    {mrs_msgs__msg__EdgeRef__FIELD_NAME__from_id, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EdgeRef__FIELD_NAME__to_id, 5, 5},
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
mrs_msgs__msg__EdgeRef__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
      {mrs_msgs__msg__EdgeRef__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Directed pair of roadmap-node identifiers reused across three JSON shapes\n"
  "# (350 \\xc2\\xa76 \"\\xec\\x9e\\xac\\xec\\x82\\xac\\xec\\x9a\\xa9 \\xea\\xb2\\xb0\\xec\\xa0\\x95 \\xeb\\x91\\x98\"): roadmap.corridors[].edges (2-element tuple),\n"
  "# stop_declaration.blocked_edge (object {from,to}), replan_request.blocked[]\n"
  "# (object {from,to}). The io/ adapter absorbs both JSON shapes into this one\n"
  "# type (350 \\xc2\\xa78 D6 item 8) \\xe2\\x80\\x94 that adapter code is outside this round's scope.\n"
  "string from_id\n"
  "string to_id";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__EdgeRef__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 403, 403},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__EdgeRef__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__EdgeRef__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
