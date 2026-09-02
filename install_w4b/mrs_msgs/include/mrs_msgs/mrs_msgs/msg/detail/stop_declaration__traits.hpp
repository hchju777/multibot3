// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/StopDeclaration.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/stop_declaration.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/stop_declaration__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'blocked_edge'
#include "mrs_msgs/msg/detail/edge_ref__traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const StopDeclaration & msg,
  std::ostream & out)
{
  out << "{";
  // member: seq
  {
    out << "seq: ";
    rosidl_generator_traits::value_to_yaml(msg.seq, out);
    out << ", ";
  }

  // member: robot
  {
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << ", ";
  }

  // member: blocked_edge
  {
    out << "blocked_edge: ";
    to_flow_style_yaml(msg.blocked_edge, out);
    out << ", ";
  }

  // member: declared
  {
    out << "declared: ";
    rosidl_generator_traits::value_to_yaml(msg.declared, out);
    out << ", ";
  }

  // member: reason
  {
    out << "reason: ";
    rosidl_generator_traits::value_to_yaml(msg.reason, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const StopDeclaration & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: seq
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "seq: ";
    rosidl_generator_traits::value_to_yaml(msg.seq, out);
    out << "\n";
  }

  // member: robot
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << "\n";
  }

  // member: blocked_edge
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "blocked_edge:\n";
    to_block_style_yaml(msg.blocked_edge, out, indentation + 2);
  }

  // member: declared
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "declared: ";
    rosidl_generator_traits::value_to_yaml(msg.declared, out);
    out << "\n";
  }

  // member: reason
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "reason: ";
    rosidl_generator_traits::value_to_yaml(msg.reason, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const StopDeclaration & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::StopDeclaration & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::StopDeclaration & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::StopDeclaration>()
{
  return "mrs_msgs::msg::StopDeclaration";
}

template<>
inline const char * name<mrs_msgs::msg::StopDeclaration>()
{
  return "mrs_msgs/msg/StopDeclaration";
}

template<>
struct has_fixed_size<mrs_msgs::msg::StopDeclaration>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::StopDeclaration>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::StopDeclaration>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__STOP_DECLARATION__TRAITS_HPP_
