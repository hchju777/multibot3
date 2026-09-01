// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/commit_gate.hpp"

namespace mrs_sadg::core
{

bool CommitGate::gate_acyclic(const ExecutionGraph& g, const CommitProposal& p)
{
    return g.is_acyclic(p.combo);  // recomputed — no cache (AG1).
}

bool CommitGate::gate_release_unopened(const ExecutionGraph& g, const CommitProposal& p)
{
    for (const auto& e : g.edges_for(p.combo))
    {
        const Segment& v = g.segments()[e.second];
        const Segment& u = g.segments()[e.first];
        // An already-opened release must not be reordered by a new, unsatisfied
        // predecessor: if v is released but its proposed predecessor u has not
        // entered, the reorder would retract an opened permit.
        if (g.is_released(v.id) && !g.is_entered(u.id))
        {
            return false;
        }
    }
    return true;
}

bool CommitGate::gate_alock_order(const ExecutionGraph& g, const CommitProposal& p)
{
    for (const auto& e : g.edges_for(p.combo))
    {
        const Segment& u = g.segments()[e.first];
        const Segment& v = g.segments()[e.second];
        if (g.is_entered(v.id) && !g.is_entered(u.id))
        {
            return false;  // committed successor behind uncommitted predecessor.
        }
    }
    return true;
}

bool CommitGate::gate_recert_inprogress(const ExecutionGraph& g,
                                        const CommitProposal& p,
                                        const ReachabilityView& rz)
{
    if (p.rz_seq != rz.rz_seq)
    {
        return false;  // stale: proposal computed against an older reachability.
    }
    for (const auto& seg : g.segments())
    {
        if (g.is_entered(seg.id) && !rz.reachable(seg.id))
        {
            return false;  // an in-progress segment must not be de-certified.
        }
    }
    return true;
}

GateId CommitGate::first_failure(const ExecutionGraph& g,
                                 const CommitProposal& p,
                                 const ReachabilityView& rz)
{
    if (!gate_acyclic(g, p))
    {
        return GateId::kAcyclic;
    }
    if (!gate_release_unopened(g, p))
    {
        return GateId::kReleaseOpened;
    }
    if (!gate_alock_order(g, p))
    {
        return GateId::kAlockOrder;
    }
    if (!gate_recert_inprogress(g, p, rz))
    {
        return GateId::kRecertStale;
    }
    return GateId::kNone;
}

}  // namespace mrs_sadg::core
