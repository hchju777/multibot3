// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:msg/RobotAssignment.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "mrs_msgs/msg/detail/robot_assignment__functions.h"
#include "mrs_msgs/msg/detail/robot_assignment__struct.hpp"
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

void RobotAssignment_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) mrs_msgs::msg::RobotAssignment(_init);
}

void RobotAssignment_fini_function(void * message_memory)
{
  auto typed_message = static_cast<mrs_msgs::msg::RobotAssignment *>(message_memory);
  typed_message->~RobotAssignment();
}

size_t size_function__RobotAssignment__goals(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::Goal> *>(untyped_member);
  return member->size();
}

const void * get_const_function__RobotAssignment__goals(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::Goal> *>(untyped_member);
  return &member[index];
}

void * get_function__RobotAssignment__goals(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::Goal> *>(untyped_member);
  return &member[index];
}

void fetch_function__RobotAssignment__goals(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::Goal *>(
    get_const_function__RobotAssignment__goals(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::Goal *>(untyped_value);
  value = item;
}

void assign_function__RobotAssignment__goals(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::Goal *>(
    get_function__RobotAssignment__goals(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::Goal *>(untyped_value);
  item = value;
}

void resize_function__RobotAssignment__goals(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::Goal> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember RobotAssignment_message_member_array[3] = {
  {
    "robot",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::RobotAssignment, robot),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "start",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::RobotAssignment, start),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "goals",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::Goal>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::RobotAssignment, goals),  // bytes offset in struct
    nullptr,  // default value
    size_function__RobotAssignment__goals,  // size() function pointer
    get_const_function__RobotAssignment__goals,  // get_const(index) function pointer
    get_function__RobotAssignment__goals,  // get(index) function pointer
    fetch_function__RobotAssignment__goals,  // fetch(index, &value) function pointer
    assign_function__RobotAssignment__goals,  // assign(index, value) function pointer
    resize_function__RobotAssignment__goals  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers RobotAssignment_message_members = {
  "mrs_msgs::msg",  // message namespace
  "RobotAssignment",  // message name
  3,  // number of fields
  sizeof(mrs_msgs::msg::RobotAssignment),
  false,  // has_any_key_member_
  RobotAssignment_message_member_array,  // message members
  RobotAssignment_init_function,  // function to initialize message memory (memory has to be allocated)
  RobotAssignment_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t RobotAssignment_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &RobotAssignment_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__RobotAssignment__get_type_hash,
  &mrs_msgs__msg__RobotAssignment__get_type_description,
  &mrs_msgs__msg__RobotAssignment__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace mrs_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::msg::RobotAssignment>()
{
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::RobotAssignment_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, msg, RobotAssignment)() {
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::RobotAssignment_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
