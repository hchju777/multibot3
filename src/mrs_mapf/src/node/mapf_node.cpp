// SPDX-License-Identifier: Apache-2.0
//
// 🔴 SKELETON ONLY — NOT built by CMakeLists (no ROS 2 runtime in this
// checkout). This is the node/ layer (CN-1): the ONLY layer that depends on
// rclcpp/mrs_msgs/pluginlib. It owns the action server + `/plan` latched
// publisher and delegates to `service::MapfPlanningService::run_once()`. Filled
// and built on a ROS 2 machine in the integration round. Kept as source so the
// wiring (topics, action name, parameters) is reviewable now — see
// `20b_impl_mapf.md` "다음 단계 전달 사항 → integration-developer".
//
// Subscriptions/inputs (consumed boundary artifacts):
//   mrs.roadmap 3.0.0 (latched, startup) · mrs.assignment 2.0.0 ·
//   mrs.replan_request 7.0.0 (action goal payload, R01 "유일한 계획 진입점") ·
//   mrs.robot_specs 1.1.0 (latched, startup — A33: only `robot`/
//   `avg_traversal_speed_mps` reach `core/`).
// Publication (produced):
//   mrs.discrete_plan 4.0.0 on `/plan`, LATCHED (§255-1-1: "정본 매체"; the
//   action result is a diagnostic-only ack per CN-17, not the plan itself).
// Parameters (method.modules.mapf.*, CN-6/CN-7 — algorithm parameters ONLY,
//   never on the boundary): plugin class key (CN-12, e.g.
//   "mrs_mapf/PrioritySafeIntervalSearch"), 전역 동기 눈금 tick_s (🔴 [값 부재] —
//   `255`§255-11-3/§255-11-5: this key's HOME is system-wide config + the
//   preregistration §1-1 row, never this module's own default; if the
//   integration launch file does not set it, `core::reserve_interval` must
//   receive `std::nullopt`, not an invented number).
//
// #include "rclcpp/rclcpp.hpp"
// #include "pluginlib/class_loader.hpp"
// #include "mrs_mapf/service/mapf_planning_service.hpp"
// #include "mrs_mapf/adapter/roadmap_adapter.hpp"
// #include "mrs_mapf/adapter/assignment_adapter.hpp"
// #include "mrs_mapf/adapter/replan_request_adapter.hpp"
// #include "mrs_mapf/adapter/robot_specs_adapter.hpp"
// #include "mrs_mapf/adapter/discrete_plan_adapter.hpp"
//
// namespace mrs_mapf::node
// {
// class MapfNode : public rclcpp::Node
// {
//   public:
//     MapfNode();  // load params, ClassLoader<IPlanningStrategy> (CN-11, outlives instances),
//                  // build MapfPlanningService, latch roadmap/robot_specs subscriptions.
//   private:
//     void on_replan_request_goal(...);   // R01 — the ONLY planning entry point (no periodic
//     timer). void on_roadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr);        // latched, startup.
//     void on_robot_specs(mrs_msgs::msg::RobotSpecs::ConstSharedPtr);  // latched, startup.
//     // Persists the last successful publish for PX09's `baseline` resolution
//     // (system-architect's "다음 스텝" — storage medium + restart durability;
//     // this module only says WHAT must be retained, not HOW — §286-13).
//     std::optional<core::DiscretePlanDraft> retained_plan_;
//     std::uint64_t plan_revision_ = 0;  // PB02 — monotonic; persistence across restarts is out of
//     this round's scope.
// };
// } // namespace mrs_mapf::node
//
// TODO(integration): wire pluginlib ClassLoader for IPlanningStrategy;
// wire the tick (전역 동기 눈금) parameter with NO module-level default value.
