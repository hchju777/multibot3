// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/self_checks.hpp"

#include <algorithm>
#include <set>

#include "mrs_sadg/core/constraint_compiler.hpp"

namespace mrs_sadg::core
{

bool is_opposing(const Segment& from, const Segment& to)
{
    // ［D5］: the two named segments cross the same edge in opposite directions.
    return to.from == from.to;
}

std::vector<std::pair<SegmentId, SegmentId>> expected_ordering_edges(const PlanView& plan,
                                                                     const ExecutionGraph& graph)
{
    std::vector<std::pair<SegmentId, SegmentId>> out;
    for (const auto& entry : plan.visit_order)
    {
        const auto& seq = entry.sequence;
        for (std::size_t k = 0; k + 1 < seq.size(); ++k)
        {
            const auto& pred = seq[k];
            const auto& succ = seq[k + 1];
            if (pred.robot == succ.robot)
            {
                continue;  // revisit — sequential already enforces it.
            }
            if (succ.index == 0)
            {
                continue;  // successor at its start vertex — not an ordering (checker's rule).
            }
            SegmentId from = pred.robot + "#" + std::to_string(pred.index);
            SegmentId to = succ.robot + "#" + std::to_string(succ.index - 1);
            const Segment* sf = graph.find_segment(from);
            const Segment* st = graph.find_segment(to);
            if (sf == nullptr || st == nullptr)
            {
                continue;  // (a) will already report the missing segment.
            }
            // The ordering edge exists for both opposing and following pairs (the
            // checker's seam (c) does not model ［D5］); opposing-ness only changes
            // release semantics, not the edge's presence.
            out.push_back({from, to});
        }
    }
    return out;
}

bool seam_selfcheck(const PlanView& plan, const ExecutionGraph& graph, SeamReport& report)
{
    bool ok = true;

    // (a) segment count = steps - 1, from/to = neighbouring vertices.
    for (const auto& r : plan.robots)
    {
        std::size_t n_expected = r.locations.empty() ? 0 : r.locations.size() - 1;
        for (std::size_t k = 0; k < n_expected; ++k)
        {
            SegmentId id = r.robot + "#" + std::to_string(k);
            const Segment* s = graph.find_segment(id);
            if (s == nullptr)
            {
                report.messages.push_back("[seam a] 세그먼트 없음: " + id);
                ok = false;
                continue;
            }
            if (s->from != r.locations[k] || s->to != r.locations[k + 1])
            {
                report.messages.push_back("[seam a] 세그먼트 " + id + " from/to 불일치");
                ok = false;
            }
        }
    }

    // (b) sequential chain complete.
    std::set<std::pair<SegmentId, SegmentId>> expected_seq;
    for (const auto& r : plan.robots)
    {
        std::size_t n_seg = r.locations.empty() ? 0 : r.locations.size() - 1;
        for (std::size_t k = 0; k + 1 < n_seg; ++k)
        {
            expected_seq.insert(
                {r.robot + "#" + std::to_string(k), r.robot + "#" + std::to_string(k + 1)});
        }
    }
    std::set<std::pair<SegmentId, SegmentId>> got_seq;
    std::set<std::pair<SegmentId, SegmentId>> got_ord;
    for (const auto& d : graph.fixed_deps())
    {
        if (d.type == DepType::kSequential)
        {
            got_seq.insert({d.from, d.to});
        }
        else
        {
            got_ord.insert({d.from, d.to});
        }
    }
    if (got_seq != expected_seq)
    {
        report.messages.push_back("[seam b] sequential 사슬이 계약 유도와 다르다");
        ok = false;
    }

    // (c) ordering mapping 퇴거->진입.
    std::set<std::pair<SegmentId, SegmentId>> expected_ord;
    for (const auto& e : expected_ordering_edges(plan, graph))
    {
        expected_ord.insert(e);
    }
    if (got_ord != expected_ord)
    {
        report.messages.push_back("[seam c] ordering 간선이 계약 사상(퇴거->진입) 유도와 다르다");
        ok = false;
    }

    // (d) H-cons (already gated at intake; re-verified here for independence).
    std::vector<std::string> hmsg;
    if (!ConstraintCompiler::hcons_ok(plan, hmsg))
    {
        for (auto& m : hmsg)
        {
            report.messages.push_back("[seam d] " + m);
        }
        ok = false;
    }

    return ok;
}

}  // namespace mrs_sadg::core
