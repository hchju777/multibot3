// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/EntryEvents.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/entry_events__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__EntryEvents__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x7c, 0xf7, 0x68, 0x27, 0xd0, 0x94, 0xd7, 0x05,
      0x4c, 0xae, 0xa2, 0x82, 0x93, 0xff, 0x88, 0xbf,
      0x4d, 0x59, 0xd6, 0xaa, 0x68, 0x9b, 0x32, 0xf0,
      0x3a, 0x8b, 0xc9, 0x43, 0x27, 0xd0, 0x3e, 0x44,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/entry_event__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__EntryEvent__EXPECTED_HASH = {1, {
    0x94, 0x84, 0xb4, 0x4c, 0x32, 0x4d, 0x82, 0xea,
    0x1e, 0x27, 0xf2, 0x1a, 0x40, 0x7f, 0x1d, 0xf0,
    0x29, 0x6f, 0xa7, 0xcb, 0x5a, 0x50, 0x61, 0x9b,
    0x00, 0x9d, 0x80, 0x58, 0x11, 0x7f, 0xd7, 0xad,
  }};
#endif

static char mrs_msgs__msg__EntryEvents__TYPE_NAME[] = "mrs_msgs/msg/EntryEvents";
static char mrs_msgs__msg__EntryEvent__TYPE_NAME[] = "mrs_msgs/msg/EntryEvent";

// Define type names, field names, and default values
static char mrs_msgs__msg__EntryEvents__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__EntryEvents__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__EntryEvents__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__EntryEvents__FIELD_NAME__events[] = "events";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__EntryEvents__FIELDS[] = {
  {
    {mrs_msgs__msg__EntryEvents__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvents__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvents__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__EntryEvents__FIELD_NAME__events, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__EntryEvent__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__EntryEvents__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__EntryEvent__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__EntryEvents__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__EntryEvents__TYPE_NAME, 24, 24},
      {mrs_msgs__msg__EntryEvents__FIELDS, 4, 4},
    },
    {mrs_msgs__msg__EntryEvents__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__EntryEvent__EXPECTED_HASH, mrs_msgs__msg__EntryEvent__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__EntryEvent__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "EntryEvent[] events";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__EntryEvents__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__EntryEvents__TYPE_NAME, 24, 24},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 75, 75},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__EntryEvents__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__EntryEvents__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__EntryEvent__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
