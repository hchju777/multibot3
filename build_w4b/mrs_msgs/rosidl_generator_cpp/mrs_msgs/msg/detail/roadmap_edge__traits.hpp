// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap_edge.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/roadmap_edge__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RoadmapEdge & msg,
  std::ostream & out)
{
  out << "{";
  // member: from_id
  {
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << ", ";
  }

  // member: to_id
  {
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
    out << ", ";
  }

  // member: traversal
  {
    out << "traversal: ";
    rosidl_generator_traits::value_to_yaml(msg.traversal, out);
    out << ", ";
  }

  // member: length_m
  {
    out << "length_m: ";
    rosidl_generator_traits::value_to_yaml(msg.length_m, out);
    out << ", ";
  }

  // member: width_m
  {
    out << "width_m: ";
    rosidl_generator_traits::value_to_yaml(msg.width_m, out);
    out << ", ";
  }

  // member: capacity_robots
  {
    out << "capacity_robots: ";
    rosidl_generator_traits::value_to_yaml(msg.capacity_robots, out);
    out << ", ";
  }

  // member: corridor
  {
    out << "corridor: ";
    rosidl_generator_traits::value_to_yaml(msg.corridor, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RoadmapEdge & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: from_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "from_id: ";
    rosidl_generator_traits::value_to_yaml(msg.from_id, out);
    out << "\n";
  }

  // member: to_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "to_id: ";
    rosidl_generator_traits::value_to_yaml(msg.to_id, out);
    out << "\n";
  }

  // member: traversal
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "traversal: ";
    rosidl_generator_traits::value_to_yaml(msg.traversal, out);
    out << "\n";
  }

  // member: length_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "length_m: ";
    rosidl_generator_traits::value_to_yaml(msg.length_m, out);
    out << "\n";
  }

  // member: width_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "width_m: ";
    rosidl_generator_traits::value_to_yaml(msg.width_m, out);
    out << "\n";
  }

  // member: capacity_robots
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "capacity_robots: ";
    rosidl_generator_traits::value_to_yaml(msg.capacity_robots, out);
    out << "\n";
  }

  // member: corridor
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "corridor: ";
    rosidl_generator_traits::value_to_yaml(msg.corridor, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RoadmapEdge & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::RoadmapEdge & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::RoadmapEdge & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::RoadmapEdge>()
{
  return "mrs_msgs::msg::RoadmapEdge";
}

template<>
inline const char * name<mrs_msgs::msg::RoadmapEdge>()
{
  return "mrs_msgs/msg/RoadmapEdge";
}

template<>
struct has_fixed_size<mrs_msgs::msg::RoadmapEdge>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::RoadmapEdge>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::RoadmapEdge>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP_EDGE__TRAITS_HPP_
