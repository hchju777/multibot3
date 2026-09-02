// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/ExecutionConstraints.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/execution_constraints__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__ExecutionConstraints__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x2d, 0xa6, 0xb3, 0xe4, 0x8b, 0xeb, 0x2d, 0x39,
      0x12, 0x45, 0x07, 0xd5, 0xbe, 0x1d, 0x4b, 0x1f,
      0x55, 0x89, 0xc4, 0x3e, 0x7c, 0x90, 0x8d, 0xfa,
      0xd5, 0x71, 0xa4, 0x6b, 0x5c, 0x1e, 0x0f, 0x18,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/segment__functions.h"
#include "mrs_msgs/msg/detail/alternative__functions.h"
#include "mrs_msgs/msg/detail/switch_group__functions.h"
#include "mrs_msgs/msg/detail/dependency__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__Alternative__EXPECTED_HASH = {1, {
    0xae, 0xdb, 0xda, 0x80, 0xc6, 0x64, 0xb6, 0xa0,
    0x28, 0xe5, 0x73, 0xb7, 0xf7, 0x97, 0x46, 0x68,
    0x8d, 0xc5, 0xfc, 0xc3, 0x7a, 0xa6, 0xc8, 0x12,
    0xcc, 0xcd, 0x2c, 0xf7, 0x85, 0x4c, 0x34, 0x50,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__Dependency__EXPECTED_HASH = {1, {
    0x95, 0xaa, 0x3e, 0xba, 0xce, 0x79, 0x3c, 0xa7,
    0x7c, 0x07, 0xb5, 0x68, 0x38, 0x85, 0xe8, 0x18,
    0x11, 0xef, 0xed, 0x45, 0x8f, 0xff, 0x8b, 0x00,
    0x98, 0xf0, 0x0d, 0x66, 0x64, 0xa0, 0x3e, 0x37,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__Segment__EXPECTED_HASH = {1, {
    0xe5, 0x30, 0xc1, 0x49, 0x19, 0x91, 0x3e, 0xa3,
    0xa7, 0x6e, 0x86, 0xab, 0x8d, 0x2c, 0xb4, 0x99,
    0x45, 0xb1, 0x0e, 0x11, 0xe9, 0x9f, 0x92, 0x34,
    0xa1, 0x21, 0xcb, 0x98, 0x87, 0x06, 0x37, 0x7c,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__SwitchGroup__EXPECTED_HASH = {1, {
    0x7e, 0x5a, 0x53, 0x26, 0x8d, 0x58, 0x42, 0x61,
    0x95, 0xd5, 0x46, 0xc7, 0x93, 0x52, 0xb0, 0xda,
    0x51, 0xd7, 0xf5, 0x4a, 0x01, 0x80, 0x55, 0x89,
    0x9f, 0x22, 0x59, 0x01, 0x59, 0x7d, 0x73, 0x62,
  }};
#endif

static char mrs_msgs__msg__ExecutionConstraints__TYPE_NAME[] = "mrs_msgs/msg/ExecutionConstraints";
static char mrs_msgs__msg__Alternative__TYPE_NAME[] = "mrs_msgs/msg/Alternative";
static char mrs_msgs__msg__Dependency__TYPE_NAME[] = "mrs_msgs/msg/Dependency";
static char mrs_msgs__msg__Segment__TYPE_NAME[] = "mrs_msgs/msg/Segment";
static char mrs_msgs__msg__SwitchGroup__TYPE_NAME[] = "mrs_msgs/msg/SwitchGroup";

// Define type names, field names, and default values
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__commit_seq[] = "commit_seq";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__segments[] = "segments";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__dependencies[] = "dependencies";
static char mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__switch_groups[] = "switch_groups";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__ExecutionConstraints__FIELDS[] = {
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__commit_seq, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__segments, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__Segment__TYPE_NAME, 20, 20},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__dependencies, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__Dependency__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__ExecutionConstraints__FIELD_NAME__switch_groups, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__SwitchGroup__TYPE_NAME, 24, 24},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__ExecutionConstraints__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__Alternative__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Dependency__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__Segment__TYPE_NAME, 20, 20},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__SwitchGroup__TYPE_NAME, 24, 24},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__ExecutionConstraints__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__ExecutionConstraints__TYPE_NAME, 33, 33},
      {mrs_msgs__msg__ExecutionConstraints__FIELDS, 7, 7},
    },
    {mrs_msgs__msg__ExecutionConstraints__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__Alternative__EXPECTED_HASH, mrs_msgs__msg__Alternative__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__Alternative__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__Dependency__EXPECTED_HASH, mrs_msgs__msg__Dependency__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = mrs_msgs__msg__Dependency__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__Segment__EXPECTED_HASH, mrs_msgs__msg__Segment__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = mrs_msgs__msg__Segment__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__SwitchGroup__EXPECTED_HASH, mrs_msgs__msg__SwitchGroup__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = mrs_msgs__msg__SwitchGroup__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "uint64 commit_seq\n"
  "Segment[] segments\n"
  "Dependency[] dependencies\n"
  "SwitchGroup[] switch_groups";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__ExecutionConstraints__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__ExecutionConstraints__TYPE_NAME, 33, 33},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 146, 146},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__ExecutionConstraints__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__ExecutionConstraints__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__Alternative__get_individual_type_description_source(NULL);
    sources[2] = *mrs_msgs__msg__Dependency__get_individual_type_description_source(NULL);
    sources[3] = *mrs_msgs__msg__Segment__get_individual_type_description_source(NULL);
    sources[4] = *mrs_msgs__msg__SwitchGroup__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
