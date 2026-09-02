// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/Corridor.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/corridor__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `id`
#include "rosidl_runtime_c/string_functions.h"
// Member `edges`
#include "mrs_msgs/msg/detail/edge_ref__functions.h"

bool
mrs_msgs__msg__Corridor__init(mrs_msgs__msg__Corridor * msg)
{
  if (!msg) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    mrs_msgs__msg__Corridor__fini(msg);
    return false;
  }
  // edges
  if (!mrs_msgs__msg__EdgeRef__Sequence__init(&msg->edges, 0)) {
    mrs_msgs__msg__Corridor__fini(msg);
    return false;
  }
  // length_m
  return true;
}

void
mrs_msgs__msg__Corridor__fini(mrs_msgs__msg__Corridor * msg)
{
  if (!msg) {
    return;
  }
  // id
  rosidl_runtime_c__String__fini(&msg->id);
  // edges
  mrs_msgs__msg__EdgeRef__Sequence__fini(&msg->edges);
  // length_m
}

bool
mrs_msgs__msg__Corridor__are_equal(const mrs_msgs__msg__Corridor * lhs, const mrs_msgs__msg__Corridor * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  // edges
  if (!mrs_msgs__msg__EdgeRef__Sequence__are_equal(
      &(lhs->edges), &(rhs->edges)))
  {
    return false;
  }
  // length_m
  if (lhs->length_m != rhs->length_m) {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__Corridor__copy(
  const mrs_msgs__msg__Corridor * input,
  mrs_msgs__msg__Corridor * output)
{
  if (!input || !output) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  // edges
  if (!mrs_msgs__msg__EdgeRef__Sequence__copy(
      &(input->edges), &(output->edges)))
  {
    return false;
  }
  // length_m
  output->length_m = input->length_m;
  return true;
}

mrs_msgs__msg__Corridor *
mrs_msgs__msg__Corridor__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Corridor * msg = (mrs_msgs__msg__Corridor *)allocator.allocate(sizeof(mrs_msgs__msg__Corridor), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__Corridor));
  bool success = mrs_msgs__msg__Corridor__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__Corridor__destroy(mrs_msgs__msg__Corridor * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__Corridor__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__Corridor__Sequence__init(mrs_msgs__msg__Corridor__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Corridor * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__Corridor)) {
      return false;
    }
    data = (mrs_msgs__msg__Corridor *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__Corridor), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__Corridor__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__Corridor__fini(&data[i - 1]);
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
mrs_msgs__msg__Corridor__Sequence__fini(mrs_msgs__msg__Corridor__Sequence * array)
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
      mrs_msgs__msg__Corridor__fini(&array->data[i]);
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

mrs_msgs__msg__Corridor__Sequence *
mrs_msgs__msg__Corridor__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Corridor__Sequence * array = (mrs_msgs__msg__Corridor__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__Corridor__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__Corridor__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__Corridor__Sequence__destroy(mrs_msgs__msg__Corridor__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__Corridor__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__Corridor__Sequence__are_equal(const mrs_msgs__msg__Corridor__Sequence * lhs, const mrs_msgs__msg__Corridor__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__Corridor__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__Corridor__Sequence__copy(
  const mrs_msgs__msg__Corridor__Sequence * input,
  mrs_msgs__msg__Corridor__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__Corridor)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__Corridor);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__Corridor * data =
      (mrs_msgs__msg__Corridor *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__Corridor__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__Corridor__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__Corridor__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
