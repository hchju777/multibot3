// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/RobotPlan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_plan.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/robot_plan__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'steps'
#include "mrs_msgs/msg/detail/plan_step__traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotPlan & msg,
  std::ostream & out)
{
  out << "{";
  // member: robot
  {
    out << "robot: ";
    rosidl_generator_traits::value_to_yaml(msg.robot, out);
    out << ", ";
  }

  // member: terminal
  {
    out << "terminal: ";
    rosidl_generator_traits::value_to_yaml(msg.terminal, out);
    out << ", ";
  }

  // member: steps
  {
    if (msg.steps.size() == 0) {
      out << "steps: []";
    } else {
      out << "steps: [";
      size_t pending_items = msg.steps.size();
      for (auto item : msg.steps) {
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
  const RobotPlan & msg,
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

  // member: terminal
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "terminal: ";
    rosidl_generator_traits::value_to_yaml(msg.terminal, out);
    out << "\n";
  }

  // member: steps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.steps.size() == 0) {
      out << "steps: []\n";
    } else {
      out << "steps:\n";
      for (auto item : msg.steps) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotPlan & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::RobotPlan & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::RobotPlan & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::RobotPlan>()
{
  return "mrs_msgs::msg::RobotPlan";
}

template<>
inline const char * name<mrs_msgs::msg::RobotPlan>()
{
  return "mrs_msgs/msg/RobotPlan";
}

template<>
struct has_fixed_size<mrs_msgs::msg::RobotPlan>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::RobotPlan>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::RobotPlan>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_PLAN__TRAITS_HPP_
