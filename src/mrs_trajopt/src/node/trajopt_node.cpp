// SPDX-License-Identifier: Apache-2.0
//
// 🔴 SKELETON ONLY — NOT built by CMakeLists (brief §1: no ROS runtime here).
// This is the node/ layer (CN-1): the ONLY layer that depends on rclcpp/mrs_msgs.
// It owns one 50 ms timer and delegates to ControlTickService (one per robot).
// Filled and built on a ROS 2 machine in the integration round. Kept as source
// so the wiring (subscriptions, publications, parameter names) is reviewable now.
//
// Subscriptions (consumed boundary artifacts):
//   mrs.execution_constraints 3.0.0, mrs.discrete_plan 4.0.0,
//   mrs.segment_release 2.0.0, mrs.robot_specs 1.1.0 (latched),
//   mrs.trajectories.limits (fleet set).
// Publications (produced):
//   mrs.trajectories 2.0.0 (via TrajectoriesAdapter::to_boundary),
//   mrs.stop_declaration 5.0.0 (via StopDeclarationAdapter::to_boundary).
// Parameters (method.modules.trajopt.*, CN-7):
//   traj_replan_period_ticks, subgoal_replan_period_trajcycles,
//   round_cap_budget, backtrack_budget, peer_board_rounds_max.
//
// #include "rclcpp/rclcpp.hpp"
// #include "mrs_trajopt/core/control_tick_service.hpp"
// #include "mrs_trajopt/adapter/trajectories_adapter.hpp"
// #include "mrs_trajopt/adapter/robot_specs_adapter.hpp"
// #include "mrs_trajopt/adapter/stop_declaration_adapter.hpp"
//
// namespace mrs_trajopt::node
// {
// class TrajoptNode : public rclcpp::Node
// {
//   public:
//     TrajoptNode();                       // load params, build services, start 50 ms timer
//   private:
//     void on_control_timer();             // -> ControlTickService::run_tick per robot
//     void on_execution_constraints(mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr);
//     void on_robot_specs(mrs_msgs::msg::RobotSpecs::ConstSharedPtr);   // latched, startup
//     // ... publishers, subscriptions, ClassLoader (CN-11, outlives instances) ...
// };
// } // namespace mrs_trajopt::node
//
// TODO(integration): wire pluginlib ClassLoader for ITrajectorySearch etc.
