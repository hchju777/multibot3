// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/RobotBaseline.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_baseline.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/robot_baseline__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotBaseline & msg,
  std::ostream & out)
{
  out << "{";
  // member: robot
  {
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << ", ";
  }

  // member: committed_segment_count
  {
    out << "committed_segment_count: ";
    rosidl_generator_traits::value_to_yaml(msg.committed_segment_count, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotBaseline & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: robot
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << "\n";
  }

  // member: committed_segment_count
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "committed_segment_count: ";
    rosidl_generator_traits::value_to_yaml(msg.committed_segment_count, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotBaseline & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::RobotBaseline & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::RobotBaseline & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::RobotBaseline>()
{
  return "mrs_msgs::msg::RobotBaseline";
}

template<>
inline const char * name<mrs_msgs::msg::RobotBaseline>()
{
  return "mrs_msgs/msg/RobotBaseline";
}

template<>
struct has_fixed_size<mrs_msgs::msg::RobotBaseline>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::RobotBaseline>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::RobotBaseline>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_BASELINE__TRAITS_HPP_
