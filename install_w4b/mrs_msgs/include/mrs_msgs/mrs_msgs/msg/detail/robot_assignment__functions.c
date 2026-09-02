// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/RobotAssignment.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/robot_assignment__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `robot`
// Member `start`
#include "rosidl_runtime_c/string_functions.h"
// Member `goals`
#include "mrs_msgs/msg/detail/goal__functions.h"

bool
mrs_msgs__msg__RobotAssignment__init(mrs_msgs__msg__RobotAssignment * msg)
{
  if (!msg) {
    return false;
  }
  // robot
  if (!rosidl_runtime_c__String__init(&msg->robot)) {
    mrs_msgs__msg__RobotAssignment__fini(msg);
    return false;
  }
  // start
  if (!rosidl_runtime_c__String__init(&msg->start)) {
    mrs_msgs__msg__RobotAssignment__fini(msg);
    return false;
  }
  // goals
  if (!mrs_msgs__msg__Goal__Sequence__init(&msg->goals, 0)) {
    mrs_msgs__msg__RobotAssignment__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__RobotAssignment__fini(mrs_msgs__msg__RobotAssignment * msg)
{
  if (!msg) {
    return;
  }
  // robot
  rosidl_runtime_c__String__fini(&msg->robot);
  // start
  rosidl_runtime_c__String__fini(&msg->start);
  // goals
  mrs_msgs__msg__Goal__Sequence__fini(&msg->goals);
}

bool
mrs_msgs__msg__RobotAssignment__are_equal(const mrs_msgs__msg__RobotAssignment * lhs, const mrs_msgs__msg__RobotAssignment * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // robot
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->robot), &(rhs->robot)))
  {
    return false;
  }
  // start
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->start), &(rhs->start)))
  {
    return false;
  }
  // goals
  if (!mrs_msgs__msg__Goal__Sequence__are_equal(
      &(lhs->goals), &(rhs->goals)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__RobotAssignment__copy(
  const mrs_msgs__msg__RobotAssignment * input,
  mrs_msgs__msg__RobotAssignment * output)
{
  if (!input || !output) {
    return false;
  }
  // robot
  if (!rosidl_runtime_c__String__copy(
      &(input->robot), &(output->robot)))
  {
    return false;
  }
  // start
  if (!rosidl_runtime_c__String__copy(
      &(input->start), &(output->start)))
  {
    return false;
  }
  // goals
  if (!mrs_msgs__msg__Goal__Sequence__copy(
      &(input->goals), &(output->goals)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__RobotAssignment *
mrs_msgs__msg__RobotAssignment__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotAssignment * msg = (mrs_msgs__msg__RobotAssignment *)allocator.allocate(sizeof(mrs_msgs__msg__RobotAssignment), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__RobotAssignment));
  bool success = mrs_msgs__msg__RobotAssignment__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__RobotAssignment__destroy(mrs_msgs__msg__RobotAssignment * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__RobotAssignment__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__RobotAssignment__Sequence__init(mrs_msgs__msg__RobotAssignment__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotAssignment * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__RobotAssignment)) {
      return false;
    }
    data = (mrs_msgs__msg__RobotAssignment *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__RobotAssignment), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__RobotAssignment__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__RobotAssignment__fini(&data[i - 1]);
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
mrs_msgs__msg__RobotAssignment__Sequence__fini(mrs_msgs__msg__RobotAssignment__Sequence * array)
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
      mrs_msgs__msg__RobotAssignment__fini(&array->data[i]);
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

mrs_msgs__msg__RobotAssignment__Sequence *
mrs_msgs__msg__RobotAssignment__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotAssignment__Sequence * array = (mrs_msgs__msg__RobotAssignment__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__RobotAssignment__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__RobotAssignment__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__RobotAssignment__Sequence__destroy(mrs_msgs__msg__RobotAssignment__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__RobotAssignment__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__RobotAssignment__Sequence__are_equal(const mrs_msgs__msg__RobotAssignment__Sequence * lhs, const mrs_msgs__msg__RobotAssignment__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__RobotAssignment__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__RobotAssignment__Sequence__copy(
  const mrs_msgs__msg__RobotAssignment__Sequence * input,
  mrs_msgs__msg__RobotAssignment__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__RobotAssignment)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__RobotAssignment);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__RobotAssignment * data =
      (mrs_msgs__msg__RobotAssignment *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__RobotAssignment__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__RobotAssignment__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__RobotAssignment__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
