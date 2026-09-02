// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/action/detail/replan__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `affected_robots`
// Member `blocked_hash`
#include "rosidl_runtime_c/string_functions.h"
// Member `blocked`
#include "mrs_msgs/msg/detail/edge_ref__functions.h"
// Member `ordering_constraints`
#include "mrs_msgs/msg/detail/visit_order__functions.h"
// Member `baseline`
#include "mrs_msgs/msg/detail/robot_baseline__functions.h"

bool
mrs_msgs__action__Replan_Goal__init(mrs_msgs__action__Replan_Goal * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // reason
  // affected_robots
  if (!rosidl_runtime_c__String__Sequence__init(&msg->affected_robots, 0)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // blocked
  if (!mrs_msgs__msg__EdgeRef__Sequence__init(&msg->blocked, 0)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // blocked_hash
  if (!rosidl_runtime_c__String__init(&msg->blocked_hash)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // ordering_constraints
  if (!mrs_msgs__msg__VisitOrder__Sequence__init(&msg->ordering_constraints, 0)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // baseline
  if (!mrs_msgs__msg__RobotBaseline__Sequence__init(&msg->baseline, 0)) {
    mrs_msgs__action__Replan_Goal__fini(msg);
    return false;
  }
  // plan_revision
  return true;
}

void
mrs_msgs__action__Replan_Goal__fini(mrs_msgs__action__Replan_Goal * msg)
{
  if (!msg) {
    return;
  }
  // schema
  rosidl_runtime_c__String__fini(&msg->schema);
  // schema_version
  rosidl_runtime_c__String__fini(&msg->schema_version);
  // instance_id
  rosidl_runtime_c__String__fini(&msg->instance_id);
  // reason
  // affected_robots
  rosidl_runtime_c__String__Sequence__fini(&msg->affected_robots);
  // blocked
  mrs_msgs__msg__EdgeRef__Sequence__fini(&msg->blocked);
  // blocked_hash
  rosidl_runtime_c__String__fini(&msg->blocked_hash);
  // ordering_constraints
  mrs_msgs__msg__VisitOrder__Sequence__fini(&msg->ordering_constraints);
  // baseline
  mrs_msgs__msg__RobotBaseline__Sequence__fini(&msg->baseline);
  // plan_revision
}

bool
mrs_msgs__action__Replan_Goal__are_equal(const mrs_msgs__action__Replan_Goal * lhs, const mrs_msgs__action__Replan_Goal * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->schema), &(rhs->schema)))
  {
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->schema_version), &(rhs->schema_version)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->instance_id), &(rhs->instance_id)))
  {
    return false;
  }
  // reason
  if (lhs->reason != rhs->reason) {
    return false;
  }
  // affected_robots
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->affected_robots), &(rhs->affected_robots)))
  {
    return false;
  }
  // blocked
  if (!mrs_msgs__msg__EdgeRef__Sequence__are_equal(
      &(lhs->blocked), &(rhs->blocked)))
  {
    return false;
  }
  // blocked_hash
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->blocked_hash), &(rhs->blocked_hash)))
  {
    return false;
  }
  // ordering_constraints
  if (!mrs_msgs__msg__VisitOrder__Sequence__are_equal(
      &(lhs->ordering_constraints), &(rhs->ordering_constraints)))
  {
    return false;
  }
  // baseline
  if (!mrs_msgs__msg__RobotBaseline__Sequence__are_equal(
      &(lhs->baseline), &(rhs->baseline)))
  {
    return false;
  }
  // plan_revision
  if (lhs->plan_revision != rhs->plan_revision) {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_Goal__copy(
  const mrs_msgs__action__Replan_Goal * input,
  mrs_msgs__action__Replan_Goal * output)
{
  if (!input || !output) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__copy(
      &(input->schema), &(output->schema)))
  {
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__copy(
      &(input->schema_version), &(output->schema_version)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__copy(
      &(input->instance_id), &(output->instance_id)))
  {
    return false;
  }
  // reason
  output->reason = input->reason;
  // affected_robots
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->affected_robots), &(output->affected_robots)))
  {
    return false;
  }
  // blocked
  if (!mrs_msgs__msg__EdgeRef__Sequence__copy(
      &(input->blocked), &(output->blocked)))
  {
    return false;
  }
  // blocked_hash
  if (!rosidl_runtime_c__String__copy(
      &(input->blocked_hash), &(output->blocked_hash)))
  {
    return false;
  }
  // ordering_constraints
  if (!mrs_msgs__msg__VisitOrder__Sequence__copy(
      &(input->ordering_constraints), &(output->ordering_constraints)))
  {
    return false;
  }
  // baseline
  if (!mrs_msgs__msg__RobotBaseline__Sequence__copy(
      &(input->baseline), &(output->baseline)))
  {
    return false;
  }
  // plan_revision
  output->plan_revision = input->plan_revision;
  return true;
}

mrs_msgs__action__Replan_Goal *
mrs_msgs__action__Replan_Goal__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Goal * msg = (mrs_msgs__action__Replan_Goal *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Goal), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_Goal));
  bool success = mrs_msgs__action__Replan_Goal__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_Goal__destroy(mrs_msgs__action__Replan_Goal * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_Goal__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_Goal__Sequence__init(mrs_msgs__action__Replan_Goal__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Goal * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Goal)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_Goal *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_Goal), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_Goal__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_Goal__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_Goal__Sequence__fini(mrs_msgs__action__Replan_Goal__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_Goal__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_Goal__Sequence *
mrs_msgs__action__Replan_Goal__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Goal__Sequence * array = (mrs_msgs__action__Replan_Goal__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Goal__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_Goal__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_Goal__Sequence__destroy(mrs_msgs__action__Replan_Goal__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_Goal__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_Goal__Sequence__are_equal(const mrs_msgs__action__Replan_Goal__Sequence * lhs, const mrs_msgs__action__Replan_Goal__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_Goal__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_Goal__Sequence__copy(
  const mrs_msgs__action__Replan_Goal__Sequence * input,
  mrs_msgs__action__Replan_Goal__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Goal)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_Goal);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_Goal * data =
      (mrs_msgs__action__Replan_Goal *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_Goal__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_Goal__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_Goal__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `blocked_hash`
// Member `unreachable_robots`
// already included above
// #include "rosidl_runtime_c/string_functions.h"
// Member `components`
#include "mrs_msgs/msg/detail/robot_group__functions.h"

bool
mrs_msgs__action__Replan_Result__init(mrs_msgs__action__Replan_Result * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  // outcome
  // plan_revision
  // blocked_hash
  if (!rosidl_runtime_c__String__init(&msg->blocked_hash)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  // unreachable_robots
  if (!rosidl_runtime_c__String__Sequence__init(&msg->unreachable_robots, 0)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  // components
  if (!mrs_msgs__msg__RobotGroup__Sequence__init(&msg->components, 0)) {
    mrs_msgs__action__Replan_Result__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_Result__fini(mrs_msgs__action__Replan_Result * msg)
{
  if (!msg) {
    return;
  }
  // schema
  rosidl_runtime_c__String__fini(&msg->schema);
  // schema_version
  rosidl_runtime_c__String__fini(&msg->schema_version);
  // instance_id
  rosidl_runtime_c__String__fini(&msg->instance_id);
  // outcome
  // plan_revision
  // blocked_hash
  rosidl_runtime_c__String__fini(&msg->blocked_hash);
  // unreachable_robots
  rosidl_runtime_c__String__Sequence__fini(&msg->unreachable_robots);
  // components
  mrs_msgs__msg__RobotGroup__Sequence__fini(&msg->components);
}

bool
mrs_msgs__action__Replan_Result__are_equal(const mrs_msgs__action__Replan_Result * lhs, const mrs_msgs__action__Replan_Result * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->schema), &(rhs->schema)))
  {
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->schema_version), &(rhs->schema_version)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->instance_id), &(rhs->instance_id)))
  {
    return false;
  }
  // outcome
  if (lhs->outcome != rhs->outcome) {
    return false;
  }
  // plan_revision
  if (lhs->plan_revision != rhs->plan_revision) {
    return false;
  }
  // blocked_hash
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->blocked_hash), &(rhs->blocked_hash)))
  {
    return false;
  }
  // unreachable_robots
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->unreachable_robots), &(rhs->unreachable_robots)))
  {
    return false;
  }
  // components
  if (!mrs_msgs__msg__RobotGroup__Sequence__are_equal(
      &(lhs->components), &(rhs->components)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_Result__copy(
  const mrs_msgs__action__Replan_Result * input,
  mrs_msgs__action__Replan_Result * output)
{
  if (!input || !output) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__copy(
      &(input->schema), &(output->schema)))
  {
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__copy(
      &(input->schema_version), &(output->schema_version)))
  {
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__copy(
      &(input->instance_id), &(output->instance_id)))
  {
    return false;
  }
  // outcome
  output->outcome = input->outcome;
  // plan_revision
  output->plan_revision = input->plan_revision;
  // blocked_hash
  if (!rosidl_runtime_c__String__copy(
      &(input->blocked_hash), &(output->blocked_hash)))
  {
    return false;
  }
  // unreachable_robots
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->unreachable_robots), &(output->unreachable_robots)))
  {
    return false;
  }
  // components
  if (!mrs_msgs__msg__RobotGroup__Sequence__copy(
      &(input->components), &(output->components)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_Result *
mrs_msgs__action__Replan_Result__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Result * msg = (mrs_msgs__action__Replan_Result *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Result), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_Result));
  bool success = mrs_msgs__action__Replan_Result__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_Result__destroy(mrs_msgs__action__Replan_Result * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_Result__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_Result__Sequence__init(mrs_msgs__action__Replan_Result__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Result * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Result)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_Result *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_Result), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_Result__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_Result__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_Result__Sequence__fini(mrs_msgs__action__Replan_Result__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_Result__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_Result__Sequence *
mrs_msgs__action__Replan_Result__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Result__Sequence * array = (mrs_msgs__action__Replan_Result__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Result__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_Result__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_Result__Sequence__destroy(mrs_msgs__action__Replan_Result__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_Result__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_Result__Sequence__are_equal(const mrs_msgs__action__Replan_Result__Sequence * lhs, const mrs_msgs__action__Replan_Result__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_Result__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_Result__Sequence__copy(
  const mrs_msgs__action__Replan_Result__Sequence * input,
  mrs_msgs__action__Replan_Result__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Result)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_Result);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_Result * data =
      (mrs_msgs__action__Replan_Result *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_Result__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_Result__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_Result__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
mrs_msgs__action__Replan_Feedback__init(mrs_msgs__action__Replan_Feedback * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
mrs_msgs__action__Replan_Feedback__fini(mrs_msgs__action__Replan_Feedback * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
mrs_msgs__action__Replan_Feedback__are_equal(const mrs_msgs__action__Replan_Feedback * lhs, const mrs_msgs__action__Replan_Feedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // structure_needs_at_least_one_member
  if (lhs->structure_needs_at_least_one_member != rhs->structure_needs_at_least_one_member) {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_Feedback__copy(
  const mrs_msgs__action__Replan_Feedback * input,
  mrs_msgs__action__Replan_Feedback * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

mrs_msgs__action__Replan_Feedback *
mrs_msgs__action__Replan_Feedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Feedback * msg = (mrs_msgs__action__Replan_Feedback *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Feedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_Feedback));
  bool success = mrs_msgs__action__Replan_Feedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_Feedback__destroy(mrs_msgs__action__Replan_Feedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_Feedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_Feedback__Sequence__init(mrs_msgs__action__Replan_Feedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Feedback * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Feedback)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_Feedback *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_Feedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_Feedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_Feedback__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_Feedback__Sequence__fini(mrs_msgs__action__Replan_Feedback__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_Feedback__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_Feedback__Sequence *
mrs_msgs__action__Replan_Feedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_Feedback__Sequence * array = (mrs_msgs__action__Replan_Feedback__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_Feedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_Feedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_Feedback__Sequence__destroy(mrs_msgs__action__Replan_Feedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_Feedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_Feedback__Sequence__are_equal(const mrs_msgs__action__Replan_Feedback__Sequence * lhs, const mrs_msgs__action__Replan_Feedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_Feedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_Feedback__Sequence__copy(
  const mrs_msgs__action__Replan_Feedback__Sequence * input,
  mrs_msgs__action__Replan_Feedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_Feedback)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_Feedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_Feedback * data =
      (mrs_msgs__action__Replan_Feedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_Feedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_Feedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_Feedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `goal`
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"

bool
mrs_msgs__action__Replan_SendGoal_Request__init(mrs_msgs__action__Replan_SendGoal_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    mrs_msgs__action__Replan_SendGoal_Request__fini(msg);
    return false;
  }
  // goal
  if (!mrs_msgs__action__Replan_Goal__init(&msg->goal)) {
    mrs_msgs__action__Replan_SendGoal_Request__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Request__fini(mrs_msgs__action__Replan_SendGoal_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // goal
  mrs_msgs__action__Replan_Goal__fini(&msg->goal);
}

bool
mrs_msgs__action__Replan_SendGoal_Request__are_equal(const mrs_msgs__action__Replan_SendGoal_Request * lhs, const mrs_msgs__action__Replan_SendGoal_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // goal
  if (!mrs_msgs__action__Replan_Goal__are_equal(
      &(lhs->goal), &(rhs->goal)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Request__copy(
  const mrs_msgs__action__Replan_SendGoal_Request * input,
  mrs_msgs__action__Replan_SendGoal_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // goal
  if (!mrs_msgs__action__Replan_Goal__copy(
      &(input->goal), &(output->goal)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_SendGoal_Request *
mrs_msgs__action__Replan_SendGoal_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Request * msg = (mrs_msgs__action__Replan_SendGoal_Request *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_SendGoal_Request));
  bool success = mrs_msgs__action__Replan_SendGoal_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_SendGoal_Request__destroy(mrs_msgs__action__Replan_SendGoal_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_SendGoal_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(mrs_msgs__action__Replan_SendGoal_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Request)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_SendGoal_Request *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_SendGoal_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_SendGoal_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_SendGoal_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(mrs_msgs__action__Replan_SendGoal_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_SendGoal_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_SendGoal_Request__Sequence *
mrs_msgs__action__Replan_SendGoal_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Request__Sequence * array = (mrs_msgs__action__Replan_SendGoal_Request__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_SendGoal_Request__Sequence__destroy(mrs_msgs__action__Replan_SendGoal_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_SendGoal_Request__Sequence__are_equal(const mrs_msgs__action__Replan_SendGoal_Request__Sequence * lhs, const mrs_msgs__action__Replan_SendGoal_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Request__Sequence__copy(
  const mrs_msgs__action__Replan_SendGoal_Request__Sequence * input,
  mrs_msgs__action__Replan_SendGoal_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_SendGoal_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_SendGoal_Request * data =
      (mrs_msgs__action__Replan_SendGoal_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_SendGoal_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_SendGoal_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
mrs_msgs__action__Replan_SendGoal_Response__init(mrs_msgs__action__Replan_SendGoal_Response * msg)
{
  if (!msg) {
    return false;
  }
  // accepted
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    mrs_msgs__action__Replan_SendGoal_Response__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Response__fini(mrs_msgs__action__Replan_SendGoal_Response * msg)
{
  if (!msg) {
    return;
  }
  // accepted
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
mrs_msgs__action__Replan_SendGoal_Response__are_equal(const mrs_msgs__action__Replan_SendGoal_Response * lhs, const mrs_msgs__action__Replan_SendGoal_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // accepted
  if (lhs->accepted != rhs->accepted) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Response__copy(
  const mrs_msgs__action__Replan_SendGoal_Response * input,
  mrs_msgs__action__Replan_SendGoal_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // accepted
  output->accepted = input->accepted;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_SendGoal_Response *
mrs_msgs__action__Replan_SendGoal_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Response * msg = (mrs_msgs__action__Replan_SendGoal_Response *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_SendGoal_Response));
  bool success = mrs_msgs__action__Replan_SendGoal_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_SendGoal_Response__destroy(mrs_msgs__action__Replan_SendGoal_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_SendGoal_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(mrs_msgs__action__Replan_SendGoal_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Response)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_SendGoal_Response *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_SendGoal_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_SendGoal_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_SendGoal_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(mrs_msgs__action__Replan_SendGoal_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_SendGoal_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_SendGoal_Response__Sequence *
mrs_msgs__action__Replan_SendGoal_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Response__Sequence * array = (mrs_msgs__action__Replan_SendGoal_Response__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_SendGoal_Response__Sequence__destroy(mrs_msgs__action__Replan_SendGoal_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_SendGoal_Response__Sequence__are_equal(const mrs_msgs__action__Replan_SendGoal_Response__Sequence * lhs, const mrs_msgs__action__Replan_SendGoal_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Response__Sequence__copy(
  const mrs_msgs__action__Replan_SendGoal_Response__Sequence * input,
  mrs_msgs__action__Replan_SendGoal_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_SendGoal_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_SendGoal_Response * data =
      (mrs_msgs__action__Replan_SendGoal_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_SendGoal_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_SendGoal_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"

bool
mrs_msgs__action__Replan_SendGoal_Event__init(mrs_msgs__action__Replan_SendGoal_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    mrs_msgs__action__Replan_SendGoal_Event__fini(msg);
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(&msg->request, 0)) {
    mrs_msgs__action__Replan_SendGoal_Event__fini(msg);
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(&msg->response, 0)) {
    mrs_msgs__action__Replan_SendGoal_Event__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Event__fini(mrs_msgs__action__Replan_SendGoal_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(&msg->request);
  // response
  mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(&msg->response);
}

bool
mrs_msgs__action__Replan_SendGoal_Event__are_equal(const mrs_msgs__action__Replan_SendGoal_Event * lhs, const mrs_msgs__action__Replan_SendGoal_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_SendGoal_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_SendGoal_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Event__copy(
  const mrs_msgs__action__Replan_SendGoal_Event * input,
  mrs_msgs__action__Replan_SendGoal_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_SendGoal_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_SendGoal_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_SendGoal_Event *
mrs_msgs__action__Replan_SendGoal_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Event * msg = (mrs_msgs__action__Replan_SendGoal_Event *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_SendGoal_Event));
  bool success = mrs_msgs__action__Replan_SendGoal_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_SendGoal_Event__destroy(mrs_msgs__action__Replan_SendGoal_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_SendGoal_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_SendGoal_Event__Sequence__init(mrs_msgs__action__Replan_SendGoal_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Event)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_SendGoal_Event *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_SendGoal_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_SendGoal_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_SendGoal_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_SendGoal_Event__Sequence__fini(mrs_msgs__action__Replan_SendGoal_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_SendGoal_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_SendGoal_Event__Sequence *
mrs_msgs__action__Replan_SendGoal_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_SendGoal_Event__Sequence * array = (mrs_msgs__action__Replan_SendGoal_Event__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_SendGoal_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_SendGoal_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_SendGoal_Event__Sequence__destroy(mrs_msgs__action__Replan_SendGoal_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_SendGoal_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_SendGoal_Event__Sequence__are_equal(const mrs_msgs__action__Replan_SendGoal_Event__Sequence * lhs, const mrs_msgs__action__Replan_SendGoal_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_SendGoal_Event__Sequence__copy(
  const mrs_msgs__action__Replan_SendGoal_Event__Sequence * input,
  mrs_msgs__action__Replan_SendGoal_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_SendGoal_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_SendGoal_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_SendGoal_Event * data =
      (mrs_msgs__action__Replan_SendGoal_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_SendGoal_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_SendGoal_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_SendGoal_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"

bool
mrs_msgs__action__Replan_GetResult_Request__init(mrs_msgs__action__Replan_GetResult_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    mrs_msgs__action__Replan_GetResult_Request__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Request__fini(mrs_msgs__action__Replan_GetResult_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
}

bool
mrs_msgs__action__Replan_GetResult_Request__are_equal(const mrs_msgs__action__Replan_GetResult_Request * lhs, const mrs_msgs__action__Replan_GetResult_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Request__copy(
  const mrs_msgs__action__Replan_GetResult_Request * input,
  mrs_msgs__action__Replan_GetResult_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_GetResult_Request *
mrs_msgs__action__Replan_GetResult_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Request * msg = (mrs_msgs__action__Replan_GetResult_Request *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_GetResult_Request));
  bool success = mrs_msgs__action__Replan_GetResult_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_GetResult_Request__destroy(mrs_msgs__action__Replan_GetResult_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_GetResult_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_GetResult_Request__Sequence__init(mrs_msgs__action__Replan_GetResult_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Request)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_GetResult_Request *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_GetResult_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_GetResult_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_GetResult_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(mrs_msgs__action__Replan_GetResult_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_GetResult_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_GetResult_Request__Sequence *
mrs_msgs__action__Replan_GetResult_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Request__Sequence * array = (mrs_msgs__action__Replan_GetResult_Request__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_GetResult_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_GetResult_Request__Sequence__destroy(mrs_msgs__action__Replan_GetResult_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_GetResult_Request__Sequence__are_equal(const mrs_msgs__action__Replan_GetResult_Request__Sequence * lhs, const mrs_msgs__action__Replan_GetResult_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Request__Sequence__copy(
  const mrs_msgs__action__Replan_GetResult_Request__Sequence * input,
  mrs_msgs__action__Replan_GetResult_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_GetResult_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_GetResult_Request * data =
      (mrs_msgs__action__Replan_GetResult_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_GetResult_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_GetResult_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `result`
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"

bool
mrs_msgs__action__Replan_GetResult_Response__init(mrs_msgs__action__Replan_GetResult_Response * msg)
{
  if (!msg) {
    return false;
  }
  // status
  // result
  if (!mrs_msgs__action__Replan_Result__init(&msg->result)) {
    mrs_msgs__action__Replan_GetResult_Response__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Response__fini(mrs_msgs__action__Replan_GetResult_Response * msg)
{
  if (!msg) {
    return;
  }
  // status
  // result
  mrs_msgs__action__Replan_Result__fini(&msg->result);
}

bool
mrs_msgs__action__Replan_GetResult_Response__are_equal(const mrs_msgs__action__Replan_GetResult_Response * lhs, const mrs_msgs__action__Replan_GetResult_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // status
  if (lhs->status != rhs->status) {
    return false;
  }
  // result
  if (!mrs_msgs__action__Replan_Result__are_equal(
      &(lhs->result), &(rhs->result)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Response__copy(
  const mrs_msgs__action__Replan_GetResult_Response * input,
  mrs_msgs__action__Replan_GetResult_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // status
  output->status = input->status;
  // result
  if (!mrs_msgs__action__Replan_Result__copy(
      &(input->result), &(output->result)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_GetResult_Response *
mrs_msgs__action__Replan_GetResult_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Response * msg = (mrs_msgs__action__Replan_GetResult_Response *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_GetResult_Response));
  bool success = mrs_msgs__action__Replan_GetResult_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_GetResult_Response__destroy(mrs_msgs__action__Replan_GetResult_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_GetResult_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_GetResult_Response__Sequence__init(mrs_msgs__action__Replan_GetResult_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Response)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_GetResult_Response *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_GetResult_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_GetResult_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_GetResult_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(mrs_msgs__action__Replan_GetResult_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_GetResult_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_GetResult_Response__Sequence *
mrs_msgs__action__Replan_GetResult_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Response__Sequence * array = (mrs_msgs__action__Replan_GetResult_Response__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_GetResult_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_GetResult_Response__Sequence__destroy(mrs_msgs__action__Replan_GetResult_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_GetResult_Response__Sequence__are_equal(const mrs_msgs__action__Replan_GetResult_Response__Sequence * lhs, const mrs_msgs__action__Replan_GetResult_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Response__Sequence__copy(
  const mrs_msgs__action__Replan_GetResult_Response__Sequence * input,
  mrs_msgs__action__Replan_GetResult_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_GetResult_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_GetResult_Response * data =
      (mrs_msgs__action__Replan_GetResult_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_GetResult_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_GetResult_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
// already included above
// #include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"

bool
mrs_msgs__action__Replan_GetResult_Event__init(mrs_msgs__action__Replan_GetResult_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    mrs_msgs__action__Replan_GetResult_Event__fini(msg);
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_GetResult_Request__Sequence__init(&msg->request, 0)) {
    mrs_msgs__action__Replan_GetResult_Event__fini(msg);
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_GetResult_Response__Sequence__init(&msg->response, 0)) {
    mrs_msgs__action__Replan_GetResult_Event__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Event__fini(mrs_msgs__action__Replan_GetResult_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(&msg->request);
  // response
  mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(&msg->response);
}

bool
mrs_msgs__action__Replan_GetResult_Event__are_equal(const mrs_msgs__action__Replan_GetResult_Event * lhs, const mrs_msgs__action__Replan_GetResult_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_GetResult_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_GetResult_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Event__copy(
  const mrs_msgs__action__Replan_GetResult_Event * input,
  mrs_msgs__action__Replan_GetResult_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!mrs_msgs__action__Replan_GetResult_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!mrs_msgs__action__Replan_GetResult_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_GetResult_Event *
mrs_msgs__action__Replan_GetResult_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Event * msg = (mrs_msgs__action__Replan_GetResult_Event *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_GetResult_Event));
  bool success = mrs_msgs__action__Replan_GetResult_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_GetResult_Event__destroy(mrs_msgs__action__Replan_GetResult_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_GetResult_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_GetResult_Event__Sequence__init(mrs_msgs__action__Replan_GetResult_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Event)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_GetResult_Event *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_GetResult_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_GetResult_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_GetResult_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_GetResult_Event__Sequence__fini(mrs_msgs__action__Replan_GetResult_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_GetResult_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_GetResult_Event__Sequence *
mrs_msgs__action__Replan_GetResult_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_GetResult_Event__Sequence * array = (mrs_msgs__action__Replan_GetResult_Event__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_GetResult_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_GetResult_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_GetResult_Event__Sequence__destroy(mrs_msgs__action__Replan_GetResult_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_GetResult_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_GetResult_Event__Sequence__are_equal(const mrs_msgs__action__Replan_GetResult_Event__Sequence * lhs, const mrs_msgs__action__Replan_GetResult_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_GetResult_Event__Sequence__copy(
  const mrs_msgs__action__Replan_GetResult_Event__Sequence * input,
  mrs_msgs__action__Replan_GetResult_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_GetResult_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_GetResult_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_GetResult_Event * data =
      (mrs_msgs__action__Replan_GetResult_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_GetResult_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_GetResult_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_GetResult_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `feedback`
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"

bool
mrs_msgs__action__Replan_FeedbackMessage__init(mrs_msgs__action__Replan_FeedbackMessage * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    mrs_msgs__action__Replan_FeedbackMessage__fini(msg);
    return false;
  }
  // feedback
  if (!mrs_msgs__action__Replan_Feedback__init(&msg->feedback)) {
    mrs_msgs__action__Replan_FeedbackMessage__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__action__Replan_FeedbackMessage__fini(mrs_msgs__action__Replan_FeedbackMessage * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // feedback
  mrs_msgs__action__Replan_Feedback__fini(&msg->feedback);
}

bool
mrs_msgs__action__Replan_FeedbackMessage__are_equal(const mrs_msgs__action__Replan_FeedbackMessage * lhs, const mrs_msgs__action__Replan_FeedbackMessage * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // feedback
  if (!mrs_msgs__action__Replan_Feedback__are_equal(
      &(lhs->feedback), &(rhs->feedback)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__action__Replan_FeedbackMessage__copy(
  const mrs_msgs__action__Replan_FeedbackMessage * input,
  mrs_msgs__action__Replan_FeedbackMessage * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // feedback
  if (!mrs_msgs__action__Replan_Feedback__copy(
      &(input->feedback), &(output->feedback)))
  {
    return false;
  }
  return true;
}

mrs_msgs__action__Replan_FeedbackMessage *
mrs_msgs__action__Replan_FeedbackMessage__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_FeedbackMessage * msg = (mrs_msgs__action__Replan_FeedbackMessage *)allocator.allocate(sizeof(mrs_msgs__action__Replan_FeedbackMessage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__action__Replan_FeedbackMessage));
  bool success = mrs_msgs__action__Replan_FeedbackMessage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__action__Replan_FeedbackMessage__destroy(mrs_msgs__action__Replan_FeedbackMessage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__action__Replan_FeedbackMessage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__action__Replan_FeedbackMessage__Sequence__init(mrs_msgs__action__Replan_FeedbackMessage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_FeedbackMessage * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_FeedbackMessage)) {
      return false;
    }
    data = (mrs_msgs__action__Replan_FeedbackMessage *)allocator.zero_allocate(size, sizeof(mrs_msgs__action__Replan_FeedbackMessage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__action__Replan_FeedbackMessage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__action__Replan_FeedbackMessage__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mrs_msgs__action__Replan_FeedbackMessage__Sequence__fini(mrs_msgs__action__Replan_FeedbackMessage__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mrs_msgs__action__Replan_FeedbackMessage__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mrs_msgs__action__Replan_FeedbackMessage__Sequence *
mrs_msgs__action__Replan_FeedbackMessage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__action__Replan_FeedbackMessage__Sequence * array = (mrs_msgs__action__Replan_FeedbackMessage__Sequence *)allocator.allocate(sizeof(mrs_msgs__action__Replan_FeedbackMessage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__action__Replan_FeedbackMessage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__action__Replan_FeedbackMessage__Sequence__destroy(mrs_msgs__action__Replan_FeedbackMessage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__action__Replan_FeedbackMessage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__action__Replan_FeedbackMessage__Sequence__are_equal(const mrs_msgs__action__Replan_FeedbackMessage__Sequence * lhs, const mrs_msgs__action__Replan_FeedbackMessage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__action__Replan_FeedbackMessage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__action__Replan_FeedbackMessage__Sequence__copy(
  const mrs_msgs__action__Replan_FeedbackMessage__Sequence * input,
  mrs_msgs__action__Replan_FeedbackMessage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__action__Replan_FeedbackMessage)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__action__Replan_FeedbackMessage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__action__Replan_FeedbackMessage * data =
      (mrs_msgs__action__Replan_FeedbackMessage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__action__Replan_FeedbackMessage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__action__Replan_FeedbackMessage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__action__Replan_FeedbackMessage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
