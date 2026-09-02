// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/Assignment.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/assignment__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `unassigned_tasks`
#include "rosidl_runtime_c/string_functions.h"
// Member `assignments`
#include "mrs_msgs/msg/detail/robot_assignment__functions.h"

bool
mrs_msgs__msg__Assignment__init(mrs_msgs__msg__Assignment * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__msg__Assignment__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__msg__Assignment__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__msg__Assignment__fini(msg);
    return false;
  }
  // revision
  // assignments
  if (!mrs_msgs__msg__RobotAssignment__Sequence__init(&msg->assignments, 0)) {
    mrs_msgs__msg__Assignment__fini(msg);
    return false;
  }
  // unassigned_tasks
  if (!rosidl_runtime_c__String__Sequence__init(&msg->unassigned_tasks, 0)) {
    mrs_msgs__msg__Assignment__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__Assignment__fini(mrs_msgs__msg__Assignment * msg)
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
  // revision
  // assignments
  mrs_msgs__msg__RobotAssignment__Sequence__fini(&msg->assignments);
  // unassigned_tasks
  rosidl_runtime_c__String__Sequence__fini(&msg->unassigned_tasks);
}

bool
mrs_msgs__msg__Assignment__are_equal(const mrs_msgs__msg__Assignment * lhs, const mrs_msgs__msg__Assignment * rhs)
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
  // revision
  if (lhs->revision != rhs->revision) {
    return false;
  }
  // assignments
  if (!mrs_msgs__msg__RobotAssignment__Sequence__are_equal(
      &(lhs->assignments), &(rhs->assignments)))
  {
    return false;
  }
  // unassigned_tasks
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->unassigned_tasks), &(rhs->unassigned_tasks)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__Assignment__copy(
  const mrs_msgs__msg__Assignment * input,
  mrs_msgs__msg__Assignment * output)
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
  // revision
  output->revision = input->revision;
  // assignments
  if (!mrs_msgs__msg__RobotAssignment__Sequence__copy(
      &(input->assignments), &(output->assignments)))
  {
    return false;
  }
  // unassigned_tasks
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->unassigned_tasks), &(output->unassigned_tasks)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__Assignment *
mrs_msgs__msg__Assignment__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Assignment * msg = (mrs_msgs__msg__Assignment *)allocator.allocate(sizeof(mrs_msgs__msg__Assignment), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__Assignment));
  bool success = mrs_msgs__msg__Assignment__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__Assignment__destroy(mrs_msgs__msg__Assignment * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__Assignment__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__Assignment__Sequence__init(mrs_msgs__msg__Assignment__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Assignment * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__Assignment)) {
      return false;
    }
    data = (mrs_msgs__msg__Assignment *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__Assignment), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__Assignment__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__Assignment__fini(&data[i - 1]);
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
mrs_msgs__msg__Assignment__Sequence__fini(mrs_msgs__msg__Assignment__Sequence * array)
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
      mrs_msgs__msg__Assignment__fini(&array->data[i]);
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

mrs_msgs__msg__Assignment__Sequence *
mrs_msgs__msg__Assignment__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Assignment__Sequence * array = (mrs_msgs__msg__Assignment__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__Assignment__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__Assignment__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__Assignment__Sequence__destroy(mrs_msgs__msg__Assignment__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__Assignment__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__Assignment__Sequence__are_equal(const mrs_msgs__msg__Assignment__Sequence * lhs, const mrs_msgs__msg__Assignment__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__Assignment__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__Assignment__Sequence__copy(
  const mrs_msgs__msg__Assignment__Sequence * input,
  mrs_msgs__msg__Assignment__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__Assignment)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__Assignment);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__Assignment * data =
      (mrs_msgs__msg__Assignment *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__Assignment__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__Assignment__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__Assignment__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
