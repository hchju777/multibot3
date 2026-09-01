// SPDX-License-Identifier: Apache-2.0
//
// switch_selector_node — the order-search action server (337 §3, process
// `sadg_t1`, MultiThreadedExecutor(2)).
//
// 🔴 47차 통합(P2) — BLOCKED, minimal wiring only. This node's entire job is
// to serve a `Reorder` action from `sadg_t0_node` (see file doc history), but
// `350_msg_idl_decisions.md` §5-2 (D3-b) found the Reorder channel has ZERO
// boundary-schema backing (`x-ros-msg` never points at it across all 15
// schemas) and left its transport as [결정 부재] — explicitly not this
// developer's call to invent (that decision belongs to `system-architect` +
// `integration-developer` together, per D3-b's own text). This round's brief
// (`352`) is more explicit still: "Reorder.action을 만들지 마라". So there is
// no legal channel to serve this action THROUGH this round.
//
// What this node does instead: subscribes /discrete_plan (its own
// PlanAdapter, matching the original skeleton comment) so the executable is a
// real, spinning ROS 2 node rather than an empty placeholder — but it serves
// no action and commits nothing (E1 unchanged: only sadg_t0_node ever
// commits/publishes execution_constraints).

#include <memory>

#include "mrs_msgs/msg/discrete_plan.hpp"
#include "mrs_sadg/adapter/plan_adapter.hpp"
#include "mrs_sadg/core/status.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_sadg::node
{

class SwitchSelectorNode : public rclcpp::Node
{
public:
    SwitchSelectorNode() : rclcpp::Node("switch_selector_node")
    {
        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        plan_sub_ = this->create_subscription<mrs_msgs::msg::DiscretePlan>(
            "/discrete_plan",
            latched_qos,
            [this](mrs_msgs::msg::DiscretePlan::ConstSharedPtr msg)
            {
                this->OnPlan(msg);
            });

        RCLCPP_WARN(this->get_logger(),
                    "switch_selector_node: Reorder action NOT served this round — "
                    "no boundary schema exists for it (350 D3-b, 352 brief). See "
                    "21_integration.md.");
    }

private:
    void OnPlan(mrs_msgs::msg::DiscretePlan::ConstSharedPtr msg)
    {
        adapter::BoundaryDiscretePlan boundary;
        boundary.schema = msg->schema;
        boundary.schema_version = msg->schema_version;
        boundary.instance_id = msg->instance_id;
        boundary.plan_revision = msg->plan_revision;
        // Full field copy is sadg_t0_node's job (it is the one that compiles
        // and publishes) — this node only keeps its own envelope-checked
        // snapshot alive for when the Reorder channel lands.
        core::PlanView view;
        adapter::PlanAdapter::from_boundary(boundary, view);
    }

    rclcpp::Subscription<mrs_msgs::msg::DiscretePlan>::SharedPtr plan_sub_;
};

}  // namespace mrs_sadg::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<mrs_sadg::node::SwitchSelectorNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
