// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:msg/Trajectories.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "mrs_msgs/msg/detail/trajectories__functions.h"
#include "mrs_msgs/msg/detail/trajectories__struct.hpp"
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

void Trajectories_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) mrs_msgs::msg::Trajectories(_init);
}

void Trajectories_fini_function(void * message_memory)
{
  auto typed_message = static_cast<mrs_msgs::msg::Trajectories *>(message_memory);
  typed_message->~Trajectories();
}

size_t size_function__Trajectories__trajectories(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::RobotTrajectory> *>(untyped_member);
  return member->size();
}

const void * get_const_function__Trajectories__trajectories(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::RobotTrajectory> *>(untyped_member);
  return &member[index];
}

void * get_function__Trajectories__trajectories(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::RobotTrajectory> *>(untyped_member);
  return &member[index];
}

void fetch_function__Trajectories__trajectories(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::RobotTrajectory *>(
    get_const_function__Trajectories__trajectories(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::RobotTrajectory *>(untyped_value);
  value = item;
}

void assign_function__Trajectories__trajectories(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::RobotTrajectory *>(
    get_function__Trajectories__trajectories(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::RobotTrajectory *>(untyped_value);
  item = value;
}

void resize_function__Trajectories__trajectories(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::RobotTrajectory> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember Trajectories_message_member_array[5] = {
  {
    "schema",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Trajectories, schema),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "schema_version",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Trajectories, schema_version),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "instance_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Trajectories, instance_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "limits",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::Limits>(),  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Trajectories, limits),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "trajectories",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::RobotTrajectory>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::Trajectories, trajectories),  // bytes offset in struct
    nullptr,  // default value
    size_function__Trajectories__trajectories,  // size() function pointer
    get_const_function__Trajectories__trajectories,  // get_const(index) function pointer
    get_function__Trajectories__trajectories,  // get(index) function pointer
    fetch_function__Trajectories__trajectories,  // fetch(index, &value) function pointer
    assign_function__Trajectories__trajectories,  // assign(index, value) function pointer
    resize_function__Trajectories__trajectories  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers Trajectories_message_members = {
  "mrs_msgs::msg",  // message namespace
  "Trajectories",  // message name
  5,  // number of fields
  sizeof(mrs_msgs::msg::Trajectories),
  false,  // has_any_key_member_
  Trajectories_message_member_array,  // message members
  Trajectories_init_function,  // function to initialize message memory (memory has to be allocated)
  Trajectories_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t Trajectories_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &Trajectories_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__Trajectories__get_type_hash,
  &mrs_msgs__msg__Trajectories__get_type_description,
  &mrs_msgs__msg__Trajectories__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace mrs_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::msg::Trajectories>()
{
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::Trajectories_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, msg, Trajectories)() {
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::Trajectories_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
