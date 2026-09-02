// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/limits__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
mrs_msgs__msg__Limits__init(mrs_msgs__msg__Limits * msg)
{
  if (!msg) {
    return false;
  }
  // a_max
  // j_max
  // min_separation_m
  // omega_max_radps
  // yaw_accel_max_radps2
  return true;
}

void
mrs_msgs__msg__Limits__fini(mrs_msgs__msg__Limits * msg)
{
  if (!msg) {
    return;
  }
  // a_max
  // j_max
  // min_separation_m
  // omega_max_radps
  // yaw_accel_max_radps2
}

bool
mrs_msgs__msg__Limits__are_equal(const mrs_msgs__msg__Limits * lhs, const mrs_msgs__msg__Limits * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // a_max
  if (lhs->a_max != rhs->a_max) {
    return false;
  }
  // j_max
  if (lhs->j_max != rhs->j_max) {
    return false;
  }
  // min_separation_m
  if (lhs->min_separation_m != rhs->min_separation_m) {
    return false;
  }
  // omega_max_radps
  if (lhs->omega_max_radps != rhs->omega_max_radps) {
    return false;
  }
  // yaw_accel_max_radps2
  if (lhs->yaw_accel_max_radps2 != rhs->yaw_accel_max_radps2) {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__Limits__copy(
  const mrs_msgs__msg__Limits * input,
  mrs_msgs__msg__Limits * output)
{
  if (!input || !output) {
    return false;
  }
  // a_max
  output->a_max = input->a_max;
  // j_max
  output->j_max = input->j_max;
  // min_separation_m
  output->min_separation_m = input->min_separation_m;
  // omega_max_radps
  output->omega_max_radps = input->omega_max_radps;
  // yaw_accel_max_radps2
  output->yaw_accel_max_radps2 = input->yaw_accel_max_radps2;
  return true;
}

mrs_msgs__msg__Limits *
mrs_msgs__msg__Limits__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Limits * msg = (mrs_msgs__msg__Limits *)allocator.allocate(sizeof(mrs_msgs__msg__Limits), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__Limits));
  bool success = mrs_msgs__msg__Limits__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__Limits__destroy(mrs_msgs__msg__Limits * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__Limits__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__Limits__Sequence__init(mrs_msgs__msg__Limits__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Limits * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__Limits)) {
      return false;
    }
    data = (mrs_msgs__msg__Limits *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__Limits), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__Limits__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__Limits__fini(&data[i - 1]);
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
mrs_msgs__msg__Limits__Sequence__fini(mrs_msgs__msg__Limits__Sequence * array)
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
      mrs_msgs__msg__Limits__fini(&array->data[i]);
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

mrs_msgs__msg__Limits__Sequence *
mrs_msgs__msg__Limits__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Limits__Sequence * array = (mrs_msgs__msg__Limits__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__Limits__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__Limits__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__Limits__Sequence__destroy(mrs_msgs__msg__Limits__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__Limits__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__Limits__Sequence__are_equal(const mrs_msgs__msg__Limits__Sequence * lhs, const mrs_msgs__msg__Limits__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__Limits__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__Limits__Sequence__copy(
  const mrs_msgs__msg__Limits__Sequence * input,
  mrs_msgs__msg__Limits__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__Limits)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__Limits);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__Limits * data =
      (mrs_msgs__msg__Limits *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__Limits__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__Limits__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__Limits__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
