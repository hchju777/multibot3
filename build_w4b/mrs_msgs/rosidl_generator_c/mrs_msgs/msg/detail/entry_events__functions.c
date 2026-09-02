// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/EntryEvents.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/entry_events__functions.h"

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
// Member `events`
#include "mrs_msgs/msg/detail/entry_event__functions.h"

bool
mrs_msgs__msg__EntryEvents__init(mrs_msgs__msg__EntryEvents * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__msg__EntryEvents__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__msg__EntryEvents__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__msg__EntryEvents__fini(msg);
    return false;
  }
  // events
  if (!mrs_msgs__msg__EntryEvent__Sequence__init(&msg->events, 0)) {
    mrs_msgs__msg__EntryEvents__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__EntryEvents__fini(mrs_msgs__msg__EntryEvents * msg)
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
  // events
  mrs_msgs__msg__EntryEvent__Sequence__fini(&msg->events);
}

bool
mrs_msgs__msg__EntryEvents__are_equal(const mrs_msgs__msg__EntryEvents * lhs, const mrs_msgs__msg__EntryEvents * rhs)
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
  // events
  if (!mrs_msgs__msg__EntryEvent__Sequence__are_equal(
      &(lhs->events), &(rhs->events)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__EntryEvents__copy(
  const mrs_msgs__msg__EntryEvents * input,
  mrs_msgs__msg__EntryEvents * output)
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
  // events
  if (!mrs_msgs__msg__EntryEvent__Sequence__copy(
      &(input->events), &(output->events)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__EntryEvents *
mrs_msgs__msg__EntryEvents__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvents * msg = (mrs_msgs__msg__EntryEvents *)allocator.allocate(sizeof(mrs_msgs__msg__EntryEvents), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__EntryEvents));
  bool success = mrs_msgs__msg__EntryEvents__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__EntryEvents__destroy(mrs_msgs__msg__EntryEvents * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__EntryEvents__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__EntryEvents__Sequence__init(mrs_msgs__msg__EntryEvents__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvents * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__EntryEvents)) {
      return false;
    }
    data = (mrs_msgs__msg__EntryEvents *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__EntryEvents), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__EntryEvents__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__EntryEvents__fini(&data[i - 1]);
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
mrs_msgs__msg__EntryEvents__Sequence__fini(mrs_msgs__msg__EntryEvents__Sequence * array)
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
      mrs_msgs__msg__EntryEvents__fini(&array->data[i]);
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

mrs_msgs__msg__EntryEvents__Sequence *
mrs_msgs__msg__EntryEvents__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvents__Sequence * array = (mrs_msgs__msg__EntryEvents__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__EntryEvents__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__EntryEvents__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__EntryEvents__Sequence__destroy(mrs_msgs__msg__EntryEvents__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__EntryEvents__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__EntryEvents__Sequence__are_equal(const mrs_msgs__msg__EntryEvents__Sequence * lhs, const mrs_msgs__msg__EntryEvents__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__EntryEvents__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__EntryEvents__Sequence__copy(
  const mrs_msgs__msg__EntryEvents__Sequence * input,
  mrs_msgs__msg__EntryEvents__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__EntryEvents)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__EntryEvents);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__EntryEvents * data =
      (mrs_msgs__msg__EntryEvents *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__EntryEvents__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__EntryEvents__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__EntryEvents__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
