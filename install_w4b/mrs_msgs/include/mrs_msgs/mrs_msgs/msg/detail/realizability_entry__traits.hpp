// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/RealizabilityEntry.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/realizability_entry.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/realizability_entry__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RealizabilityEntry & msg,
  std::ostream & out)
{
  out << "{";
  // member: group
  {
    out << "group: ";
    rosidl_generator_traits::value_to_yaml(msg.group, out);
    out << ", ";
  }

  // member: alternative
  {
    out << "alternative: ";
    rosidl_generator_traits::value_to_yaml(msg.alternative, out);
    out << ", ";
  }

  // member: realizable
  {
    out << "realizable: ";
    rosidl_generator_traits::value_to_yaml(msg.realizable, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RealizabilityEntry & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: group
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "group: ";
    rosidl_generator_traits::value_to_yaml(msg.group, out);
    out << "\n";
  }

  // member: alternative
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "alternative: ";
    rosidl_generator_traits::value_to_yaml(msg.alternative, out);
    out << "\n";
  }

  // member: realizable
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "realizable: ";
    rosidl_generator_traits::value_to_yaml(msg.realizable, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RealizabilityEntry & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::RealizabilityEntry & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::RealizabilityEntry & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::RealizabilityEntry>()
{
  return "mrs_msgs::msg::RealizabilityEntry";
}

template<>
inline const char * name<mrs_msgs::msg::RealizabilityEntry>()
{
  return "mrs_msgs/msg/RealizabilityEntry";
}

template<>
struct has_fixed_size<mrs_msgs::msg::RealizabilityEntry>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::RealizabilityEntry>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::RealizabilityEntry>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__REALIZABILITY_ENTRY__TRAITS_HPP_
