// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/Alternative.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/alternative.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ALTERNATIVE__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__ALTERNATIVE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/alternative__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'dependencies'
#include "mrs_msgs/msg/detail/dependency__traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Alternative & msg,
  std::ostream & out)
{
  out << "{";
  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: dependencies
  {
    if (msg.dependencies.size() == 0) {
      out << "dependencies: []";
    } else {
      out << "dependencies: [";
      size_t pending_items = msg.dependencies.size();
      for (auto item : msg.dependencies) {
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
  const Alternative & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: dependencies
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.dependencies.size() == 0) {
      out << "dependencies: []\n";
    } else {
      out << "dependencies:\n";
      for (auto item : msg.dependencies) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Alternative & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::Alternative & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::Alternative & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::Alternative>()
{
  return "mrs_msgs::msg::Alternative";
}

template<>
inline const char * name<mrs_msgs::msg::Alternative>()
{
  return "mrs_msgs/msg/Alternative";
}

template<>
struct has_fixed_size<mrs_msgs::msg::Alternative>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::Alternative>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::Alternative>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__ALTERNATIVE__TRAITS_HPP_
