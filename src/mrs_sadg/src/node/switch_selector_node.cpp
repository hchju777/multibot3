// SPDX-License-Identifier: Apache-2.0
//
// switch_selector_node — the order-search action server (337 §3, process
// `sadg_t1`, MultiThreadedExecutor(2)).
//
// 🔴 SKELETON ONLY — NOT built by this pure-CMake CMakeLists. Enabled on a ROS
// machine in the integration round.
//
// Wiring (337 §5-2):
//   * subscribes latched /plan (mrs.discrete_plan 4.0.0) -> its own PlanAdapter
//   * serves a Reorder action: on goal, runs OrderSearchService::run against a
//     const graph snapshot + Rz + node budget; returns a CommitProposal.
//   * honours cancellation inside the search loop (RS-5).
//   * NEVER commits or publishes execution_constraints (E1) — the T0 node does.
//
// The one in-flight goal invariant (12c §A-2) is what makes the graph snapshot
// lock-free: no two threads mutate it concurrently.

int main()
{
    return 0;  // placeholder; the real node is built only under ament/rclcpp.
}
