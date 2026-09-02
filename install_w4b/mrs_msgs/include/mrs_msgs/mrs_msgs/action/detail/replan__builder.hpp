// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/action/replan.hpp"


#ifndef MRS_MSGS__ACTION__DETAIL__REPLAN__BUILDER_HPP_
#define MRS_MSGS__ACTION__DETAIL__REPLAN__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/action/detail/replan__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_Goal_plan_revision
{
public:
  explicit Init_Replan_Goal_plan_revision(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_Goal plan_revision(::mrs_msgs::action::Replan_Goal::_plan_revision_type arg)
  {
    msg_.plan_revision = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_baseline
{
public:
  explicit Init_Replan_Goal_baseline(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_plan_revision baseline(::mrs_msgs::action::Replan_Goal::_baseline_type arg)
  {
    msg_.baseline = std::move(arg);
    return Init_Replan_Goal_plan_revision(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_ordering_constraints
{
public:
  explicit Init_Replan_Goal_ordering_constraints(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_baseline ordering_constraints(::mrs_msgs::action::Replan_Goal::_ordering_constraints_type arg)
  {
    msg_.ordering_constraints = std::move(arg);
    return Init_Replan_Goal_baseline(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_blocked_hash
{
public:
  explicit Init_Replan_Goal_blocked_hash(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_ordering_constraints blocked_hash(::mrs_msgs::action::Replan_Goal::_blocked_hash_type arg)
  {
    msg_.blocked_hash = std::move(arg);
    return Init_Replan_Goal_ordering_constraints(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_blocked
{
public:
  explicit Init_Replan_Goal_blocked(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_blocked_hash blocked(::mrs_msgs::action::Replan_Goal::_blocked_type arg)
  {
    msg_.blocked = std::move(arg);
    return Init_Replan_Goal_blocked_hash(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_affected_robots
{
public:
  explicit Init_Replan_Goal_affected_robots(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_blocked affected_robots(::mrs_msgs::action::Replan_Goal::_affected_robots_type arg)
  {
    msg_.affected_robots = std::move(arg);
    return Init_Replan_Goal_blocked(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_reason
{
public:
  explicit Init_Replan_Goal_reason(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_affected_robots reason(::mrs_msgs::action::Replan_Goal::_reason_type arg)
  {
    msg_.reason = std::move(arg);
    return Init_Replan_Goal_affected_robots(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_instance_id
{
public:
  explicit Init_Replan_Goal_instance_id(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_reason instance_id(::mrs_msgs::action::Replan_Goal::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Replan_Goal_reason(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_schema_version
{
public:
  explicit Init_Replan_Goal_schema_version(::mrs_msgs::action::Replan_Goal & msg)
  : msg_(msg)
  {}
  Init_Replan_Goal_instance_id schema_version(::mrs_msgs::action::Replan_Goal::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Replan_Goal_instance_id(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

class Init_Replan_Goal_schema
{
public:
  Init_Replan_Goal_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_Goal_schema_version schema(::mrs_msgs::action::Replan_Goal::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Replan_Goal_schema_version(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Goal msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_Goal>()
{
  return mrs_msgs::action::builder::Init_Replan_Goal_schema();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_Result_components
{
public:
  explicit Init_Replan_Result_components(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_Result components(::mrs_msgs::action::Replan_Result::_components_type arg)
  {
    msg_.components = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_unreachable_robots
{
public:
  explicit Init_Replan_Result_unreachable_robots(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_components unreachable_robots(::mrs_msgs::action::Replan_Result::_unreachable_robots_type arg)
  {
    msg_.unreachable_robots = std::move(arg);
    return Init_Replan_Result_components(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_blocked_hash
{
public:
  explicit Init_Replan_Result_blocked_hash(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_unreachable_robots blocked_hash(::mrs_msgs::action::Replan_Result::_blocked_hash_type arg)
  {
    msg_.blocked_hash = std::move(arg);
    return Init_Replan_Result_unreachable_robots(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_plan_revision
{
public:
  explicit Init_Replan_Result_plan_revision(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_blocked_hash plan_revision(::mrs_msgs::action::Replan_Result::_plan_revision_type arg)
  {
    msg_.plan_revision = std::move(arg);
    return Init_Replan_Result_blocked_hash(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_outcome
{
public:
  explicit Init_Replan_Result_outcome(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_plan_revision outcome(::mrs_msgs::action::Replan_Result::_outcome_type arg)
  {
    msg_.outcome = std::move(arg);
    return Init_Replan_Result_plan_revision(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_instance_id
{
public:
  explicit Init_Replan_Result_instance_id(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_outcome instance_id(::mrs_msgs::action::Replan_Result::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Replan_Result_outcome(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_schema_version
{
public:
  explicit Init_Replan_Result_schema_version(::mrs_msgs::action::Replan_Result & msg)
  : msg_(msg)
  {}
  Init_Replan_Result_instance_id schema_version(::mrs_msgs::action::Replan_Result::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Replan_Result_instance_id(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

class Init_Replan_Result_schema
{
public:
  Init_Replan_Result_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_Result_schema_version schema(::mrs_msgs::action::Replan_Result::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Replan_Result_schema_version(msg_);
  }

private:
  ::mrs_msgs::action::Replan_Result msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_Result>()
{
  return mrs_msgs::action::builder::Init_Replan_Result_schema();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{


}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_Feedback>()
{
  return ::mrs_msgs::action::Replan_Feedback(rosidl_runtime_cpp::MessageInitialization::ZERO);
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_SendGoal_Request_goal
{
public:
  explicit Init_Replan_SendGoal_Request_goal(::mrs_msgs::action::Replan_SendGoal_Request & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_SendGoal_Request goal(::mrs_msgs::action::Replan_SendGoal_Request::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Request msg_;
};

class Init_Replan_SendGoal_Request_goal_id
{
public:
  Init_Replan_SendGoal_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_SendGoal_Request_goal goal_id(::mrs_msgs::action::Replan_SendGoal_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_Replan_SendGoal_Request_goal(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_SendGoal_Request>()
{
  return mrs_msgs::action::builder::Init_Replan_SendGoal_Request_goal_id();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_SendGoal_Response_stamp
{
public:
  explicit Init_Replan_SendGoal_Response_stamp(::mrs_msgs::action::Replan_SendGoal_Response & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_SendGoal_Response stamp(::mrs_msgs::action::Replan_SendGoal_Response::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Response msg_;
};

class Init_Replan_SendGoal_Response_accepted
{
public:
  Init_Replan_SendGoal_Response_accepted()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_SendGoal_Response_stamp accepted(::mrs_msgs::action::Replan_SendGoal_Response::_accepted_type arg)
  {
    msg_.accepted = std::move(arg);
    return Init_Replan_SendGoal_Response_stamp(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_SendGoal_Response>()
{
  return mrs_msgs::action::builder::Init_Replan_SendGoal_Response_accepted();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_SendGoal_Event_response
{
public:
  explicit Init_Replan_SendGoal_Event_response(::mrs_msgs::action::Replan_SendGoal_Event & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_SendGoal_Event response(::mrs_msgs::action::Replan_SendGoal_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Event msg_;
};

class Init_Replan_SendGoal_Event_request
{
public:
  explicit Init_Replan_SendGoal_Event_request(::mrs_msgs::action::Replan_SendGoal_Event & msg)
  : msg_(msg)
  {}
  Init_Replan_SendGoal_Event_response request(::mrs_msgs::action::Replan_SendGoal_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_Replan_SendGoal_Event_response(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Event msg_;
};

class Init_Replan_SendGoal_Event_info
{
public:
  Init_Replan_SendGoal_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_SendGoal_Event_request info(::mrs_msgs::action::Replan_SendGoal_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_Replan_SendGoal_Event_request(msg_);
  }

private:
  ::mrs_msgs::action::Replan_SendGoal_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_SendGoal_Event>()
{
  return mrs_msgs::action::builder::Init_Replan_SendGoal_Event_info();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_GetResult_Request_goal_id
{
public:
  Init_Replan_GetResult_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::mrs_msgs::action::Replan_GetResult_Request goal_id(::mrs_msgs::action::Replan_GetResult_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_GetResult_Request>()
{
  return mrs_msgs::action::builder::Init_Replan_GetResult_Request_goal_id();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_GetResult_Response_result
{
public:
  explicit Init_Replan_GetResult_Response_result(::mrs_msgs::action::Replan_GetResult_Response & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_GetResult_Response result(::mrs_msgs::action::Replan_GetResult_Response::_result_type arg)
  {
    msg_.result = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Response msg_;
};

class Init_Replan_GetResult_Response_status
{
public:
  Init_Replan_GetResult_Response_status()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_GetResult_Response_result status(::mrs_msgs::action::Replan_GetResult_Response::_status_type arg)
  {
    msg_.status = std::move(arg);
    return Init_Replan_GetResult_Response_result(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_GetResult_Response>()
{
  return mrs_msgs::action::builder::Init_Replan_GetResult_Response_status();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_GetResult_Event_response
{
public:
  explicit Init_Replan_GetResult_Event_response(::mrs_msgs::action::Replan_GetResult_Event & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_GetResult_Event response(::mrs_msgs::action::Replan_GetResult_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Event msg_;
};

class Init_Replan_GetResult_Event_request
{
public:
  explicit Init_Replan_GetResult_Event_request(::mrs_msgs::action::Replan_GetResult_Event & msg)
  : msg_(msg)
  {}
  Init_Replan_GetResult_Event_response request(::mrs_msgs::action::Replan_GetResult_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_Replan_GetResult_Event_response(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Event msg_;
};

class Init_Replan_GetResult_Event_info
{
public:
  Init_Replan_GetResult_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_GetResult_Event_request info(::mrs_msgs::action::Replan_GetResult_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_Replan_GetResult_Event_request(msg_);
  }

private:
  ::mrs_msgs::action::Replan_GetResult_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_GetResult_Event>()
{
  return mrs_msgs::action::builder::Init_Replan_GetResult_Event_info();
}

}  // namespace mrs_msgs


namespace mrs_msgs
{

namespace action
{

namespace builder
{

class Init_Replan_FeedbackMessage_feedback
{
public:
  explicit Init_Replan_FeedbackMessage_feedback(::mrs_msgs::action::Replan_FeedbackMessage & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::action::Replan_FeedbackMessage feedback(::mrs_msgs::action::Replan_FeedbackMessage::_feedback_type arg)
  {
    msg_.feedback = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::action::Replan_FeedbackMessage msg_;
};

class Init_Replan_FeedbackMessage_goal_id
{
public:
  Init_Replan_FeedbackMessage_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Replan_FeedbackMessage_feedback goal_id(::mrs_msgs::action::Replan_FeedbackMessage::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_Replan_FeedbackMessage_feedback(msg_);
  }

private:
  ::mrs_msgs::action::Replan_FeedbackMessage msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::action::Replan_FeedbackMessage>()
{
  return mrs_msgs::action::builder::Init_Replan_FeedbackMessage_goal_id();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__ACTION__DETAIL__REPLAN__BUILDER_HPP_
