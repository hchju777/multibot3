// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:msg/DiscretePlan.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "mrs_msgs/msg/detail/discrete_plan__functions.h"
#include "mrs_msgs/msg/detail/discrete_plan__struct.hpp"
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

void DiscretePlan_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) mrs_msgs::msg::DiscretePlan(_init);
}

void DiscretePlan_fini_function(void * message_memory)
{
  auto typed_message = static_cast<mrs_msgs::msg::DiscretePlan *>(message_memory);
  typed_message->~DiscretePlan();
}

size_t size_function__DiscretePlan__plans(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::RobotPlan> *>(untyped_member);
  return member->size();
}

const void * get_const_function__DiscretePlan__plans(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::RobotPlan> *>(untyped_member);
  return &member[index];
}

void * get_function__DiscretePlan__plans(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::RobotPlan> *>(untyped_member);
  return &member[index];
}

void fetch_function__DiscretePlan__plans(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::RobotPlan *>(
    get_const_function__DiscretePlan__plans(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::RobotPlan *>(untyped_value);
  value = item;
}

void assign_function__DiscretePlan__plans(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::RobotPlan *>(
    get_function__DiscretePlan__plans(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::RobotPlan *>(untyped_value);
  item = value;
}

void resize_function__DiscretePlan__plans(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::RobotPlan> *>(untyped_member);
  member->resize(size);
}

size_t size_function__DiscretePlan__visit_order(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<mrs_msgs::msg::VisitOrder> *>(untyped_member);
  return member->size();
}

const void * get_const_function__DiscretePlan__visit_order(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<mrs_msgs::msg::VisitOrder> *>(untyped_member);
  return &member[index];
}

void * get_function__DiscretePlan__visit_order(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<mrs_msgs::msg::VisitOrder> *>(untyped_member);
  return &member[index];
}

void fetch_function__DiscretePlan__visit_order(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const mrs_msgs::msg::VisitOrder *>(
    get_const_function__DiscretePlan__visit_order(untyped_member, index));
  auto & value = *reinterpret_cast<mrs_msgs::msg::VisitOrder *>(untyped_value);
  value = item;
}

void assign_function__DiscretePlan__visit_order(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<mrs_msgs::msg::VisitOrder *>(
    get_function__DiscretePlan__visit_order(untyped_member, index));
  const auto & value = *reinterpret_cast<const mrs_msgs::msg::VisitOrder *>(untyped_value);
  item = value;
}

void resize_function__DiscretePlan__visit_order(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<mrs_msgs::msg::VisitOrder> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember DiscretePlan_message_member_array[6] = {
  {
    "schema",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::DiscretePlan, schema),  // bytes offset in struct
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
    offsetof(mrs_msgs::msg::DiscretePlan, schema_version),  // bytes offset in struct
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
    offsetof(mrs_msgs::msg::DiscretePlan, instance_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "plan_revision",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::DiscretePlan, plan_revision),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "plans",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::RobotPlan>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::DiscretePlan, plans),  // bytes offset in struct
    nullptr,  // default value
    size_function__DiscretePlan__plans,  // size() function pointer
    get_const_function__DiscretePlan__plans,  // get_const(index) function pointer
    get_function__DiscretePlan__plans,  // get(index) function pointer
    fetch_function__DiscretePlan__plans,  // fetch(index, &value) function pointer
    assign_function__DiscretePlan__plans,  // assign(index, value) function pointer
    resize_function__DiscretePlan__plans  // resize(index) function pointer
  },
  {
    "visit_order",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<mrs_msgs::msg::VisitOrder>(),  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mrs_msgs::msg::DiscretePlan, visit_order),  // bytes offset in struct
    nullptr,  // default value
    size_function__DiscretePlan__visit_order,  // size() function pointer
    get_const_function__DiscretePlan__visit_order,  // get_const(index) function pointer
    get_function__DiscretePlan__visit_order,  // get(index) function pointer
    fetch_function__DiscretePlan__visit_order,  // fetch(index, &value) function pointer
    assign_function__DiscretePlan__visit_order,  // assign(index, value) function pointer
    resize_function__DiscretePlan__visit_order  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers DiscretePlan_message_members = {
  "mrs_msgs::msg",  // message namespace
  "DiscretePlan",  // message name
  6,  // number of fields
  sizeof(mrs_msgs::msg::DiscretePlan),
  false,  // has_any_key_member_
  DiscretePlan_message_member_array,  // message members
  DiscretePlan_init_function,  // function to initialize message memory (memory has to be allocated)
  DiscretePlan_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t DiscretePlan_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &DiscretePlan_message_members,
  get_message_typesupport_handle_function,
  &mrs_msgs__msg__DiscretePlan__get_type_hash,
  &mrs_msgs__msg__DiscretePlan__get_type_description,
  &mrs_msgs__msg__DiscretePlan__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace mrs_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::msg::DiscretePlan>()
{
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::DiscretePlan_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, msg, DiscretePlan)() {
  return &::mrs_msgs::msg::rosidl_typesupport_introspection_cpp::DiscretePlan_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
