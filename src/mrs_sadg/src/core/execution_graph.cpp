// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/execution_graph.hpp"

#include <stdexcept>

namespace mrs_sadg::core
{

void ExecutionGraph::add_segment(const Segment& seg)
{
    if (index_.find(seg.id) != index_.end())
    {
        return;  // idempotent: a duplicate id is rejected by the self-check, not here.
    }
    index_[seg.id] = segments_.size();
    segments_.push_back(seg);
}

void ExecutionGraph::add_fixed_dep(const Dependency& dep)
{
    fixed_deps_.push_back(dep);
}

void ExecutionGraph::add_switch_group(const SwitchGroup& group)
{
    groups_.push_back(group);
}

void ExecutionGraph::reset_runtime()
{
    entered_.clear();
    completed_.clear();
    opposing_edges_.clear();
    released_.clear();
    blocked_.clear();
    deficit_.clear();
    committed_combo_.clear();
    cur_rung_ = 0;
    tick_seq_ = 0;
    commit_seq_ = 0;
}

std::vector<Dependency> ExecutionGraph::committed_ordering_deps() const
{
    std::vector<Dependency> out;
    for (const auto& d : fixed_deps_)
    {
        if (d.type == DepType::kOrdering)
        {
            out.push_back(d);
        }
    }
    for (std::size_t g = 0; g < groups_.size() && g < committed_combo_.size(); ++g)
    {
        std::size_t idx = committed_combo_[g];
        if (idx < groups_[g].alternatives.size())
        {
            for (const auto& d : groups_[g].alternatives[idx].deps)
            {
                out.push_back(d);
            }
        }
    }
    return out;
}

std::size_t ExecutionGraph::index_of(const SegmentId& id) const
{
    auto it = index_.find(id);
    if (it == index_.end())
    {
        throw std::out_of_range("ExecutionGraph::index_of: unknown segment " + id);
    }
    return it->second;
}

const Segment* ExecutionGraph::find_segment(const SegmentId& id) const
{
    auto it = index_.find(id);
    if (it == index_.end())
    {
        return nullptr;
    }
    return &segments_[it->second];
}

void ExecutionGraph::set_release(const SegmentId& id, bool permit)
{
    if (permit)
    {
        released_.insert(id);
    }
    else
    {
        released_.erase(id);
    }
}

void ExecutionGraph::apply_blocked_delta(
    const std::vector<std::pair<std::string, std::string>>& edges)
{
    // 🔴 ET17 / §11-2③: fill V_blk from the upstream stop-declaration channel.
    // Additive: trajopt reports edges it cannot traverse; SADG accumulates them.
    for (const auto& e : edges)
    {
        blocked_.insert(e);
    }
}

bool ExecutionGraph::is_blocked_edge(const std::string& from, const std::string& to) const
{
    return blocked_.count({from, to}) != 0;
}

double ExecutionGraph::deficit(const std::string& robot) const
{
    auto it = deficit_.find(robot);
    return it == deficit_.end() ? 0.0 : it->second;
}

std::vector<Edge> ExecutionGraph::edges_for(const std::vector<std::size_t>& combo) const
{
    std::vector<Edge> edges;
    edges.reserve(fixed_deps_.size());
    for (const auto& d : fixed_deps_)
    {
        auto a = index_.find(d.from);
        auto b = index_.find(d.to);
        if (a != index_.end() && b != index_.end())
        {
            edges.emplace_back(a->second, b->second);
        }
    }
    for (std::size_t g = 0; g < groups_.size() && g < combo.size(); ++g)
    {
        std::size_t idx = combo[g];
        if (idx >= groups_[g].alternatives.size())
        {
            continue;
        }
        for (const auto& d : groups_[g].alternatives[idx].deps)
        {
            auto a = index_.find(d.from);
            auto b = index_.find(d.to);
            if (a != index_.end() && b != index_.end())
            {
                edges.emplace_back(a->second, b->second);
            }
        }
    }
    return edges;
}

bool ExecutionGraph::is_acyclic(const std::vector<std::size_t>& combo) const
{
    return is_dag(segments_.size(), edges_for(combo));
}

bool is_dag(std::size_t n, const std::vector<Edge>& edges)
{
    std::vector<std::vector<std::size_t>> adj(n);
    std::vector<std::size_t> indeg(n, 0);
    for (const auto& e : edges)
    {
        if (e.first >= n || e.second >= n)
        {
            continue;
        }
        adj[e.first].push_back(e.second);
        ++indeg[e.second];
    }
    std::vector<std::size_t> queue;
    for (std::size_t i = 0; i < n; ++i)
    {
        if (indeg[i] == 0)
        {
            queue.push_back(i);
        }
    }
    std::size_t settled = 0;
    while (!queue.empty())
    {
        std::size_t u = queue.back();
        queue.pop_back();
        ++settled;
        for (std::size_t v : adj[u])
        {
            if (--indeg[v] == 0)
            {
                queue.push_back(v);
            }
        }
    }
    return settled == n;
}

}  // namespace mrs_sadg::core
