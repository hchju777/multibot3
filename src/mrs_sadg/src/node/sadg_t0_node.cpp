// SPDX-License-Identifier: Apache-2.0
//
// sadg_t0_node — the SADG(T0) presentation node (337 §3, process `control`).
//
// 🔴 SKELETON ONLY — NOT built by this pure-CMake CMakeLists (no ROS 2 in this
// checkout). Enabled on a ROS machine in the integration round (see the
// "ROS WRAPPER (DISABLED)" block in CMakeLists.txt). The node owns no algorithm:
// its body is adapter calls + one ExecutionTickService::run_tick per tick, then
// publishing the boundary artifacts. Kept here so the node/ layer exists and the
// wiring is legible to integration-developer.
//
// Wiring (337 §5-2, §8):
//   * subscribes  latched /plan            (mrs.discrete_plan 4.0.0)   -> PlanAdapter
//   * on IExecutionTick::on_tick(seq, now) -> ExecutionTickService::run_tick
//   * publishes   latched /execution_constraints (3.0.0) + gab-bu bits  -> ConstraintsAdapter
//   * publishes   /replan_request (7.0.0) on escalation / EVAC back-out
//   * receives    entry_events / stop_declaration (upstream)            -> apply_blocked_delta
//   * receives    realizability (Rz + rz_seq)                           -> RzAdapter
//   🔴 subscribes 0 operator channels (337c §9-3 — reading it falsifies SC-4).
//
// The compile worker (off tick, on plan change only, RG-2) runs ConstraintCompiler.
//
// This file intentionally contains no ROS includes so the repository greps clean
// for `rclcpp` in built targets; the real node body lands in the integration
// round together with the mrs_core clock/instrumentation seams.

int main()
{
    return 0;  // placeholder; the real node is built only under ament/rclcpp.
}
