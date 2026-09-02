// SPDX-License-Identifier: Apache-2.0
//
// sadg_t0_node — the SADG(T0) presentation node (337 §3, process `control`).
//
// 🔴 48차 웨이브 2-A(`_workspace/372_sadg_tick_wiring.md`) — closes the per-tick
// loop that 47차(P2) left unwired (`21_integration.md` §8 item 5, §9). What
// THIS round wires:
//   - the ExecutionTickService per-tick call order (ET00-ET36, 340§2), driven
//     by this node's OWN periodic timer (see D1 below — there is no shared
//     `mrs_core::IExecutionTick` cross-module interface for trajopt_node to
//     call through, and trajopt is out of this round's write scope).
//   - /entry_events (upstream, trajopt->sadg_t0) -> ET03/ET16 (mark_entered /
//     mark_completed) via the new EntryEventsAdapter.
//   - /stop_declaration (upstream, trajopt->sadg_t0) -> ET17
//     (apply_blocked_delta) via the new StopDeclarationsAdapter.
//   - /{robot}/segment_release (2.0.0) publication — ET33's boundary output
//     (the arrival-vertex entry permit bits), via the new ReleaseAdapter. This
//     is the FIRST externally-visible artifact this round's wiring produces
//     that changes every tick (unlike /execution_constraints, which is
//     compile-triggered, not tick-triggered) — see 372's evidence section.
//
// 🔴 Decisions made to close the loop without inventing values (reported in
// 372_sadg_tick_wiring.md §"신고한 축약", not hidden):
//   D1. Own periodic ROS timer (node-clock based, NOT create_wall_timer, so
//       use_sim_time governs it — mirrors mrs_trajopt/src/node/trajopt_node.cpp's
//       own D1-era control timer, read-only precedent). 337§0-2/§5-2 places the
//       tick call under trajopt_node's control-timer ownership via a shared
//       `mrs_core::IExecutionTick` seam, but that seam does not exist in
//       `mrs_core` today and `mrs_trajopt`/`mrs_core` are both out of this
//       round's write scope (concurrent sibling work) — so this node drives
//       its own tick instead of waiting on an interface nobody can add this
//       round. Reported to `system-architect`/`integration-developer` (372§
//       "다음 단계 전달 사항"), not silently substituted.
//   D2. `control_period_ms` declared with a baked default of 50 — mirrors
//       trajopt_node's own D7 exactly: this is the fleet-wide 50ms invariant
//       (multibot3/CLAUDE.md invariant 5), not a swappable algorithm
//       parameter, so CN-6/CN-7 (method.modules.sadg.*) do not govern it.
//   D3. Realizability (/realizability, mrs.realizability 1.0.0) is NOT
//       subscribed this round. `TickInput.rz`/`rz_present`/`t1_present` only
//       have any effect inside the `if (in.t1_present && ...)` branch of
//       `ExecutionTickService::run_tick` (execution_tick_service.cpp) — and
//       `t1_present` is always false this round (see D4) — so wiring
//       realizability would be inert code with zero observable effect. Left
//       at TickInput's struct defaults (rz_present=true, empty rz).
//   D4. The Reorder channel (sadg_t0 <-> switch_selector_node) is explicitly
//       out of this round's scope (brief: "Reorder 액션 서빙" excluded,
//       schema 0건, `350`§5-2 [결정 부재]) — `TickInput.t1_present` stays
//       `false` every tick (no order-search response ever arrives). AG1-AG5
//       are therefore never exercised live this round (they ARE exercised by
//       the pre-existing `test/core/test_execution_tick.cpp`, at the
//       core/service layer — this round adds no gate logic, only wires real
//       inputs into the parts of TickInput that matter without a reorder
//       response: entry events, completions, blocked edges).
//   D5. Per-robot progress deficit (ET20, `TickInput.deficits`) is left EMPTY
//       every tick. `D_r = now - tau_nom[frontier[r]]` needs a nominal
//       schedule `tau_nom` per segment, and no upstream artifact carries one
//       (mrs.discrete_plan/execution_constraints deliberately carry no
//       absolute time or timesteps — multibot3/CLAUDE.md invariant 3). This is
//       the pre-existing [값 부재] V-2 item (`337_arch_sadg_r2_p3.md`§10-2) —
//       not invented here, not silently dropped either.
//   D6. STRUCT_WC1 detection (ET22, `TickInput.wc`) is left at an EMPTY
//       `WcInput{}` (zero corridors) every tick, so `detect_wc1` always
//       returns `detected=false`. Building a real corridor-occupancy snapshot
//       needs to fuse /roadmap corridor definitions with segment
//       entered/completed state and residual-plan direction — a genuine new
//       derivation, not a boundary-to-TickInput field copy — and its only
//       consumer (EVAC replan-request publication, `run_evac`/EW/ZF/GB) is
//       explicitly out of this round's scope ("EVAC 발행부... 제외"). Building
//       the detector with no legal consumer this round would be scope creep;
//       reported as deferred, not silently stubbed as "done".
//   D7. Stop-declaration `reason` values are validated against the closed
//       vocabulary (`mrs_core_msgs::IsLegalForStopDeclaration`) before being
//       relayed; an out-of-vocabulary value is counted
//       (`n_stop_reason_unknown`) and the declaration is skipped — the
//       schema's own consumer obligation ("그 밖의 값이 오면 계약 위반이며
//       수신 측이 카운터로 센다"), not new policy.
//   D8. `segment_release.commit_seq` is node-owned bookkeeping that echoes the
//       LAST-PUBLISHED `/execution_constraints.commit_seq` value — NOT
//       `ExecutionGraph::commit_seq()` (a different counter that only bumps on
//       an accepted reorder, `ET12`, and stays 0 forever this round per D4).
//       See `adapter/release_adapter.hpp`'s file doc for the full argument;
//       `ConstraintsAdapter`'s own to_boundary()/node-override pattern
//       (`constraints_adapter.cpp` sets it from `g.commit_seq()`, then
//       `sadg_t0_node.cpp` immediately overwrites it with its own persistent
//       counter) is the existing precedent this mirrors.

#include <chrono>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_msgs/msg/dependency.hpp"
#include "mrs_msgs/msg/discrete_plan.hpp"
#include "mrs_msgs/msg/edge_ref.hpp"
#include "mrs_msgs/msg/entry_event.hpp"
#include "mrs_msgs/msg/entry_events.hpp"
#include "mrs_msgs/msg/execution_constraints.hpp"
#include "mrs_msgs/msg/segment.hpp"
#include "mrs_msgs/msg/segment_release.hpp"
#include "mrs_msgs/msg/segment_release_item.hpp"
#include "mrs_msgs/msg/stop_declaration.hpp"
#include "mrs_msgs/msg/stop_declarations.hpp"
#include "mrs_msgs/msg/switch_group.hpp"
#include "mrs_sadg/adapter/constraints_adapter.hpp"
#include "mrs_sadg/adapter/entry_events_adapter.hpp"
#include "mrs_sadg/adapter/plan_adapter.hpp"
#include "mrs_sadg/adapter/release_adapter.hpp"
#include "mrs_sadg/adapter/stop_declarations_adapter.hpp"
#include "mrs_sadg/core/constraint_compiler.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/instr_ring.hpp"
#include "mrs_sadg/core/safety_envelope.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/plugins/alt_head_lift.hpp"
#include "mrs_sadg/plugins/policy_deficit_ladder.hpp"
#include "mrs_sadg/service/execution_tick_service.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_sadg::node
{

class SadgT0Node : public rclcpp::Node
{
public:
    SadgT0Node()
        : rclcpp::Node("sadg_t0_node"),
          compiler_(generator_),
          tick_svc_(graph_, env_, policy_, instr_)
    {
        // D2 — the fleet-wide 50ms control-period invariant, not a swappable
        // algorithm parameter (mirrors trajopt_node.cpp's own D7 reasoning).
        const int control_period_ms = this->declare_parameter<int>("control_period_ms", 50);

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

        // Upstream event channels (337§8 seam 5) — NOT latched: these are
        // seq-numbered event streams, drained every tick, not late-joiner
        // snapshots like /discrete_plan or /execution_constraints.
        rclcpp::QoS event_qos(rclcpp::KeepLast(64));
        event_qos.reliable();

        entry_events_sub_ = this->create_subscription<mrs_msgs::msg::EntryEvents>(
            "/entry_events",
            event_qos,
            [this](mrs_msgs::msg::EntryEvents::ConstSharedPtr msg)
            {
                this->OnEntryEvents(msg);
            });

        stop_declarations_sub_ = this->create_subscription<mrs_msgs::msg::StopDeclarations>(
            "/stop_declaration",
            event_qos,
            [this](mrs_msgs::msg::StopDeclarations::ConstSharedPtr msg)
            {
                this->OnStopDeclarations(msg);
            });

        // D1 — this node's own tick loop (node-clock based, NOT
        // create_wall_timer, so use_sim_time governs it).
        control_timer_ = this->create_timer(std::chrono::milliseconds(control_period_ms),
                                            [this]()
                                            {
                                                this->OnTick();
                                            });
    }

private:
    void OnPlan(mrs_msgs::msg::DiscretePlan::ConstSharedPtr msg)
    {
        try  // module-developer convention: every callback top level is try-wrapped.
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
                RCLCPP_ERROR(this->get_logger(),
                             "sadg_t0_node: plan adapter rejected /discrete_plan");
                return;
            }

            // Replace the tick-service's shared graph IN PLACE (same object
            // identity — `tick_svc_` holds a reference, §5-1 ownership).
            graph_ = core::ExecutionGraph();
            core::CompileReport report;
            if (!mrs_core::ok(compiler_.compile(plan_view, graph_, report)))
            {
                RCLCPP_ERROR(this->get_logger(),
                             "sadg_t0_node: ConstraintCompiler rejected plan_revision=%lu "
                             "(outcome=%d)",
                             static_cast<unsigned long>(msg->plan_revision),
                             static_cast<int>(report.outcome));
                return;
            }

            adapter::BoundaryExecutionConstraints out_boundary;
            adapter::ConstraintsAdapter::to_boundary(graph_, out_boundary);
            // D8 — node-owned per-compile counter (NOT graph_.commit_seq()).
            out_boundary.commit_seq = commit_seq_;
            active_commit_seq_ = commit_seq_;
            ++commit_seq_;

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
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnPlan: %s", e.what());
        }
    }

    /// @brief D3/entry_events classification -> per-tick drain buffers +
    /// the persistent (never-cleared) per-robot next-expected-seq map.
    void OnEntryEvents(mrs_msgs::msg::EntryEvents::ConstSharedPtr msg)
    {
        try
        {
            adapter::BoundaryEntryEvents boundary;
            boundary.instance_id = msg->instance_id;
            boundary.events.reserve(msg->events.size());
            for (const auto& e : msg->events)
            {
                boundary.events.push_back({e.seq, e.robot, e.segment, e.location, e.entered});
            }
            adapter::EntryEventsView view;
            if (!mrs_core::ok(adapter::EntryEventsAdapter::from_boundary(boundary, view)))
            {
                RCLCPP_ERROR(this->get_logger(),
                             "sadg_t0_node: entry_events adapter rejected message (envelope "
                             "mismatch)");
                return;
            }
            pending_started_.insert(pending_started_.end(),
                                    view.started.begin(),
                                    view.started.end());
            pending_completed_.insert(pending_completed_.end(),
                                      view.completed.begin(),
                                      view.completed.end());
            for (const auto& [robot, next] : view.next_seq_by_robot)
            {
                auto& cur = entry_seq_ack_[robot];
                if (next > cur)
                {
                    cur = next;
                }
            }
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnEntryEvents: %s", e.what());
        }
    }

    /// @brief D7/reason validation, then StopDeclarationsAdapter -> the
    /// per-tick blocked-edge drain buffer.
    void OnStopDeclarations(mrs_msgs::msg::StopDeclarations::ConstSharedPtr msg)
    {
        try
        {
            adapter::BoundaryStopDeclarations boundary;
            boundary.instance_id = msg->instance_id;
            boundary.declarations.reserve(msg->declarations.size());
            for (const auto& d : msg->declarations)
            {
                if (!mrs_core_msgs::IsLegalForStopDeclaration(d.reason))
                {
                    instr_.count("n_stop_reason_unknown");  // D7: schema-mandated counter, skip.
                    continue;
                }
                const auto reason_str = mrs_core_msgs::StopReasonToString(d.reason);
                boundary.declarations.push_back(
                    {d.seq,
                     d.robot,
                     d.blocked_edge.from_id,
                     d.blocked_edge.to_id,
                     d.declared,
                     reason_str.ok() ? reason_str.value() : std::string()});
            }
            if (!mrs_core::ok(
                    adapter::StopDeclarationsAdapter::from_boundary(boundary, pending_blocked_)))
            {
                RCLCPP_ERROR(this->get_logger(),
                             "sadg_t0_node: stop_declarations adapter rejected message "
                             "(envelope mismatch)");
            }
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnStopDeclarations: %s", e.what());
        }
    }

    /// @brief The per-tick loop: drain -> ExecutionTickService::run_tick (ET00-
    /// ET36) -> publish the boundary output (ET33, arrival-vertex entry permit
    /// bits). CN-16: never lets an exception escape.
    void OnTick()
    {
        try
        {
            service::TickInput in;
            in.tick_seq = tick_seq_++;
            in.entry_events = std::move(pending_started_);
            pending_started_.clear();
            in.completions = std::move(pending_completed_);
            pending_completed_.clear();
            in.blocked_decls = std::move(pending_blocked_);
            pending_blocked_.clear();
            // D3: rz/t1 left at TickInput defaults (rz_present=true, empty rz,
            // t1_present=false) — inert this round, Reorder channel out of scope.
            // D5: deficits left empty ([값 부재] tau_nom).
            // D6: wc left default WcInput{} (zero corridors).

            const service::TickOutput out = tick_svc_.run_tick(in);
            (void)out;  // rung/republish_needed have no external sink this round
                        // (EVAC/replan publication explicitly out of scope, D6).

            PublishReleases();
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnTick: %s", e.what());
        }
    }

    /// @brief ET33: publish one mrs.segment_release 2.0.0 per robot currently
    /// present in the compiled graph.
    void PublishReleases()
    {
        std::set<std::string> robots;
        for (const auto& seg : graph_.segments())
        {
            robots.insert(seg.robot);
        }
        for (const auto& robot : robots)
        {
            adapter::BoundarySegmentRelease boundary;
            adapter::ReleaseAdapter::to_boundary(graph_, robot, boundary);
            boundary.instance_id = graph_.instance_id();
            boundary.commit_seq = active_commit_seq_;  // D8.
            boundary.release_seq = release_seq_[robot]++;
            boundary.entry_seq_ack = entry_seq_ack_[robot];  // default 0 if never seen.

            mrs_msgs::msg::SegmentRelease out;
            out.schema = boundary.schema;
            out.schema_version = boundary.schema_version;
            out.instance_id = boundary.instance_id;
            out.robot = boundary.robot;
            out.release_seq = boundary.release_seq;
            out.commit_seq = boundary.commit_seq;
            out.entry_seq_ack = boundary.entry_seq_ack;
            out.segments.reserve(boundary.segments.size());
            for (const auto& s : boundary.segments)
            {
                mrs_msgs::msg::SegmentReleaseItem item;
                item.id = s.id;
                item.released = s.released;
                out.segments.push_back(item);
            }
            GetOrCreateReleasePublisher(robot)->publish(out);
        }
    }

    rclcpp::Publisher<mrs_msgs::msg::SegmentRelease>::SharedPtr GetOrCreateReleasePublisher(
        const std::string& robot)
    {
        auto it = release_pubs_.find(robot);
        if (it != release_pubs_.end())
        {
            return it->second;
        }
        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();
        auto pub =
            this->create_publisher<mrs_msgs::msg::SegmentRelease>("/" + robot + "/segment_release",
                                                                  latched_qos);
        release_pubs_.emplace(robot, pub);
        return pub;
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

    // ---- plan-compile side (unchanged in spirit from 47차) ----------------
    std::uint64_t commit_seq_ = 0;         ///< next /execution_constraints commit_seq.
    std::uint64_t active_commit_seq_ = 0;  ///< D8: last-published value, echoed by segment_release.
    plugins::AltHeadLift generator_;
    core::ConstraintCompiler compiler_;

    // ---- tick side (this round) --------------------------------------------
    core::ExecutionGraph graph_;  ///< declared before tick_svc_ (member init order).
    core::SafetyEnvelope env_;
    plugins::PolicyDeficitLadder policy_;
    core::InstrRing instr_;
    service::ExecutionTickService tick_svc_;

    std::uint64_t tick_seq_ = 0;
    std::vector<std::string> pending_started_;    ///< drained into TickInput.entry_events.
    std::vector<std::string> pending_completed_;  ///< drained into TickInput.completions.
    std::vector<std::pair<std::string, std::string>>
        pending_blocked_;  ///< drained into blocked_decls.
    std::map<std::string, std::uint64_t>
        entry_seq_ack_;                                 ///< persistent, never cleared (per robot).
    std::map<std::string, std::uint64_t> release_seq_;  ///< persistent per-robot publish counter.
    std::map<std::string, rclcpp::Publisher<mrs_msgs::msg::SegmentRelease>::SharedPtr>
        release_pubs_;

    rclcpp::Publisher<mrs_msgs::msg::ExecutionConstraints>::SharedPtr constraints_pub_;
    rclcpp::Subscription<mrs_msgs::msg::DiscretePlan>::SharedPtr plan_sub_;
    rclcpp::Subscription<mrs_msgs::msg::EntryEvents>::SharedPtr entry_events_sub_;
    rclcpp::Subscription<mrs_msgs::msg::StopDeclarations>::SharedPtr stop_declarations_sub_;
    rclcpp::TimerBase::SharedPtr control_timer_;
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
