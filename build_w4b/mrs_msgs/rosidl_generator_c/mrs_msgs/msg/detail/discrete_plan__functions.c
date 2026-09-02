// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/discrete_plan__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
#include "rosidl_runtime_c/string_functions.h"
// Member `plans`
#include "mrs_msgs/msg/detail/robot_plan__functions.h"
// Member `visit_order`
#include "mrs_msgs/msg/detail/visit_order__functions.h"

bool
mrs_msgs__msg__DiscretePlan__init(mrs_msgs__msg__DiscretePlan * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
    return false;
  }
  // plan_revision
  // plans
  if (!mrs_msgs__msg__RobotPlan__Sequence__init(&msg->plans, 0)) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
    return false;
  }
  // visit_order
  if (!mrs_msgs__msg__VisitOrder__Sequence__init(&msg->visit_order, 0)) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__DiscretePlan__fini(mrs_msgs__msg__DiscretePlan * msg)
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
  // plan_revision
  // plans
  mrs_msgs__msg__RobotPlan__Sequence__fini(&msg->plans);
  // visit_order
  mrs_msgs__msg__VisitOrder__Sequence__fini(&msg->visit_order);
}

bool
mrs_msgs__msg__DiscretePlan__are_equal(const mrs_msgs__msg__DiscretePlan * lhs, const mrs_msgs__msg__DiscretePlan * rhs)
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
  // plan_revision
  if (lhs->plan_revision != rhs->plan_revision) {
    return false;
  }
  // plans
  if (!mrs_msgs__msg__RobotPlan__Sequence__are_equal(
      &(lhs->plans), &(rhs->plans)))
  {
    return false;
  }
  // visit_order
  if (!mrs_msgs__msg__VisitOrder__Sequence__are_equal(
      &(lhs->visit_order), &(rhs->visit_order)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__DiscretePlan__copy(
  const mrs_msgs__msg__DiscretePlan * input,
  mrs_msgs__msg__DiscretePlan * output)
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
  // plan_revision
  output->plan_revision = input->plan_revision;
  // plans
  if (!mrs_msgs__msg__RobotPlan__Sequence__copy(
      &(input->plans), &(output->plans)))
  {
    return false;
  }
  // visit_order
  if (!mrs_msgs__msg__VisitOrder__Sequence__copy(
      &(input->visit_order), &(output->visit_order)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__DiscretePlan *
mrs_msgs__msg__DiscretePlan__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__DiscretePlan * msg = (mrs_msgs__msg__DiscretePlan *)allocator.allocate(sizeof(mrs_msgs__msg__DiscretePlan), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__DiscretePlan));
  bool success = mrs_msgs__msg__DiscretePlan__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__DiscretePlan__destroy(mrs_msgs__msg__DiscretePlan * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__DiscretePlan__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__DiscretePlan__Sequence__init(mrs_msgs__msg__DiscretePlan__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__DiscretePlan * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__DiscretePlan)) {
      return false;
    }
    data = (mrs_msgs__msg__DiscretePlan *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__DiscretePlan), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__DiscretePlan__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__DiscretePlan__fini(&data[i - 1]);
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
mrs_msgs__msg__DiscretePlan__Sequence__fini(mrs_msgs__msg__DiscretePlan__Sequence * array)
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
      mrs_msgs__msg__DiscretePlan__fini(&array->data[i]);
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

mrs_msgs__msg__DiscretePlan__Sequence *
mrs_msgs__msg__DiscretePlan__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__DiscretePlan__Sequence * array = (mrs_msgs__msg__DiscretePlan__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__DiscretePlan__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__DiscretePlan__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__DiscretePlan__Sequence__destroy(mrs_msgs__msg__DiscretePlan__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__DiscretePlan__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__DiscretePlan__Sequence__are_equal(const mrs_msgs__msg__DiscretePlan__Sequence * lhs, const mrs_msgs__msg__DiscretePlan__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__DiscretePlan__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__DiscretePlan__Sequence__copy(
  const mrs_msgs__msg__DiscretePlan__Sequence * input,
  mrs_msgs__msg__DiscretePlan__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__DiscretePlan)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__DiscretePlan);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__DiscretePlan * data =
      (mrs_msgs__msg__DiscretePlan *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__DiscretePlan__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__DiscretePlan__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__DiscretePlan__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
