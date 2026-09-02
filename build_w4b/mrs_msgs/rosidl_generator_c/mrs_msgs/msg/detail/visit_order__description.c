// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/VisitOrder.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/visit_order__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__VisitOrder__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x92, 0x48, 0x08, 0x93, 0xf4, 0x1d, 0x51, 0xb6,
      0x03, 0x36, 0x82, 0x21, 0x2f, 0x0f, 0xb9, 0xc6,
      0xaf, 0xb5, 0xb5, 0xb7, 0xf3, 0xd9, 0x13, 0xbb,
      0x0a, 0x03, 0x5e, 0x06, 0x9c, 0xd1, 0xc2, 0xa8,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/visit_item__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__VisitItem__EXPECTED_HASH = {1, {
    0x50, 0xe2, 0x7f, 0xc4, 0xbd, 0xea, 0xfa, 0xfa,
    0xe6, 0x8d, 0x55, 0x3e, 0xf5, 0xa7, 0x41, 0x5b,
    0x63, 0xad, 0x73, 0xad, 0x0d, 0xb4, 0x1f, 0x60,
    0x3a, 0x2e, 0xdb, 0xe1, 0x8a, 0x8a, 0xf7, 0x13,
  }};
#endif

static char mrs_msgs__msg__VisitOrder__TYPE_NAME[] = "mrs_msgs/msg/VisitOrder";
static char mrs_msgs__msg__VisitItem__TYPE_NAME[] = "mrs_msgs/msg/VisitItem";

// Define type names, field names, and default values
static char mrs_msgs__msg__VisitOrder__FIELD_NAME__location[] = "location";
static char mrs_msgs__msg__VisitOrder__FIELD_NAME__sequence[] = "sequence";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__VisitOrder__FIELDS[] = {
  {
    {mrs_msgs__msg__VisitOrder__FIELD_NAME__location, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__VisitOrder__FIELD_NAME__sequence, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__VisitItem__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__VisitOrder__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__VisitItem__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__VisitOrder__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__VisitOrder__TYPE_NAME, 23, 23},
      {mrs_msgs__msg__VisitOrder__FIELDS, 2, 2},
    },
    {mrs_msgs__msg__VisitOrder__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__VisitItem__EXPECTED_HASH, mrs_msgs__msg__VisitItem__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__VisitItem__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Reused in two places with the SAME field set but DIFFERENT values (350 \\xc2\\xa76\n"
  "# \"\\xec\\x9e\\xac\\xec\\x82\\xac\\xec\\x9a\\xa9 \\xea\\xb2\\xb0\\xec\\xa0\\x95 \\xeb\\x91\\x98\" / \\xc2\\xa78 D6 item 7): discrete_plan.visit_order[] (full order)\n"
  "# and replan_request.ordering_constraints[] (realized prefix of the former).\n"
  "# Do not write code that assumes \"same type => same value\".\n"
  "string location\n"
  "VisitItem[] sequence";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__VisitOrder__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__VisitOrder__TYPE_NAME, 23, 23},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 321, 321},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__VisitOrder__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__VisitOrder__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__VisitItem__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
