// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/Limits.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/limits.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__LIMITS__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__LIMITS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/limits__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Limits & msg,
  std::ostream & out)
{
  out << "{";
  // member: a_max
  {
    out << "a_max: ";
    rosidl_generator_traits::value_to_yaml(msg.a_max, out);
    out << ", ";
  }

  // member: j_max
  {
    out << "j_max: ";
    rosidl_generator_traits::value_to_yaml(msg.j_max, out);
    out << ", ";
  }

  // member: min_separation_m
  {
    out << "min_separation_m: ";
    rosidl_generator_traits::value_to_yaml(msg.min_separation_m, out);
    out << ", ";
  }

  // member: omega_max_radps
  {
    out << "omega_max_radps: ";
    rosidl_generator_traits::value_to_yaml(msg.omega_max_radps, out);
    out << ", ";
  }

  // member: yaw_accel_max_radps2
  {
    out << "yaw_accel_max_radps2: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_accel_max_radps2, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Limits & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: a_max
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "a_max: ";
    rosidl_generator_traits::value_to_yaml(msg.a_max, out);
    out << "\n";
  }

  // member: j_max
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "j_max: ";
    rosidl_generator_traits::value_to_yaml(msg.j_max, out);
    out << "\n";
  }

  // member: min_separation_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "min_separation_m: ";
    rosidl_generator_traits::value_to_yaml(msg.min_separation_m, out);
    out << "\n";
  }

  // member: omega_max_radps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "omega_max_radps: ";
    rosidl_generator_traits::value_to_yaml(msg.omega_max_radps, out);
    out << "\n";
  }

  // member: yaw_accel_max_radps2
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "yaw_accel_max_radps2: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_accel_max_radps2, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Limits & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::Limits & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::Limits & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::Limits>()
{
  return "mrs_msgs::msg::Limits";
}

template<>
inline const char * name<mrs_msgs::msg::Limits>()
{
  return "mrs_msgs/msg/Limits";
}

template<>
struct has_fixed_size<mrs_msgs::msg::Limits>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<mrs_msgs::msg::Limits>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<mrs_msgs::msg::Limits>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__LIMITS__TRAITS_HPP_
