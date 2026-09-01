cmake_minimum_required(VERSION 3.16)
project(mrs_msgs)

# P0 (47차) — mrs_msgs를 순수 스키마 디렉터리에서 ROS 패키지로 실체화한다.
# 렌더링 결정 정본: _workspace/350_msg_idl_decisions.md(하네스 레포).
# 🔴 schema/*.json 15종은 계약 원본이며 이 CMakeLists가 그것을 읽거나 바꾸지 않는다 —
# .msg/.action은 그 결정을 손으로 옮겨 적은 것이지 스키마에서 생성한 것이 아니다.

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(action_msgs REQUIRED)

set(msg_files
  # 루트 13 (350 §6)
  "msg/Roadmap.msg"
  "msg/RobotSpecs.msg"
  "msg/TaskRelease.msg"
  "msg/Assignment.msg"
  "msg/GoalCompletion.msg"
  "msg/DiscretePlan.msg"
  "msg/ExecutionConstraints.msg"
  "msg/SegmentRelease.msg"
  "msg/EntryEvents.msg"
  "msg/StopDeclarations.msg"
  "msg/Realizability.msg"
  "msg/Trajectories.msg"
  "msg/OperatorCommands.msg"
  # 중첩 27 (350 §6)
  "msg/RoadmapNode.msg"
  "msg/RoadmapEdge.msg"
  "msg/Corridor.msg"
  "msg/EdgeRef.msg"
  "msg/RoadmapProvenance.msg"
  "msg/RobotSpec.msg"
  "msg/TaskGoal.msg"
  "msg/RobotAssignment.msg"
  "msg/Goal.msg"
  "msg/RobotPlan.msg"
  "msg/PlanStep.msg"
  "msg/VisitOrder.msg"
  "msg/VisitItem.msg"
  "msg/Segment.msg"
  "msg/Dependency.msg"
  "msg/SwitchGroup.msg"
  "msg/Alternative.msg"
  "msg/SegmentReleaseItem.msg"
  "msg/EntryEvent.msg"
  "msg/StopDeclaration.msg"
  "msg/RealizabilityEntry.msg"
  "msg/Limits.msg"
  "msg/RobotTrajectory.msg"
  "msg/TrajectoryPoint.msg"
  "msg/OperatorCommand.msg"
  "msg/RobotGroup.msg"
  "msg/RobotBaseline.msg"
  # 상수 전용 1 (350 §3-4 — [추정], 첫 빌드에서 확인 대상)
  "msg/StopReason.msg"
)

set(action_files
  "action/Replan.action"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  ${action_files}
  DEPENDENCIES action_msgs
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
