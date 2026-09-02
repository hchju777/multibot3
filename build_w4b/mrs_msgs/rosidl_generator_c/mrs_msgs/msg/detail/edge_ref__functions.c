// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/EdgeRef.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/edge_ref__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `from_id`
// Member `to_id`
#include "rosidl_runtime_c/string_functions.h"

bool
mrs_msgs__msg__EdgeRef__init(mrs_msgs__msg__EdgeRef * msg)
{
  if (!msg) {
    return false;
  }
  // from_id
  if (!rosidl_runtime_c__String__init(&msg->from_id)) {
    mrs_msgs__msg__EdgeRef__fini(msg);
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__init(&msg->to_id)) {
    mrs_msgs__msg__EdgeRef__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__EdgeRef__fini(mrs_msgs__msg__EdgeRef * msg)
{
  if (!msg) {
    return;
  }
  // from_id
  rosidl_runtime_c__String__fini(&msg->from_id);
  // to_id
  rosidl_runtime_c__String__fini(&msg->to_id);
}

bool
mrs_msgs__msg__EdgeRef__are_equal(const mrs_msgs__msg__EdgeRef * lhs, const mrs_msgs__msg__EdgeRef * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // from_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->from_id), &(rhs->from_id)))
  {
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->to_id), &(rhs->to_id)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__EdgeRef__copy(
  const mrs_msgs__msg__EdgeRef * input,
  mrs_msgs__msg__EdgeRef * output)
{
  if (!input || !output) {
    return false;
  }
  // from_id
  if (!rosidl_runtime_c__String__copy(
      &(input->from_id), &(output->from_id)))
  {
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__copy(
      &(input->to_id), &(output->to_id)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__EdgeRef *
mrs_msgs__msg__EdgeRef__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EdgeRef * msg = (mrs_msgs__msg__EdgeRef *)allocator.allocate(sizeof(mrs_msgs__msg__EdgeRef), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__EdgeRef));
  bool success = mrs_msgs__msg__EdgeRef__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__EdgeRef__destroy(mrs_msgs__msg__EdgeRef * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__EdgeRef__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__EdgeRef__Sequence__init(mrs_msgs__msg__EdgeRef__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EdgeRef * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__EdgeRef)) {
      return false;
    }
    data = (mrs_msgs__msg__EdgeRef *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__EdgeRef), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__EdgeRef__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__EdgeRef__fini(&data[i - 1]);
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
mrs_msgs__msg__EdgeRef__Sequence__fini(mrs_msgs__msg__EdgeRef__Sequence * array)
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
      mrs_msgs__msg__EdgeRef__fini(&array->data[i]);
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

mrs_msgs__msg__EdgeRef__Sequence *
mrs_msgs__msg__EdgeRef__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EdgeRef__Sequence * array = (mrs_msgs__msg__EdgeRef__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__EdgeRef__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__EdgeRef__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__EdgeRef__Sequence__destroy(mrs_msgs__msg__EdgeRef__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__EdgeRef__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__EdgeRef__Sequence__are_equal(const mrs_msgs__msg__EdgeRef__Sequence * lhs, const mrs_msgs__msg__EdgeRef__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__EdgeRef__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__EdgeRef__Sequence__copy(
  const mrs_msgs__msg__EdgeRef__Sequence * input,
  mrs_msgs__msg__EdgeRef__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__EdgeRef)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__EdgeRef);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__EdgeRef * data =
      (mrs_msgs__msg__EdgeRef *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__EdgeRef__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__EdgeRef__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__EdgeRef__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
