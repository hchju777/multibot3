// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice
#include "mrs_msgs/msg/detail/roadmap__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `schema`
// Member `schema_version`
// Member `instance_id`
// Member `endpoints`
#include "rosidl_runtime_c/string_functions.h"
// Member `nodes`
#include "mrs_msgs/msg/detail/roadmap_node__functions.h"
// Member `edges`
#include "mrs_msgs/msg/detail/roadmap_edge__functions.h"
// Member `corridors`
#include "mrs_msgs/msg/detail/corridor__functions.h"
// Member `provenance`
#include "mrs_msgs/msg/detail/roadmap_provenance__functions.h"

bool
mrs_msgs__msg__Roadmap__init(mrs_msgs__msg__Roadmap * msg)
{
  if (!msg) {
    return false;
  }
  // schema
  if (!rosidl_runtime_c__String__init(&msg->schema)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // schema_version
  if (!rosidl_runtime_c__String__init(&msg->schema_version)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // instance_id
  if (!rosidl_runtime_c__String__init(&msg->instance_id)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // nodes
  if (!mrs_msgs__msg__RoadmapNode__Sequence__init(&msg->nodes, 0)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // edges
  if (!mrs_msgs__msg__RoadmapEdge__Sequence__init(&msg->edges, 0)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // endpoints
  if (!rosidl_runtime_c__String__Sequence__init(&msg->endpoints, 0)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // corridors
  if (!mrs_msgs__msg__Corridor__Sequence__init(&msg->corridors, 0)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  // provenance
  if (!mrs_msgs__msg__RoadmapProvenance__init(&msg->provenance)) {
    mrs_msgs__msg__Roadmap__fini(msg);
    return false;
  }
  return true;
}

void
mrs_msgs__msg__Roadmap__fini(mrs_msgs__msg__Roadmap * msg)
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
  // nodes
  mrs_msgs__msg__RoadmapNode__Sequence__fini(&msg->nodes);
  // edges
  mrs_msgs__msg__RoadmapEdge__Sequence__fini(&msg->edges);
  // endpoints
  rosidl_runtime_c__String__Sequence__fini(&msg->endpoints);
  // corridors
  mrs_msgs__msg__Corridor__Sequence__fini(&msg->corridors);
  // provenance
  mrs_msgs__msg__RoadmapProvenance__fini(&msg->provenance);
}

bool
mrs_msgs__msg__Roadmap__are_equal(const mrs_msgs__msg__Roadmap * lhs, const mrs_msgs__msg__Roadmap * rhs)
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
  // nodes
  if (!mrs_msgs__msg__RoadmapNode__Sequence__are_equal(
      &(lhs->nodes), &(rhs->nodes)))
  {
    return false;
  }
  // edges
  if (!mrs_msgs__msg__RoadmapEdge__Sequence__are_equal(
      &(lhs->edges), &(rhs->edges)))
  {
    return false;
  }
  // endpoints
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->endpoints), &(rhs->endpoints)))
  {
    return false;
  }
  // corridors
  if (!mrs_msgs__msg__Corridor__Sequence__are_equal(
      &(lhs->corridors), &(rhs->corridors)))
  {
    return false;
  }
  // provenance
  if (!mrs_msgs__msg__RoadmapProvenance__are_equal(
      &(lhs->provenance), &(rhs->provenance)))
  {
    return false;
  }
  return true;
}

bool
mrs_msgs__msg__Roadmap__copy(
  const mrs_msgs__msg__Roadmap * input,
  mrs_msgs__msg__Roadmap * output)
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
  // nodes
  if (!mrs_msgs__msg__RoadmapNode__Sequence__copy(
      &(input->nodes), &(output->nodes)))
  {
    return false;
  }
  // edges
  if (!mrs_msgs__msg__RoadmapEdge__Sequence__copy(
      &(input->edges), &(output->edges)))
  {
    return false;
  }
  // endpoints
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->endpoints), &(output->endpoints)))
  {
    return false;
  }
  // corridors
  if (!mrs_msgs__msg__Corridor__Sequence__copy(
      &(input->corridors), &(output->corridors)))
  {
    return false;
  }
  // provenance
  if (!mrs_msgs__msg__RoadmapProvenance__copy(
      &(input->provenance), &(output->provenance)))
  {
    return false;
  }
  return true;
}

mrs_msgs__msg__Roadmap *
mrs_msgs__msg__Roadmap__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Roadmap * msg = (mrs_msgs__msg__Roadmap *)allocator.allocate(sizeof(mrs_msgs__msg__Roadmap), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mrs_msgs__msg__Roadmap));
  bool success = mrs_msgs__msg__Roadmap__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mrs_msgs__msg__Roadmap__destroy(mrs_msgs__msg__Roadmap * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mrs_msgs__msg__Roadmap__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mrs_msgs__msg__Roadmap__Sequence__init(mrs_msgs__msg__Roadmap__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Roadmap * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(mrs_msgs__msg__Roadmap)) {
      return false;
    }
    data = (mrs_msgs__msg__Roadmap *)allocator.zero_allocate(size, sizeof(mrs_msgs__msg__Roadmap), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mrs_msgs__msg__Roadmap__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mrs_msgs__msg__Roadmap__fini(&data[i - 1]);
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
mrs_msgs__msg__Roadmap__Sequence__fini(mrs_msgs__msg__Roadmap__Sequence * array)
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
      mrs_msgs__msg__Roadmap__fini(&array->data[i]);
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

mrs_msgs__msg__Roadmap__Sequence *
mrs_msgs__msg__Roadmap__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mrs_msgs__msg__Roadmap__Sequence * array = (mrs_msgs__msg__Roadmap__Sequence *)allocator.allocate(sizeof(mrs_msgs__msg__Roadmap__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mrs_msgs__msg__Roadmap__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mrs_msgs__msg__Roadmap__Sequence__destroy(mrs_msgs__msg__Roadmap__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mrs_msgs__msg__Roadmap__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mrs_msgs__msg__Roadmap__Sequence__are_equal(const mrs_msgs__msg__Roadmap__Sequence * lhs, const mrs_msgs__msg__Roadmap__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mrs_msgs__msg__Roadmap__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mrs_msgs__msg__Roadmap__Sequence__copy(
  const mrs_msgs__msg__Roadmap__Sequence * input,
  mrs_msgs__msg__Roadmap__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(mrs_msgs__msg__Roadmap)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(mrs_msgs__msg__Roadmap);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mrs_msgs__msg__Roadmap * data =
      (mrs_msgs__msg__Roadmap *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mrs_msgs__msg__Roadmap__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mrs_msgs__msg__Roadmap__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mrs_msgs__msg__Roadmap__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
