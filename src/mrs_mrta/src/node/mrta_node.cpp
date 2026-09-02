// SPDX-License-Identifier: Apache-2.0
//
// node/ layer (CN-1/CN-23 Presentation) — the only layer in this package that
// depends on rclcpp/mrs_msgs/pluginlib. Delegates every mutation to
// service::AssignmentService::drain_once (see file doc there).
//
// 🔴 Plugin loading note (47차 통합 P2): `mrs_mrta` DOES have a `plugins.xml`
// and DOES depend on `pluginlib` (package.xml), but this node uses
// `service::AssignerFactory` — a plain compile-time registry — rather than a
// real `pluginlib::ClassLoader<core::IAssigner>`. Reason: `src/plugins/
// assigner_impls.cpp` has zero `PLUGINLIB_EXPORT_CLASS` registration macro
// calls (grep-confirmed), and that file is outside this round's write scope
// (`347_arch_integration_delta.md` §9-3 — plugins/ algorithm code untouched).
// A `ClassLoader::createUniqueInstance` call would compile but throw
// `pluginlib::CreateClassException` at node startup — a real, observable
// failure, not a silent one. `AssignerFactory`'s own file doc anticipated
// exactly this fallback ("stands in for [pluginlib]... swap textually at
// integration time"). Reported to `module-developer`@mrta in
// `21_integration.md` as a debt, not fixed here.

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_mrta/adapter/assignment_adapter.hpp"
#include "mrs_mrta/adapter/goal_completion_adapter.hpp"
#include "mrs_mrta/adapter/operator_command_adapter.hpp"
#include "mrs_mrta/adapter/roadmap_adapter.hpp"
#include "mrs_mrta/adapter/task_release_adapter.hpp"
#include "mrs_mrta/core/i_assigner.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/node/core_distance_oracle.hpp"
#include "mrs_mrta/node/deferred_release_queue.hpp"
#include "mrs_mrta/plugins/assigner_impls.hpp"
#include "mrs_mrta/service/assigner_factory.hpp"
#include "mrs_mrta/service/assignment_service.hpp"
#include "mrs_msgs/msg/assignment.hpp"
#include "mrs_msgs/msg/goal_completion.hpp"
#include "mrs_msgs/msg/operator_command.hpp"
#include "mrs_msgs/msg/operator_commands.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/task_release.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_mrta::node
{

/// @brief `mrta_node` — 4 subscriptions (roadmap/task_release/goal_completion,
/// latched or reliable per topic) + N per-robot operator_command subscriptions
/// (12a §3 M1), 1 latched publisher (mrs.assignment).
class MrtaNode : public rclcpp::Node
{
public:
    MrtaNode() : rclcpp::Node("mrta_node")
    {
        // Parameters (method.modules.mrta.*, CN-7 — belongs in mrs_bringup/config,
        // NOT this package's config/, CN-6). mrs_bringup does not exist yet in
        // this round (P3, U47-1 out of scope) so these are declared here as
        // plain ROS parameters — a stand-in a launch file will fill later.
        // 🔴 F47-1 (코드 게이트 22§6): 빈 `{}`는 이 오버로드의 두 번째 인자
        // (`ParameterValue`/`ParameterDescriptor`) 사이에서 모호하게 해석돼
        // `ParameterDescriptor_(MessageInitialization)`의 explicit 기본 생성자를
        // 거치는 변환 경고를 낸다. 원인은 ROS 헤더가 아니라 이 호출 형태였다 —
        // 명시적으로 타입을 준 빈 벡터를 쓰면 그 모호성이 없어진다.
        const auto robots_param =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});
        // 🔴 [결정, integration-developer, 367_pipeline_inputs.md] — `initial_
        // vertices` (parallel to `robots`, same order) seeds `robot_vertex[]`
        // at startup. This is NOT the "로봇 상태(계약 없음, 이음매 ⑥)" live
        // channel `336_mrta_canon_p2.md` §17 documents (no boundary contract
        // exists for that this round, and this round forbids adding one) — it
        // is a one-time, config-supplied stand-in, the same category as the
        // already-established `robots`/`instance_id` run-instance parameters
        // (node/ skeleton comment above: "not algorithm parameters"). Without
        // it every robot's `robot_vertex[]` stays at its default `""`
        // (`types.cpp:41`), which is not a roadmap vertex — every task is
        // then judged unreachable and pooled (367_pipeline_inputs.md §3
        // measured this: `/assignment` published with all 6 robots' `goals:
        // []`, all 8 tasks in `unassigned_tasks`, downstream mapf_node then
        // fails even its safety-stop fallback's acyclicity self-check because
        // 6 robots collide on the same non-vertex `""`).
        const auto initial_vertices =
            this->declare_parameter<std::vector<std::string>>("initial_vertices",
                                                              std::vector<std::string>{});
        robots_ = robots_param;
        std::sort(robots_.begin(), robots_.end());
        const std::string policy_key =
            this->declare_parameter<std::string>("method.modules.mrta.policy",
                                                 "mrs_mrta/GreedyTailAppend");
        const auto goal_queue_capacity_ta = static_cast<std::size_t>(
            this->declare_parameter<int>("method.modules.mrta.goal_queue_capacity", 4));
        const auto event_ring_slots = static_cast<std::size_t>(
            this->declare_parameter<int>("method.modules.mrta.event_ring_slots", 64));
        instance_id_ = this->declare_parameter<std::string>("instance_id", "");

        if (robots_.empty())
        {
            // Startup path only (CN-15) — main() below turns this into exit code 1.
            throw mrs_core::ContractViolation(
                "mrta_node: 'robots' parameter is empty — cannot fix a roster");
        }

        service::AssignerFactory factory;
        factory.register_policy("mrs_mrta/GreedyTailAppend",
                                []
                                {
                                    return std::make_unique<plugins::GreedyTailAppend>();
                                });
        factory.register_policy("mrs_mrta/FaultyOverflow",
                                []
                                {
                                    return std::make_unique<plugins::FaultyOverflow>();
                                });
        assigner_ = factory.create(policy_key);  // throws ContractViolation if unknown (CN-15).

        core::AssignerContext ctx;
        ctx.robot_count = robots_.size();
        ctx.goal_queue_capacity_ta = goal_queue_capacity_ta;
        // Shared distance table is mrs_core's (347_arch_integration_delta.md
        // §3-1 item 6). 🔴 That deferral's own reversal condition ("계획기
        // 자문 선검사가 구현되면 즉시 올린다") fired — a consumer exists now
        // (this node) — so `367_pipeline_inputs.md` 웨이브 1-A ③ builds it.
        // `distance_oracle_` is a member so this pointer stays valid for the
        // node's whole lifetime; its *table* is still empty at this point
        // (configure() runs before any topic has been received) and gets
        // populated later, in OnRoadmap() — see that function's comment for
        // why `ctx.distances != nullptr` here does not require the table to
        // already hold data.
        ctx.distances = &distance_oracle_;
        if (!mrs_core::ok(assigner_->configure(ctx)))
        {
            throw mrs_core::ContractViolation("mrta_node: assigner_->configure() failed");
        }

        service_ = std::make_unique<service::AssignmentService>(robots_,
                                                                goal_queue_capacity_ta,
                                                                event_ring_slots,
                                                                *assigner_,
                                                                nullptr);

        if (!initial_vertices.empty())
        {
            if (initial_vertices.size() != robots_param.size())
            {
                // Startup path (CN-15) — a malformed pairing is a config bug, not
                // a runtime condition to degrade past.
                throw mrs_core::ContractViolation("mrta_node: 'initial_vertices' size (" +
                                                  std::to_string(initial_vertices.size()) +
                                                  ") does not match 'robots' size (" +
                                                  std::to_string(robots_param.size()) + ")");
            }
            for (std::size_t i = 0; i < robots_param.size(); ++i)
            {
                service_->set_robot_vertex(robots_param[i], initial_vertices[i]);
            }
        }

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        assignment_pub_ =
            this->create_publisher<mrs_msgs::msg::Assignment>("/assignment", latched_qos);

        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            latched_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                this->OnRoadmap(msg);
            });

        task_release_sub_ = this->create_subscription<mrs_msgs::msg::TaskRelease>(
            "/task_release",
            rclcpp::QoS(10).reliable(),
            [this](mrs_msgs::msg::TaskRelease::ConstSharedPtr msg)
            {
                this->OnTaskRelease(msg);
            });

        // RELIABLE — loss = permanent stall (canon X15, node/ skeleton comment).
        rclcpp::QoS completion_qos(rclcpp::KeepLast(robots_.size() * (goal_queue_capacity_ta + 1)));
        completion_qos.reliable();
        goal_completion_sub_ = this->create_subscription<mrs_msgs::msg::GoalCompletion>(
            "/goal_completion",
            completion_qos,
            [this](mrs_msgs::msg::GoalCompletion::ConstSharedPtr msg)
            {
                this->OnGoalCompletion(msg);
            });

        // N per-robot operator_command topics, TRANSIENT_LOCAL·RELIABLE·KEEP_LAST/1
        // (node/ skeleton comment). Topic naming ("/{robot}/operator_command") is
        // an integration-layer choice, not fixed by the contract — [결정], not
        // yet cross-checked against mrs_sim's publisher side (P4, out of scope).
        operator_subs_.reserve(robots_.size());
        for (const auto& robot : robots_)
        {
            operator_subs_.push_back(this->create_subscription<mrs_msgs::msg::OperatorCommands>(
                "/" + robot + "/operator_command",
                latched_qos,
                [this](mrs_msgs::msg::OperatorCommands::ConstSharedPtr msg)
                {
                    this->OnOperatorCommand(msg);
                }));
        }
    }

private:
    void OnRoadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
    {
        // Arm only — no drain (skeleton comment §336-0 #6-adjacent: roadmap never
        // wakes assignment). Only node_ids/endpoints are read (12a §4) by the
        // adapter path; the distance table below reads edges/lengths too, but
        // that is this node's own concern (ports::IDistanceOracle), not a
        // widening of what the adapter passes into core/.
        adapter::BoundaryRoadmap boundary;
        boundary.instance_id = msg->instance_id;
        boundary.node_ids.reserve(msg->nodes.size());
        for (const auto& n : msg->nodes)
        {
            boundary.node_ids.push_back(n.id);
        }
        boundary.endpoints = msg->endpoints;

        core::RoadmapView view;
        if (!mrs_core::ok(adapter::RoadmapAdapter::from_boundary(boundary, view)))
        {
            RCLCPP_ERROR(this->get_logger(), "mrta_node: roadmap adapter rejected /roadmap");
            return;
        }
        roadmap_ = std::move(view);

        // Populate the distance table now that the graph is known. Until this
        // runs, ctx.distances is a valid-but-empty oracle (IsBuilt() == false,
        // every query returns std::nullopt) — see the ctx.distances comment in
        // the constructor. 367_pipeline_inputs.md flagged that a task_release
        // arriving before this point would find every candidate robot
        // unreachable and pool the task permanently; `371` measured that race
        // firing in practice and this function's second half (below) is the
        // fix — see `OnTaskRelease`'s file-doc comment for the full account.
        distance_oracle_.Build(*msg);

        // 371 fix, part 2/2: replay whatever `OnTaskRelease` deferred while
        // the table above was still empty, in the order they were received
        // (== release_index order for this race window, since
        // `task_release_publisher` never reorders its source array). Each
        // replayed release runs through the exact same
        // `ProcessTaskRelease`/`MaybePublish` path a same-timed release would
        // have taken had the table already been built — no assigner logic
        // changes, only the arrival-order race is removed.
        if (!deferred_task_releases_.empty())
        {
            for (const auto& deferred_msg : deferred_task_releases_.drain())
            {
                ProcessTaskRelease(deferred_msg);
                MaybePublish();
            }
        }
    }

    void OnTaskRelease(mrs_msgs::msg::TaskRelease::ConstSharedPtr msg)
    {
        // 371 — "배정 비결정성" fix. `/roadmap` (latched) and `/task_release`
        // (volatile stream) are two independent topics with no cross-topic
        // delivery-order guarantee. `task_release_publisher` starts its timer
        // as soon as it sees a subscriber on `/task_release` — that has
        // nothing to do with whether *this* node has already processed its
        // own `/roadmap` message yet. Empirically (5-run smoke, `371` §2)
        // release_index 0 arrives before `distance_oracle_.IsBuilt()` in a
        // non-trivial fraction of runs; every later release (100 ms apart)
        // always finds the table already built. Before this fix, an
        // un-built table made `dist_to_endpoint` return `std::nullopt` for
        // every candidate robot, which `pick_candidate` cannot distinguish
        // from genuine unreachability — release_index 0's task was pooled
        // and, since nothing else ever completes in that short window,
        // stayed pooled forever, while release_index 1's task took the slot
        // release_index 0 would otherwise have won. Same two boundary inputs
        // (`/roadmap` contents, the `/task_release` stream), two different
        // `/assignment` outputs — a `CN-18` violation (`46_convention_canon.md`
        // "재현성은 시계·난수·경계 입력 셋에만 걸려 있다"): the discriminating
        // variable was wall-clock arrival order, not boundary input content.
        //
        // Fix: a release that arrives while the distance table is not yet
        // built is *deferred* (not ingested, not pooled) until `/roadmap` is
        // received and the table is built — see `OnRoadmap`'s replay below.
        // This is event-driven (the wake-up is the `/roadmap` message
        // arriving, not a timer or sleep) and does not change the assigner:
        // once processed, the exact same `pick_candidate` / 5-tier tie-break
        // runs on the exact same event, now with a real (never-race) distance
        // table. Deferred releases are replayed in the order they were
        // received, which for this race window is release_index order
        // (`task_release_publisher` never reorders — README "task_release.json").
        if (!distance_oracle_.IsBuilt())
        {
            deferred_task_releases_.push(msg);
            return;
        }
        ProcessTaskRelease(msg);
        MaybePublish();
    }

    /// @brief Adapter + ingest for one `/task_release` message. Factored out
    /// of `OnTaskRelease` so `OnRoadmap`'s deferred-release replay (see the
    /// 371 fix note in `OnTaskRelease`) can reuse the identical conversion
    /// path instead of duplicating it.
    /// @param msg The boundary task-release message.
    void ProcessTaskRelease(const mrs_msgs::msg::TaskRelease::ConstSharedPtr& msg)
    {
        adapter::BoundaryTaskRelease boundary;
        boundary.instance_id = msg->instance_id;
        boundary.release_index = msg->release_index;
        boundary.task = msg->task;
        boundary.goals.reserve(msg->goals.size());
        for (const auto& g : msg->goals)
        {
            boundary.goals.push_back({g.goal_id, g.location});
        }

        core::ReleaseEvent ev;
        if (!mrs_core::ok(adapter::TaskReleaseAdapter::from_boundary(boundary, ev)))
        {
            RCLCPP_ERROR(this->get_logger(), "mrta_node: task_release adapter rejected input");
            return;
        }
        if (!mrs_core::ok(service_->ingest_release(std::move(ev))))
        {
            RCLCPP_WARN(this->get_logger(), "mrta_node: event ring full, release dropped");
        }
    }

    void OnGoalCompletion(mrs_msgs::msg::GoalCompletion::ConstSharedPtr msg)
    {
        adapter::BoundaryGoalCompletion boundary;
        boundary.instance_id = msg->instance_id;
        boundary.seq = msg->seq;
        boundary.robot = msg->robot;
        boundary.goal_id = msg->goal_id;
        boundary.revision = msg->revision;

        core::CompletionEvent ev;
        adapter::GoalCompletionAdapter::from_boundary(boundary, ev);  // always kOk.
        if (!mrs_core::ok(service_->ingest_completion(std::move(ev))))
        {
            RCLCPP_WARN(this->get_logger(), "mrta_node: event ring full, completion dropped");
        }
        MaybePublish();
    }

    void OnOperatorCommand(mrs_msgs::msg::OperatorCommands::ConstSharedPtr msg)
    {
        for (const auto& cmd : msg->commands)
        {
            // Filter to goal_override only — this module's only concern
            // (341_arch_mrta_addendum.md §341-4, OperatorCommandAdapter file doc).
            if (cmd.verb != mrs_msgs::msg::OperatorCommand::VERB_GOAL_OVERRIDE)
            {
                continue;
            }
            adapter::BoundaryOperatorCommand boundary;
            boundary.seq = cmd.seq;
            boundary.robot = cmd.robot;
            const auto verb_str = mrs_core_msgs::OperatorVerbToString(cmd.verb);
            boundary.verb = verb_str.ok() ? verb_str.value() : std::string();
            boundary.active = cmd.active;
            boundary.goal_location = cmd.goal_location;

            core::OperatorEvent ev;
            if (!mrs_core::ok(adapter::OperatorCommandAdapter::from_boundary(boundary, ev)))
            {
                RCLCPP_WARN(this->get_logger(),
                            "mrta_node: operator_command adapter rejected input");
                continue;
            }
            if (!mrs_core::ok(service_->ingest_operator(std::move(ev))))
            {
                RCLCPP_WARN(this->get_logger(),
                            "mrta_node: event ring full, operator event dropped");
            }
        }
        MaybePublish();
    }

    void MaybePublish()
    {
        const auto view = service_->drain_once();
        if (!view.has_value())
        {
            return;  // No observable change (canon A32) — no-op publish suppressed.
        }

        adapter::BoundaryAssignment boundary;
        adapter::AssignmentAdapter::to_boundary(*view, boundary);

        mrs_msgs::msg::Assignment out;
        out.schema = "mrs.assignment";
        out.schema_version = "2.0.0";
        out.instance_id = instance_id_;
        out.revision = boundary.revision;
        out.assignments.reserve(boundary.assignments.size());
        for (const auto& a : boundary.assignments)
        {
            mrs_msgs::msg::RobotAssignment ra;
            ra.robot = a.robot;
            ra.start = a.start;
            ra.goals.reserve(a.goals.size());
            for (const auto& g : a.goals)
            {
                mrs_msgs::msg::Goal gm;
                gm.goal_id = g.goal_id;
                gm.task = g.task;
                gm.location = g.location;
                ra.goals.push_back(gm);
            }
            out.assignments.push_back(ra);
        }
        out.unassigned_tasks = boundary.unassigned_tasks;
        assignment_pub_->publish(out);
    }

    std::vector<std::string> robots_;
    std::string instance_id_;
    std::optional<core::RoadmapView> roadmap_;
    node::CoreDistanceOracle distance_oracle_;  ///< ctx.distances target (see constructor).
    /// @brief `/task_release` messages received before `distance_oracle_` was
    /// built, held for `OnRoadmap` to replay (371 fix — see `OnTaskRelease`).
    node::DeferredQueue<mrs_msgs::msg::TaskRelease::ConstSharedPtr> deferred_task_releases_;
    std::unique_ptr<core::IAssigner> assigner_;
    std::unique_ptr<service::AssignmentService> service_;

    rclcpp::Publisher<mrs_msgs::msg::Assignment>::SharedPtr assignment_pub_;
    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::Subscription<mrs_msgs::msg::TaskRelease>::SharedPtr task_release_sub_;
    rclcpp::Subscription<mrs_msgs::msg::GoalCompletion>::SharedPtr goal_completion_sub_;
    std::vector<rclcpp::Subscription<mrs_msgs::msg::OperatorCommands>::SharedPtr> operator_subs_;
};

}  // namespace mrs_mrta::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_mrta::node::MrtaNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        // CN-15: startup-path violation -> exit 1, no fallback.
        RCLCPP_FATAL(rclcpp::get_logger("mrta_node"), "startup contract violation: %s", cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
