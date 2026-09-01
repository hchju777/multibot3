// SPDX-License-Identifier: Apache-2.0
//
// 🔴 SKELETON ONLY — NOT built by CMakeLists (brief §1: no ROS runtime here).
// This is the node/ layer (CN-1): the ONLY layer that depends on rclcpp/mrs_msgs.
// It owns the pluginlib ClassLoader (outlives the loaded IAssigner instance,
// CN-11) and delegates every mutation to service::AssignmentService::drain_once.
//
// Subscriptions (mrs.roadmap 2.0.0 [latched] · mrs.task_release 2.0.0 ·
//   mrs.goal_completion 1.0.0 [RELIABLE — loss = permanent stall, canon X15] ·
//   robot state [no schema, seam 6] · mrs.operator_command 1.0.0 x N per-robot
//   topics [TRANSIENT_LOCAL·RELIABLE·KEEP_LAST/1]) — exactly 4+N subscriptions,
// all on a single MutuallyExclusive callback group `G_main` (12a §3 M1, 341 §341-3).
// Publications: mrs.assignment 2.0.0 (latched, revision++ only when dirty) +
//   /diagnostics (1 Hz, canon A20-A22).
// Parameters (method.modules.mrta.*, CN-7, lives in mrs_bringup — NOT this
//   package's config/, CN-6): goal_queue_capacity (Q^ta), event_ring_slots,
//   pending_pool_capacity, operator_override_slots_per_robot.
//
// #include "rclcpp/rclcpp.hpp"
// #include "mrs_mrta/service/assignment_service.hpp"
// #include "mrs_mrta/service/assigner_factory.hpp"
// #include "mrs_mrta/adapter/assignment_adapter.hpp"
// #include "mrs_mrta/adapter/task_release_adapter.hpp"
// #include "mrs_mrta/adapter/goal_completion_adapter.hpp"
// #include "mrs_mrta/adapter/operator_command_adapter.hpp"
// #include "mrs_mrta/adapter/roadmap_adapter.hpp"
//
// namespace mrs_mrta::node
// {
// class MrtaNode : public rclcpp::Node
// {
//   public:
//     MrtaNode();  // load params, pluginlib ClassLoader, N per-robot operator
//                  // subscriptions from the robot roster, latched publisher
//   private:
//     void on_roadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr);       // arm only, no drain
//     void on_robot_states(/* seam 6, no schema */);                 // D04, no drain (336 §336-0
//     #6) void on_task_release(mrs_msgs::msg::TaskRelease::ConstSharedPtr); void
//     on_completion_event(mrs_msgs::msg::GoalCompletion::ConstSharedPtr); void
//     on_operator_command(mrs_msgs::msg::OperatorCommands::ConstSharedPtr);  // x N, filters verb
//     void on_diag_timer();  // 1 Hz, does not touch assignment state
//     void maybe_publish(std::optional<core::AssignmentView>);  // drain_once() result -> publish
//     // ... publisher, N subscriptions, pluginlib::ClassLoader<IAssigner> ...
// };
// } // namespace mrs_mrta::node
//
// TODO(integration): wire pluginlib ClassLoader; wire the per-robot operator
// topic set from the roster (341 §341-3); wire startup checks C-10..C-19.
