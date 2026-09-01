// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/core/segment_graph.hpp"

#include <set>

namespace mrs_mapf::core
{

namespace
{

/// @brief robot -> number of segments (= steps.size() - 1, or 0 if steps.size() <= 1).
std::map<std::string, std::uint32_t> segment_counts(const DiscretePlanDraft& plan)
{
    std::map<std::string, std::uint32_t> counts;
    for (const auto& p : plan.plans)
    {
        counts[p.robot] = p.steps.size() > 0 ? static_cast<std::uint32_t>(p.steps.size() - 1) : 0;
    }
    return counts;
}

}  // namespace

SegmentGraph build_segment_graph(const DiscretePlanDraft& plan,
                                 std::vector<std::string>& missing_edges)
{
    SegmentGraph g;
    const auto counts = segment_counts(plan);

    for (const auto& [robot, n] : counts)
    {
        for (std::uint32_t i = 0; i < n; ++i)
        {
            SegmentId id{robot, i};
            g.nodes.push_back(id);
            g.adjacency[id] = {};
        }
    }

    // Type-1 — same-robot consecutive segments r#i -> r#(i+1). Mandatory (§286-1 #3/#4).
    for (const auto& [robot, n] : counts)
    {
        for (std::uint32_t i = 0; i + 1 < n; ++i)
        {
            SegmentId src{robot, i};
            SegmentId dst{robot, i + 1};
            g.adjacency[src].push_back(dst);
            ++g.edge_count;
        }
    }

    // Type-2 — "퇴거 -> 진입" contract mapping over visit_order.
    for (const auto& entry : plan.visit_order)
    {
        for (std::size_t k = 0; k + 1 < entry.sequence.size(); ++k)
        {
            const VisitItem& lead = entry.sequence[k];
            const VisitItem& follow = entry.sequence[k + 1];
            if (lead.robot == follow.robot)
            {
                // Same-robot revisit creates no dependency — the Type-1 chain already
                // covers it (contract $defs.dependency / sadg_core.ordering_edges).
                continue;
            }
            const SegmentId src{lead.robot, lead.index};  // departure segment of the lead item.
            if (follow.index < 1)
            {
                missing_edges.push_back(
                    "visit_order(" + entry.location + ") 의 " + lead.robot + "#" +
                    std::to_string(lead.index) + " -> " + follow.robot + "#" +
                    std::to_string(follow.index) +
                    " 쌍은 후행 항목이 index 0이라 진입 세그먼트가 원리적으로 없다");
                continue;
            }
            const SegmentId dst{follow.robot,
                                follow.index - 1};  // entry segment of the follow item.
            const bool src_exists = g.adjacency.find(src) != g.adjacency.end();
            const bool dst_exists = g.adjacency.find(dst) != g.adjacency.end();
            if (!src_exists || !dst_exists)
            {
                missing_edges.push_back("visit_order(" + entry.location + ") 의 " + lead.robot +
                                        "#" + std::to_string(lead.index) + " -> " + follow.robot +
                                        "#" + std::to_string(follow.index) + " 쌍은 " +
                                        (!src_exists ? std::string("선행의 퇴거 세그먼트")
                                                     : std::string("후행의 진입 세그먼트")) +
                                        "가 없다");
                continue;
            }
            g.adjacency[src].push_back(dst);
            ++g.edge_count;
        }
    }

    return g;
}

bool find_cycle(const SegmentGraph& graph, std::vector<SegmentId>& cycle_out)
{
    enum class Mark
    {
        kWhite,
        kGray,
        kBlack
    };
    std::map<SegmentId, Mark> mark;
    for (const auto& n : graph.nodes)
    {
        mark[n] = Mark::kWhite;
    }

    std::vector<SegmentId> stack;

    // Deterministic node order — graph.nodes was built by iterating a std::map, already sorted.
    for (const auto& start : graph.nodes)
    {
        if (mark[start] != Mark::kWhite)
        {
            continue;
        }
        // Iterative DFS with an explicit frame stack, tracking edge cursor per frame.
        std::vector<std::pair<SegmentId, std::size_t>> frames;
        frames.emplace_back(start, 0);
        mark[start] = Mark::kGray;
        stack.push_back(start);

        while (!frames.empty())
        {
            auto& [node, cursor] = frames.back();
            const auto& neighbors = graph.adjacency.at(node);
            if (cursor < neighbors.size())
            {
                const SegmentId next = neighbors[cursor];
                ++cursor;
                if (mark[next] == Mark::kGray)
                {
                    // Found a back edge — reconstruct the cycle from the stack.
                    cycle_out.clear();
                    bool started = false;
                    for (const auto& s : stack)
                    {
                        if (s == next)
                        {
                            started = true;
                        }
                        if (started)
                        {
                            cycle_out.push_back(s);
                        }
                    }
                    cycle_out.push_back(next);
                    return true;
                }
                if (mark[next] == Mark::kWhite)
                {
                    mark[next] = Mark::kGray;
                    stack.push_back(next);
                    frames.emplace_back(next, 0);
                }
            }
            else
            {
                mark[node] = Mark::kBlack;
                stack.pop_back();
                frames.pop_back();
            }
        }
    }
    return false;
}

}  // namespace mrs_mapf::core
