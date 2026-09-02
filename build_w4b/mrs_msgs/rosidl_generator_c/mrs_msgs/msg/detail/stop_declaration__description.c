// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/stop_declaration__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__StopDeclaration__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x67, 0xfe, 0xe2, 0xfb, 0x3b, 0xd3, 0xce, 0xdd,
      0x3f, 0x4c, 0x52, 0xc1, 0xff, 0x03, 0x31, 0xd9,
      0x72, 0x8d, 0x08, 0xd4, 0x97, 0x60, 0x55, 0x04,
      0xd4, 0x5d, 0x68, 0xce, 0x00, 0x8f, 0x03, 0xb0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/edge_ref__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__EdgeRef__EXPECTED_HASH = {1, {
    0x3d, 0x64, 0x47, 0xdf, 0x6c, 0x61, 0x15, 0xee,
    0xf5, 0x86, 0xb8, 0xd5, 0x52, 0x81, 0xc7, 0xac,
    0x5b, 0x0b, 0x43, 0x0d, 0xd5, 0xad, 0xd7, 0x31,
    0x6e, 0xf7, 0x12, 0x39, 0x0a, 0x5f, 0x48, 0x9c,
  }};
#endif

static char mrs_msgs__msg__StopDeclaration__TYPE_NAME[] = "mrs_msgs/msg/StopDeclaration";
static char mrs_msgs__msg__EdgeRef__TYPE_NAME[] = "mrs_msgs/msg/EdgeRef";

// Define type names, field names, and default values
static char mrs_msgs__msg__StopDeclaration__FIELD_NAME__seq[] = "seq";
static char mrs_msgs__msg__StopDeclaration__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__StopDeclaration__FIELD_NAME__blocked_edge[] = "blocked_edge";
static char mrs_msgs__msg__StopDeclaration__FIELD_NAME__declared[] = "declared";
static char mrs_msgs__msg__StopDeclaration__FIELD_NAME__reason[] = "reason";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__StopDeclaration__FIELDS[] = {
  {
    {mrs_msgs__msg__StopDeclaration__FIELD_NAME__seq, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclaration__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclaration__FIELD_NAME__blocked_edge, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclaration__FIELD_NAME__declared, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__StopDeclaration__FIELD_NAME__reason, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__StopDeclaration__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__EdgeRef__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__StopDeclaration__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__StopDeclaration__TYPE_NAME, 28, 28},
      {mrs_msgs__msg__StopDeclaration__FIELDS, 5, 5},
    },
    {mrs_msgs__msg__StopDeclaration__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__EdgeRef__EXPECTED_HASH, mrs_msgs__msg__EdgeRef__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__EdgeRef__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# reason uses the SHARED StopReason vocabulary (mrs_msgs/msg/StopReason.msg,\n"
  "# 350 \\xc2\\xa73-2 V1). This channel's legal subset is {1,3,5} \\xe2\\x80\\x94 NON-contiguous.\n"
  "# \\xf0\\x9f\\x94\\xb4 Do not validate with a range check (350 \\xc2\\xa73-2, \\xc2\\xa74). Validate by set\n"
  "# membership against the {1,3,5} subset.\n"
  "uint64 seq\n"
  "string robot\n"
  "EdgeRef blocked_edge\n"
  "bool declared\n"
  "uint8 reason";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__StopDeclaration__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__StopDeclaration__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 334, 334},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__StopDeclaration__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__StopDeclaration__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__EdgeRef__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
