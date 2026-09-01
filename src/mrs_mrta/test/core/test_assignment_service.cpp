// SPDX-License-Identifier: Apache-2.0
// CN-22: test/core/ — service-layer tests: drain_once total order
// (`336_mrta_canon_p2.md` §336-4-2, `336b`§336b-2-2), no-op publish
// suppression, and operator-override projection (`336b`§336b-3, §336b-6).
#include <optional>
#include <string>

#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "mrs_mrta/plugins/assigner_impls.hpp"
#include "mrs_mrta/service/assignment_service.hpp"
#include "test_util.hpp"

using mrs_mrta::core::AssignmentView;
using mrs_mrta::core::CompletionEvent;
using mrs_mrta::core::OperatorEvent;
using mrs_mrta::core::ReleaseEvent;
using mrs_mrta::core::Status;
using mrs_mrta::plugins::GreedyTailAppend;
using mrs_mrta::service::AssignmentService;
using mrs_mrta::testutil::FixedOverrideGoalIdResolver;
using mrs_mrta::testutil::MapDistanceOracle;

namespace
{

ReleaseEvent make_release(std::uint64_t idx,
                          const std::string& task,
                          const std::string& goal_id,
                          const std::string& location)
{
    ReleaseEvent ev;
    ev.release_index = idx;
    ev.task = task;
    mrs_mrta::core::GoalRecord g;
    g.id = goal_id;
    g.task = task;
    g.location = location;
    ev.goals.push_back(g);
    return ev;
}

const mrs_mrta::core::RobotGoalsView* find_robot(const AssignmentView& view,
                                                 const std::string& robot)
{
    for (const auto& rv : view.robots)
    {
        if (rv.robot == robot)
        {
            return &rv;
        }
    }
    return nullptr;
}

void test_total_order_completion_before_release()
{
    MapDistanceOracle distances;
    distances.set("V", "PX", 5);
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    AssignmentService svc({"r1"}, 3, 8, assigner, nullptr);
    svc.set_robot_vertex("r1", "V");

    // Stage a completion for a goal that only the release (staged next) will create.
    // The canon total order (completions THEN releases) must process the completion
    // first, against a still-empty queue -> mismatch, not a match.
    CompletionEvent early_completion;
    early_completion.robot = "r1";
    early_completion.goal_id = "gX";
    MRTA_CHECK(svc.ingest_completion(early_completion) == Status::kOk);
    MRTA_CHECK(svc.ingest_release(make_release(1, "tX", "gX", "PX")) == Status::kOk);

    const auto view = svc.drain_once();
    MRTA_CHECK(view.has_value());
    MRTA_CHECK(svc.completion_mismatch_count() ==
               1);  // Proves completion ran against an empty queue.
    const auto* r1 = find_robot(*view, "r1");
    MRTA_CHECK(r1 != nullptr && r1->goals.size() == 1 && r1->goals[0].id == "gX");
}

void test_no_op_publish_suppression()
{
    MapDistanceOracle distances;
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    AssignmentService svc({"r1"}, 3, 8, assigner, nullptr);

    // A deactivation of a latch that was never active changes nothing observable.
    OperatorEvent noop;
    noop.robot = "r1";
    noop.active = false;
    noop.seq = 0;
    MRTA_CHECK(svc.ingest_operator(noop) == Status::kOk);
    const auto view = svc.drain_once();
    MRTA_CHECK(!view.has_value());
    MRTA_CHECK(svc.revision() == 0);
}

void test_override_projection_leaves_base_untouched()
{
    MapDistanceOracle distances;
    distances.set("V", "P1", 1);
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    FixedOverrideGoalIdResolver resolver;
    resolver.set("r1", "OVR", "reserved-goal-1");

    AssignmentService svc({"r1"}, 3, 8, assigner, &resolver);
    svc.set_robot_vertex("r1", "V");
    svc.ingest_release(make_release(1, "t1", "g1", "P1"));
    svc.drain_once();  // Base queue now holds g1.

    OperatorEvent ov;
    ov.robot = "r1";
    ov.active = true;
    ov.goal_location = "OVR";
    ov.seq = 1;
    svc.ingest_operator(ov);
    const auto view = svc.drain_once();
    MRTA_CHECK(view.has_value());
    MRTA_CHECK(svc.override_active("r1"));

    const auto* r1 = find_robot(*view, "r1");
    MRTA_CHECK(r1 != nullptr);
    MRTA_CHECK(r1->goals.size() == 1);
    MRTA_CHECK(r1->goals[0].id == "reserved-goal-1");
    MRTA_CHECK(r1->goals[0].location == "OVR");

    // 🔴 Base is untouched — the published view is a projection, not a mutation
    // (336b§336b-3-1). The original queued goal g1 is still there underneath.
    MRTA_CHECK(svc.state().residual_len(svc.state().index_of("r1")) == 1);
    MRTA_CHECK(svc.state().peek_head(svc.state().index_of("r1"))->id == "g1");

    // Deactivating restores the base view exactly (no snapshot/restore logic ran).
    OperatorEvent off;
    off.robot = "r1";
    off.active = false;
    off.seq = 2;
    svc.ingest_operator(off);
    const auto restored = svc.drain_once();
    MRTA_CHECK(restored.has_value());
    const auto* r1_restored = find_robot(*restored, "r1");
    MRTA_CHECK(r1_restored->goals.size() == 1 && r1_restored->goals[0].id == "g1");
}

void test_highest_seq_wins()
{
    MapDistanceOracle distances;
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    FixedOverrideGoalIdResolver resolver;
    resolver.set("r1", "A", "gA");
    resolver.set("r1", "B", "gB");

    AssignmentService svc({"r1"}, 3, 8, assigner, &resolver);

    // Two events land in the same drain_once call, arrival order reversed relative to seq —
    // EventRing::pop_all_ordered sorts by (robot, seq) ascending before processing, so the
    // higher-seq event (B) must win regardless of ingest order.
    OperatorEvent evB;
    evB.robot = "r1";
    evB.active = true;
    evB.goal_location = "B";
    evB.seq = 5;
    OperatorEvent evA;
    evA.robot = "r1";
    evA.active = true;
    evA.goal_location = "A";
    evA.seq = 3;
    svc.ingest_operator(evB);  // Pushed first, but has the higher seq.
    svc.ingest_operator(evA);
    const auto view = svc.drain_once();
    MRTA_CHECK(view.has_value());
    MRTA_CHECK(find_robot(*view, "r1")->goals[0].id == "gB");

    // A stale (lower-seq) event in a later drain must be ignored (no publish).
    OperatorEvent stale;
    stale.robot = "r1";
    stale.active = true;
    stale.goal_location = "A";
    stale.seq = 1;
    svc.ingest_operator(stale);
    const auto stale_view = svc.drain_once();
    MRTA_CHECK(!stale_view.has_value());
}

void test_unresolved_override_goal_id_falls_back_to_base()
{
    // F46-2 regression lock: an override activation whose goal_id cannot be
    // resolved falls back to publishing the (unchanged) base queue — that
    // must NOT bump revision or publish, because the boundary content is
    // identical to what was already published (no-op publish suppression,
    // canon A32 / assignment.schema.json:24).
    MapDistanceOracle distances;
    distances.set("V", "P1", 1);
    distances.set("P1", "P2", 1);
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    // No resolver at all -> every activation falls back to base and is counted,
    // but must not spuriously publish.
    AssignmentService svc({"r1"}, 3, 8, assigner, nullptr);
    svc.set_robot_vertex("r1", "V");
    svc.ingest_release(make_release(1, "t1", "g1", "P1"));
    const auto first_publish = svc.drain_once();
    MRTA_CHECK(first_publish.has_value());
    const std::uint64_t revision_before = svc.revision();

    OperatorEvent ov;
    ov.robot = "r1";
    ov.active = true;
    ov.goal_location = "OVR";
    ov.seq = 1;
    svc.ingest_operator(ov);
    const auto view = svc.drain_once();
    // 🔴 F46-2: the latch changed (active flipped true), but the resolver has
    // nothing registered, so build_view() falls back to the base queue —
    // which is exactly what was last published. Must be suppressed.
    MRTA_CHECK(!view.has_value());
    MRTA_CHECK(svc.revision() == revision_before);             // No spurious revision bump.
    MRTA_CHECK(svc.override_goal_id_unresolved_count() == 1);  // Attempt still counted.

    // A second, *real* change (a new release for the same robot) must still
    // publish normally — the suppression is content-specific, not sticky.
    MRTA_CHECK(svc.ingest_release(make_release(2, "t2", "g2", "P2")) ==
               mrs_mrta::core::Status::kOk);
    const auto second_publish = svc.drain_once();
    MRTA_CHECK(second_publish.has_value());
    MRTA_CHECK(svc.revision() == revision_before + 1);
    MRTA_CHECK(find_robot(*second_publish, "r1")->goals.size() == 2);
}

void test_event_ring_capacity_drops()
{
    MapDistanceOracle distances;
    GreedyTailAppend assigner;
    mrs_mrta::core::AssignerContext ctx;
    ctx.robot_count = 1;
    ctx.goal_queue_capacity_ta = 3;
    ctx.distances = &distances;
    assigner.configure(ctx);

    AssignmentService svc({"r1"}, 3, /*event_ring_slots=*/2, assigner, nullptr);
    MRTA_CHECK(svc.ingest_release(make_release(1, "t1", "g1", "P1")) == Status::kOk);
    MRTA_CHECK(svc.ingest_release(make_release(2, "t2", "g2", "P2")) == Status::kOk);
    MRTA_CHECK(svc.ingest_release(make_release(3, "t3", "g3", "P3")) ==
               Status::kError);  // Ring full.
}

}  // namespace

int main()
{
    test_total_order_completion_before_release();
    test_no_op_publish_suppression();
    test_override_projection_leaves_base_untouched();
    test_highest_seq_wins();
    test_unresolved_override_goal_id_falls_back_to_base();
    test_event_ring_capacity_drops();
    return 0;
}
