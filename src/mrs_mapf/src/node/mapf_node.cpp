// SPDX-License-Identifier: Apache-2.0
//
// node/ layer (CN-1/CN-23 Presentation) — the only layer that depends on
// rclcpp/rclcpp_action/mrs_msgs. Wraps service::MapfPlanningService with:
//  - 3 latched subscriptions (/roadmap, /robot_specs, /assignment) that arm a
//    full-roster (re)plan once all three are seen, and again every time a
//    freshly received /assignment's `revision` differs from the one last
//    attempted (48차 fix — see MaybePlan()/node/replan_trigger.hpp; the
//    contract does not specify either trigger — documented as [결정] below).
//  - an rclcpp_action::Server<mrs_msgs::action::Replan> for SADG-triggered
//    replans (347§4-2 mermaid: sadg_t0 -> mapf via action Replan).
//  - 1 latched publisher (/discrete_plan).
//
// 🔴 Single fixed strategy plugin (plugins::PrioritySafeIntervalSearch) is
// instantiated directly — this package has no *Factory (CN-11, service/) and
// only one concrete IPlanningStrategy exists this round (20b known
// limitation), so there is nothing to select between yet.

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_mapf/adapter/assignment_adapter.hpp"
#include "mrs_mapf/adapter/discrete_plan_adapter.hpp"
#include "mrs_mapf/adapter/replan_request_adapter.hpp"
#include "mrs_mapf/adapter/roadmap_adapter.hpp"
#include "mrs_mapf/adapter/robot_specs_adapter.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/node/replan_trigger.hpp"
#include "mrs_mapf/plugins/priority_safe_interval_search.hpp"
#include "mrs_mapf/service/mapf_planning_service.hpp"
#include "mrs_msgs/action/replan.hpp"
#include "mrs_msgs/msg/assignment.hpp"
#include "mrs_msgs/msg/discrete_plan.hpp"
#include "mrs_msgs/msg/plan_step.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/robot_plan.hpp"
#include "mrs_msgs/msg/robot_specs.hpp"
#include "mrs_msgs/msg/visit_item.hpp"
#include "mrs_msgs/msg/visit_order.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

namespace mrs_mapf::node
{

// 48차 진단(`369_p2`) — human-readable text for a SelfCheckOutcome, log-only
// (never a boundary vocabulary; mrs_core_msgs owns the wire-facing enums).
namespace
{
const char* SelfCheckOutcomeToString(core::SelfCheckOutcome outcome)
{
    switch (outcome)
    {
        case core::SelfCheckOutcome::kOk:
            return "kOk";
        case core::SelfCheckOutcome::kStartVertexCollision:
            return "kStartVertexCollision(ⓐ)";
        case core::SelfCheckOutcome::kCycleDetected:
            return "kCycleDetected(ⓑ)";
        case core::SelfCheckOutcome::kMalformedType2Edge:
            return "kMalformedType2Edge(ⓑ)";
        case core::SelfCheckOutcome::kUntested:
            return "kUntested";
    }
    return "?";
}
}  // namespace

using ReplanAction = mrs_msgs::action::Replan;
using GoalHandleReplan = rclcpp_action::ServerGoalHandle<ReplanAction>;

class MapfNode : public rclcpp::Node
{
public:
    MapfNode() : rclcpp::Node("mapf_node"), service_(strategy_)
    {
        instance_id_ = this->declare_parameter<std::string>("instance_id", "");

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        plan_pub_ =
            this->create_publisher<mrs_msgs::msg::DiscretePlan>("/discrete_plan", latched_qos);

        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            latched_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                this->OnRoadmap(msg);
            });
        robot_specs_sub_ = this->create_subscription<mrs_msgs::msg::RobotSpecs>(
            "/robot_specs",
            latched_qos,
            [this](mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
            {
                this->OnRobotSpecs(msg);
            });
        assignment_sub_ = this->create_subscription<mrs_msgs::msg::Assignment>(
            "/assignment",
            latched_qos,
            [this](mrs_msgs::msg::Assignment::ConstSharedPtr msg)
            {
                this->OnAssignment(msg);
            });

        action_server_ = rclcpp_action::create_server<ReplanAction>(
            this,
            "replan",
            [this](const rclcpp_action::GoalUUID&,
                   std::shared_ptr<const ReplanAction::Goal>) -> rclcpp_action::GoalResponse
            {
                return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
            },
            [](const std::shared_ptr<GoalHandleReplan>) -> rclcpp_action::CancelResponse
            {
                return rclcpp_action::CancelResponse::ACCEPT;
            },
            [this](const std::shared_ptr<GoalHandleReplan> gh)
            {
                this->HandleAccepted(gh);
            });
    }

private:
    // ---- boundary-in: latched arming subscriptions ----

    void OnRoadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        adapter::BoundaryRoadmap boundary;
        boundary.instance_id = msg->instance_id;
        boundary.nodes.reserve(msg->nodes.size());
        for (const auto& n : msg->nodes)
        {
            boundary.nodes.push_back({n.id, n.x, n.y, n.clearance_m});
        }
        boundary.edges.reserve(msg->edges.size());
        for (const auto& e : msg->edges)
        {
            const auto trav = mrs_core_msgs::EdgeTraversalToString(e.traversal);
            boundary.edges.push_back({e.from_id,
                                      e.to_id,
                                      trav.ok() ? trav.value() : std::string(),
                                      e.length_m,
                                      e.width_m,
                                      static_cast<int>(e.capacity_robots),
                                      e.corridor});
        }
        boundary.endpoints = msg->endpoints;

        if (!mrs_core::ok(adapter::RoadmapAdapter::from_boundary(boundary, roadmap_)))
        {
            RCLCPP_ERROR(this->get_logger(), "mapf_node: roadmap adapter rejected /roadmap");
            return;
        }
        have_roadmap_ = true;
        MaybePlan();
    }

    void OnRobotSpecs(mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
    {
        std::lock_guard<std::mutex> lk(mutex_);
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
        if (!mrs_core::ok(adapter::RobotSpecsAdapter::from_boundary(boundary, robot_specs_)))
        {
            RCLCPP_ERROR(this->get_logger(),
                         "mapf_node: robot_specs adapter rejected /robot_specs");
            return;
        }
        have_robot_specs_ = true;
        MaybePlan();
    }

    void OnAssignment(mrs_msgs::msg::Assignment::ConstSharedPtr msg)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        adapter::BoundaryAssignment boundary;
        boundary.instance_id = msg->instance_id;
        boundary.revision = msg->revision;
        boundary.assignments.reserve(msg->assignments.size());
        for (const auto& a : msg->assignments)
        {
            adapter::BoundaryAssignmentEntry entry;
            entry.robot = a.robot;
            entry.start = a.start;
            entry.goals.reserve(a.goals.size());
            for (const auto& g : a.goals)
            {
                entry.goals.push_back({g.goal_id, g.task, g.location});
            }
            boundary.assignments.push_back(std::move(entry));
        }
        boundary.unassigned_tasks = msg->unassigned_tasks;

        if (!mrs_core::ok(adapter::AssignmentAdapter::from_boundary(boundary, assignments_)))
        {
            RCLCPP_ERROR(this->get_logger(), "mapf_node: assignment adapter rejected /assignment");
            return;
        }
        have_assignment_ = true;
        latest_assignment_revision_ = boundary.revision;
        MaybePlan();
    }

    // 🔴 [결정] The contract does not specify when MAPF plans for the first
    // time, nor when it should replan off an updated `/assignment` outside the
    // SADG-triggered `Replan` action (`assignment.schema.json`'s `revision`
    // field description: "재계획 트리거는 소비 모듈의 것이다" — the trigger belongs
    // to the consumer). This node's own policy, node-layer orchestration and
    // not an algorithm choice (the search stays inside the strategy plugin):
    // plan full-roster as soon as roadmap+robot_specs+assignment have all
    // latched in at least once, and again every time a NEWLY received
    // `/assignment` carries a `revision` different from the one this node last
    // attempted to plan for (`node/replan_trigger.hpp` — never twice for the
    // same revision, no invented cooldown/period).
    void MaybePlan()
    {
        const bool inputs_complete = have_roadmap_ && have_robot_specs_ && have_assignment_;
        if (!ShouldReplanForAssignment(inputs_complete,
                                       last_attempted_assignment_revision_,
                                       latest_assignment_revision_))
        {
            return;
        }
        last_attempted_assignment_revision_ = latest_assignment_revision_;

        core::PlanningRequest req;
        req.instance_id = instance_id_;
        req.roadmap = roadmap_;
        req.robot_specs = robot_specs_;
        req.assignments = assignments_;
        req.affected_robots.reserve(assignments_.size());
        for (const auto& a : assignments_)
        {
            req.affected_robots.push_back(a.robot);
        }
        req.request_plan_revision = 0;
        req.previous_plan = std::nullopt;
        RunAndMaybePublish(req);
    }

    // Caller must hold mutex_.
    bool RunAndMaybePublish(const core::PlanningRequest& req)
    {
        const auto result = service_.run_once(req, next_plan_revision_, instance_id_);
        // 48차 진단(`369_p2`) — surface WHY a scope/global solve() failed, even
        // on the path that still publishes (the R29 safety-stop fallback):
        // this is what turns into every robot's terminal=no_progress, 1-step
        // plan, and this log line is the only place the actual PlanFailure
        // detail (discarded by service::run_once's return value) is visible.
        if (result.scope_failure.has_value())
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: floor-scope solve() failed — robot='%s' detail='%s'",
                        result.scope_failure->robot.c_str(),
                        result.scope_failure->detail.c_str());
        }
        if (result.global_failure.has_value())
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: escalated global solve() failed — robot='%s' detail='%s'",
                        result.global_failure->robot.c_str(),
                        result.global_failure->detail.c_str());
        }
        if (result.scope_self_check.has_value())
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: floor-scope solve() succeeded but its draft failed "
                        "self-check — outcome=%s",
                        SelfCheckOutcomeToString(result.scope_self_check->outcome));
            for (const auto& msg : result.scope_self_check->malformed_edges)
            {
                RCLCPP_WARN(this->get_logger(),
                            "mapf_node:   scope malformed edge: %s",
                            msg.c_str());
            }
        }
        if (result.global_self_check.has_value())
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: escalated global solve() succeeded but its draft failed "
                        "self-check — outcome=%s",
                        SelfCheckOutcomeToString(result.global_self_check->outcome));
            for (const auto& msg : result.global_self_check->malformed_edges)
            {
                RCLCPP_WARN(this->get_logger(),
                            "mapf_node:   global malformed edge: %s",
                            msg.c_str());
            }
        }
        if (result.outcome != service::RunOutcome::kPublished)
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: run_once did not publish (outcome=%d)",
                        static_cast<int>(result.outcome));
            return false;
        }
        if (result.used_safety_stop_fallback)
        {
            RCLCPP_WARN(this->get_logger(),
                        "mapf_node: published the R29 safety-stop fallback (every robot "
                        "parked, terminal=no_progress) — see the solve-failure warnings above");
        }
        PublishDraft(result.draft);
        return true;
    }

    // Caller must hold mutex_.
    void PublishDraft(const core::DiscretePlanDraft& draft)
    {
        adapter::BoundaryDiscretePlan boundary;
        adapter::DiscretePlanAdapter::to_boundary(draft, boundary);
        last_published_ = draft;
        ++next_plan_revision_;

        mrs_msgs::msg::DiscretePlan out;
        out.schema = boundary.schema;
        out.schema_version = boundary.schema_version;
        out.instance_id = boundary.instance_id;
        out.plan_revision = boundary.plan_revision;
        out.plans.reserve(boundary.plans.size());
        for (const auto& p : boundary.plans)
        {
            mrs_msgs::msg::RobotPlan rp;
            rp.robot = p.robot;
            const auto term = mrs_core_msgs::PlanTerminalFromString(p.terminal);
            rp.terminal = term.ok() ? term.value() : mrs_msgs::msg::RobotPlan::TERMINAL_UNKNOWN;
            rp.steps.reserve(p.steps.size());
            for (const auto& s : p.steps)
            {
                mrs_msgs::msg::PlanStep ps;
                ps.index = s.index;
                ps.location = s.location;
                rp.steps.push_back(ps);
            }
            out.plans.push_back(std::move(rp));
        }
        out.visit_order.reserve(boundary.visit_order.size());
        for (const auto& v : boundary.visit_order)
        {
            mrs_msgs::msg::VisitOrder vo;
            vo.location = v.location;
            vo.sequence.reserve(v.sequence.size());
            for (const auto& it : v.sequence)
            {
                mrs_msgs::msg::VisitItem vi;
                vi.robot = it.robot;
                vi.index = it.index;
                vo.sequence.push_back(vi);
            }
            out.visit_order.push_back(std::move(vo));
        }
        plan_pub_->publish(out);
    }

    void HandleAccepted(const std::shared_ptr<GoalHandleReplan> goal_handle)
    {
        // action callbacks run on the executor thread — this package's own
        // solve is single-shot and short, so a detached worker thread (the
        // standard rclcpp_action idiom) is enough; no long-running progress
        // loop is needed (350§5-1: this action has no feedback section).
        std::thread(std::bind(&MapfNode::Execute, this, goal_handle)).detach();
    }

    void Execute(const std::shared_ptr<GoalHandleReplan> goal_handle)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        const auto goal = goal_handle->get_goal();
        auto result = std::make_shared<ReplanAction::Result>();
        result->schema = "mrs.replan_result";
        result->schema_version = "1.0.0";
        result->instance_id = goal->instance_id;
        result->blocked_hash = goal->blocked_hash;
        result->plan_revision = next_plan_revision_;

        if (!have_roadmap_ || !have_robot_specs_ || !have_assignment_)
        {
            result->outcome = ReplanAction::Result::OUTCOME_REJECTED;
            goal_handle->abort(result);
            return;
        }

        adapter::BoundaryReplanRequest boundary;
        boundary.instance_id = goal->instance_id;
        const auto reason_str = mrs_core_msgs::StopReasonToString(goal->reason);
        boundary.reason = reason_str.ok() ? reason_str.value() : std::string();
        boundary.affected_robots = goal->affected_robots;
        boundary.blocked.reserve(goal->blocked.size());
        for (const auto& e : goal->blocked)
        {
            boundary.blocked.push_back({e.from_id, e.to_id});
        }
        boundary.blocked_hash = goal->blocked_hash;
        boundary.ordering_constraints.reserve(goal->ordering_constraints.size());
        for (const auto& oc : goal->ordering_constraints)
        {
            adapter::BoundaryOrderingConstraint c;
            c.location = oc.location;
            for (const auto& it : oc.sequence)
            {
                c.sequence.push_back({it.robot, it.index});
            }
            boundary.ordering_constraints.push_back(std::move(c));
        }
        boundary.baseline.reserve(goal->baseline.size());
        for (const auto& b : goal->baseline)
        {
            boundary.baseline.push_back({b.robot, b.committed_segment_count});
        }
        boundary.plan_revision = goal->plan_revision;

        adapter::ReplanRequestFields fields;
        adapter::ReplanRequestAdapter::from_boundary(boundary, fields);  // always kOk.

        core::PlanningRequest req;
        req.instance_id = instance_id_;
        req.roadmap = roadmap_;
        req.robot_specs = robot_specs_;
        req.assignments = assignments_;
        req.affected_robots = fields.affected_robots;
        req.blocked = fields.blocked;
        req.ordering_constraints = fields.ordering_constraints;
        req.baseline = fields.baseline;
        req.request_plan_revision = fields.plan_revision;
        req.previous_plan = last_published_;

        const auto run_result = service_.run_once(req, next_plan_revision_, instance_id_);
        if (run_result.outcome != service::RunOutcome::kPublished)
        {
            result->outcome = (run_result.outcome == service::RunOutcome::kNoBaselinePlan)
                                  ? ReplanAction::Result::OUTCOME_REJECTED
                                  : ReplanAction::Result::OUTCOME_FALLBACK;
            goal_handle->abort(result);
            return;
        }

        PublishDraft(run_result.draft);

        result->outcome = run_result.escalated_to_global ? ReplanAction::Result::OUTCOME_FALLBACK
                                                         : ReplanAction::Result::OUTCOME_PLANNED;
        result->plan_revision = last_published_->plan_revision;
        goal_handle->succeed(result);
    }

    std::mutex mutex_;
    std::string instance_id_;

    bool have_roadmap_ = false;
    bool have_robot_specs_ = false;
    bool have_assignment_ = false;
    std::uint64_t latest_assignment_revision_ = 0;
    std::optional<std::uint64_t> last_attempted_assignment_revision_;

    core::Roadmap roadmap_;
    std::vector<core::RobotSpec> robot_specs_;
    std::vector<core::AssignmentEntry> assignments_;
    std::optional<core::DiscretePlanDraft> last_published_;
    std::uint64_t next_plan_revision_ = 0;

    plugins::PrioritySafeIntervalSearch strategy_;
    service::MapfPlanningService service_;

    rclcpp::Publisher<mrs_msgs::msg::DiscretePlan>::SharedPtr plan_pub_;
    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::Subscription<mrs_msgs::msg::RobotSpecs>::SharedPtr robot_specs_sub_;
    rclcpp::Subscription<mrs_msgs::msg::Assignment>::SharedPtr assignment_sub_;
    rclcpp_action::Server<ReplanAction>::SharedPtr action_server_;
};

}  // namespace mrs_mapf::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<mrs_mapf::node::MapfNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
