// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/OperatorCommand.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/operator_command.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/operator_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const OperatorCommand & msg,
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

  // member: verb
  {
    out << "verb: ";
    rosidl_generator_traits::value_to_yaml(msg.verb, out);
    out << ", ";
  }

  // member: active
  {
    out << "active: ";
    rosidl_generator_traits::value_to_yaml(msg.active, out);
    out << ", ";
  }

  // member: speed_cap_mps
  {
    out << "speed_cap_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.speed_cap_mps, out);
    out << ", ";
  }

  // member: has_speed_cap_mps
  {
    out << "has_speed_cap_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.has_speed_cap_mps, out);
    out << ", ";
  }

  // member: goal_location
  {
    out << "goal_location: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_location, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const OperatorCommand & msg,
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

  // member: verb
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "verb: ";
    rosidl_generator_traits::value_to_yaml(msg.verb, out);
    out << "\n";
  }

  // member: active
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "active: ";
    rosidl_generator_traits::value_to_yaml(msg.active, out);
    out << "\n";
  }

  // member: speed_cap_mps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "speed_cap_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.speed_cap_mps, out);
    out << "\n";
  }

  // member: has_speed_cap_mps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "has_speed_cap_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.has_speed_cap_mps, out);
    out << "\n";
  }

  // member: goal_location
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_location: ";
    rosidl_generator_traits::value_to_yaml(msg.goal_location, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const OperatorCommand & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::OperatorCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::OperatorCommand & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::OperatorCommand>()
{
  return "mrs_msgs::msg::OperatorCommand";
}

template<>
inline const char * name<mrs_msgs::msg::OperatorCommand>()
{
  return "mrs_msgs/msg/OperatorCommand";
}

template<>
struct has_fixed_size<mrs_msgs::msg::OperatorCommand>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::OperatorCommand>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::OperatorCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__OPERATOR_COMMAND__TRAITS_HPP_
