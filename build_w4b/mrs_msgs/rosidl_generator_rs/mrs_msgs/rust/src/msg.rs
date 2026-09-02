#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to mrs_msgs__msg__Roadmap

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Roadmap {

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
    pub nodes: Vec<super::msg::RoadmapNode>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub edges: Vec<super::msg::RoadmapEdge>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub endpoints: Vec<std::string::String>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub corridors: Vec<super::msg::Corridor>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub provenance: super::msg::RoadmapProvenance,

}



impl Default for Roadmap {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Roadmap::default())
  }
}

impl rosidl_runtime_rs::Message for Roadmap {
  type RmwMsg = super::msg::rmw::Roadmap;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        nodes: msg.nodes
          .into_iter()
          .map(|elem| super::msg::RoadmapNode::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        edges: msg.edges
          .into_iter()
          .map(|elem| super::msg::RoadmapEdge::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        endpoints: msg.endpoints
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        corridors: msg.corridors
          .into_iter()
          .map(|elem| super::msg::Corridor::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        provenance: super::msg::RoadmapProvenance::into_rmw_message(std::borrow::Cow::Owned(msg.provenance)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        nodes: msg.nodes
          .iter()
          .map(|elem| super::msg::RoadmapNode::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        edges: msg.edges
          .iter()
          .map(|elem| super::msg::RoadmapEdge::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        endpoints: msg.endpoints
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
        corridors: msg.corridors
          .iter()
          .map(|elem| super::msg::Corridor::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        provenance: super::msg::RoadmapProvenance::into_rmw_message(std::borrow::Cow::Borrowed(&msg.provenance)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      nodes: msg.nodes
          .into_iter()
          .map(super::msg::RoadmapNode::from_rmw_message)
          .collect(),
      edges: msg.edges
          .into_iter()
          .map(super::msg::RoadmapEdge::from_rmw_message)
          .collect(),
      endpoints: msg.endpoints
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
      corridors: msg.corridors
          .into_iter()
          .map(super::msg::Corridor::from_rmw_message)
          .collect(),
      provenance: super::msg::RoadmapProvenance::from_rmw_message(msg.provenance),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotSpecs

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotSpecs {

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
    pub robots: Vec<super::msg::RobotSpec>,

}



impl Default for RobotSpecs {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotSpecs::default())
  }
}

impl rosidl_runtime_rs::Message for RobotSpecs {
  type RmwMsg = super::msg::rmw::RobotSpecs;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        robots: msg.robots
          .into_iter()
          .map(|elem| super::msg::RobotSpec::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        robots: msg.robots
          .iter()
          .map(|elem| super::msg::RobotSpec::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      robots: msg.robots
          .into_iter()
          .map(super::msg::RobotSpec::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__TaskRelease

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TaskRelease {

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
    pub release_index: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub task: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goals: Vec<super::msg::TaskGoal>,

}



impl Default for TaskRelease {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::TaskRelease::default())
  }
}

impl rosidl_runtime_rs::Message for TaskRelease {
  type RmwMsg = super::msg::rmw::TaskRelease;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        release_index: msg.release_index,
        task: msg.task.as_str().into(),
        goals: msg.goals
          .into_iter()
          .map(|elem| super::msg::TaskGoal::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      release_index: msg.release_index,
        task: msg.task.as_str().into(),
        goals: msg.goals
          .iter()
          .map(|elem| super::msg::TaskGoal::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      release_index: msg.release_index,
      task: msg.task.to_string(),
      goals: msg.goals
          .into_iter()
          .map(super::msg::TaskGoal::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Assignment

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Assignment {

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
    pub revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub assignments: Vec<super::msg::RobotAssignment>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub unassigned_tasks: Vec<std::string::String>,

}



impl Default for Assignment {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Assignment::default())
  }
}

impl rosidl_runtime_rs::Message for Assignment {
  type RmwMsg = super::msg::rmw::Assignment;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        revision: msg.revision,
        assignments: msg.assignments
          .into_iter()
          .map(|elem| super::msg::RobotAssignment::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        unassigned_tasks: msg.unassigned_tasks
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      revision: msg.revision,
        assignments: msg.assignments
          .iter()
          .map(|elem| super::msg::RobotAssignment::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        unassigned_tasks: msg.unassigned_tasks
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      revision: msg.revision,
      assignments: msg.assignments
          .into_iter()
          .map(super::msg::RobotAssignment::from_rmw_message)
          .collect(),
      unassigned_tasks: msg.unassigned_tasks
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__GoalCompletion

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GoalCompletion {

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
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub revision: u64,

}



impl Default for GoalCompletion {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::GoalCompletion::default())
  }
}

impl rosidl_runtime_rs::Message for GoalCompletion {
  type RmwMsg = super::msg::rmw::GoalCompletion;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        seq: msg.seq,
        robot: msg.robot.as_str().into(),
        goal_id: msg.goal_id.as_str().into(),
        revision: msg.revision,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      seq: msg.seq,
        robot: msg.robot.as_str().into(),
        goal_id: msg.goal_id.as_str().into(),
      revision: msg.revision,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      seq: msg.seq,
      robot: msg.robot.to_string(),
      goal_id: msg.goal_id.to_string(),
      revision: msg.revision,
    }
  }
}


// Corresponds to mrs_msgs__msg__DiscretePlan

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DiscretePlan {

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
    pub plan_revision: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plans: Vec<super::msg::RobotPlan>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub visit_order: Vec<super::msg::VisitOrder>,

}



impl Default for DiscretePlan {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::DiscretePlan::default())
  }
}

impl rosidl_runtime_rs::Message for DiscretePlan {
  type RmwMsg = super::msg::rmw::DiscretePlan;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        plan_revision: msg.plan_revision,
        plans: msg.plans
          .into_iter()
          .map(|elem| super::msg::RobotPlan::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        visit_order: msg.visit_order
          .into_iter()
          .map(|elem| super::msg::VisitOrder::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      plan_revision: msg.plan_revision,
        plans: msg.plans
          .iter()
          .map(|elem| super::msg::RobotPlan::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        visit_order: msg.visit_order
          .iter()
          .map(|elem| super::msg::VisitOrder::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      plan_revision: msg.plan_revision,
      plans: msg.plans
          .into_iter()
          .map(super::msg::RobotPlan::from_rmw_message)
          .collect(),
      visit_order: msg.visit_order
          .into_iter()
          .map(super::msg::VisitOrder::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__ExecutionConstraints

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ExecutionConstraints {

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
    pub commit_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub segments: Vec<super::msg::Segment>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dependencies: Vec<super::msg::Dependency>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub switch_groups: Vec<super::msg::SwitchGroup>,

}



impl Default for ExecutionConstraints {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ExecutionConstraints::default())
  }
}

impl rosidl_runtime_rs::Message for ExecutionConstraints {
  type RmwMsg = super::msg::rmw::ExecutionConstraints;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        commit_seq: msg.commit_seq,
        segments: msg.segments
          .into_iter()
          .map(|elem| super::msg::Segment::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        dependencies: msg.dependencies
          .into_iter()
          .map(|elem| super::msg::Dependency::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        switch_groups: msg.switch_groups
          .into_iter()
          .map(|elem| super::msg::SwitchGroup::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      commit_seq: msg.commit_seq,
        segments: msg.segments
          .iter()
          .map(|elem| super::msg::Segment::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        dependencies: msg.dependencies
          .iter()
          .map(|elem| super::msg::Dependency::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
        switch_groups: msg.switch_groups
          .iter()
          .map(|elem| super::msg::SwitchGroup::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      commit_seq: msg.commit_seq,
      segments: msg.segments
          .into_iter()
          .map(super::msg::Segment::from_rmw_message)
          .collect(),
      dependencies: msg.dependencies
          .into_iter()
          .map(super::msg::Dependency::from_rmw_message)
          .collect(),
      switch_groups: msg.switch_groups
          .into_iter()
          .map(super::msg::SwitchGroup::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__SegmentRelease

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SegmentRelease {

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
    pub robot: std::string::String,


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
    pub segments: Vec<super::msg::SegmentReleaseItem>,

}



impl Default for SegmentRelease {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::SegmentRelease::default())
  }
}

impl rosidl_runtime_rs::Message for SegmentRelease {
  type RmwMsg = super::msg::rmw::SegmentRelease;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        robot: msg.robot.as_str().into(),
        release_seq: msg.release_seq,
        commit_seq: msg.commit_seq,
        entry_seq_ack: msg.entry_seq_ack,
        segments: msg.segments
          .into_iter()
          .map(|elem| super::msg::SegmentReleaseItem::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        robot: msg.robot.as_str().into(),
      release_seq: msg.release_seq,
      commit_seq: msg.commit_seq,
      entry_seq_ack: msg.entry_seq_ack,
        segments: msg.segments
          .iter()
          .map(|elem| super::msg::SegmentReleaseItem::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      robot: msg.robot.to_string(),
      release_seq: msg.release_seq,
      commit_seq: msg.commit_seq,
      entry_seq_ack: msg.entry_seq_ack,
      segments: msg.segments
          .into_iter()
          .map(super::msg::SegmentReleaseItem::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__EntryEvents

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntryEvents {

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
    pub events: Vec<super::msg::EntryEvent>,

}



impl Default for EntryEvents {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::EntryEvents::default())
  }
}

impl rosidl_runtime_rs::Message for EntryEvents {
  type RmwMsg = super::msg::rmw::EntryEvents;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        events: msg.events
          .into_iter()
          .map(|elem| super::msg::EntryEvent::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        events: msg.events
          .iter()
          .map(|elem| super::msg::EntryEvent::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      events: msg.events
          .into_iter()
          .map(super::msg::EntryEvent::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__StopDeclarations

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StopDeclarations {

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
    pub declarations: Vec<super::msg::StopDeclaration>,

}



impl Default for StopDeclarations {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::StopDeclarations::default())
  }
}

impl rosidl_runtime_rs::Message for StopDeclarations {
  type RmwMsg = super::msg::rmw::StopDeclarations;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        declarations: msg.declarations
          .into_iter()
          .map(|elem| super::msg::StopDeclaration::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        declarations: msg.declarations
          .iter()
          .map(|elem| super::msg::StopDeclaration::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      declarations: msg.declarations
          .into_iter()
          .map(super::msg::StopDeclaration::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Realizability

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Realizability {

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
    pub rz_seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub entries: Vec<super::msg::RealizabilityEntry>,

}



impl Default for Realizability {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Realizability::default())
  }
}

impl rosidl_runtime_rs::Message for Realizability {
  type RmwMsg = super::msg::rmw::Realizability;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        rz_seq: msg.rz_seq,
        entries: msg.entries
          .into_iter()
          .map(|elem| super::msg::RealizabilityEntry::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
      rz_seq: msg.rz_seq,
        entries: msg.entries
          .iter()
          .map(|elem| super::msg::RealizabilityEntry::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      rz_seq: msg.rz_seq,
      entries: msg.entries
          .into_iter()
          .map(super::msg::RealizabilityEntry::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Trajectories

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Trajectories {

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
    pub limits: super::msg::Limits,


    // This member is not documented.
    #[allow(missing_docs)]
    pub trajectories: Vec<super::msg::RobotTrajectory>,

}



impl Default for Trajectories {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Trajectories::default())
  }
}

impl rosidl_runtime_rs::Message for Trajectories {
  type RmwMsg = super::msg::rmw::Trajectories;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        limits: super::msg::Limits::into_rmw_message(std::borrow::Cow::Owned(msg.limits)).into_owned(),
        trajectories: msg.trajectories
          .into_iter()
          .map(|elem| super::msg::RobotTrajectory::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        limits: super::msg::Limits::into_rmw_message(std::borrow::Cow::Borrowed(&msg.limits)).into_owned(),
        trajectories: msg.trajectories
          .iter()
          .map(|elem| super::msg::RobotTrajectory::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      limits: super::msg::Limits::from_rmw_message(msg.limits),
      trajectories: msg.trajectories
          .into_iter()
          .map(super::msg::RobotTrajectory::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__OperatorCommands

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct OperatorCommands {

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
    pub commands: Vec<super::msg::OperatorCommand>,

}



impl Default for OperatorCommands {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::OperatorCommands::default())
  }
}

impl rosidl_runtime_rs::Message for OperatorCommands {
  type RmwMsg = super::msg::rmw::OperatorCommands;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        commands: msg.commands
          .into_iter()
          .map(|elem| super::msg::OperatorCommand::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        schema: msg.schema.as_str().into(),
        schema_version: msg.schema_version.as_str().into(),
        instance_id: msg.instance_id.as_str().into(),
        commands: msg.commands
          .iter()
          .map(|elem| super::msg::OperatorCommand::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      schema: msg.schema.to_string(),
      schema_version: msg.schema_version.to_string(),
      instance_id: msg.instance_id.to_string(),
      commands: msg.commands
          .into_iter()
          .map(super::msg::OperatorCommand::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RoadmapNode

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RoadmapNode {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RoadmapNode::default())
  }
}

impl rosidl_runtime_rs::Message for RoadmapNode {
  type RmwMsg = super::msg::rmw::RoadmapNode;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        x: msg.x,
        y: msg.y,
        clearance_m: msg.clearance_m,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
      x: msg.x,
      y: msg.y,
      clearance_m: msg.clearance_m,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      x: msg.x,
      y: msg.y,
      clearance_m: msg.clearance_m,
    }
  }
}


// Corresponds to mrs_msgs__msg__RoadmapEdge
/// EdgeTraversal vocabulary (350 §3-2 V2~V6). 0 is reserved *_UNKNOWN across all
/// vocabularies (350 D1-a) — do not hand it to the first legal value.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RoadmapEdge {

    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: std::string::String,


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
    pub corridor: std::string::String,

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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RoadmapEdge::default())
  }
}

impl rosidl_runtime_rs::Message for RoadmapEdge {
  type RmwMsg = super::msg::rmw::RoadmapEdge;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
        traversal: msg.traversal,
        length_m: msg.length_m,
        width_m: msg.width_m,
        capacity_robots: msg.capacity_robots,
        corridor: msg.corridor.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      traversal: msg.traversal,
      length_m: msg.length_m,
      width_m: msg.width_m,
      capacity_robots: msg.capacity_robots,
        corridor: msg.corridor.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      from_id: msg.from_id.to_string(),
      to_id: msg.to_id.to_string(),
      traversal: msg.traversal,
      length_m: msg.length_m,
      width_m: msg.width_m,
      capacity_robots: msg.capacity_robots,
      corridor: msg.corridor.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Corridor

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Corridor {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub edges: Vec<super::msg::EdgeRef>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub length_m: f64,

}



impl Default for Corridor {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Corridor::default())
  }
}

impl rosidl_runtime_rs::Message for Corridor {
  type RmwMsg = super::msg::rmw::Corridor;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        edges: msg.edges
          .into_iter()
          .map(|elem| super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
        length_m: msg.length_m,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        edges: msg.edges
          .iter()
          .map(|elem| super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      length_m: msg.length_m,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      edges: msg.edges
          .into_iter()
          .map(super::msg::EdgeRef::from_rmw_message)
          .collect(),
      length_m: msg.length_m,
    }
  }
}


// Corresponds to mrs_msgs__msg__EdgeRef
/// Directed pair of roadmap-node identifiers reused across three JSON shapes
/// (350 §6 "재사용 결정 둘"): roadmap.corridors[].edges (2-element tuple),
/// stop_declaration.blocked_edge (object {from,to}), replan_request.blocked[]
/// (object {from,to}). The io/ adapter absorbs both JSON shapes into this one
/// type (350 §8 D6 item 8) — that adapter code is outside this round's scope.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EdgeRef {

    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: std::string::String,

}



impl Default for EdgeRef {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::EdgeRef::default())
  }
}

impl rosidl_runtime_rs::Message for EdgeRef {
  type RmwMsg = super::msg::rmw::EdgeRef;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      from_id: msg.from_id.to_string(),
      to_id: msg.to_id.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RoadmapProvenance

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    pub capacity_rule_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub generator_version: std::string::String,

}



impl Default for RoadmapProvenance {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RoadmapProvenance::default())
  }
}

impl rosidl_runtime_rs::Message for RoadmapProvenance {
  type RmwMsg = super::msg::rmw::RoadmapProvenance;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        min_separation_m: msg.min_separation_m,
        wall_inflation_m: msg.wall_inflation_m,
        capacity_rule_id: msg.capacity_rule_id.as_str().into(),
        generator_version: msg.generator_version.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      min_separation_m: msg.min_separation_m,
      wall_inflation_m: msg.wall_inflation_m,
        capacity_rule_id: msg.capacity_rule_id.as_str().into(),
        generator_version: msg.generator_version.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      min_separation_m: msg.min_separation_m,
      wall_inflation_m: msg.wall_inflation_m,
      capacity_rule_id: msg.capacity_rule_id.to_string(),
      generator_version: msg.generator_version.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotSpec

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotSpec {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotSpec::default())
  }
}

impl rosidl_runtime_rs::Message for RobotSpec {
  type RmwMsg = super::msg::rmw::RobotSpec;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        avg_traversal_speed_mps: msg.avg_traversal_speed_mps,
        v_max: msg.v_max,
        brake_decel_min_mps2: msg.brake_decel_min_mps2,
        curvature_max_invm: msg.curvature_max_invm,
        reverse_motion_allowed: msg.reverse_motion_allowed,
        circumradius_m: msg.circumradius_m,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
      avg_traversal_speed_mps: msg.avg_traversal_speed_mps,
      v_max: msg.v_max,
      brake_decel_min_mps2: msg.brake_decel_min_mps2,
      curvature_max_invm: msg.curvature_max_invm,
      reverse_motion_allowed: msg.reverse_motion_allowed,
      circumradius_m: msg.circumradius_m,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      avg_traversal_speed_mps: msg.avg_traversal_speed_mps,
      v_max: msg.v_max,
      brake_decel_min_mps2: msg.brake_decel_min_mps2,
      curvature_max_invm: msg.curvature_max_invm,
      reverse_motion_allowed: msg.reverse_motion_allowed,
      circumradius_m: msg.circumradius_m,
    }
  }
}


// Corresponds to mrs_msgs__msg__TaskGoal
/// 350 §7 개정 안건 ① — task_release.schema.json:31의 "Goal (공유)"는 성립하지
/// 않는다(assignment.Goal과 필드 집합이 다르다). 이 판은 개정을 적용하지 않고
/// TaskGoal로 렌더링만 한다(350 D6, 오늘의 임시 처분). 스키마 텍스트는 미개정.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct TaskGoal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,

}



impl Default for TaskGoal {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::TaskGoal::default())
  }
}

impl rosidl_runtime_rs::Message for TaskGoal {
  type RmwMsg = super::msg::rmw::TaskGoal;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: msg.goal_id.as_str().into(),
        location: msg.location.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: msg.goal_id.as_str().into(),
        location: msg.location.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: msg.goal_id.to_string(),
      location: msg.location.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotAssignment

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotAssignment {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub start: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goals: Vec<super::msg::Goal>,

}



impl Default for RobotAssignment {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotAssignment::default())
  }
}

impl rosidl_runtime_rs::Message for RobotAssignment {
  type RmwMsg = super::msg::rmw::RobotAssignment;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        start: msg.start.as_str().into(),
        goals: msg.goals
          .into_iter()
          .map(|elem| super::msg::Goal::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        start: msg.start.as_str().into(),
        goals: msg.goals
          .iter()
          .map(|elem| super::msg::Goal::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      start: msg.start.to_string(),
      goals: msg.goals
          .into_iter()
          .map(super::msg::Goal::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Goal

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub task: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,

}



impl Default for Goal {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Goal::default())
  }
}

impl rosidl_runtime_rs::Message for Goal {
  type RmwMsg = super::msg::rmw::Goal;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: msg.goal_id.as_str().into(),
        task: msg.task.as_str().into(),
        location: msg.location.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        goal_id: msg.goal_id.as_str().into(),
        task: msg.task.as_str().into(),
        location: msg.location.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      goal_id: msg.goal_id.to_string(),
      task: msg.task.to_string(),
      location: msg.location.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotPlan
/// PlanTerminal vocabulary (350 §3-2 V2~V6).

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotPlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub terminal: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub steps: Vec<super::msg::PlanStep>,

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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotPlan::default())
  }
}

impl rosidl_runtime_rs::Message for RobotPlan {
  type RmwMsg = super::msg::rmw::RobotPlan;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        terminal: msg.terminal,
        steps: msg.steps
          .into_iter()
          .map(|elem| super::msg::PlanStep::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
      terminal: msg.terminal,
        steps: msg.steps
          .iter()
          .map(|elem| super::msg::PlanStep::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      terminal: msg.terminal,
      steps: msg.steps
          .into_iter()
          .map(super::msg::PlanStep::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__PlanStep

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlanStep {

    // This member is not documented.
    #[allow(missing_docs)]
    pub index: u32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,

}



impl Default for PlanStep {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::PlanStep::default())
  }
}

impl rosidl_runtime_rs::Message for PlanStep {
  type RmwMsg = super::msg::rmw::PlanStep;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        index: msg.index,
        location: msg.location.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      index: msg.index,
        location: msg.location.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      index: msg.index,
      location: msg.location.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__VisitOrder
/// Reused in two places with the SAME field set but DIFFERENT values (350 §6
/// "재사용 결정 둘" / §8 D6 item 7): discrete_plan.visit_order[] (full order)
/// and replan_request.ordering_constraints[] (realized prefix of the former).
/// Do not write code that assumes "same type => same value".

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VisitOrder {

    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub sequence: Vec<super::msg::VisitItem>,

}



impl Default for VisitOrder {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::VisitOrder::default())
  }
}

impl rosidl_runtime_rs::Message for VisitOrder {
  type RmwMsg = super::msg::rmw::VisitOrder;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        location: msg.location.as_str().into(),
        sequence: msg.sequence
          .into_iter()
          .map(|elem| super::msg::VisitItem::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        location: msg.location.as_str().into(),
        sequence: msg.sequence
          .iter()
          .map(|elem| super::msg::VisitItem::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      location: msg.location.to_string(),
      sequence: msg.sequence
          .into_iter()
          .map(super::msg::VisitItem::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__VisitItem

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct VisitItem {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub index: u32,

}



impl Default for VisitItem {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::VisitItem::default())
  }
}

impl rosidl_runtime_rs::Message for VisitItem {
  type RmwMsg = super::msg::rmw::VisitItem;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        index: msg.index,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
      index: msg.index,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      index: msg.index,
    }
  }
}


// Corresponds to mrs_msgs__msg__Segment

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Segment {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub from_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: std::string::String,

}



impl Default for Segment {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Segment::default())
  }
}

impl rosidl_runtime_rs::Message for Segment {
  type RmwMsg = super::msg::rmw::Segment;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        robot: msg.robot.as_str().into(),
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        robot: msg.robot.as_str().into(),
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      robot: msg.robot.to_string(),
      from_id: msg.from_id.to_string(),
      to_id: msg.to_id.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Dependency
/// DependencyType vocabulary (350 §3-2 V2~V6).

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Dependency {
    /// 🔴 from_id/to_id here are SEGMENT identifiers ({robot}#{index}), NOT roadmap
    /// node ids (350 §8 D6 item 1). Do NOT nest an EdgeRef into this message — the
    /// name space differs from RoadmapEdge/Segment's from_id/to_id even though the
    /// JSON source field names ("from"/"to") are identical.
    pub from_id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub to_id: std::string::String,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Dependency::default())
  }
}

impl rosidl_runtime_rs::Message for Dependency {
  type RmwMsg = super::msg::rmw::Dependency;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
        type_: msg.type_,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        from_id: msg.from_id.as_str().into(),
        to_id: msg.to_id.as_str().into(),
      type_: msg.type_,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      from_id: msg.from_id.to_string(),
      to_id: msg.to_id.to_string(),
      type_: msg.type_,
    }
  }
}


// Corresponds to mrs_msgs__msg__SwitchGroup

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SwitchGroup {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub alternatives: Vec<super::msg::Alternative>,

}



impl Default for SwitchGroup {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::SwitchGroup::default())
  }
}

impl rosidl_runtime_rs::Message for SwitchGroup {
  type RmwMsg = super::msg::rmw::SwitchGroup;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        location: msg.location.as_str().into(),
        alternatives: msg.alternatives
          .into_iter()
          .map(|elem| super::msg::Alternative::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        location: msg.location.as_str().into(),
        alternatives: msg.alternatives
          .iter()
          .map(|elem| super::msg::Alternative::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      location: msg.location.to_string(),
      alternatives: msg.alternatives
          .into_iter()
          .map(super::msg::Alternative::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__Alternative

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Alternative {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub dependencies: Vec<super::msg::Dependency>,

}



impl Default for Alternative {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Alternative::default())
  }
}

impl rosidl_runtime_rs::Message for Alternative {
  type RmwMsg = super::msg::rmw::Alternative;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        dependencies: msg.dependencies
          .into_iter()
          .map(|elem| super::msg::Dependency::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        dependencies: msg.dependencies
          .iter()
          .map(|elem| super::msg::Dependency::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      dependencies: msg.dependencies
          .into_iter()
          .map(super::msg::Dependency::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__SegmentReleaseItem

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SegmentReleaseItem {

    // This member is not documented.
    #[allow(missing_docs)]
    pub id: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub released: bool,

}



impl Default for SegmentReleaseItem {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::SegmentReleaseItem::default())
  }
}

impl rosidl_runtime_rs::Message for SegmentReleaseItem {
  type RmwMsg = super::msg::rmw::SegmentReleaseItem;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
        released: msg.released,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        id: msg.id.as_str().into(),
      released: msg.released,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      id: msg.id.to_string(),
      released: msg.released,
    }
  }
}


// Corresponds to mrs_msgs__msg__EntryEvent

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct EntryEvent {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub segment: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub location: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub entered: bool,

}



impl Default for EntryEvent {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::EntryEvent::default())
  }
}

impl rosidl_runtime_rs::Message for EntryEvent {
  type RmwMsg = super::msg::rmw::EntryEvent;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        seq: msg.seq,
        robot: msg.robot.as_str().into(),
        segment: msg.segment.as_str().into(),
        location: msg.location.as_str().into(),
        entered: msg.entered,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      seq: msg.seq,
        robot: msg.robot.as_str().into(),
        segment: msg.segment.as_str().into(),
        location: msg.location.as_str().into(),
      entered: msg.entered,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      seq: msg.seq,
      robot: msg.robot.to_string(),
      segment: msg.segment.to_string(),
      location: msg.location.to_string(),
      entered: msg.entered,
    }
  }
}


// Corresponds to mrs_msgs__msg__StopDeclaration
/// reason uses the SHARED StopReason vocabulary (mrs_msgs/msg/StopReason.msg,
/// 350 §3-2 V1). This channel's legal subset is {1,3,5} — NON-contiguous.
/// 🔴 Do not validate with a range check (350 §3-2, §4). Validate by set
/// membership against the {1,3,5} subset.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct StopDeclaration {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub blocked_edge: super::msg::EdgeRef,


    // This member is not documented.
    #[allow(missing_docs)]
    pub declared: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub reason: u8,

}



impl Default for StopDeclaration {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::StopDeclaration::default())
  }
}

impl rosidl_runtime_rs::Message for StopDeclaration {
  type RmwMsg = super::msg::rmw::StopDeclaration;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        seq: msg.seq,
        robot: msg.robot.as_str().into(),
        blocked_edge: super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Owned(msg.blocked_edge)).into_owned(),
        declared: msg.declared,
        reason: msg.reason,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      seq: msg.seq,
        robot: msg.robot.as_str().into(),
        blocked_edge: super::msg::EdgeRef::into_rmw_message(std::borrow::Cow::Borrowed(&msg.blocked_edge)).into_owned(),
      declared: msg.declared,
      reason: msg.reason,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      seq: msg.seq,
      robot: msg.robot.to_string(),
      blocked_edge: super::msg::EdgeRef::from_rmw_message(msg.blocked_edge),
      declared: msg.declared,
      reason: msg.reason,
    }
  }
}


// Corresponds to mrs_msgs__msg__RealizabilityEntry

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RealizabilityEntry {

    // This member is not documented.
    #[allow(missing_docs)]
    pub group: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub alternative: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub realizable: bool,

}



impl Default for RealizabilityEntry {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RealizabilityEntry::default())
  }
}

impl rosidl_runtime_rs::Message for RealizabilityEntry {
  type RmwMsg = super::msg::rmw::RealizabilityEntry;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        group: msg.group.as_str().into(),
        alternative: msg.alternative.as_str().into(),
        realizable: msg.realizable,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        group: msg.group.as_str().into(),
        alternative: msg.alternative.as_str().into(),
      realizable: msg.realizable,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      group: msg.group.to_string(),
      alternative: msg.alternative.to_string(),
      realizable: msg.realizable,
    }
  }
}


// Corresponds to mrs_msgs__msg__Limits
/// Fleet-folded physical limits only (mrs.robot_specs carries the per-robot
/// four: v_max, brake_decel_min_mps2, curvature_max_invm,
/// reverse_motion_allowed — do NOT duplicate them here, 350/roadmap-style
/// single-source-of-truth rule).

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Limits::default())
  }
}

impl rosidl_runtime_rs::Message for Limits {
  type RmwMsg = super::msg::rmw::Limits;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        a_max: msg.a_max,
        j_max: msg.j_max,
        min_separation_m: msg.min_separation_m,
        omega_max_radps: msg.omega_max_radps,
        yaw_accel_max_radps2: msg.yaw_accel_max_radps2,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      a_max: msg.a_max,
      j_max: msg.j_max,
      min_separation_m: msg.min_separation_m,
      omega_max_radps: msg.omega_max_radps,
      yaw_accel_max_radps2: msg.yaw_accel_max_radps2,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      a_max: msg.a_max,
      j_max: msg.j_max,
      min_separation_m: msg.min_separation_m,
      omega_max_radps: msg.omega_max_radps,
      yaw_accel_max_radps2: msg.yaw_accel_max_radps2,
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotTrajectory

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotTrajectory {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub points: Vec<super::msg::TrajectoryPoint>,

}



impl Default for RobotTrajectory {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotTrajectory::default())
  }
}

impl rosidl_runtime_rs::Message for RobotTrajectory {
  type RmwMsg = super::msg::rmw::RobotTrajectory;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        points: msg.points
          .into_iter()
          .map(|elem| super::msg::TrajectoryPoint::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        points: msg.points
          .iter()
          .map(|elem| super::msg::TrajectoryPoint::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      points: msg.points
          .into_iter()
          .map(super::msg::TrajectoryPoint::from_rmw_message)
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__TrajectoryPoint

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::TrajectoryPoint::default())
  }
}

impl rosidl_runtime_rs::Message for TrajectoryPoint {
  type RmwMsg = super::msg::rmw::TrajectoryPoint;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        t: msg.t,
        x: msg.x,
        y: msg.y,
        yaw: msg.yaw,
        v: msg.v,
        has_v: msg.has_v,
        a: msg.a,
        has_a: msg.has_a,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      t: msg.t,
      x: msg.x,
      y: msg.y,
      yaw: msg.yaw,
      v: msg.v,
      has_v: msg.has_v,
      a: msg.a,
      has_a: msg.has_a,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      t: msg.t,
      x: msg.x,
      y: msg.y,
      yaw: msg.yaw,
      v: msg.v,
      has_v: msg.has_v,
      a: msg.a,
      has_a: msg.has_a,
    }
  }
}


// Corresponds to mrs_msgs__msg__OperatorCommand
/// OperatorVerb vocabulary (350 §3-2 V2~V6).

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct OperatorCommand {

    // This member is not documented.
    #[allow(missing_docs)]
    pub seq: u64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


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
    pub goal_location: std::string::String,

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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::OperatorCommand::default())
  }
}

impl rosidl_runtime_rs::Message for OperatorCommand {
  type RmwMsg = super::msg::rmw::OperatorCommand;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        seq: msg.seq,
        robot: msg.robot.as_str().into(),
        verb: msg.verb,
        active: msg.active,
        speed_cap_mps: msg.speed_cap_mps,
        has_speed_cap_mps: msg.has_speed_cap_mps,
        goal_location: msg.goal_location.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      seq: msg.seq,
        robot: msg.robot.as_str().into(),
      verb: msg.verb,
      active: msg.active,
      speed_cap_mps: msg.speed_cap_mps,
      has_speed_cap_mps: msg.has_speed_cap_mps,
        goal_location: msg.goal_location.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      seq: msg.seq,
      robot: msg.robot.to_string(),
      verb: msg.verb,
      active: msg.active,
      speed_cap_mps: msg.speed_cap_mps,
      has_speed_cap_mps: msg.has_speed_cap_mps,
      goal_location: msg.goal_location.to_string(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotGroup
/// string[][] has no IDL representation (350 §6 replan_result.components) —
/// one extra level of nesting via this wrapper instead (350 §7-6 "3중 이상
/// 중첩은 만들지 않는다").

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotGroup {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robots: Vec<std::string::String>,

}



impl Default for RobotGroup {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotGroup::default())
  }
}

impl rosidl_runtime_rs::Message for RobotGroup {
  type RmwMsg = super::msg::rmw::RobotGroup;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robots: msg.robots
          .into_iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robots: msg.robots
          .iter()
          .map(|elem| elem.as_str().into())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robots: msg.robots
          .into_iter()
          .map(|elem| elem.to_string())
          .collect(),
    }
  }
}


// Corresponds to mrs_msgs__msg__RobotBaseline

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotBaseline {

    // This member is not documented.
    #[allow(missing_docs)]
    pub robot: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub committed_segment_count: u32,

}



impl Default for RobotBaseline {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotBaseline::default())
  }
}

impl rosidl_runtime_rs::Message for RobotBaseline {
  type RmwMsg = super::msg::rmw::RobotBaseline;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
        committed_segment_count: msg.committed_segment_count,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        robot: msg.robot.as_str().into(),
      committed_segment_count: msg.committed_segment_count,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      robot: msg.robot.to_string(),
      committed_segment_count: msg.committed_segment_count,
    }
  }
}


// Corresponds to mrs_msgs__msg__StopReason
/// Shared closed vocabulary for mrs.stop_declaration.declarations[].reason
/// (legal subset {1,3,5}) and mrs.replan_request.reason (legal subset
/// {1,2,3,4,5,6}) — 350 §3-2 V1, D1-c. ONE numbering, not two.
/// 🔴 whether rosidl accepts a constants-only, zero-field .msg is
/// UNCONFIRMED before the first colcon build (350 §3-4). If rejected, the
/// documented fallback is to declare these seven constants inside
/// StopDeclaration.msg instead (350 §3-4) — not applied here pre-emptively.

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::StopReason::default())
  }
}

impl rosidl_runtime_rs::Message for StopReason {
  type RmwMsg = super::msg::rmw::StopReason;

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


