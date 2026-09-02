// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/StopDeclarations.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declarations.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/stop_declarations__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'declarations'
#include "mrs_msgs/msg/detail/stop_declaration__traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const StopDeclarations & msg,
  std::ostream & out)
{
  out << "{";
  // member: schema
  {
    out << "schema: ";
    rosidl_generator_traits::value_to_yaml(msg.schema, out);
    out << ", ";
  }

  // member: schema_version
  {
    out << "schema_version: ";
    rosidl_generator_traits::value_to_yaml(msg.schema_version, out);
    out << ", ";
  }

  // member: instance_id
  {
    out << "instance_id: ";
    rosidl_generator_traits::value_to_yaml(msg.instance_id, out);
    out << ", ";
  }

  // member: declarations
  {
    if (msg.declarations.size() == 0) {
      out << "declarations: []";
    } else {
      out << "declarations: [";
      size_t pending_items = msg.declarations.size();
      for (auto item : msg.declarations) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const StopDeclarations & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: schema
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "schema: ";
    rosidl_generator_traits::value_to_yaml(msg.schema, out);
    out << "\n";
  }

  // member: schema_version
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "schema_version: ";
    rosidl_generator_traits::value_to_yaml(msg.schema_version, out);
    out << "\n";
  }

  // member: instance_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "instance_id: ";
    rosidl_generator_traits::value_to_yaml(msg.instance_id, out);
    out << "\n";
  }

  // member: declarations
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.declarations.size() == 0) {
      out << "declarations: []\n";
    } else {
      out << "declarations:\n";
      for (auto item : msg.declarations) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const StopDeclarations & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::msg::StopDeclarations & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::StopDeclarations & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::StopDeclarations>()
{
  return "mrs_msgs::msg::StopDeclarations";
}

template<>
inline const char * name<mrs_msgs::msg::StopDeclarations>()
{
  return "mrs_msgs/msg/StopDeclarations";
}

template<>
struct has_fixed_size<mrs_msgs::msg::StopDeclarations>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::StopDeclarations>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::StopDeclarations>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATIONS__TRAITS_HPP_
