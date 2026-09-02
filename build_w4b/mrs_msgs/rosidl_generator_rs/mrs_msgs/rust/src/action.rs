
#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to mrs_msgs__action__Replan_Goal

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Goal {
    /// --- goal: mrs.replan_request 7.0.0 ---
    pub schema: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub schema_version: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub instance_id: std::string::String,

    /// ReplanReason == shared StopReason vocabulary (mrs_msgs/msg/StopReason.msg).
    pub reason: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub affected_robots: Vec<std::string::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked: Vec<super::msg::EdgeRef>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_hash: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub ordering_constraints: Vec<super::msg::VisitOrder>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub baseline: Vec<super::msg::RobotBaseline>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan_revision: u64,

}



impl Default for Replan_Goal {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_Goal::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_Goal {
  type RmwMsg = super::action::rmw::Replan_Goal;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        reason: msg.reason,
        affected_robots: msg.affected_robots
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        blocked: msg.blocked
          .into_iter()
          .map(|elem| super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        blocked_hash: msg.blocked_hash.as_str().into(),
        ordering_constraints: msg.ordering_constraints
          .into_iter()
          .map(|elem| super::msg::VisitOrder::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        baseline: msg.baseline
          .into_iter()
          .map(|elem| super::msg::RobotBaseline::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        plan_revision: msg.plan_revision,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      reason: msg.reason,
        affected_robots: msg.affected_robots
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        blocked: msg.blocked
          .iter()
          .map(|elem| super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        blocked_hash: msg.blocked_hash.as_str().into(),
        ordering_constraints: msg.ordering_constraints
          .iter()
          .map(|elem| super::msg::VisitOrder::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        baseline: msg.baseline
          .iter()
          .map(|elem| super::msg::RobotBaseline::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      plan_revision: msg.plan_revision,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      reason: msg.reason,
      affected_robots: msg.affected_robots
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
      blocked: msg.blocked
          .into_iter()
          .map(super::msg::EdgeRef::from_rmw_message)
          .collect(),
      blocked_hash: msg.blocked_hash.to_string(),
      ordering_constraints: msg.ordering_constraints
          .into_iter()
          .map(super::msg::VisitOrder::from_rmw_message)
          .collect(),
      baseline: msg.baseline
          .into_iter()
          .map(super::msg::RobotBaseline::from_rmw_message)
          .collect(),
      plan_revision: msg.plan_revision,
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_Result

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Result {

    // This member is not documented.
    #[allow(missing_docs)]
    pub schema: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub schema_version: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub instance_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub outcome: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan_revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_hash: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub unreachable_robots: Vec<std::string::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub components: Vec<super::msg::RobotGroup>,

}

impl Replan_Result {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const OUTCOME_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const OUTCOME_PLANNED: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const OUTCOME_FALLBACK: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const OUTCOME_PREEMPTED: u8 = 3;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const OUTCOME_REJECTED: u8 = 4;

}


impl Default for Replan_Result {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_Result::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_Result {
  type RmwMsg = super::action::rmw::Replan_Result;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        outcome: msg.outcome,
        plan_revision: msg.plan_revision,
        blocked_hash: msg.blocked_hash.as_str().into(),
        unreachable_robots: msg.unreachable_robots
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        components: msg.components
          .into_iter()
          .map(|elem| super::msg::RobotGroup::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      outcome: msg.outcome,
      plan_revision: msg.plan_revision,
        blocked_hash: msg.blocked_hash.as_str().into(),
        unreachable_robots: msg.unreachable_robots
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        components: msg.components
          .iter()
          .map(|elem| super::msg::RobotGroup::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      outcome: msg.outcome,
      plan_revision: msg.plan_revision,
      blocked_hash: msg.blocked_hash.to_string(),
      unreachable_robots: msg.unreachable_robots
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
      components: msg.components
          .into_iter()
          .map(super::msg::RobotGroup::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_Feedback

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Feedback {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for Replan_Feedback {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_Feedback::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_Feedback {
  type RmwMsg = super::action::rmw::Replan_Feedback;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      structure_needs_at_least_one_member: msg.structure_needs_at_least_one_member,
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_FeedbackMessage

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_FeedbackMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub feedback: super::action::Replan_Feedback,

}



impl Default for Replan_FeedbackMessage {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_FeedbackMessage::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_FeedbackMessage {
  type RmwMsg = super::action::rmw::Replan_FeedbackMessage;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
        feedback: super::action::Replan_Feedback::into_rmw_message(std::borrow::Cow::Owned(msg.feedback)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
        feedback: super::action::Replan_Feedback::into_rmw_message(std::borrow::Cow::Borrowed(&msg.feedback)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
      feedback: super::action::Replan_Feedback::from_rmw_message(msg.feedback),
    }
  }
}






// Corresponds to mrs_msgs__action__Replan_SendGoal_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_SendGoal_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: super::action::Replan_Goal,

}



impl Default for Replan_SendGoal_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_SendGoal_Request::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_SendGoal_Request {
  type RmwMsg = super::action::rmw::Replan_SendGoal_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
        goal: super::action::Replan_Goal::into_rmw_message(std::borrow::Cow::Owned(msg.goal)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
        goal: super::action::Replan_Goal::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
      goal: super::action::Replan_Goal::from_rmw_message(msg.goal),
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_SendGoal_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_SendGoal_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub accepted: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::Time,

}



impl Default for Replan_SendGoal_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_SendGoal_Response::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_SendGoal_Response {
  type RmwMsg = super::action::rmw::Replan_SendGoal_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        accepted: msg.accepted,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      accepted: msg.accepted,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      accepted: msg.accepted,
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_GetResult_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_GetResult_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::UUID,

}



impl Default for Replan_GetResult_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_GetResult_Request::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_GetResult_Request {
  type RmwMsg = super::action::rmw::Replan_GetResult_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Owned(msg.goal_id)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: unique_identifier_msgs::msg::UUID::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal_id)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: unique_identifier_msgs::msg::UUID::from_rmw_message(msg.goal_id),
    }
  }
}


// Corresponds to mrs_msgs__action__Replan_GetResult_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_GetResult_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub result: super::action::Replan_Result,

}



impl Default for Replan_GetResult_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::action::rmw::Replan_GetResult_Response::default())
  }
}

impl rosidl_runtime_rs::Message for Replan_GetResult_Response {
  type RmwMsg = super::action::rmw::Replan_GetResult_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        status: msg.status,
        result: super::action::Replan_Result::into_rmw_message(std::borrow::Cow::Owned(msg.result)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      status: msg.status,
        result: super::action::Replan_Result::into_rmw_message(std::borrow::Cow::Borrowed(&msg.result)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      status: msg.status,
      result: super::action::Replan_Result::from_rmw_message(msg.result),
    }
  }
}






#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__mrs_msgs__action__Replan_SendGoal() -> *const std::ffi::c_void;
}

// Corresponds to mrs_msgs__action__Replan_SendGoal
#[allow(missing_docs, non_camel_case_types)]
pub struct Replan_SendGoal;

impl rosidl_runtime_rs::Service for Replan_SendGoal {
    type Request = Replan_SendGoal_Request;
    type Response = Replan_SendGoal_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__mrs_msgs__action__Replan_SendGoal() }
    }
}




#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__mrs_msgs__action__Replan_GetResult() -> *const std::ffi::c_void;
}

// Corresponds to mrs_msgs__action__Replan_GetResult
#[allow(missing_docs, non_camel_case_types)]
pub struct Replan_GetResult;

impl rosidl_runtime_rs::Service for Replan_GetResult {
    type Request = Replan_GetResult_Request;
    type Response = Replan_GetResult_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__mrs_msgs__action__Replan_GetResult() }
    }
}






#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_action_type_support_handle__mrs_msgs__action__Replan() -> *const std::ffi::c_void;
}

// Corresponds to mrs_msgs__action__Replan
#[allow(missing_docs, non_camel_case_types)]
pub struct Replan;

impl rosidl_runtime_rs::Action for Replan {
  // --- Associated types for client library users ---
  /// The goal message defined in the action definition.
  type Goal = Replan_Goal;

  /// The result message defined in the action definition.
  type Result = Replan_Result;

  /// The feedback message defined in the action definition.
  type Feedback = Replan_Feedback;

  // --- Associated types for client library implementation ---
  /// The feedback message with generic fields which wraps the feedback message.
  type FeedbackMessage = super::action::Replan_FeedbackMessage;

  /// The send_goal service using a wrapped version of the goal message as a request.
  type SendGoalService = super::action::Replan_SendGoal;

  /// The generic service to cancel a goal.
  type CancelGoalService = action_msgs::srv::rmw::CancelGoal;

  /// The get_result service using a wrapped version of the result message as a response.
  type GetResultService = super::action::Replan_GetResult;

  // --- Methods for client library implementation ---
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_action_type_support_handle__mrs_msgs__action__Replan() }
  }

  fn create_goal_request(
    goal_id: &[u8; 16],
    goal: super::action::rmw::Replan_Goal,
  ) -> super::action::rmw::Replan_SendGoal_Request {
   super::action::rmw::Replan_SendGoal_Request {
      goal_id: unique_identifier_msgs::msg::rmw::UUID { uuid: *goal_id },
      goal,
    }
  }

  fn split_goal_request(
    request: super::action::rmw::Replan_SendGoal_Request,
  ) -> (
    [u8; 16],
   super::action::rmw::Replan_Goal,
  ) {
    (request.goal_id.uuid, request.goal)
  }

  fn create_goal_response(
    accepted: bool,
    stamp: (i32, u32),
  ) -> super::action::rmw::Replan_SendGoal_Response {
   super::action::rmw::Replan_SendGoal_Response {
      accepted,
      stamp: builtin_interfaces::msg::rmw::Time {
        sec: stamp.0,
        nanosec: stamp.1,
      },
    }
  }

  fn get_goal_response_accepted(
    response: &super::action::rmw::Replan_SendGoal_Response,
  ) -> bool {
    response.accepted
  }

  fn get_goal_response_stamp(
    response: &super::action::rmw::Replan_SendGoal_Response,
  ) -> (i32, u32) {
    (response.stamp.sec, response.stamp.nanosec)
  }

  fn create_feedback_message(
    goal_id: &[u8; 16],
    feedback: super::action::rmw::Replan_Feedback,
  ) -> super::action::rmw::Replan_FeedbackMessage {
    let mut message = super::action::rmw::Replan_FeedbackMessage::default();
    message.goal_id.uuid = *goal_id;
    message.feedback = feedback;
    message
  }

  fn split_feedback_message(
    feedback: super::action::rmw::Replan_FeedbackMessage,
  ) -> (
    [u8; 16],
   super::action::rmw::Replan_Feedback,
  ) {
    (feedback.goal_id.uuid, feedback.feedback)
  }

  fn create_result_request(
    goal_id: &[u8; 16],
  ) -> super::action::rmw::Replan_GetResult_Request {
   super::action::rmw::Replan_GetResult_Request {
      goal_id: unique_identifier_msgs::msg::rmw::UUID { uuid: *goal_id },
    }
  }

  fn get_result_request_uuid(
    request: &super::action::rmw::Replan_GetResult_Request,
  ) -> &[u8; 16] {
    &request.goal_id.uuid
  }

  fn create_result_response(
    status: i8,
    result: super::action::rmw::Replan_Result,
  ) -> super::action::rmw::Replan_GetResult_Response {
   super::action::rmw::Replan_GetResult_Response {
      status,
      result,
    }
  }

  fn split_result_response(
    response: super::action::rmw::Replan_GetResult_Response
  ) -> (
    i8,
   super::action::rmw::Replan_Result,
  ) {
    (response.status, response.result)
  }
}


