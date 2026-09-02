// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/action/replan.hpp"


#ifndef MRS_MSGS__ACTION__DETAIL__REPLAN__TRAITS_HPP_
#define MRS_MSGS__ACTION__DETAIL__REPLAN__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mrs_msgs/action/detail/replan__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'blocked'
#include "mrs_msgs/msg/detail/edge_ref__traits.hpp"
// Member 'ordering_constraints'
#include "mrs_msgs/msg/detail/visit_order__traits.hpp"
// Member 'baseline'
#include "mrs_msgs/msg/detail/robot_baseline__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_Goal & msg,
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

  // member: reason
  {
    out << "reason: ";
    rosidl_generator_traits::value_to_yaml(msg.reason, out);
    out << ", ";
  }

  // member: affected_robots
  {
    if (msg.affected_robots.size() == 0) {
      out << "affected_robots: []";
    } else {
      out << "affected_robots: [";
      size_t pending_items = msg.affected_robots.size();
      for (auto item : msg.affected_robots) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: blocked
  {
    if (msg.blocked.size() == 0) {
      out << "blocked: []";
    } else {
      out << "blocked: [";
      size_t pending_items = msg.blocked.size();
      for (auto item : msg.blocked) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: blocked_hash
  {
    out << "blocked_hash: ";
    rosidl_generator_traits::value_to_yaml(msg.blocked_hash, out);
    out << ", ";
  }

  // member: ordering_constraints
  {
    if (msg.ordering_constraints.size() == 0) {
      out << "ordering_constraints: []";
    } else {
      out << "ordering_constraints: [";
      size_t pending_items = msg.ordering_constraints.size();
      for (auto item : msg.ordering_constraints) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: baseline
  {
    if (msg.baseline.size() == 0) {
      out << "baseline: []";
    } else {
      out << "baseline: [";
      size_t pending_items = msg.baseline.size();
      for (auto item : msg.baseline) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: plan_revision
  {
    out << "plan_revision: ";
    rosidl_generator_traits::value_to_yaml(msg.plan_revision, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_Goal & msg,
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

  // member: reason
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "reason: ";
    rosidl_generator_traits::value_to_yaml(msg.reason, out);
    out << "\n";
  }

  // member: affected_robots
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.affected_robots.size() == 0) {
      out << "affected_robots: []\n";
    } else {
      out << "affected_robots:\n";
      for (auto item : msg.affected_robots) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: blocked
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.blocked.size() == 0) {
      out << "blocked: []\n";
    } else {
      out << "blocked:\n";
      for (auto item : msg.blocked) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: blocked_hash
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "blocked_hash: ";
    rosidl_generator_traits::value_to_yaml(msg.blocked_hash, out);
    out << "\n";
  }

  // member: ordering_constraints
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.ordering_constraints.size() == 0) {
      out << "ordering_constraints: []\n";
    } else {
      out << "ordering_constraints:\n";
      for (auto item : msg.ordering_constraints) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: baseline
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.baseline.size() == 0) {
      out << "baseline: []\n";
    } else {
      out << "baseline:\n";
      for (auto item : msg.baseline) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: plan_revision
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "plan_revision: ";
    rosidl_generator_traits::value_to_yaml(msg.plan_revision, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_Goal & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_Goal & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_Goal & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_Goal>()
{
  return "mrs_msgs::action::Replan_Goal";
}

template<>
inline const char * name<mrs_msgs::action::Replan_Goal>()
{
  return "mrs_msgs/action/Replan_Goal";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_Goal>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_Goal>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::action::Replan_Goal>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'components'
#include "mrs_msgs/msg/detail/robot_group__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_Result & msg,
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

  // member: outcome
  {
    out << "outcome: ";
    rosidl_generator_traits::value_to_yaml(msg.outcome, out);
    out << ", ";
  }

  // member: plan_revision
  {
    out << "plan_revision: ";
    rosidl_generator_traits::value_to_yaml(msg.plan_revision, out);
    out << ", ";
  }

  // member: blocked_hash
  {
    out << "blocked_hash: ";
    rosidl_generator_traits::value_to_yaml(msg.blocked_hash, out);
    out << ", ";
  }

  // member: unreachable_robots
  {
    if (msg.unreachable_robots.size() == 0) {
      out << "unreachable_robots: []";
    } else {
      out << "unreachable_robots: [";
      size_t pending_items = msg.unreachable_robots.size();
      for (auto item : msg.unreachable_robots) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: components
  {
    if (msg.components.size() == 0) {
      out << "components: []";
    } else {
      out << "components: [";
      size_t pending_items = msg.components.size();
      for (auto item : msg.components) {
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
  const Replan_Result & msg,
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

  // member: outcome
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "outcome: ";
    rosidl_generator_traits::value_to_yaml(msg.outcome, out);
    out << "\n";
  }

  // member: plan_revision
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "plan_revision: ";
    rosidl_generator_traits::value_to_yaml(msg.plan_revision, out);
    out << "\n";
  }

  // member: blocked_hash
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "blocked_hash: ";
    rosidl_generator_traits::value_to_yaml(msg.blocked_hash, out);
    out << "\n";
  }

  // member: unreachable_robots
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.unreachable_robots.size() == 0) {
      out << "unreachable_robots: []\n";
    } else {
      out << "unreachable_robots:\n";
      for (auto item : msg.unreachable_robots) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: components
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.components.size() == 0) {
      out << "components: []\n";
    } else {
      out << "components:\n";
      for (auto item : msg.components) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_Result & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_Result & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_Result & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_Result>()
{
  return "mrs_msgs::action::Replan_Result";
}

template<>
inline const char * name<mrs_msgs::action::Replan_Result>()
{
  return "mrs_msgs/action/Replan_Result";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_Result>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_Result>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mrs_msgs::action::Replan_Result>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_Feedback & msg,
  std::ostream & out)
{
  (void)msg;
  out << "null";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_Feedback & msg,
  std::ostream & out, size_t indentation = 0)
{
  (void)msg;
  (void)indentation;
  out << "null\n";
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_Feedback & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_Feedback & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_Feedback & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_Feedback>()
{
  return "mrs_msgs::action::Replan_Feedback";
}

template<>
inline const char * name<mrs_msgs::action::Replan_Feedback>()
{
  return "mrs_msgs/action/Replan_Feedback";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_Feedback>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_Feedback>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<mrs_msgs::action::Replan_Feedback>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__traits.hpp"
// Member 'goal'
#include "mrs_msgs/action/detail/replan__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_SendGoal_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: goal_id
  {
    out << "goal_id: ";
    to_flow_style_yaml(msg.goal_id, out);
    out << ", ";
  }

  // member: goal
  {
    out << "goal: ";
    to_flow_style_yaml(msg.goal, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_SendGoal_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: goal_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_id:\n";
    to_block_style_yaml(msg.goal_id, out, indentation + 2);
  }

  // member: goal
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal:\n";
    to_block_style_yaml(msg.goal, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_SendGoal_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_SendGoal_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_SendGoal_Request & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_SendGoal_Request>()
{
  return "mrs_msgs::action::Replan_SendGoal_Request";
}

template<>
inline const char * name<mrs_msgs::action::Replan_SendGoal_Request>()
{
  return "mrs_msgs/action/Replan_SendGoal_Request";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_SendGoal_Request>
  : std::integral_constant<bool, has_fixed_size<mrs_msgs::action::Replan_Goal>::value && has_fixed_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_SendGoal_Request>
  : std::integral_constant<bool, has_bounded_size<mrs_msgs::action::Replan_Goal>::value && has_bounded_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_SendGoal_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_SendGoal_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: accepted
  {
    out << "accepted: ";
    rosidl_generator_traits::value_to_yaml(msg.accepted, out);
    out << ", ";
  }

  // member: stamp
  {
    out << "stamp: ";
    to_flow_style_yaml(msg.stamp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_SendGoal_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: accepted
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "accepted: ";
    rosidl_generator_traits::value_to_yaml(msg.accepted, out);
    out << "\n";
  }

  // member: stamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "stamp:\n";
    to_block_style_yaml(msg.stamp, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_SendGoal_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_SendGoal_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_SendGoal_Response & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_SendGoal_Response>()
{
  return "mrs_msgs::action::Replan_SendGoal_Response";
}

template<>
inline const char * name<mrs_msgs::action::Replan_SendGoal_Response>()
{
  return "mrs_msgs/action/Replan_SendGoal_Response";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_SendGoal_Response>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_SendGoal_Response>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_SendGoal_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_SendGoal_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
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
  const Replan_SendGoal_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_SendGoal_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_SendGoal_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_SendGoal_Event & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_SendGoal_Event>()
{
  return "mrs_msgs::action::Replan_SendGoal_Event";
}

template<>
inline const char * name<mrs_msgs::action::Replan_SendGoal_Event>()
{
  return "mrs_msgs/action/Replan_SendGoal_Event";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_SendGoal_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_SendGoal_Event>
  : std::integral_constant<bool, has_bounded_size<mrs_msgs::action::Replan_SendGoal_Request>::value && has_bounded_size<mrs_msgs::action::Replan_SendGoal_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_SendGoal_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<mrs_msgs::action::Replan_SendGoal>()
{
  return "mrs_msgs::action::Replan_SendGoal";
}

template<>
inline const char * name<mrs_msgs::action::Replan_SendGoal>()
{
  return "mrs_msgs/action/Replan_SendGoal";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_SendGoal>
  : std::integral_constant<
    bool,
    has_fixed_size<mrs_msgs::action::Replan_SendGoal_Request>::value &&
    has_fixed_size<mrs_msgs::action::Replan_SendGoal_Response>::value
  >
{
};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_SendGoal>
  : std::integral_constant<
    bool,
    has_bounded_size<mrs_msgs::action::Replan_SendGoal_Request>::value &&
    has_bounded_size<mrs_msgs::action::Replan_SendGoal_Response>::value
  >
{
};

template<>
struct is_service<mrs_msgs::action::Replan_SendGoal>
  : std::true_type
{
};

template<>
struct is_service_request<mrs_msgs::action::Replan_SendGoal_Request>
  : std::true_type
{
};

template<>
struct is_service_response<mrs_msgs::action::Replan_SendGoal_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_GetResult_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: goal_id
  {
    out << "goal_id: ";
    to_flow_style_yaml(msg.goal_id, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_GetResult_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: goal_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_id:\n";
    to_block_style_yaml(msg.goal_id, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_GetResult_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_GetResult_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_GetResult_Request & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_GetResult_Request>()
{
  return "mrs_msgs::action::Replan_GetResult_Request";
}

template<>
inline const char * name<mrs_msgs::action::Replan_GetResult_Request>()
{
  return "mrs_msgs/action/Replan_GetResult_Request";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_GetResult_Request>
  : std::integral_constant<bool, has_fixed_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_GetResult_Request>
  : std::integral_constant<bool, has_bounded_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_GetResult_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'result'
// already included above
// #include "mrs_msgs/action/detail/replan__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_GetResult_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: status
  {
    out << "status: ";
    rosidl_generator_traits::value_to_yaml(msg.status, out);
    out << ", ";
  }

  // member: result
  {
    out << "result: ";
    to_flow_style_yaml(msg.result, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_GetResult_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: status
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "status: ";
    rosidl_generator_traits::value_to_yaml(msg.status, out);
    out << "\n";
  }

  // member: result
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "result:\n";
    to_block_style_yaml(msg.result, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_GetResult_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_GetResult_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_GetResult_Response & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_GetResult_Response>()
{
  return "mrs_msgs::action::Replan_GetResult_Response";
}

template<>
inline const char * name<mrs_msgs::action::Replan_GetResult_Response>()
{
  return "mrs_msgs/action/Replan_GetResult_Response";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_GetResult_Response>
  : std::integral_constant<bool, has_fixed_size<mrs_msgs::action::Replan_Result>::value> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_GetResult_Response>
  : std::integral_constant<bool, has_bounded_size<mrs_msgs::action::Replan_Result>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_GetResult_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_GetResult_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
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
  const Replan_GetResult_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_GetResult_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_GetResult_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_GetResult_Event & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_GetResult_Event>()
{
  return "mrs_msgs::action::Replan_GetResult_Event";
}

template<>
inline const char * name<mrs_msgs::action::Replan_GetResult_Event>()
{
  return "mrs_msgs/action/Replan_GetResult_Event";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_GetResult_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_GetResult_Event>
  : std::integral_constant<bool, has_bounded_size<mrs_msgs::action::Replan_GetResult_Request>::value && has_bounded_size<mrs_msgs::action::Replan_GetResult_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_GetResult_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<mrs_msgs::action::Replan_GetResult>()
{
  return "mrs_msgs::action::Replan_GetResult";
}

template<>
inline const char * name<mrs_msgs::action::Replan_GetResult>()
{
  return "mrs_msgs/action/Replan_GetResult";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_GetResult>
  : std::integral_constant<
    bool,
    has_fixed_size<mrs_msgs::action::Replan_GetResult_Request>::value &&
    has_fixed_size<mrs_msgs::action::Replan_GetResult_Response>::value
  >
{
};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_GetResult>
  : std::integral_constant<
    bool,
    has_bounded_size<mrs_msgs::action::Replan_GetResult_Request>::value &&
    has_bounded_size<mrs_msgs::action::Replan_GetResult_Response>::value
  >
{
};

template<>
struct is_service<mrs_msgs::action::Replan_GetResult>
  : std::true_type
{
};

template<>
struct is_service_request<mrs_msgs::action::Replan_GetResult_Request>
  : std::true_type
{
};

template<>
struct is_service_response<mrs_msgs::action::Replan_GetResult_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__traits.hpp"
// Member 'feedback'
// already included above
// #include "mrs_msgs/action/detail/replan__traits.hpp"

namespace mrs_msgs
{

namespace action
{

inline void to_flow_style_yaml(
  const Replan_FeedbackMessage & msg,
  std::ostream & out)
{
  out << "{";
  // member: goal_id
  {
    out << "goal_id: ";
    to_flow_style_yaml(msg.goal_id, out);
    out << ", ";
  }

  // member: feedback
  {
    out << "feedback: ";
    to_flow_style_yaml(msg.feedback, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Replan_FeedbackMessage & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: goal_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal_id:\n";
    to_block_style_yaml(msg.goal_id, out, indentation + 2);
  }

  // member: feedback
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "feedback:\n";
    to_block_style_yaml(msg.feedback, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Replan_FeedbackMessage & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mrs_msgs::action::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mrs_msgs::action::Replan_FeedbackMessage & msg,
  std::ostream & out, size_t indentation = 0)
{
  mrs_msgs::action::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mrs_msgs::action::to_yaml() instead")]]
inline std::string to_yaml(const mrs_msgs::action::Replan_FeedbackMessage & msg)
{
  return mrs_msgs::action::to_yaml(msg);
}

template<>
inline const char * data_type<mrs_msgs::action::Replan_FeedbackMessage>()
{
  return "mrs_msgs::action::Replan_FeedbackMessage";
}

template<>
inline const char * name<mrs_msgs::action::Replan_FeedbackMessage>()
{
  return "mrs_msgs/action/Replan_FeedbackMessage";
}

template<>
struct has_fixed_size<mrs_msgs::action::Replan_FeedbackMessage>
  : std::integral_constant<bool, has_fixed_size<mrs_msgs::action::Replan_Feedback>::value && has_fixed_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct has_bounded_size<mrs_msgs::action::Replan_FeedbackMessage>
  : std::integral_constant<bool, has_bounded_size<mrs_msgs::action::Replan_Feedback>::value && has_bounded_size<unique_identifier_msgs::msg::UUID>::value> {};

template<>
struct is_message<mrs_msgs::action::Replan_FeedbackMessage>
  : std::true_type {};

}  // namespace rosidl_generator_traits


namespace rosidl_generator_traits
{

template<>
struct is_action<mrs_msgs::action::Replan>
  : std::true_type
{
};

template<>
struct is_action_goal<mrs_msgs::action::Replan_Goal>
  : std::true_type
{
};

template<>
struct is_action_result<mrs_msgs::action::Replan_Result>
  : std::true_type
{
};

template<>
struct is_action_feedback<mrs_msgs::action::Replan_Feedback>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits


#endif  // MRS_MSGS__ACTION__DETAIL__REPLAN__TRAITS_HPP_
