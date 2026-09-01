// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/core/self_checks.hpp"

#include <map>
#include <set>

namespace mrs_mapf::core
{

std::vector<std::string> check_start_vertex_uniqueness(const DiscretePlanDraft& plan)
{
    std::map<std::string, int> occurrences;
    for (const auto& p : plan.plans)
    {
        if (p.steps.empty())
        {
            continue;
        }
        ++occurrences[p.steps.front().location];
    }
    std::vector<std::string> collisions;
    for (const auto& [loc, n] : occurrences)
    {
        if (n >= 2)
        {
            collisions.push_back(loc);
        }
    }
    return collisions;
}

bool check_segment_graph_acyclic(const DiscretePlanDraft& plan, SelfCheckReport& report)
{
    std::vector<std::string> missing;
    SegmentGraph g = build_segment_graph(plan, missing);

    if (!missing.empty())
    {
        report.malformed_edges = missing;
        return false;
    }

    std::vector<SegmentId> cycle;
    if (find_cycle(g, cycle))
    {
        report.cycle = cycle;
        return false;
    }

    if (g.edge_count == 0)
    {
        // Not a failure, but not a tested pass either — §255-9's "untested" bucket
        // (03_formulation.md §2-9), the same rule check_mapf_solution.py applies.
        report.outcome = SelfCheckOutcome::kUntested;
        return false;
    }

    return true;
}

SelfCheckOutcome run_self_checks(const DiscretePlanDraft& plan, SelfCheckReport& report)
{
    report = SelfCheckReport{};

    // ⓐ first.
    auto collisions = check_start_vertex_uniqueness(plan);
    if (!collisions.empty())
    {
        report.outcome = SelfCheckOutcome::kStartVertexCollision;
        report.collisions = collisions;
        return report.outcome;
    }

    // ⓑ.
    SelfCheckReport sub;
    sub.outcome = SelfCheckOutcome::kOk;
    const bool acyclic_and_tested = check_segment_graph_acyclic(plan, sub);
    if (acyclic_and_tested)
    {
        report.outcome = SelfCheckOutcome::kOk;
        return report.outcome;
    }
    if (sub.outcome == SelfCheckOutcome::kUntested)
    {
        report.outcome = SelfCheckOutcome::kUntested;
        return report.outcome;
    }
    if (!sub.malformed_edges.empty())
    {
        report.outcome = SelfCheckOutcome::kMalformedType2Edge;
        report.malformed_edges = sub.malformed_edges;
        return report.outcome;
    }
    report.outcome = SelfCheckOutcome::kCycleDetected;
    report.cycle = sub.cycle;
    return report.outcome;
}

}  // namespace mrs_mapf::core
