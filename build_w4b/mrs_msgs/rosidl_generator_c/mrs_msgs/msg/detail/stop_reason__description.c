// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/StopReason.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/stop_reason__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__StopReason__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x8e, 0x4d, 0xc2, 0x2c, 0x89, 0xd2, 0xe6, 0xdb,
      0xf1, 0xa7, 0xdb, 0x60, 0x93, 0x28, 0xc2, 0xc0,
      0x43, 0xcc, 0x1d, 0xd6, 0x68, 0xf5, 0x6e, 0xe5,
      0xe4, 0xa7, 0x71, 0x34, 0x04, 0xe4, 0xa7, 0x71,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char mrs_msgs__msg__StopReason__TYPE_NAME[] = "mrs_msgs/msg/StopReason";

// Define type names, field names, and default values
static char mrs_msgs__msg__StopReason__FIELD_NAME__structure_needs_at_least_one_member[] = "structure_needs_at_least_one_member";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__StopReason__FIELDS[] = {
  {
    {mrs_msgs__msg__StopReason__FIELD_NAME__structure_needs_at_least_one_member, 35, 35},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__StopReason__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__StopReason__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__StopReason__FIELDS, 1, 1},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Shared closed vocabulary for mrs.stop_declaration.declarations[].reason\n"
  "# (legal subset {1,3,5}) and mrs.replan_request.reason (legal subset\n"
  "# {1,2,3,4,5,6}) \\xe2\\x80\\x94 350 \\xc2\\xa73-2 V1, D1-c. ONE numbering, not two.\n"
  "# \\xf0\\x9f\\x94\\xb4 [\\xec\\xb6\\x94\\xec\\xa0\\x95] whether rosidl accepts a constants-only, zero-field .msg is\n"
  "# UNCONFIRMED before the first colcon build (350 \\xc2\\xa73-4). If rejected, the\n"
  "# documented fallback is to declare these seven constants inside\n"
  "# StopDeclaration.msg instead (350 \\xc2\\xa73-4) \\xe2\\x80\\x94 not applied here pre-emptively.\n"
  "uint8 REASON_UNKNOWN=0\n"
  "uint8 REASON_EXOGENOUS_BLOCK=1\n"
  "uint8 REASON_CTRL_STOP_DECLARED=2\n"
  "uint8 REASON_INFEASIBLE_SUBGOAL=3\n"
  "uint8 REASON_ORIENTATION_FLIP=4\n"
  "uint8 REASON_UNRESOLVABLE_LOCALLY=5\n"
  "uint8 REASON_HORIZON_EXHAUSTING=6";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__StopReason__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__StopReason__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 711, 711},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__StopReason__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__StopReason__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
