// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/roadmap_edge__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `from_id`
// Member `to_id`
// Member `corridor`
#include "rosidl_runtime_c/string_functions.h"

bool
mrs_msgs__msg__RoadmapEdge__init(mrs_msgs__msg__RoadmapEdge * msg)
{
  if (!msg) {
    return false;
  }
  // from_id
  if (!rosidl_runtime_c__String__init(&msg->from_id)) {
    mrs_msgs__msg__RoadmapEdge__fini(msg);
    return false;
  }
  // to_id
  if (!rosidl_runtime_c__String__init(&msg->to_id)) {
    mrs_msgs__msg__RoadmapEdge__fini(msg);
    return false;
  }
  // traversal
  // length_m
  // width_m
  // capacity_robots
  // corridor
  if (!rosidl_runtime_c__String__init(&msg->corridor)) {
    mrs_msgs__msg__RoadmapEdge__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__RoadmapEdge__fini(mrs_msgs__msg__RoadmapEdge * msg)
{
  if (!msg) {
    return;
  }
  // from_id
  rosidl_runtime_c__String__fini(&msg->from_id);
  // to_id
  rosidl_runtime_c__String__fini(&msg->to_id);
  // traversal
  // length_m
  // width_m
  // capacity_robots
  // corridor
  rosidl_runtime_c__String__fini(&msg->corridor);
}

bool
mrs_msgs__msg__RoadmapEdge__are_equal(const mrs_msgs__msg__RoadmapEdge * lhs, const mrs_msgs__msg__RoadmapEdge * rhs)
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
  // traversal
  if (lhs->traversal != rhs->traversal) {
    return false;
  }
  // length_m
  if (lhs->length_m != rhs->length_m) {
    return false;
  }
  // width_m
  if (lhs->width_m != rhs->width_m) {
    return false;
  }
  // capacity_robots
  if (lhs->capacity_robots != rhs->capacity_robots) {
    return false;
  }
  // corridor
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->corridor), &(rhs->corridor)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__RoadmapEdge__copy(
  const mrs_msgs__msg__RoadmapEdge * input,
  mrs_msgs__msg__RoadmapEdge * output)
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
  // traversal
  output->traversal = input->traversal;
  // length_m
  output->length_m = input->length_m;
  // width_m
  output->width_m = input->width_m;
  // capacity_robots
  output->capacity_robots = input->capacity_robots;
  // corridor
  if (!rosidl_runtime_c__String__copy(
      &(input->corridor), &(output->corridor)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__RoadmapEdge *
mrs_msgs__msg__RoadmapEdge__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapEdge * msg = (mrs_msgs__msg__RoadmapEdge *)allocator.allocate(sizeof(mrs_msgs__msg__RoadmapEdge), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__RoadmapEdge));
  bool success = mrs_msgs__msg__RoadmapEdge__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__RoadmapEdge__destroy(mrs_msgs__msg__RoadmapEdge * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__RoadmapEdge__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__RoadmapEdge__Sequence__init(mrs_msgs__msg__RoadmapEdge__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapEdge * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__RoadmapEdge)) {
      return false;
    }
    data = (mrs_msgs__msg__RoadmapEdge *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__RoadmapEdge), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__RoadmapEdge__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__RoadmapEdge__fini(&data[i - 1]);
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
mrs_msgs__msg__RoadmapEdge__Sequence__fini(mrs_msgs__msg__RoadmapEdge__Sequence * array)
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
      mrs_msgs__msg__RoadmapEdge__fini(&array->data[i]);
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

mrs_msgs__msg__RoadmapEdge__Sequence *
mrs_msgs__msg__RoadmapEdge__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapEdge__Sequence * array = (mrs_msgs__msg__RoadmapEdge__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__RoadmapEdge__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__RoadmapEdge__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__RoadmapEdge__Sequence__destroy(mrs_msgs__msg__RoadmapEdge__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__RoadmapEdge__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__RoadmapEdge__Sequence__are_equal(const mrs_msgs__msg__RoadmapEdge__Sequence * lhs, const mrs_msgs__msg__RoadmapEdge__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__RoadmapEdge__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__RoadmapEdge__Sequence__copy(
  const mrs_msgs__msg__RoadmapEdge__Sequence * input,
  mrs_msgs__msg__RoadmapEdge__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__RoadmapEdge)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__RoadmapEdge);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__RoadmapEdge * data =
      (mrs_msgs__msg__RoadmapEdge *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__RoadmapEdge__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__RoadmapEdge__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__RoadmapEdge__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
