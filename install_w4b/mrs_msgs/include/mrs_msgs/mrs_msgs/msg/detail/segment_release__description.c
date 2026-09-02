// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/SegmentRelease.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/segment_release__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__SegmentRelease__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x11, 0x4d, 0x37, 0x1e, 0xee, 0xa4, 0x23, 0xb6,
      0x1c, 0xc0, 0x79, 0xa1, 0x0e, 0x11, 0x7a, 0xdd,
      0xb2, 0x4f, 0x8a, 0xab, 0x19, 0x97, 0x03, 0x27,
      0x84, 0xd1, 0xbc, 0x73, 0xd1, 0x42, 0x98, 0xfa,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/segment_release_item__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__SegmentReleaseItem__EXPECTED_HASH = {1, {
    0xbd, 0x9a, 0x60, 0xc8, 0xab, 0x23, 0x9f, 0x79,
    0xf5, 0x49, 0xee, 0x4c, 0x15, 0xe5, 0x9b, 0x7b,
    0x2d, 0x49, 0x99, 0x0a, 0xbe, 0x65, 0x40, 0x34,
    0x06, 0xc1, 0x59, 0x65, 0x48, 0x41, 0x78, 0x32,
  }};
#endif

static char mrs_msgs__msg__SegmentRelease__TYPE_NAME[] = "mrs_msgs/msg/SegmentRelease";
static char mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME[] = "mrs_msgs/msg/SegmentReleaseItem";

// Define type names, field names, and default values
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__robot[] = "robot";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__release_seq[] = "release_seq";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__commit_seq[] = "commit_seq";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__entry_seq_ack[] = "entry_seq_ack";
static char mrs_msgs__msg__SegmentRelease__FIELD_NAME__segments[] = "segments";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__SegmentRelease__FIELDS[] = {
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__robot, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__release_seq, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__commit_seq, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__entry_seq_ack, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SegmentRelease__FIELD_NAME__segments, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME, 31, 31},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__SegmentRelease__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__SegmentReleaseItem__TYPE_NAME, 31, 31},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__SegmentRelease__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__SegmentRelease__TYPE_NAME, 27, 27},
      {mrs_msgs__msg__SegmentRelease__FIELDS, 8, 8},
    },
    {mrs_msgs__msg__SegmentRelease__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__SegmentReleaseItem__EXPECTED_HASH, mrs_msgs__msg__SegmentReleaseItem__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__SegmentReleaseItem__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "string robot\n"
  "uint64 release_seq\n"
  "uint64 commit_seq\n"
  "uint64 entry_seq_ack\n"
  "SegmentReleaseItem[] segments";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__SegmentRelease__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__SegmentRelease__TYPE_NAME, 27, 27},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 156, 156},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__SegmentRelease__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__SegmentRelease__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__SegmentReleaseItem__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
