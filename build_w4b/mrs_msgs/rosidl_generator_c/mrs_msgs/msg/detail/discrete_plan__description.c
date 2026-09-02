// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

#include "mrs_msgs/msg/detail/discrete_plan__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__DiscretePlan__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x50, 0x36, 0xb7, 0x3e, 0x7e, 0xc2, 0x2c, 0x3b,
      0xb3, 0x5a, 0x28, 0x1c, 0x02, 0xc2, 0xfa, 0xfc,
      0xc9, 0x2f, 0xd2, 0xaa, 0x74, 0x79, 0x61, 0x41,
      0x6e, 0xb3, 0x72, 0x49, 0x06, 0x03, 0x9c, 0xc7,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "mrs_msgs/msg/detail/visit_item__functions.h"
#include "mrs_msgs/msg/detail/plan_step__functions.h"
#include "mrs_msgs/msg/detail/visit_order__functions.h"
#include "mrs_msgs/msg/detail/robot_plan__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t mrs_msgs__msg__PlanStep__EXPECTED_HASH = {1, {
    0xa1, 0x67, 0x52, 0xe3, 0xcc, 0x54, 0x20, 0xc4,
    0x98, 0x1b, 0x60, 0x63, 0xc5, 0x26, 0xb1, 0x26,
    0x8f, 0x93, 0xdb, 0x45, 0xd3, 0xd3, 0x6b, 0xd6,
    0x4f, 0x7b, 0x21, 0x0c, 0x0a, 0xe5, 0x44, 0x00,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__RobotPlan__EXPECTED_HASH = {1, {
    0x62, 0xea, 0x63, 0x50, 0x01, 0x41, 0x89, 0x9f,
    0x7b, 0x3a, 0x58, 0x6e, 0xa8, 0x89, 0x3a, 0xf7,
    0xed, 0x0c, 0x4f, 0xcb, 0x8b, 0xf1, 0x7c, 0x5e,
    0x56, 0x8f, 0xc6, 0x23, 0x0e, 0xa3, 0x9a, 0x83,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__VisitItem__EXPECTED_HASH = {1, {
    0x50, 0xe2, 0x7f, 0xc4, 0xbd, 0xea, 0xfa, 0xfa,
    0xe6, 0x8d, 0x55, 0x3e, 0xf5, 0xa7, 0x41, 0x5b,
    0x63, 0xad, 0x73, 0xad, 0x0d, 0xb4, 0x1f, 0x60,
    0x3a, 0x2e, 0xdb, 0xe1, 0x8a, 0x8a, 0xf7, 0x13,
  }};
static const rosidl_type_hash_t mrs_msgs__msg__VisitOrder__EXPECTED_HASH = {1, {
    0x92, 0x48, 0x08, 0x93, 0xf4, 0x1d, 0x51, 0xb6,
    0x03, 0x36, 0x82, 0x21, 0x2f, 0x0f, 0xb9, 0xc6,
    0xaf, 0xb5, 0xb5, 0xb7, 0xf3, 0xd9, 0x13, 0xbb,
    0x0a, 0x03, 0x5e, 0x06, 0x9c, 0xd1, 0xc2, 0xa8,
  }};
#endif

static char mrs_msgs__msg__DiscretePlan__TYPE_NAME[] = "mrs_msgs/msg/DiscretePlan";
static char mrs_msgs__msg__PlanStep__TYPE_NAME[] = "mrs_msgs/msg/PlanStep";
static char mrs_msgs__msg__RobotPlan__TYPE_NAME[] = "mrs_msgs/msg/RobotPlan";
static char mrs_msgs__msg__VisitItem__TYPE_NAME[] = "mrs_msgs/msg/VisitItem";
static char mrs_msgs__msg__VisitOrder__TYPE_NAME[] = "mrs_msgs/msg/VisitOrder";

// Define type names, field names, and default values
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__schema[] = "schema";
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__schema_version[] = "schema_version";
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__instance_id[] = "instance_id";
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__plan_revision[] = "plan_revision";
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__plans[] = "plans";
static char mrs_msgs__msg__DiscretePlan__FIELD_NAME__visit_order[] = "visit_order";

static rosidl_runtime_c__type_description__Field mrs_msgs__msg__DiscretePlan__FIELDS[] = {
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__schema, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__schema_version, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__instance_id, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__plan_revision, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__plans, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__RobotPlan__TYPE_NAME, 22, 22},
    },
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__DiscretePlan__FIELD_NAME__visit_order, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE,
      0,
      0,
      {mrs_msgs__msg__VisitOrder__TYPE_NAME, 23, 23},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription mrs_msgs__msg__DiscretePlan__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {mrs_msgs__msg__PlanStep__TYPE_NAME, 21, 21},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__RobotPlan__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__VisitItem__TYPE_NAME, 22, 22},
    {NULL, 0, 0},
  },
  {
    {mrs_msgs__msg__VisitOrder__TYPE_NAME, 23, 23},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__DiscretePlan__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {mrs_msgs__msg__DiscretePlan__TYPE_NAME, 25, 25},
      {mrs_msgs__msg__DiscretePlan__FIELDS, 6, 6},
    },
    {mrs_msgs__msg__DiscretePlan__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&mrs_msgs__msg__PlanStep__EXPECTED_HASH, mrs_msgs__msg__PlanStep__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = mrs_msgs__msg__PlanStep__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__RobotPlan__EXPECTED_HASH, mrs_msgs__msg__RobotPlan__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = mrs_msgs__msg__RobotPlan__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__VisitItem__EXPECTED_HASH, mrs_msgs__msg__VisitItem__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[2].fields = mrs_msgs__msg__VisitItem__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&mrs_msgs__msg__VisitOrder__EXPECTED_HASH, mrs_msgs__msg__VisitOrder__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = mrs_msgs__msg__VisitOrder__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string schema\n"
  "string schema_version\n"
  "string instance_id\n"
  "uint64 plan_revision\n"
  "RobotPlan[] plans\n"
  "VisitOrder[] visit_order";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__DiscretePlan__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {mrs_msgs__msg__DiscretePlan__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 119, 119},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__DiscretePlan__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *mrs_msgs__msg__DiscretePlan__get_individual_type_description_source(NULL),
    sources[1] = *mrs_msgs__msg__PlanStep__get_individual_type_description_source(NULL);
    sources[2] = *mrs_msgs__msg__RobotPlan__get_individual_type_description_source(NULL);
    sources[3] = *mrs_msgs__msg__VisitItem__get_individual_type_description_source(NULL);
    sources[4] = *mrs_msgs__msg__VisitOrder__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
