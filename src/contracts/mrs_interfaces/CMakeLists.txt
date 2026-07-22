cmake_minimum_required(VERSION 3.28)
project(mrs_interfaces)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)

# 계약 레지스트리 §8 파일 목록 순서를 그대로 따른다 (contract-registry.md).
# 새 .msg/.srv 추가는 계약 변경 게이트(§7)를 거친 뒤에만 여기 반영한다.
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/CommitState.msg"
  "msg/CommitStatus.msg"
  "msg/CorridorFlipEvent.msg"
  "msg/EscalationReport.msg"
  "msg/ExecutionWindow.msg"
  "msg/FrozenOrder.msg"
  "msg/JudgeVerdict.msg"
  "msg/LocalPlanShare.msg"
  "msg/NodeMapping.msg"
  "msg/ObservationUncertainty.msg"
  "msg/PlannedPaths.msg"
  "msg/PlanRequestEntry.msg"
  "msg/PlanTick.msg"
  "msg/Pose2D.msg"
  "msg/PredecessorConstraint.msg"
  "msg/RoadmapEdge.msg"
  "msg/RoadmapNode.msg"
  "msg/RobotPath.msg"
  "msg/RobotState.msg"
  "msg/RungEvent.msg"
  "msg/SimCapabilities.msg"
  "msg/SimMetricSample.msg"
  "msg/TaskAssignment.msg"
  "msg/TimedNodeVisit.msg"
  "msg/ViewScope.msg"
  "msg/WindowSegment.msg"
  "srv/GetDependencySkeleton.srv"
  "srv/GetRoadmap.srv"
  "srv/GetUniformView.srv"
  "srv/PartialReplan.srv"
  "srv/PlanPaths.srv"
  "srv/ReassignRequest.srv"
  "srv/ReorderPassing.srv"
  "srv/SimInject.srv"
  "srv/SimQueryCapabilities.srv"
  "srv/SimStep.srv"
  "srv/TransformNode.srv"
  "srv/ValidateMap.srv"
  DEPENDENCIES std_msgs builtin_interfaces
)

ament_export_dependencies(rosidl_default_runtime)

if(BUILD_TESTING)
  # gtest 골격은 Phase 5(test-author)가 내용과 함께 만든다 (design-decision v5 build_policy).
  # 자리만 잡는다 — 빈 테스트 파일은 만들지 않는다.
endif()

ament_package()
