// SPDX-License-Identifier: Apache-2.0
//
// trajopt_node — onboard node/ layer (CN-1/CN-23 Presentation).
//
// 🔴 47차 통합(P2) — SCOPED-DOWN WIRING, and the scope boundary here is not
// arbitrary: the full 3-clock `ControlTickService::run_tick` loop needs a
// live odometry feed (`TickInput::pose`/`v`) that only `mrs_sim` produces
// (`347_arch_integration_delta.md` §4-2 mermaid: `ODO -->|/odom_batch| CTRL`),
// and `mrs_sim` is P4 — explicitly OUT of this round (U47-1, `348`). Running
// the tick loop against a fabricated pose would not be "배선"; it would be
// inventing simulation data inside node/, which is a module/architecture
// decision, not an integration one.
//
// What IS wired, and is real, testable node-layer glue:
//   - /robot_specs (latched, in) -> RobotSpecsAdapter -> core::RobotLimits[]
//   - fleet limits + trajopt config periods/budgets as ROS parameters
//     (method.modules.trajopt.*, CN-7 — mrs_bringup/config does not exist
//     yet, P3 out of scope, so these are declared here as a stand-in a
//     launch file will fill later, same pattern as mrta_node/mapf_node).
//   - core::StartupChecks::run(specs, fleet, cfg) — SU00-SU13, a REAL startup
//     check straight from this module's own header, run once robot_specs
//     latches in. A ContractViolation here is exit code 1 (CN-15), not a
//     silent skip.
// NOT wired: execution_constraints/discrete_plan/segment_release subscriptions,
// the 50 ms control timer, ControlTickService, /cmd_vel, /trajectories,
// /stop_declaration publication — all of it needs the odom feed above.

#include <memory>
#include <string>
#include <vector>

#include "mrs_msgs/msg/robot_specs.hpp"
#include "mrs_trajopt/adapter/robot_specs_adapter.hpp"
#include "mrs_trajopt/core/startup_checks.hpp"
#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_trajopt::node
{

class TrajoptNode : public rclcpp::Node
{
public:
    TrajoptNode() : rclcpp::Node("trajopt_node")
    {
        // Fleet-folded limits + config (method.modules.trajopt.*, CN-6/CN-7 —
        // belongs in mrs_bringup/config, not this package's config/; declared
        // here as a stand-in, P3 out of scope this round).
        fleet_.a_max = this->declare_parameter<double>("method.modules.trajopt.a_max", 0.0);
        fleet_.j_max = this->declare_parameter<double>("method.modules.trajopt.j_max", 0.0);
        fleet_.omega_max_radps =
            this->declare_parameter<double>("method.modules.trajopt.omega_max_radps", 0.0);
        fleet_.yaw_accel_max_radps2 =
            this->declare_parameter<double>("method.modules.trajopt.yaw_accel_max_radps2", 0.0);
        fleet_.min_separation_m =
            this->declare_parameter<double>("method.modules.trajopt.min_separation_m", 0.0);

        cfg_.traj_replan_period_ticks =
            this->declare_parameter<int>("method.modules.trajopt.traj_replan_period_ticks", 0);
        cfg_.subgoal_replan_period_trajcycles =
            this->declare_parameter<int>("method.modules.trajopt.subgoal_replan_period_trajcycles",
                                         0);
        cfg_.round_cap_budget =
            this->declare_parameter<int>("method.modules.trajopt.round_cap_budget", 0);
        cfg_.backtrack_budget =
            this->declare_parameter<int>("method.modules.trajopt.backtrack_budget", 0);
        cfg_.peer_board_rounds_max =
            this->declare_parameter<int>("method.modules.trajopt.peer_board_rounds_max", 0);
        // SU06 scans this list for a forbidden independent eta_slf/self_trigger*
        // key — every method.modules.trajopt.* key this node declared, so far.
        cfg_.config_key_names = {"method.modules.trajopt.a_max",
                                 "method.modules.trajopt.j_max",
                                 "method.modules.trajopt.omega_max_radps",
                                 "method.modules.trajopt.yaw_accel_max_radps2",
                                 "method.modules.trajopt.min_separation_m",
                                 "method.modules.trajopt.traj_replan_period_ticks",
                                 "method.modules.trajopt.subgoal_replan_period_trajcycles",
                                 "method.modules.trajopt.round_cap_budget",
                                 "method.modules.trajopt.backtrack_budget",
                                 "method.modules.trajopt.peer_board_rounds_max"};

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        robot_specs_sub_ = this->create_subscription<mrs_msgs::msg::RobotSpecs>(
            "/robot_specs",
            latched_qos,
            [this](mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
            {
                this->OnRobotSpecs(msg);
            });
    }

private:
    void OnRobotSpecs(mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
    {
        adapter::BoundaryRobotSpecs boundary;
        boundary.instance_id = msg->instance_id;
        boundary.robots.reserve(msg->robots.size());
        for (const auto& r : msg->robots)
        {
            boundary.robots.push_back({r.robot,
                                       r.avg_traversal_speed_mps,
                                       r.v_max,
                                       r.brake_decel_min_mps2,
                                       r.curvature_max_invm,
                                       r.reverse_motion_allowed,
                                       r.circumradius_m});
        }

        std::vector<core::RobotLimits> specs;
        adapter::RobotSpecsAdapter::from_boundary(boundary, specs);  // always kOk.

        // Throws ContractViolation (CN-15) on any SU00-SU13 refusal — startup
        // path only. main() below turns that into exit code 1.
        core::StartupChecks::run(specs, fleet_, cfg_);

        RCLCPP_INFO(this->get_logger(),
                    "trajopt_node: startup checks passed for %zu robot(s). Control-tick "
                    "loop NOT started this round (needs mrs_sim odom, P4 out of scope).",
                    specs.size());
    }

    core::FleetLimits fleet_;
    core::TrajoptConfig cfg_;
    rclcpp::Subscription<mrs_msgs::msg::RobotSpecs>::SharedPtr robot_specs_sub_;
};

}  // namespace mrs_trajopt::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_trajopt::node::TrajoptNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("trajopt_node"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
