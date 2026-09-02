
#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Goal() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_Goal__init(msg: *mut Replan_Goal) -> bool;
    fn mrs_msgs__action__Replan_Goal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_Goal>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_Goal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_Goal>);
    fn mrs_msgs__action__Replan_Goal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_Goal>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_Goal>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_Goal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Goal {
    /// --- goal: mrs.replan_request 7.0.0 ---
    pub schema: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub schema_version: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub instance_id: rosidl_runtime_rs::String,

    /// ReplanReason == shared StopReason vocabulary (mrs_msgs/msg/StopReason.msg).
    pub reason: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub affected_robots: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked: rosidl_runtime_rs::Sequence<super::super::msg::rmw::EdgeRef>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_hash: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub ordering_constraints: rosidl_runtime_rs::Sequence<super::super::msg::rmw::VisitOrder>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub baseline: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotBaseline>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan_revision: u64,

}



impl Default for Replan_Goal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_Goal__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_Goal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_Goal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Goal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Goal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Goal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_Goal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_Goal where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_Goal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Goal() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Result() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_Result__init(msg: *mut Replan_Result) -> bool;
    fn mrs_msgs__action__Replan_Result__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_Result>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_Result__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_Result>);
    fn mrs_msgs__action__Replan_Result__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_Result>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_Result>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_Result
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Result {

    // This member is not documented.
    #[allow(missing_docs)]
    pub schema: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub schema_version: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub instance_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub outcome: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan_revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_hash: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub unreachable_robots: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub components: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotGroup>,

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
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_Result__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_Result__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_Result {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Result__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Result__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Result__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_Result {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_Result where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_Result";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Result() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Feedback() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_Feedback__init(msg: *mut Replan_Feedback) -> bool;
    fn mrs_msgs__action__Replan_Feedback__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_Feedback>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_Feedback__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_Feedback>);
    fn mrs_msgs__action__Replan_Feedback__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_Feedback>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_Feedback>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_Feedback
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_Feedback {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for Replan_Feedback {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_Feedback__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_Feedback__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_Feedback {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Feedback__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Feedback__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_Feedback__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_Feedback {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_Feedback where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_Feedback";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_Feedback() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_FeedbackMessage() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_FeedbackMessage__init(msg: *mut Replan_FeedbackMessage) -> bool;
    fn mrs_msgs__action__Replan_FeedbackMessage__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_FeedbackMessage>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_FeedbackMessage__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_FeedbackMessage>);
    fn mrs_msgs__action__Replan_FeedbackMessage__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_FeedbackMessage>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_FeedbackMessage>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_FeedbackMessage
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_FeedbackMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub feedback: super::super::action::rmw::Replan_Feedback,

}



impl Default for Replan_FeedbackMessage {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_FeedbackMessage__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_FeedbackMessage__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_FeedbackMessage {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_FeedbackMessage__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_FeedbackMessage__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_FeedbackMessage__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_FeedbackMessage {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_FeedbackMessage where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_FeedbackMessage";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_FeedbackMessage() }
  }
}




#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_SendGoal_Request() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_SendGoal_Request__init(msg: *mut Replan_SendGoal_Request) -> bool;
    fn mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Request>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Request>);
    fn mrs_msgs__action__Replan_SendGoal_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_SendGoal_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Request>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_SendGoal_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_SendGoal_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: super::super::action::rmw::Replan_Goal,

}



impl Default for Replan_SendGoal_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_SendGoal_Request__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_SendGoal_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_SendGoal_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_SendGoal_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_SendGoal_Request where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_SendGoal_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_SendGoal_Request() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_SendGoal_Response() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_SendGoal_Response__init(msg: *mut Replan_SendGoal_Response) -> bool;
    fn mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Response>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Response>);
    fn mrs_msgs__action__Replan_SendGoal_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_SendGoal_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_SendGoal_Response>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_SendGoal_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_SendGoal_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub accepted: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}



impl Default for Replan_SendGoal_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_SendGoal_Response__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_SendGoal_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_SendGoal_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_SendGoal_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_SendGoal_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_SendGoal_Response where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_SendGoal_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_SendGoal_Response() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_GetResult_Request() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_GetResult_Request__init(msg: *mut Replan_GetResult_Request) -> bool;
    fn mrs_msgs__action__Replan_GetResult_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Request>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Request>);
    fn mrs_msgs__action__Replan_GetResult_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_GetResult_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Request>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_GetResult_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_GetResult_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,

}



impl Default for Replan_GetResult_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_GetResult_Request__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_GetResult_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_GetResult_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_GetResult_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_GetResult_Request where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_GetResult_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_GetResult_Request() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_GetResult_Response() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__action__Replan_GetResult_Response__init(msg: *mut Replan_GetResult_Response) -> bool;
    fn mrs_msgs__action__Replan_GetResult_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Response>, size: usize) -> bool;
    fn mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Response>);
    fn mrs_msgs__action__Replan_GetResult_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Replan_GetResult_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<Replan_GetResult_Response>) -> bool;
}

// Corresponds to mrs_msgs__action__Replan_GetResult_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Replan_GetResult_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub result: super::super::action::rmw::Replan_Result,

}



impl Default for Replan_GetResult_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__action__Replan_GetResult_Response__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__action__Replan_GetResult_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Replan_GetResult_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__action__Replan_GetResult_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Replan_GetResult_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Replan_GetResult_Response where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/action/Replan_GetResult_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__action__Replan_GetResult_Response() }
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


