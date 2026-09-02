// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:msg/EdgeRef.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "mrs_msgs/msg/detail/edge_ref__functions.h"
#include "mrs_msgs/msg/detail/edge_ref__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace mrs_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void EdgeRef_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) mrs_msgs::msg::EdgeRef(_init);
}

void EdgeRef_fini_function(void * message_memory)
{
  auto typed_message = static_cast<mrs_msgs::msg::EdgeRef *>(message_memory);
  typed_message->~EdgeRef();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember EdgeRef_message_member_array[2] = {
  {
    "from_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::EdgeRef, from_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "to_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::EdgeRef, to_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers EdgeRef_message_members = {
  "mrs_msgs::msg",  // message namespace
  "EdgeRef",  // message name
  2,  // number of fields
  sizeof(mrs_msgs::msg::EdgeRef),
  false,  // has_any_key_member_
  EdgeRef_message_member_array,  // message members
  EdgeRef_init_function,  // function to initialize message memory (memory has to be allocated)
  EdgeRef_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t EdgeRef_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &EdgeRef_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__EdgeRef__get_type_hash,
  &mrs_msgs__msg__EdgeRef__get_type_description,
  &mrs_msgs__msg__EdgeRef__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace mrs_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::msg::EdgeRef>()
{
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::EdgeRef_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, msg, EdgeRef)() {
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::EdgeRef_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
