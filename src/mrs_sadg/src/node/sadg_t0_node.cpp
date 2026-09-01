// SPDX-License-Identifier: Apache-2.0
//
// sadg_t0_node — the SADG(T0) presentation node (337 §3, process `control`).
//
// 🔴 47차 통합(P2) — SCOPED-DOWN WIRING. This node wires only the plan-change
// path: /discrete_plan (latched, in) -> PlanAdapter -> ConstraintCompiler ->
// ConstraintsAdapter -> /execution_constraints (latched, out). It does NOT
// wire service::ExecutionTickService's per-tick orchestration (release bits,
// escalation rungs, EVAC detection, the /replan_request action client) —
// that needs a clock seam (mrs_core::ISteadyClock), a deficit-tracking
// component, and a WcInput snapshot builder, none of which exist yet and
// which are algorithm-adjacent enough that inventing them here would be
// "통합이 알고리즘을 만드는" the exact thing this round forbids. Reported as
// a gap in `21_integration.md`, not silently glossed over.
//
// 🔴 Also NOT wired: the Reorder channel to switch_selector_node. `350
// §5-2` (D3-b) left its transport as [결정 부재] and this round's brief
// explicitly says "Reorder.action을 만들지 마라" (no경계 스키마 for it) — so
// there is nothing legal to wire it through this round.

#include <cstdint>
#include <memory>
#include <string>

#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_msgs/msg/dependency.hpp"
#include "mrs_msgs/msg/discrete_plan.hpp"
#include "mrs_msgs/msg/execution_constraints.hpp"
#include "mrs_msgs/msg/segment.hpp"
#include "mrs_msgs/msg/switch_group.hpp"
#include "mrs_sadg/adapter/constraints_adapter.hpp"
#include "mrs_sadg/adapter/plan_adapter.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_sadg::node
{

class SadgT0Node : public rclcpp::Node
{
public:
    SadgT0Node() : rclcpp::Node("sadg_t0_node"), compiler_(generator_)
    {
        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        constraints_pub_ =
            this->create_publisher<mrs_msgs::msg::ExecutionConstraints>("/execution_constraints",
                                                                        latched_qos);

        plan_sub_ = this->create_subscription<mrs_msgs::msg::DiscretePlan>(
            "/discrete_plan",
            latched_qos,
            [this](mrs_msgs::msg::DiscretePlan::ConstSharedPtr msg)
            {
                this->OnPlan(msg);
            });
    }

private:
    void OnPlan(mrs_msgs::msg::DiscretePlan::ConstSharedPtr msg)
    {
        adapter::BoundaryDiscretePlan boundary;
        boundary.schema = msg->schema;
        boundary.schema_version = msg->schema_version;
        boundary.instance_id = msg->instance_id;
        boundary.plan_revision = msg->plan_revision;
        boundary.plans.reserve(msg->plans.size());
        for (const auto& p : msg->plans)
        {
            adapter::BoundaryRobotPlan rp;
            rp.robot = p.robot;
            const auto term = mrs_core_msgs::PlanTerminalToString(p.terminal);
            rp.terminal = term.ok() ? term.value() : std::string();
            rp.steps.reserve(p.steps.size());
            for (const auto& s : p.steps)
            {
                rp.steps.push_back({s.index, s.location});
            }
            boundary.plans.push_back(std::move(rp));
        }
        boundary.visit_order.reserve(msg->visit_order.size());
        for (const auto& v : msg->visit_order)
        {
            adapter::BoundaryVisitOrderEntry entry;
            entry.location = v.location;
            for (const auto& it : v.sequence)
            {
                entry.sequence.push_back({it.robot, it.index});
            }
            boundary.visit_order.push_back(std::move(entry));
        }

        core::PlanView plan_view;
        if (!mrs_core::ok(adapter::PlanAdapter::from_boundary(boundary, plan_view)))
        {
            RCLCPP_ERROR(this->get_logger(), "sadg_t0_node: plan adapter rejected /discrete_plan");
            return;
        }

        core::ExecutionGraph graph;
        core::CompileReport report;
        if (!mrs_core::ok(compiler_.compile(plan_view, graph, report)))
        {
            RCLCPP_ERROR(this->get_logger(),
                         "sadg_t0_node: ConstraintCompiler rejected plan_revision=%lu (outcome=%d)",
                         static_cast<unsigned long>(msg->plan_revision),
                         static_cast<int>(report.outcome));
            return;
        }

        adapter::BoundaryExecutionConstraints out_boundary;
        adapter::ConstraintsAdapter::to_boundary(graph, out_boundary);
        out_boundary.commit_seq = commit_seq_++;

        mrs_msgs::msg::ExecutionConstraints out;
        out.schema = out_boundary.schema;
        out.schema_version = out_boundary.schema_version;
        out.instance_id = out_boundary.instance_id;
        out.commit_seq = out_boundary.commit_seq;
        out.segments.reserve(out_boundary.segments.size());
        for (const auto& s : out_boundary.segments)
        {
            mrs_msgs::msg::Segment sm;
            sm.id = s.id;
            sm.robot = s.robot;
            sm.from_id = s.from;
            sm.to_id = s.to;
            out.segments.push_back(sm);
        }
        out.dependencies.reserve(out_boundary.dependencies.size());
        for (const auto& d : out_boundary.dependencies)
        {
            out.dependencies.push_back(ToMsg(d));
        }
        out.switch_groups.reserve(out_boundary.switch_groups.size());
        for (const auto& g : out_boundary.switch_groups)
        {
            mrs_msgs::msg::SwitchGroup gm;
            gm.id = g.id;
            gm.location = g.location;
            for (const auto& a : g.alternatives)
            {
                mrs_msgs::msg::Alternative am;
                am.id = a.id;
                for (const auto& d : a.dependencies)
                {
                    am.dependencies.push_back(ToMsg(d));
                }
                gm.alternatives.push_back(am);
            }
            out.switch_groups.push_back(gm);
        }
        constraints_pub_->publish(out);
    }

    static mrs_msgs::msg::Dependency ToMsg(const adapter::BoundaryDependency& d)
    {
        mrs_msgs::msg::Dependency dm;
        dm.from_id = d.from;
        dm.to_id = d.to;
        const auto type = mrs_core_msgs::DependencyTypeFromString(d.type);
        dm.type = type.ok() ? type.value() : mrs_msgs::msg::Dependency::DEP_UNKNOWN;
        return dm;
    }

    std::uint64_t commit_seq_ = 0;
    plugins::AltHeadLift generator_;
    core::ConstraintCompiler compiler_;

    rclcpp::Publisher<mrs_msgs::msg::ExecutionConstraints>::SharedPtr constraints_pub_;
    rclcpp::Subscription<mrs_msgs::msg::DiscretePlan>::SharedPtr plan_sub_;
};

}  // namespace mrs_sadg::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<mrs_sadg::node::SadgT0Node>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
