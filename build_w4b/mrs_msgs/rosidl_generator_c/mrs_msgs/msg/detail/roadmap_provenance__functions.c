// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/RoadmapProvenance.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/roadmap_provenance__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `capacity_rule_id`
// Member `generator_version`
#include "rosidl_runtime_c/string_functions.h"

bool
mrs_msgs__msg__RoadmapProvenance__init(mrs_msgs__msg__RoadmapProvenance * msg)
{
  if (!msg) {
    return false;
  }
  // min_separation_m
  // wall_inflation_m
  // capacity_rule_id
  if (!rosidl_runtime_c__String__init(&msg->capacity_rule_id)) {
    mrs_msgs__msg__RoadmapProvenance__fini(msg);
    return false;
  }
  // generator_version
  if (!rosidl_runtime_c__String__init(&msg->generator_version)) {
    mrs_msgs__msg__RoadmapProvenance__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__RoadmapProvenance__fini(mrs_msgs__msg__RoadmapProvenance * msg)
{
  if (!msg) {
    return;
  }
  // min_separation_m
  // wall_inflation_m
  // capacity_rule_id
  rosidl_runtime_c__String__fini(&msg->capacity_rule_id);
  // generator_version
  rosidl_runtime_c__String__fini(&msg->generator_version);
}

bool
mrs_msgs__msg__RoadmapProvenance__are_equal(const mrs_msgs__msg__RoadmapProvenance * lhs, const mrs_msgs__msg__RoadmapProvenance * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // min_separation_m
  if (lhs->min_separation_m != rhs->min_separation_m) {
    return false;
  }
  // wall_inflation_m
  if (lhs->wall_inflation_m != rhs->wall_inflation_m) {
    return false;
  }
  // capacity_rule_id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->capacity_rule_id), &(rhs->capacity_rule_id)))
  {
    return false;
  }
  // generator_version
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->generator_version), &(rhs->generator_version)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__RoadmapProvenance__copy(
  const mrs_msgs__msg__RoadmapProvenance * input,
  mrs_msgs__msg__RoadmapProvenance * output)
{
  if (!input || !output) {
    return false;
  }
  // min_separation_m
  output->min_separation_m = input->min_separation_m;
  // wall_inflation_m
  output->wall_inflation_m = input->wall_inflation_m;
  // capacity_rule_id
  if (!rosidl_runtime_c__String__copy(
      &(input->capacity_rule_id), &(output->capacity_rule_id)))
  {
    return false;
  }
  // generator_version
  if (!rosidl_runtime_c__String__copy(
      &(input->generator_version), &(output->generator_version)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__RoadmapProvenance *
mrs_msgs__msg__RoadmapProvenance__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapProvenance * msg = (mrs_msgs__msg__RoadmapProvenance *)allocator.allocate(sizeof(mrs_msgs__msg__RoadmapProvenance), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__RoadmapProvenance));
  bool success = mrs_msgs__msg__RoadmapProvenance__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__RoadmapProvenance__destroy(mrs_msgs__msg__RoadmapProvenance * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__RoadmapProvenance__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__RoadmapProvenance__Sequence__init(mrs_msgs__msg__RoadmapProvenance__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapProvenance * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__RoadmapProvenance)) {
      return false;
    }
    data = (mrs_msgs__msg__RoadmapProvenance *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__RoadmapProvenance), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__RoadmapProvenance__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__RoadmapProvenance__fini(&data[i - 1]);
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
mrs_msgs__msg__RoadmapProvenance__Sequence__fini(mrs_msgs__msg__RoadmapProvenance__Sequence * array)
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
      mrs_msgs__msg__RoadmapProvenance__fini(&array->data[i]);
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

mrs_msgs__msg__RoadmapProvenance__Sequence *
mrs_msgs__msg__RoadmapProvenance__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__RoadmapProvenance__Sequence * array = (mrs_msgs__msg__RoadmapProvenance__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__RoadmapProvenance__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__RoadmapProvenance__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__RoadmapProvenance__Sequence__destroy(mrs_msgs__msg__RoadmapProvenance__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__RoadmapProvenance__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__RoadmapProvenance__Sequence__are_equal(const mrs_msgs__msg__RoadmapProvenance__Sequence * lhs, const mrs_msgs__msg__RoadmapProvenance__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__RoadmapProvenance__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__RoadmapProvenance__Sequence__copy(
  const mrs_msgs__msg__RoadmapProvenance__Sequence * input,
  mrs_msgs__msg__RoadmapProvenance__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__RoadmapProvenance)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__RoadmapProvenance);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__RoadmapProvenance * data =
      (mrs_msgs__msg__RoadmapProvenance *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__RoadmapProvenance__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__RoadmapProvenance__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__RoadmapProvenance__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
