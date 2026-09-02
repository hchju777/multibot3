// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/EntryEvent.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/entry_event__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `robot`
// Member `segment`
// Member `location`
#include "rosidl_runtime_c/string_functions.h"

bool
mrs_msgs__msg__EntryEvent__init(mrs_msgs__msg__EntryEvent * msg)
{
  if (!msg) {
    return false;
  }
  // seq
  // robot
  if (!rosidl_runtime_c__String__init(&msg->robot)) {
    mrs_msgs__msg__EntryEvent__fini(msg);
    return false;
  }
  // segment
  if (!rosidl_runtime_c__String__init(&msg->segment)) {
    mrs_msgs__msg__EntryEvent__fini(msg);
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__init(&msg->location)) {
    mrs_msgs__msg__EntryEvent__fini(msg);
    return false;
  }
  // entered
  return true;
}

void
mrs_msgs__msg__EntryEvent__fini(mrs_msgs__msg__EntryEvent * msg)
{
  if (!msg) {
    return;
  }
  // seq
  // robot
  rosidl_runtime_c__String__fini(&msg->robot);
  // segment
  rosidl_runtime_c__String__fini(&msg->segment);
  // location
  rosidl_runtime_c__String__fini(&msg->location);
  // entered
}

bool
mrs_msgs__msg__EntryEvent__are_equal(const mrs_msgs__msg__EntryEvent * lhs, const mrs_msgs__msg__EntryEvent * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // seq
  if (lhs->seq != rhs->seq) {
    return false;
  }
  // robot
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->robot), &(rhs->robot)))
  {
    return false;
  }
  // segment
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->segment), &(rhs->segment)))
  {
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->location), &(rhs->location)))
  {
    return false;
  }
  // entered
  if (lhs->entered != rhs->entered) {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__EntryEvent__copy(
  const mrs_msgs__msg__EntryEvent * input,
  mrs_msgs__msg__EntryEvent * output)
{
  if (!input || !output) {
    return false;
  }
  // seq
  output->seq = input->seq;
  // robot
  if (!rosidl_runtime_c__String__copy(
      &(input->robot), &(output->robot)))
  {
    return false;
  }
  // segment
  if (!rosidl_runtime_c__String__copy(
      &(input->segment), &(output->segment)))
  {
    return false;
  }
  // location
  if (!rosidl_runtime_c__String__copy(
      &(input->location), &(output->location)))
  {
    return false;
  }
  // entered
  output->entered = input->entered;
  return true;
}

mrs_msgs__msg__EntryEvent *
mrs_msgs__msg__EntryEvent__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvent * msg = (mrs_msgs__msg__EntryEvent *)allocator.allocate(sizeof(mrs_msgs__msg__EntryEvent), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__EntryEvent));
  bool success = mrs_msgs__msg__EntryEvent__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__EntryEvent__destroy(mrs_msgs__msg__EntryEvent * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__EntryEvent__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__EntryEvent__Sequence__init(mrs_msgs__msg__EntryEvent__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvent * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__EntryEvent)) {
      return false;
    }
    data = (mrs_msgs__msg__EntryEvent *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__EntryEvent), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__EntryEvent__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__EntryEvent__fini(&data[i - 1]);
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
mrs_msgs__msg__EntryEvent__Sequence__fini(mrs_msgs__msg__EntryEvent__Sequence * array)
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
      mrs_msgs__msg__EntryEvent__fini(&array->data[i]);
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

mrs_msgs__msg__EntryEvent__Sequence *
mrs_msgs__msg__EntryEvent__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__EntryEvent__Sequence * array = (mrs_msgs__msg__EntryEvent__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__EntryEvent__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__EntryEvent__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__EntryEvent__Sequence__destroy(mrs_msgs__msg__EntryEvent__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__EntryEvent__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__EntryEvent__Sequence__are_equal(const mrs_msgs__msg__EntryEvent__Sequence * lhs, const mrs_msgs__msg__EntryEvent__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__EntryEvent__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__EntryEvent__Sequence__copy(
  const mrs_msgs__msg__EntryEvent__Sequence * input,
  mrs_msgs__msg__EntryEvent__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__EntryEvent)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__EntryEvent);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__EntryEvent * data =
      (mrs_msgs__msg__EntryEvent *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__EntryEvent__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__EntryEvent__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__EntryEvent__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
