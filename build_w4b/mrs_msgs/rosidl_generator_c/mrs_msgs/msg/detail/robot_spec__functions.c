// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/robot_spec__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `robot`
#include "rosidl_runtime_c/string_functions.h"

bool
mrs_msgs__msg__RobotSpec__init(mrs_msgs__msg__RobotSpec * msg)
{
  if (!msg) {
    return false;
  }
  // robot
  if (!rosidl_runtime_c__String__init(&msg->robot)) {
    mrs_msgs__msg__RobotSpec__fini(msg);
    return false;
  }
  // avg_traversal_speed_mps
  // v_max
  // brake_decel_min_mps2
  // curvature_max_invm
  // reverse_motion_allowed
  // circumradius_m
  return true;
}

void
mrs_msgs__msg__RobotSpec__fini(mrs_msgs__msg__RobotSpec * msg)
{
  if (!msg) {
    return;
  }
  // robot
  rosidl_runtime_c__String__fini(&msg->robot);
  // avg_traversal_speed_mps
  // v_max
  // brake_decel_min_mps2
  // curvature_max_invm
  // reverse_motion_allowed
  // circumradius_m
}

bool
mrs_msgs__msg__RobotSpec__are_equal(const mrs_msgs__msg__RobotSpec * lhs, const mrs_msgs__msg__RobotSpec * rhs)
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
  // avg_traversal_speed_mps
  if (lhs->avg_traversal_speed_mps != rhs->avg_traversal_speed_mps) {
    return false;
  }
  // v_max
  if (lhs->v_max != rhs->v_max) {
    return false;
  }
  // brake_decel_min_mps2
  if (lhs->brake_decel_min_mps2 != rhs->brake_decel_min_mps2) {
    return false;
  }
  // curvature_max_invm
  if (lhs->curvature_max_invm != rhs->curvature_max_invm) {
    return false;
  }
  // reverse_motion_allowed
  if (lhs->reverse_motion_allowed != rhs->reverse_motion_allowed) {
    return false;
  }
  // circumradius_m
  if (lhs->circumradius_m != rhs->circumradius_m) {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__RobotSpec__copy(
  const mrs_msgs__msg__RobotSpec * input,
  mrs_msgs__msg__RobotSpec * output)
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
  // avg_traversal_speed_mps
  output->avg_traversal_speed_mps = input->avg_traversal_speed_mps;
  // v_max
  output->v_max = input->v_max;
  // brake_decel_min_mps2
  output->brake_decel_min_mps2 = input->brake_decel_min_mps2;
  // curvature_max_invm
  output->curvature_max_invm = input->curvature_max_invm;
  // reverse_motion_allowed
  output->reverse_motion_allowed = input->reverse_motion_allowed;
  // circumradius_m
  output->circumradius_m = input->circumradius_m;
  return true;
}

mrs_msgs__msg__RobotSpec *
mrs_msgs__msg__RobotSpec__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotSpec * msg = (mrs_msgs__msg__RobotSpec *)allocator.allocate(sizeof(mrs_msgs__msg__RobotSpec), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__RobotSpec));
  bool success = mrs_msgs__msg__RobotSpec__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__RobotSpec__destroy(mrs_msgs__msg__RobotSpec * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__RobotSpec__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__RobotSpec__Sequence__init(mrs_msgs__msg__RobotSpec__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotSpec * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__RobotSpec)) {
      return false;
    }
    data = (mrs_msgs__msg__RobotSpec *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__RobotSpec), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__RobotSpec__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__RobotSpec__fini(&data[i - 1]);
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
mrs_msgs__msg__RobotSpec__Sequence__fini(mrs_msgs__msg__RobotSpec__Sequence * array)
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
      mrs_msgs__msg__RobotSpec__fini(&array->data[i]);
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

mrs_msgs__msg__RobotSpec__Sequence *
mrs_msgs__msg__RobotSpec__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RobotSpec__Sequence * array = (mrs_msgs__msg__RobotSpec__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__RobotSpec__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__RobotSpec__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__RobotSpec__Sequence__destroy(mrs_msgs__msg__RobotSpec__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__RobotSpec__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__RobotSpec__Sequence__are_equal(const mrs_msgs__msg__RobotSpec__Sequence * lhs, const mrs_msgs__msg__RobotSpec__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__RobotSpec__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__RobotSpec__Sequence__copy(
  const mrs_msgs__msg__RobotSpec__Sequence * input,
  mrs_msgs__msg__RobotSpec__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__RobotSpec)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__RobotSpec);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__RobotSpec * data =
      (mrs_msgs__msg__RobotSpec *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__RobotSpec__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__RobotSpec__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__RobotSpec__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
