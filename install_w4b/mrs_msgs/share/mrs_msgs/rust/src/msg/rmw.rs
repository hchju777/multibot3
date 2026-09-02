#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Roadmap() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Roadmap__init(msg: *mut Roadmap) -> bool;
    fn mrs_msgs__msg__Roadmap__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Roadmap>, size: usize) -> bool;
    fn mrs_msgs__msg__Roadmap__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Roadmap>);
    fn mrs_msgs__msg__Roadmap__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Roadmap>, out_seq: *mut rosidl_runtime_rs::Sequence<Roadmap>) -> bool;
}

// Corresponds to mrs_msgs__msg__Roadmap
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Roadmap {

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
    pub nodes: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RoadmapNode>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub edges: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RoadmapEdge>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub endpoints: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub corridors: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Corridor>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub provenance: super::super::msg::rmw::RoadmapProvenance,

}



impl Default for Roadmap {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Roadmap__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Roadmap__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Roadmap {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Roadmap__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Roadmap__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Roadmap__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Roadmap {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Roadmap where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Roadmap";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Roadmap() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotSpecs() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotSpecs__init(msg: *mut RobotSpecs) -> bool;
    fn mrs_msgs__msg__RobotSpecs__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotSpecs>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotSpecs__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotSpecs>);
    fn mrs_msgs__msg__RobotSpecs__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotSpecs>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotSpecs>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotSpecs
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotSpecs {

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
    pub robots: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotSpec>,

}



impl Default for RobotSpecs {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotSpecs__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotSpecs__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotSpecs {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpecs__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpecs__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpecs__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotSpecs {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotSpecs where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotSpecs";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotSpecs() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TaskRelease() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__TaskRelease__init(msg: *mut TaskRelease) -> bool;
    fn mrs_msgs__msg__TaskRelease__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<TaskRelease>, size: usize) -> bool;
    fn mrs_msgs__msg__TaskRelease__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<TaskRelease>);
    fn mrs_msgs__msg__TaskRelease__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<TaskRelease>, out_seq: *mut rosidl_runtime_rs::Sequence<TaskRelease>) -> bool;
}

// Corresponds to mrs_msgs__msg__TaskRelease
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TaskRelease {

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
    pub release_index: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub task: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goals: rosidl_runtime_rs::Sequence<super::super::msg::rmw::TaskGoal>,

}



impl Default for TaskRelease {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__TaskRelease__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__TaskRelease__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for TaskRelease {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskRelease__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskRelease__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskRelease__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for TaskRelease {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for TaskRelease where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/TaskRelease";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TaskRelease() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Assignment() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Assignment__init(msg: *mut Assignment) -> bool;
    fn mrs_msgs__msg__Assignment__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Assignment>, size: usize) -> bool;
    fn mrs_msgs__msg__Assignment__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Assignment>);
    fn mrs_msgs__msg__Assignment__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Assignment>, out_seq: *mut rosidl_runtime_rs::Sequence<Assignment>) -> bool;
}

// Corresponds to mrs_msgs__msg__Assignment
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Assignment {

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
    pub revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub assignments: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotAssignment>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub unassigned_tasks: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,

}



impl Default for Assignment {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Assignment__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Assignment__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Assignment {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Assignment__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Assignment__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Assignment__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Assignment {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Assignment where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Assignment";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Assignment() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__GoalCompletion() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__GoalCompletion__init(msg: *mut GoalCompletion) -> bool;
    fn mrs_msgs__msg__GoalCompletion__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GoalCompletion>, size: usize) -> bool;
    fn mrs_msgs__msg__GoalCompletion__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GoalCompletion>);
    fn mrs_msgs__msg__GoalCompletion__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GoalCompletion>, out_seq: *mut rosidl_runtime_rs::Sequence<GoalCompletion>) -> bool;
}

// Corresponds to mrs_msgs__msg__GoalCompletion
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GoalCompletion {

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
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub revision: u64,

}



impl Default for GoalCompletion {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__GoalCompletion__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__GoalCompletion__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GoalCompletion {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__GoalCompletion__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__GoalCompletion__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__GoalCompletion__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GoalCompletion {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GoalCompletion where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/GoalCompletion";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__GoalCompletion() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__DiscretePlan() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__DiscretePlan__init(msg: *mut DiscretePlan) -> bool;
    fn mrs_msgs__msg__DiscretePlan__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DiscretePlan>, size: usize) -> bool;
    fn mrs_msgs__msg__DiscretePlan__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DiscretePlan>);
    fn mrs_msgs__msg__DiscretePlan__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DiscretePlan>, out_seq: *mut rosidl_runtime_rs::Sequence<DiscretePlan>) -> bool;
}

// Corresponds to mrs_msgs__msg__DiscretePlan
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DiscretePlan {

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
    pub plan_revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plans: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotPlan>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub visit_order: rosidl_runtime_rs::Sequence<super::super::msg::rmw::VisitOrder>,

}



impl Default for DiscretePlan {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__DiscretePlan__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__DiscretePlan__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DiscretePlan {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__DiscretePlan__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__DiscretePlan__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__DiscretePlan__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DiscretePlan {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DiscretePlan where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/DiscretePlan";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__DiscretePlan() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__ExecutionConstraints() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__ExecutionConstraints__init(msg: *mut ExecutionConstraints) -> bool;
    fn mrs_msgs__msg__ExecutionConstraints__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ExecutionConstraints>, size: usize) -> bool;
    fn mrs_msgs__msg__ExecutionConstraints__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ExecutionConstraints>);
    fn mrs_msgs__msg__ExecutionConstraints__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ExecutionConstraints>, out_seq: *mut rosidl_runtime_rs::Sequence<ExecutionConstraints>) -> bool;
}

// Corresponds to mrs_msgs__msg__ExecutionConstraints
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ExecutionConstraints {

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
    pub commit_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub segments: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Segment>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dependencies: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Dependency>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub switch_groups: rosidl_runtime_rs::Sequence<super::super::msg::rmw::SwitchGroup>,

}



impl Default for ExecutionConstraints {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__ExecutionConstraints__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__ExecutionConstraints__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ExecutionConstraints {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__ExecutionConstraints__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__ExecutionConstraints__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__ExecutionConstraints__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ExecutionConstraints {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ExecutionConstraints where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/ExecutionConstraints";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__ExecutionConstraints() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SegmentRelease() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__SegmentRelease__init(msg: *mut SegmentRelease) -> bool;
    fn mrs_msgs__msg__SegmentRelease__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SegmentRelease>, size: usize) -> bool;
    fn mrs_msgs__msg__SegmentRelease__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SegmentRelease>);
    fn mrs_msgs__msg__SegmentRelease__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SegmentRelease>, out_seq: *mut rosidl_runtime_rs::Sequence<SegmentRelease>) -> bool;
}

// Corresponds to mrs_msgs__msg__SegmentRelease
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SegmentRelease {

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
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub release_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub commit_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub entry_seq_ack: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub segments: rosidl_runtime_rs::Sequence<super::super::msg::rmw::SegmentReleaseItem>,

}



impl Default for SegmentRelease {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__SegmentRelease__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__SegmentRelease__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SegmentRelease {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentRelease__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentRelease__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentRelease__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SegmentRelease {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SegmentRelease where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/SegmentRelease";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SegmentRelease() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EntryEvents() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__EntryEvents__init(msg: *mut EntryEvents) -> bool;
    fn mrs_msgs__msg__EntryEvents__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<EntryEvents>, size: usize) -> bool;
    fn mrs_msgs__msg__EntryEvents__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<EntryEvents>);
    fn mrs_msgs__msg__EntryEvents__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<EntryEvents>, out_seq: *mut rosidl_runtime_rs::Sequence<EntryEvents>) -> bool;
}

// Corresponds to mrs_msgs__msg__EntryEvents
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntryEvents {

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
    pub events: rosidl_runtime_rs::Sequence<super::super::msg::rmw::EntryEvent>,

}



impl Default for EntryEvents {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__EntryEvents__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__EntryEvents__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for EntryEvents {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvents__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvents__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvents__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for EntryEvents {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for EntryEvents where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/EntryEvents";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EntryEvents() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopDeclarations() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__StopDeclarations__init(msg: *mut StopDeclarations) -> bool;
    fn mrs_msgs__msg__StopDeclarations__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<StopDeclarations>, size: usize) -> bool;
    fn mrs_msgs__msg__StopDeclarations__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<StopDeclarations>);
    fn mrs_msgs__msg__StopDeclarations__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<StopDeclarations>, out_seq: *mut rosidl_runtime_rs::Sequence<StopDeclarations>) -> bool;
}

// Corresponds to mrs_msgs__msg__StopDeclarations
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StopDeclarations {

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
    pub declarations: rosidl_runtime_rs::Sequence<super::super::msg::rmw::StopDeclaration>,

}



impl Default for StopDeclarations {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__StopDeclarations__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__StopDeclarations__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for StopDeclarations {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclarations__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclarations__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclarations__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for StopDeclarations {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for StopDeclarations where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/StopDeclarations";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopDeclarations() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Realizability() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Realizability__init(msg: *mut Realizability) -> bool;
    fn mrs_msgs__msg__Realizability__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Realizability>, size: usize) -> bool;
    fn mrs_msgs__msg__Realizability__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Realizability>);
    fn mrs_msgs__msg__Realizability__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Realizability>, out_seq: *mut rosidl_runtime_rs::Sequence<Realizability>) -> bool;
}

// Corresponds to mrs_msgs__msg__Realizability
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Realizability {

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
    pub rz_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub entries: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RealizabilityEntry>,

}



impl Default for Realizability {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Realizability__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Realizability__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Realizability {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Realizability__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Realizability__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Realizability__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Realizability {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Realizability where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Realizability";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Realizability() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Trajectories() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Trajectories__init(msg: *mut Trajectories) -> bool;
    fn mrs_msgs__msg__Trajectories__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Trajectories>, size: usize) -> bool;
    fn mrs_msgs__msg__Trajectories__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Trajectories>);
    fn mrs_msgs__msg__Trajectories__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Trajectories>, out_seq: *mut rosidl_runtime_rs::Sequence<Trajectories>) -> bool;
}

// Corresponds to mrs_msgs__msg__Trajectories
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Trajectories {

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
    pub limits: super::super::msg::rmw::Limits,


    // This member is not documented.
    #[allow(missing_docs)]
    pub trajectories: rosidl_runtime_rs::Sequence<super::super::msg::rmw::RobotTrajectory>,

}



impl Default for Trajectories {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Trajectories__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Trajectories__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Trajectories {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Trajectories__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Trajectories__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Trajectories__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Trajectories {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Trajectories where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Trajectories";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Trajectories() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__OperatorCommands() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__OperatorCommands__init(msg: *mut OperatorCommands) -> bool;
    fn mrs_msgs__msg__OperatorCommands__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<OperatorCommands>, size: usize) -> bool;
    fn mrs_msgs__msg__OperatorCommands__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<OperatorCommands>);
    fn mrs_msgs__msg__OperatorCommands__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<OperatorCommands>, out_seq: *mut rosidl_runtime_rs::Sequence<OperatorCommands>) -> bool;
}

// Corresponds to mrs_msgs__msg__OperatorCommands
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct OperatorCommands {

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
    pub commands: rosidl_runtime_rs::Sequence<super::super::msg::rmw::OperatorCommand>,

}



impl Default for OperatorCommands {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__OperatorCommands__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__OperatorCommands__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for OperatorCommands {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommands__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommands__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommands__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for OperatorCommands {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for OperatorCommands where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/OperatorCommands";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__OperatorCommands() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapNode() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RoadmapNode__init(msg: *mut RoadmapNode) -> bool;
    fn mrs_msgs__msg__RoadmapNode__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RoadmapNode>, size: usize) -> bool;
    fn mrs_msgs__msg__RoadmapNode__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RoadmapNode>);
    fn mrs_msgs__msg__RoadmapNode__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RoadmapNode>, out_seq: *mut rosidl_runtime_rs::Sequence<RoadmapNode>) -> bool;
}

// Corresponds to mrs_msgs__msg__RoadmapNode
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RoadmapNode {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub clearance_m: f64,

}



impl Default for RoadmapNode {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RoadmapNode__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RoadmapNode__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RoadmapNode {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapNode__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapNode__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapNode__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RoadmapNode {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RoadmapNode where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RoadmapNode";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapNode() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapEdge() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RoadmapEdge__init(msg: *mut RoadmapEdge) -> bool;
    fn mrs_msgs__msg__RoadmapEdge__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RoadmapEdge>, size: usize) -> bool;
    fn mrs_msgs__msg__RoadmapEdge__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RoadmapEdge>);
    fn mrs_msgs__msg__RoadmapEdge__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RoadmapEdge>, out_seq: *mut rosidl_runtime_rs::Sequence<RoadmapEdge>) -> bool;
}

// Corresponds to mrs_msgs__msg__RoadmapEdge
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// EdgeTraversal vocabulary (350 §3-2 V2~V6). 0 is reserved *_UNKNOWN across all
/// vocabularies (350 D1-a) — do not hand it to the first legal value.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RoadmapEdge {

    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub traversal: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub length_m: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub width_m: f64,

    /// 350 §3-2 V7: uint8 but NOT a vocabulary — no constants, value carried as-is.
    pub capacity_robots: u8,

    /// "" sentinel = no corridor reference (roadmap.schema.json:88, 350 §4-3).
    pub corridor: rosidl_runtime_rs::String,

}

impl RoadmapEdge {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TRAVERSAL_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TRAVERSAL_BIDIRECTIONAL: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TRAVERSAL_FORWARD_ONLY: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TRAVERSAL_BACKWARD_ONLY: u8 = 3;

}


impl Default for RoadmapEdge {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RoadmapEdge__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RoadmapEdge__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RoadmapEdge {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapEdge__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapEdge__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapEdge__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RoadmapEdge {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RoadmapEdge where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RoadmapEdge";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapEdge() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Corridor() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Corridor__init(msg: *mut Corridor) -> bool;
    fn mrs_msgs__msg__Corridor__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Corridor>, size: usize) -> bool;
    fn mrs_msgs__msg__Corridor__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Corridor>);
    fn mrs_msgs__msg__Corridor__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Corridor>, out_seq: *mut rosidl_runtime_rs::Sequence<Corridor>) -> bool;
}

// Corresponds to mrs_msgs__msg__Corridor
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Corridor {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub edges: rosidl_runtime_rs::Sequence<super::super::msg::rmw::EdgeRef>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub length_m: f64,

}



impl Default for Corridor {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Corridor__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Corridor__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Corridor {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Corridor__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Corridor__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Corridor__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Corridor {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Corridor where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Corridor";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Corridor() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EdgeRef() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__EdgeRef__init(msg: *mut EdgeRef) -> bool;
    fn mrs_msgs__msg__EdgeRef__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<EdgeRef>, size: usize) -> bool;
    fn mrs_msgs__msg__EdgeRef__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<EdgeRef>);
    fn mrs_msgs__msg__EdgeRef__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<EdgeRef>, out_seq: *mut rosidl_runtime_rs::Sequence<EdgeRef>) -> bool;
}

// Corresponds to mrs_msgs__msg__EdgeRef
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Directed pair of roadmap-node identifiers reused across three JSON shapes
/// (350 §6 "재사용 결정 둘"): roadmap.corridors[].edges (2-element tuple),
/// stop_declaration.blocked_edge (object {from,to}), replan_request.blocked[]
/// (object {from,to}). The io/ adapter absorbs both JSON shapes into this one
/// type (350 §8 D6 item 8) — that adapter code is outside this round's scope.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EdgeRef {

    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: rosidl_runtime_rs::String,

}



impl Default for EdgeRef {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__EdgeRef__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__EdgeRef__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for EdgeRef {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EdgeRef__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EdgeRef__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EdgeRef__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for EdgeRef {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for EdgeRef where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/EdgeRef";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EdgeRef() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapProvenance() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RoadmapProvenance__init(msg: *mut RoadmapProvenance) -> bool;
    fn mrs_msgs__msg__RoadmapProvenance__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RoadmapProvenance>, size: usize) -> bool;
    fn mrs_msgs__msg__RoadmapProvenance__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RoadmapProvenance>);
    fn mrs_msgs__msg__RoadmapProvenance__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RoadmapProvenance>, out_seq: *mut rosidl_runtime_rs::Sequence<RoadmapProvenance>) -> bool;
}

// Corresponds to mrs_msgs__msg__RoadmapProvenance
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RoadmapProvenance {

    // This member is not documented.
    #[allow(missing_docs)]
    pub min_separation_m: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub wall_inflation_m: f64,

    /// 350 §3-2 V8 (트랩): enum in JSON but x-ros-type is "string" — do NOT render
    /// as uint8. No consuming module branches on this string (audit-only).
    pub capacity_rule_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub generator_version: rosidl_runtime_rs::String,

}



impl Default for RoadmapProvenance {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RoadmapProvenance__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RoadmapProvenance__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RoadmapProvenance {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapProvenance__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapProvenance__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RoadmapProvenance__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RoadmapProvenance {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RoadmapProvenance where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RoadmapProvenance";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RoadmapProvenance() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotSpec() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotSpec__init(msg: *mut RobotSpec) -> bool;
    fn mrs_msgs__msg__RobotSpec__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotSpec>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotSpec__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotSpec>);
    fn mrs_msgs__msg__RobotSpec__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotSpec>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotSpec>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotSpec
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotSpec {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub avg_traversal_speed_mps: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub v_max: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub brake_decel_min_mps2: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub curvature_max_invm: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub reverse_motion_allowed: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub circumradius_m: f64,

}



impl Default for RobotSpec {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotSpec__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotSpec__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotSpec {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpec__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpec__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotSpec__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotSpec {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotSpec where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotSpec";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotSpec() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TaskGoal() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__TaskGoal__init(msg: *mut TaskGoal) -> bool;
    fn mrs_msgs__msg__TaskGoal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<TaskGoal>, size: usize) -> bool;
    fn mrs_msgs__msg__TaskGoal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<TaskGoal>);
    fn mrs_msgs__msg__TaskGoal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<TaskGoal>, out_seq: *mut rosidl_runtime_rs::Sequence<TaskGoal>) -> bool;
}

// Corresponds to mrs_msgs__msg__TaskGoal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// 350 §7 개정 안건 ① — task_release.schema.json:31의 "Goal (공유)"는 성립하지
/// 않는다(assignment.Goal과 필드 집합이 다르다). 이 판은 개정을 적용하지 않고
/// TaskGoal로 렌더링만 한다(350 D6, 오늘의 임시 처분). 스키마 텍스트는 미개정.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TaskGoal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,

}



impl Default for TaskGoal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__TaskGoal__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__TaskGoal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for TaskGoal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskGoal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskGoal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TaskGoal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for TaskGoal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for TaskGoal where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/TaskGoal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TaskGoal() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotAssignment() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotAssignment__init(msg: *mut RobotAssignment) -> bool;
    fn mrs_msgs__msg__RobotAssignment__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotAssignment>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotAssignment__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotAssignment>);
    fn mrs_msgs__msg__RobotAssignment__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotAssignment>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotAssignment>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotAssignment
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotAssignment {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub start: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goals: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Goal>,

}



impl Default for RobotAssignment {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotAssignment__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotAssignment__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotAssignment {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotAssignment__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotAssignment__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotAssignment__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotAssignment {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotAssignment where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotAssignment";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotAssignment() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Goal() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Goal__init(msg: *mut Goal) -> bool;
    fn mrs_msgs__msg__Goal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Goal>, size: usize) -> bool;
    fn mrs_msgs__msg__Goal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Goal>);
    fn mrs_msgs__msg__Goal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Goal>, out_seq: *mut rosidl_runtime_rs::Sequence<Goal>) -> bool;
}

// Corresponds to mrs_msgs__msg__Goal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub task: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,

}



impl Default for Goal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Goal__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Goal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Goal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Goal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Goal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Goal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Goal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Goal where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Goal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Goal() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotPlan() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotPlan__init(msg: *mut RobotPlan) -> bool;
    fn mrs_msgs__msg__RobotPlan__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotPlan>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotPlan__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotPlan>);
    fn mrs_msgs__msg__RobotPlan__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotPlan>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotPlan>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotPlan
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// PlanTerminal vocabulary (350 §3-2 V2~V6).

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotPlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub terminal: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub steps: rosidl_runtime_rs::Sequence<super::super::msg::rmw::PlanStep>,

}

impl RobotPlan {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TERMINAL_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TERMINAL_GOAL_REACHED: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TERMINAL_PLAN_TRUNCATED: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const TERMINAL_NO_PROGRESS: u8 = 3;

}


impl Default for RobotPlan {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotPlan__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotPlan__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotPlan {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotPlan__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotPlan__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotPlan__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotPlan {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotPlan where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotPlan";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotPlan() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__PlanStep() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__PlanStep__init(msg: *mut PlanStep) -> bool;
    fn mrs_msgs__msg__PlanStep__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PlanStep>, size: usize) -> bool;
    fn mrs_msgs__msg__PlanStep__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PlanStep>);
    fn mrs_msgs__msg__PlanStep__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PlanStep>, out_seq: *mut rosidl_runtime_rs::Sequence<PlanStep>) -> bool;
}

// Corresponds to mrs_msgs__msg__PlanStep
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlanStep {

    // This member is not documented.
    #[allow(missing_docs)]
    pub index: u32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,

}



impl Default for PlanStep {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__PlanStep__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__PlanStep__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PlanStep {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__PlanStep__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__PlanStep__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__PlanStep__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PlanStep {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PlanStep where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/PlanStep";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__PlanStep() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__VisitOrder() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__VisitOrder__init(msg: *mut VisitOrder) -> bool;
    fn mrs_msgs__msg__VisitOrder__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<VisitOrder>, size: usize) -> bool;
    fn mrs_msgs__msg__VisitOrder__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<VisitOrder>);
    fn mrs_msgs__msg__VisitOrder__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<VisitOrder>, out_seq: *mut rosidl_runtime_rs::Sequence<VisitOrder>) -> bool;
}

// Corresponds to mrs_msgs__msg__VisitOrder
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Reused in two places with the SAME field set but DIFFERENT values (350 §6
/// "재사용 결정 둘" / §8 D6 item 7): discrete_plan.visit_order[] (full order)
/// and replan_request.ordering_constraints[] (realized prefix of the former).
/// Do not write code that assumes "same type => same value".

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VisitOrder {

    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub sequence: rosidl_runtime_rs::Sequence<super::super::msg::rmw::VisitItem>,

}



impl Default for VisitOrder {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__VisitOrder__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__VisitOrder__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for VisitOrder {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitOrder__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitOrder__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitOrder__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for VisitOrder {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for VisitOrder where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/VisitOrder";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__VisitOrder() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__VisitItem() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__VisitItem__init(msg: *mut VisitItem) -> bool;
    fn mrs_msgs__msg__VisitItem__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<VisitItem>, size: usize) -> bool;
    fn mrs_msgs__msg__VisitItem__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<VisitItem>);
    fn mrs_msgs__msg__VisitItem__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<VisitItem>, out_seq: *mut rosidl_runtime_rs::Sequence<VisitItem>) -> bool;
}

// Corresponds to mrs_msgs__msg__VisitItem
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VisitItem {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub index: u32,

}



impl Default for VisitItem {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__VisitItem__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__VisitItem__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for VisitItem {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitItem__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitItem__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__VisitItem__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for VisitItem {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for VisitItem where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/VisitItem";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__VisitItem() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Segment() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Segment__init(msg: *mut Segment) -> bool;
    fn mrs_msgs__msg__Segment__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Segment>, size: usize) -> bool;
    fn mrs_msgs__msg__Segment__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Segment>);
    fn mrs_msgs__msg__Segment__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Segment>, out_seq: *mut rosidl_runtime_rs::Sequence<Segment>) -> bool;
}

// Corresponds to mrs_msgs__msg__Segment
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Segment {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: rosidl_runtime_rs::String,

}



impl Default for Segment {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Segment__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Segment__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Segment {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Segment__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Segment__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Segment__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Segment {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Segment where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Segment";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Segment() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Dependency() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Dependency__init(msg: *mut Dependency) -> bool;
    fn mrs_msgs__msg__Dependency__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Dependency>, size: usize) -> bool;
    fn mrs_msgs__msg__Dependency__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Dependency>);
    fn mrs_msgs__msg__Dependency__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Dependency>, out_seq: *mut rosidl_runtime_rs::Sequence<Dependency>) -> bool;
}

// Corresponds to mrs_msgs__msg__Dependency
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// DependencyType vocabulary (350 §3-2 V2~V6).

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Dependency {
    /// 🔴 from_id/to_id here are SEGMENT identifiers ({robot}#{index}), NOT roadmap
    /// node ids (350 §8 D6 item 1). Do NOT nest an EdgeRef into this message — the
    /// name space differs from RoadmapEdge/Segment's from_id/to_id even though the
    /// JSON source field names ("from"/"to") are identical.
    pub from_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub type_: u8,

}

impl Dependency {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEP_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEP_SEQUENTIAL: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const DEP_ORDERING: u8 = 2;

}


impl Default for Dependency {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Dependency__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Dependency__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Dependency {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Dependency__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Dependency__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Dependency__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Dependency {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Dependency where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Dependency";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Dependency() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SwitchGroup() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__SwitchGroup__init(msg: *mut SwitchGroup) -> bool;
    fn mrs_msgs__msg__SwitchGroup__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SwitchGroup>, size: usize) -> bool;
    fn mrs_msgs__msg__SwitchGroup__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SwitchGroup>);
    fn mrs_msgs__msg__SwitchGroup__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SwitchGroup>, out_seq: *mut rosidl_runtime_rs::Sequence<SwitchGroup>) -> bool;
}

// Corresponds to mrs_msgs__msg__SwitchGroup
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SwitchGroup {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub alternatives: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Alternative>,

}



impl Default for SwitchGroup {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__SwitchGroup__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__SwitchGroup__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SwitchGroup {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SwitchGroup__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SwitchGroup__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SwitchGroup__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SwitchGroup {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SwitchGroup where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/SwitchGroup";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SwitchGroup() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Alternative() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Alternative__init(msg: *mut Alternative) -> bool;
    fn mrs_msgs__msg__Alternative__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Alternative>, size: usize) -> bool;
    fn mrs_msgs__msg__Alternative__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Alternative>);
    fn mrs_msgs__msg__Alternative__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Alternative>, out_seq: *mut rosidl_runtime_rs::Sequence<Alternative>) -> bool;
}

// Corresponds to mrs_msgs__msg__Alternative
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Alternative {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dependencies: rosidl_runtime_rs::Sequence<super::super::msg::rmw::Dependency>,

}



impl Default for Alternative {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Alternative__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Alternative__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Alternative {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Alternative__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Alternative__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Alternative__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Alternative {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Alternative where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Alternative";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Alternative() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SegmentReleaseItem() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__SegmentReleaseItem__init(msg: *mut SegmentReleaseItem) -> bool;
    fn mrs_msgs__msg__SegmentReleaseItem__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SegmentReleaseItem>, size: usize) -> bool;
    fn mrs_msgs__msg__SegmentReleaseItem__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SegmentReleaseItem>);
    fn mrs_msgs__msg__SegmentReleaseItem__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SegmentReleaseItem>, out_seq: *mut rosidl_runtime_rs::Sequence<SegmentReleaseItem>) -> bool;
}

// Corresponds to mrs_msgs__msg__SegmentReleaseItem
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SegmentReleaseItem {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub released: bool,

}



impl Default for SegmentReleaseItem {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__SegmentReleaseItem__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__SegmentReleaseItem__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SegmentReleaseItem {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentReleaseItem__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentReleaseItem__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__SegmentReleaseItem__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SegmentReleaseItem {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SegmentReleaseItem where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/SegmentReleaseItem";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__SegmentReleaseItem() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EntryEvent() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__EntryEvent__init(msg: *mut EntryEvent) -> bool;
    fn mrs_msgs__msg__EntryEvent__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<EntryEvent>, size: usize) -> bool;
    fn mrs_msgs__msg__EntryEvent__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<EntryEvent>);
    fn mrs_msgs__msg__EntryEvent__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<EntryEvent>, out_seq: *mut rosidl_runtime_rs::Sequence<EntryEvent>) -> bool;
}

// Corresponds to mrs_msgs__msg__EntryEvent
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntryEvent {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub segment: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub entered: bool,

}



impl Default for EntryEvent {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__EntryEvent__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__EntryEvent__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for EntryEvent {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvent__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvent__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__EntryEvent__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for EntryEvent {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for EntryEvent where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/EntryEvent";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__EntryEvent() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopDeclaration() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__StopDeclaration__init(msg: *mut StopDeclaration) -> bool;
    fn mrs_msgs__msg__StopDeclaration__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<StopDeclaration>, size: usize) -> bool;
    fn mrs_msgs__msg__StopDeclaration__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<StopDeclaration>);
    fn mrs_msgs__msg__StopDeclaration__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<StopDeclaration>, out_seq: *mut rosidl_runtime_rs::Sequence<StopDeclaration>) -> bool;
}

// Corresponds to mrs_msgs__msg__StopDeclaration
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// reason uses the SHARED StopReason vocabulary (mrs_msgs/msg/StopReason.msg,
/// 350 §3-2 V1). This channel's legal subset is {1,3,5} — NON-contiguous.
/// 🔴 Do not validate with a range check (350 §3-2, §4). Validate by set
/// membership against the {1,3,5} subset.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StopDeclaration {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_edge: super::super::msg::rmw::EdgeRef,


    // This member is not documented.
    #[allow(missing_docs)]
    pub declared: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub reason: u8,

}



impl Default for StopDeclaration {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__StopDeclaration__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__StopDeclaration__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for StopDeclaration {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclaration__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclaration__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopDeclaration__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for StopDeclaration {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for StopDeclaration where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/StopDeclaration";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopDeclaration() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RealizabilityEntry() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RealizabilityEntry__init(msg: *mut RealizabilityEntry) -> bool;
    fn mrs_msgs__msg__RealizabilityEntry__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RealizabilityEntry>, size: usize) -> bool;
    fn mrs_msgs__msg__RealizabilityEntry__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RealizabilityEntry>);
    fn mrs_msgs__msg__RealizabilityEntry__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RealizabilityEntry>, out_seq: *mut rosidl_runtime_rs::Sequence<RealizabilityEntry>) -> bool;
}

// Corresponds to mrs_msgs__msg__RealizabilityEntry
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RealizabilityEntry {

    // This member is not documented.
    #[allow(missing_docs)]
    pub group: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub alternative: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub realizable: bool,

}



impl Default for RealizabilityEntry {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RealizabilityEntry__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RealizabilityEntry__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RealizabilityEntry {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RealizabilityEntry__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RealizabilityEntry__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RealizabilityEntry__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RealizabilityEntry {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RealizabilityEntry where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RealizabilityEntry";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RealizabilityEntry() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Limits() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__Limits__init(msg: *mut Limits) -> bool;
    fn mrs_msgs__msg__Limits__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Limits>, size: usize) -> bool;
    fn mrs_msgs__msg__Limits__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Limits>);
    fn mrs_msgs__msg__Limits__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Limits>, out_seq: *mut rosidl_runtime_rs::Sequence<Limits>) -> bool;
}

// Corresponds to mrs_msgs__msg__Limits
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Fleet-folded physical limits only (mrs.robot_specs carries the per-robot
/// four: v_max, brake_decel_min_mps2, curvature_max_invm,
/// reverse_motion_allowed — do NOT duplicate them here, 350/roadmap-style
/// single-source-of-truth rule).

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Limits {

    // This member is not documented.
    #[allow(missing_docs)]
    pub a_max: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub j_max: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub min_separation_m: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub omega_max_radps: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub yaw_accel_max_radps2: f64,

}



impl Default for Limits {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__Limits__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__Limits__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Limits {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Limits__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Limits__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__Limits__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Limits {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Limits where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/Limits";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__Limits() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotTrajectory() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotTrajectory__init(msg: *mut RobotTrajectory) -> bool;
    fn mrs_msgs__msg__RobotTrajectory__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotTrajectory>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotTrajectory__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotTrajectory>);
    fn mrs_msgs__msg__RobotTrajectory__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotTrajectory>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotTrajectory>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotTrajectory
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotTrajectory {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub points: rosidl_runtime_rs::Sequence<super::super::msg::rmw::TrajectoryPoint>,

}



impl Default for RobotTrajectory {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotTrajectory__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotTrajectory__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotTrajectory {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotTrajectory__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotTrajectory__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotTrajectory__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotTrajectory {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotTrajectory where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotTrajectory";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotTrajectory() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TrajectoryPoint() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__TrajectoryPoint__init(msg: *mut TrajectoryPoint) -> bool;
    fn mrs_msgs__msg__TrajectoryPoint__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<TrajectoryPoint>, size: usize) -> bool;
    fn mrs_msgs__msg__TrajectoryPoint__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<TrajectoryPoint>);
    fn mrs_msgs__msg__TrajectoryPoint__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<TrajectoryPoint>, out_seq: *mut rosidl_runtime_rs::Sequence<TrajectoryPoint>) -> bool;
}

// Corresponds to mrs_msgs__msg__TrajectoryPoint
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TrajectoryPoint {

    // This member is not documented.
    #[allow(missing_docs)]
    pub t: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub yaw: f64,

    /// Optional numeric fields: has_* companion booleans (350 §4-3 rule). has_* is
    /// NEVER carried in the JSON boundary dump — absence there IS absence here.
    pub v: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub has_v: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub a: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub has_a: bool,

}



impl Default for TrajectoryPoint {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__TrajectoryPoint__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__TrajectoryPoint__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for TrajectoryPoint {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TrajectoryPoint__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TrajectoryPoint__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__TrajectoryPoint__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for TrajectoryPoint {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for TrajectoryPoint where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/TrajectoryPoint";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__TrajectoryPoint() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__OperatorCommand() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__OperatorCommand__init(msg: *mut OperatorCommand) -> bool;
    fn mrs_msgs__msg__OperatorCommand__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<OperatorCommand>, size: usize) -> bool;
    fn mrs_msgs__msg__OperatorCommand__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<OperatorCommand>);
    fn mrs_msgs__msg__OperatorCommand__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<OperatorCommand>, out_seq: *mut rosidl_runtime_rs::Sequence<OperatorCommand>) -> bool;
}

// Corresponds to mrs_msgs__msg__OperatorCommand
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// OperatorVerb vocabulary (350 §3-2 V2~V6).

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct OperatorCommand {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub verb: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub active: bool,

    /// Optional numeric field: has_* companion (350 §4-3). Required only when
    /// verb==VERB_SPEED_CAP and active==true (layer-2 condition, not layer-1).
    pub speed_cap_mps: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub has_speed_cap_mps: bool,

    /// Optional STRING field: "" sentinel, NOT a has_* flag (350 §4-3 rule — this
    /// message is the one place both conventions coexist). Required only when
    /// verb==VERB_GOAL_OVERRIDE and active==true.
    pub goal_location: rosidl_runtime_rs::String,

}

impl OperatorCommand {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const VERB_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const VERB_HOLD: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const VERB_SPEED_CAP: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const VERB_GOAL_OVERRIDE: u8 = 3;

}


impl Default for OperatorCommand {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__OperatorCommand__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__OperatorCommand__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for OperatorCommand {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommand__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommand__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__OperatorCommand__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for OperatorCommand {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for OperatorCommand where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/OperatorCommand";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__OperatorCommand() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotGroup() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotGroup__init(msg: *mut RobotGroup) -> bool;
    fn mrs_msgs__msg__RobotGroup__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotGroup>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotGroup__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotGroup>);
    fn mrs_msgs__msg__RobotGroup__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotGroup>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotGroup>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotGroup
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// string[][] has no IDL representation (350 §6 replan_result.components) —
/// one extra level of nesting via this wrapper instead (350 §7-6 "3중 이상
/// 중첩은 만들지 않는다").

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotGroup {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robots: rosidl_runtime_rs::Sequence<rosidl_runtime_rs::String>,

}



impl Default for RobotGroup {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotGroup__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotGroup__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotGroup {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotGroup__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotGroup__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotGroup__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotGroup {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotGroup where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotGroup";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotGroup() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotBaseline() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__RobotBaseline__init(msg: *mut RobotBaseline) -> bool;
    fn mrs_msgs__msg__RobotBaseline__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotBaseline>, size: usize) -> bool;
    fn mrs_msgs__msg__RobotBaseline__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotBaseline>);
    fn mrs_msgs__msg__RobotBaseline__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotBaseline>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotBaseline>) -> bool;
}

// Corresponds to mrs_msgs__msg__RobotBaseline
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotBaseline {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub committed_segment_count: u32,

}



impl Default for RobotBaseline {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__RobotBaseline__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__RobotBaseline__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotBaseline {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotBaseline__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotBaseline__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__RobotBaseline__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotBaseline {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotBaseline where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/RobotBaseline";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__RobotBaseline() }
  }
}


#[link(name = "mrs_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopReason() -> *const std::ffi::c_void;
}

#[link(name = "mrs_msgs__rosidl_generator_c")]
extern "C" {
    fn mrs_msgs__msg__StopReason__init(msg: *mut StopReason) -> bool;
    fn mrs_msgs__msg__StopReason__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<StopReason>, size: usize) -> bool;
    fn mrs_msgs__msg__StopReason__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<StopReason>);
    fn mrs_msgs__msg__StopReason__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<StopReason>, out_seq: *mut rosidl_runtime_rs::Sequence<StopReason>) -> bool;
}

// Corresponds to mrs_msgs__msg__StopReason
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Shared closed vocabulary for mrs.stop_declaration.declarations[].reason
/// (legal subset {1,3,5}) and mrs.replan_request.reason (legal subset
/// {1,2,3,4,5,6}) — 350 §3-2 V1, D1-c. ONE numbering, not two.
/// 🔴 whether rosidl accepts a constants-only, zero-field .msg is
/// UNCONFIRMED before the first colcon build (350 §3-4). If rejected, the
/// documented fallback is to declare these seven constants inside
/// StopDeclaration.msg instead (350 §3-4) — not applied here pre-emptively.

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StopReason {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}

impl StopReason {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_UNKNOWN: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_EXOGENOUS_BLOCK: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_CTRL_STOP_DECLARED: u8 = 2;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_INFEASIBLE_SUBGOAL: u8 = 3;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_ORIENTATION_FLIP: u8 = 4;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_UNRESOLVABLE_LOCALLY: u8 = 5;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const REASON_HORIZON_EXHAUSTING: u8 = 6;

}


impl Default for StopReason {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mrs_msgs__msg__StopReason__init(&mut msg as *mut _) {
        panic!("Call to mrs_msgs__msg__StopReason__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for StopReason {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopReason__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopReason__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mrs_msgs__msg__StopReason__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for StopReason {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for StopReason where Self: Sized {
  const TYPE_NAME: &'static str = "mrs_msgs/msg/StopReason";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mrs_msgs__msg__StopReason() }
  }
}


