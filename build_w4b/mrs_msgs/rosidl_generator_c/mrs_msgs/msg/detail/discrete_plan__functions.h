// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/discrete_plan.h"


#ifndef MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__FUNCTIONS_H_
#define MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"
#include "mrs_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "mrs_msgs/msg/detail/discrete_plan__struct.h"

/// Initialize msg/DiscretePlan message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * mrs_msgs__msg__DiscretePlan
 * )) before or use
 * mrs_msgs__msg__DiscretePlan__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__init(mrs_msgs__msg__DiscretePlan * msg);

/// Finalize msg/DiscretePlan message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
void
mrs_msgs__msg__DiscretePlan__fini(mrs_msgs__msg__DiscretePlan * msg);

/// Create msg/DiscretePlan message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * mrs_msgs__msg__DiscretePlan__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
mrs_msgs__msg__DiscretePlan *
mrs_msgs__msg__DiscretePlan__create(void);

/// Destroy msg/DiscretePlan message.
/**
 * It calls
 * mrs_msgs__msg__DiscretePlan__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
void
mrs_msgs__msg__DiscretePlan__destroy(mrs_msgs__msg__DiscretePlan * msg);

/// Check for msg/DiscretePlan message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__are_equal(const mrs_msgs__msg__DiscretePlan * lhs, const mrs_msgs__msg__DiscretePlan * rhs);

/// Copy a msg/DiscretePlan message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__copy(
  const mrs_msgs__msg__DiscretePlan * input,
  mrs_msgs__msg__DiscretePlan * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_type_hash_t *
mrs_msgs__msg__DiscretePlan__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_runtime_c__type_description__TypeDescription *
mrs_msgs__msg__DiscretePlan__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_runtime_c__type_description__TypeSource *
mrs_msgs__msg__DiscretePlan__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
mrs_msgs__msg__DiscretePlan__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/DiscretePlan messages.
/**
 * It allocates the memory for the number of elements and calls
 * mrs_msgs__msg__DiscretePlan__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__Sequence__init(mrs_msgs__msg__DiscretePlan__Sequence * array, size_t size);

/// Finalize array of msg/DiscretePlan messages.
/**
 * It calls
 * mrs_msgs__msg__DiscretePlan__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
void
mrs_msgs__msg__DiscretePlan__Sequence__fini(mrs_msgs__msg__DiscretePlan__Sequence * array);

/// Create array of msg/DiscretePlan messages.
/**
 * It allocates the memory for the array and calls
 * mrs_msgs__msg__DiscretePlan__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
mrs_msgs__msg__DiscretePlan__Sequence *
mrs_msgs__msg__DiscretePlan__Sequence__create(size_t size);

/// Destroy array of msg/DiscretePlan messages.
/**
 * It calls
 * mrs_msgs__msg__DiscretePlan__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
void
mrs_msgs__msg__DiscretePlan__Sequence__destroy(mrs_msgs__msg__DiscretePlan__Sequence * array);

/// Check for msg/DiscretePlan message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__Sequence__are_equal(const mrs_msgs__msg__DiscretePlan__Sequence * lhs, const mrs_msgs__msg__DiscretePlan__Sequence * rhs);

/// Copy an array of msg/DiscretePlan messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_mrs_msgs
bool
mrs_msgs__msg__DiscretePlan__Sequence__copy(
  const mrs_msgs__msg__DiscretePlan__Sequence * input,
  mrs_msgs__msg__DiscretePlan__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // MRS_MSGS__MSG__DETAIL__DISCRETE_PLAN__FUNCTIONS_H_
