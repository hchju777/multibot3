// SPDX-License-Identifier: Apache-2.0
#include <map>

#include "../test_util.hpp"
#include "mrs_mapf/core/segment_graph.hpp"

using namespace mrs_mapf::core;

/// @file test_segment_graph.cpp
/// @brief 🔴 함정 ②(발주서) — "퇴거 -> 진입" is the contract mapping; "진입 ->
/// 진입" is a DIFFERENT graph that lets exactly the same swap plan through. This
/// test builds both, on the same fixture, and shows the divergence directly —
/// reproducing `274_algo_gate.md` F-1 / `discrete_plan.schema.json`
/// `x-layer2-note`'s finding inside this package (not just citing it).

namespace
{

DiscretePlanDraft edge_swap_plan()
{
    DiscretePlanDraft d;
    RobotPlan p0;
    p0.robot = "r0";
    p0.terminal = Terminal::kGoalReached;
    p0.steps = {{0, "A"}, {1, "B"}};
    RobotPlan p1;
    p1.robot = "r1";
    p1.terminal = Terminal::kGoalReached;
    p1.steps = {{0, "B"}, {1, "A"}};
    d.plans = {p0, p1};

    VisitOrderEntry at_a;
    at_a.location = "A";
    at_a.sequence = {{"r0", 0}, {"r1", 1}};
    VisitOrderEntry at_b;
    at_b.location = "B";
    at_b.sequence = {{"r1", 0}, {"r0", 1}};
    d.visit_order = {at_a, at_b};
    return d;
}

/// @brief The WRONG mapping (entry -> entry): consecutive visit_order pairs
/// wired as `(lead.robot,lead.index) -> (follow.robot,follow.index)` — no "one
/// slot earlier" shift. Built here ONLY to demonstrate the divergence; this
/// package's production self-check (`self_checks.cpp`) never builds this graph.
bool entry_to_entry_has_cycle(const DiscretePlanDraft& plan)
{
    std::map<std::string, std::uint32_t> seg_count;
    for (const auto& p : plan.plans)
    {
        seg_count[p.robot] = p.steps.empty() ? 0 : static_cast<std::uint32_t>(p.steps.size() - 1);
    }
    // Nodes are visit ITEMS (robot,index), not segments.
    std::map<std::pair<std::string, std::uint32_t>,
             std::vector<std::pair<std::string, std::uint32_t>>>
        adj;
    for (const auto& p : plan.plans)
    {
        for (std::uint32_t i = 0; i < p.steps.size(); ++i)
        {
            adj[{p.robot, i}] = {};
        }
    }
    for (const auto& entry : plan.visit_order)
    {
        for (std::size_t k = 0; k + 1 < entry.sequence.size(); ++k)
        {
            const auto& lead = entry.sequence[k];
            const auto& follow = entry.sequence[k + 1];
            adj[{lead.robot, lead.index}].push_back(
                {follow.robot, follow.index});  // entry -> entry (WRONG).
        }
    }
    // Simple DFS cycle check.
    std::map<std::pair<std::string, std::uint32_t>, int> mark;
    std::vector<std::pair<std::string, std::uint32_t>> stack;
    for (const auto& [node, _] : adj)
    {
        if (mark[node] != 0)
        {
            continue;
        }
        std::vector<std::pair<std::pair<std::string, std::uint32_t>, std::size_t>> frames;
        frames.push_back({node, 0});
        mark[node] = 1;
        stack.push_back(node);
        while (!frames.empty())
        {
            auto& [u, cursor] = frames.back();
            const auto& nbrs = adj.at(u);
            if (cursor < nbrs.size())
            {
                auto v = nbrs[cursor];
                ++cursor;
                if (mark[v] == 1)
                {
                    return true;
                }
                if (mark[v] == 0)
                {
                    mark[v] = 1;
                    stack.push_back(v);
                    frames.push_back({v, 0});
                }
            }
            else
            {
                mark[u] = 2;
                stack.pop_back();
                frames.pop_back();
            }
        }
    }
    return false;
}

}  // namespace

int main()
{
    DiscretePlanDraft d = edge_swap_plan();

    // Correct mapping (퇴거 -> 진입, this package's production code) DOES catch it.
    std::vector<std::string> missing;
    SegmentGraph g = build_segment_graph(d, missing);
    CHECK(missing.empty());
    CHECK(g.nodes.size() == 2);  // r0#0, r1#0 (each robot has exactly 1 segment).
    CHECK(g.edge_count == 2);  // the two Type-2 edges (no Type-1 — each robot has only 1 segment).

    std::vector<SegmentId> cycle;
    CHECK(find_cycle(g, cycle));
    CHECK(cycle.size() >= 2);

    // WRONG mapping (진입 -> 진입) does NOT catch the same plan — the trap.
    CHECK(!entry_to_entry_has_cycle(d));

    // A longer, 3-step-per-robot corridor swap (both ends' visit_order
    // disagree) also cycles once Type-1 chains its robot's own segments in —
    // a second, independent fixture for the retreat->entry mapping (not an
    // isolation test of Type-1 alone; see `20b_impl_mapf.md` known limitations).
    {
        DiscretePlanDraft d3;
        RobotPlan p0;
        p0.robot = "r0";
        p0.terminal = Terminal::kGoalReached;
        p0.steps = {{0, "A"}, {1, "B"}, {2, "C"}};
        RobotPlan p1;
        p1.robot = "r1";
        p1.terminal = Terminal::kGoalReached;
        p1.steps = {{0, "C"}, {1, "B"}, {2, "A"}};
        d3.plans = {p0, p1};
        VisitOrderEntry at_a;
        at_a.location = "A";
        at_a.sequence = {{"r0", 0}, {"r1", 2}};
        VisitOrderEntry at_c;
        at_c.location = "C";
        at_c.sequence = {{"r1", 0}, {"r0", 2}};
        VisitOrderEntry at_b;
        at_b.location = "B";
        // Both ends "flipped" relative to a consistent single-file order at B too.
        at_b.sequence = {{"r1", 1}, {"r0", 1}};
        d3.visit_order = {at_a, at_b, at_c};

        std::vector<std::string> missing3;
        SegmentGraph g3 = build_segment_graph(d3, missing3);
        CHECK(missing3.empty());
        std::vector<SegmentId> cyc3;
        CHECK(find_cycle(g3,
                         cyc3));  // Type-1 (r#i -> r#(i+1)) is present and required to close this.
    }

    return mapf_test::summary();
}
