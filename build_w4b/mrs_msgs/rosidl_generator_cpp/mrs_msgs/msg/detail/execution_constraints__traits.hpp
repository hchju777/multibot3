// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:msg/ExecutionConstraints.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/execution_constraints.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__TRAITS_HPP_
#define MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/msg/detail/execution_constraints__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'segments'
#include "mrs_msgs/msg/detail/segment__traits.hpp"
// Member 'dependencies'
#include "mrs_msgs/msg/detail/dependency__traits.hpp"
// Member 'switch_groups'
#include "mrs_msgs/msg/detail/switch_group__traits.hpp"

namespace mrs_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const ExecutionConstraints & msg,
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

  // member: commit_seq
  {
    out << "commit_seq: ";
    rosidl_generator_traits::value_to_yaml(msg.commit_seq, out);
    out << ", ";
  }

  // member: segments
  {
    if (msg.segments.size() == 0) {
      out << "segments: []";
    } else {
      out << "segments: [";
      size_t pending_items = msg.segments.size();
      for (auto item : msg.segments) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
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
    out << ", ";
  }

  // member: switch_groups
  {
    if (msg.switch_groups.size() == 0) {
      out << "switch_groups: []";
    } else {
      out << "switch_groups: [";
      size_t pending_items = msg.switch_groups.size();
      for (auto item : msg.switch_groups) {
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
  const ExecutionConstraints & msg,
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

  // member: commit_seq
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "commit_seq: ";
    rosidl_generator_traits::value_to_yaml(msg.commit_seq, out);
    out << "\n";
  }

  // member: segments
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.segments.size() == 0) {
      out << "segments: []\n";
    } else {
      out << "segments:\n";
      for (auto item : msg.segments) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
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

  // member: switch_groups
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.switch_groups.size() == 0) {
      out << "switch_groups: []\n";
    } else {
      out << "switch_groups:\n";
      for (auto item : msg.switch_groups) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ExecutionConstraints & msg, bool use_flow_style = false)
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
  const mrs_msgs::msg::ExecutionConstraints & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::msg::ExecutionConstraints & msg)
{
  return mrs_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::msg::ExecutionConstraints>()
{
  return "mrs_msgs::msg::ExecutionConstraints";
}

template<>
inline const char * name<mrs_msgs::msg::ExecutionConstraints>()
{
  return "mrs_msgs/msg/ExecutionConstraints";
}

template<>
struct has_fixed_size<mrs_msgs::msg::ExecutionConstraints>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::msg::ExecutionConstraints>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::msg::ExecutionConstraints>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MRS_MSGS__MSG__DETAIL__EXECUTION_CONSTRAINTS__TRAITS_HPP_
