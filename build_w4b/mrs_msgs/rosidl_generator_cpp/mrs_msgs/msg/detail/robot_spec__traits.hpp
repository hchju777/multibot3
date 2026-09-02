// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_spec.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/robot_spec__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotSpec & msg,
  std::ostream & out)
{
  out << "{";
  // member: robot
  {
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << ", ";
  }

  // member: avg_traversal_speed_mps
  {
    out << "avg_traversal_speed_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.avg_traversal_speed_mps, out);
    out << ", ";
  }

  // member: v_max
  {
    out << "v_max: ";
    rosidl_generator_traits::value_to_yaml(msg.v_max, out);
    out << ", ";
  }

  // member: brake_decel_min_mps2
  {
    out << "brake_decel_min_mps2: ";
    rosidl_generator_traits::value_to_yaml(msg.brake_decel_min_mps2, out);
    out << ", ";
  }

  // member: curvature_max_invm
  {
    out << "curvature_max_invm: ";
    rosidl_generator_traits::value_to_yaml(msg.curvature_max_invm, out);
    out << ", ";
  }

  // member: reverse_motion_allowed
  {
    out << "reverse_motion_allowed: ";
    rosidl_generator_traits::value_to_yaml(msg.reverse_motion_allowed, out);
    out << ", ";
  }

  // member: circumradius_m
  {
    out << "circumradius_m: ";
    rosidl_generator_traits::value_to_yaml(msg.circumradius_m, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotSpec & msg,
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

  // member: avg_traversal_speed_mps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "avg_traversal_speed_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.avg_traversal_speed_mps, out);
    out << "\n";
  }

  // member: v_max
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "v_max: ";
    rosidl_generator_traits::value_to_yaml(msg.v_max, out);
    out << "\n";
  }

  // member: brake_decel_min_mps2
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "brake_decel_min_mps2: ";
    rosidl_generator_traits::value_to_yaml(msg.brake_decel_min_mps2, out);
    out << "\n";
  }

  // member: curvature_max_invm
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "curvature_max_invm: ";
    rosidl_generator_traits::value_to_yaml(msg.curvature_max_invm, out);
    out << "\n";
  }

  // member: reverse_motion_allowed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "reverse_motion_allowed: ";
    rosidl_generator_traits::value_to_yaml(msg.reverse_motion_allowed, out);
    out << "\n";
  }

  // member: circumradius_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "circumradius_m: ";
    rosidl_generator_traits::value_to_yaml(msg.circumradius_m, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotSpec & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::RobotSpec & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::RobotSpec & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::RobotSpec>()
{
  return "mrs_msgs::msg::RobotSpec";
}

template<>
inline const char * name<mrs_msgs::msg::RobotSpec>()
{
  return "mrs_msgs/msg/RobotSpec";
}

template<>
struct has_fixed_size<mrs_msgs::msg::RobotSpec>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::RobotSpec>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::RobotSpec>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPEC__TRAITS_HPP_
