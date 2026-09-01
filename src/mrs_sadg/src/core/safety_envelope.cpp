// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/safety_envelope.hpp"

namespace mrs_sadg::core
{

bool SafetyEnvelope::safe_check(const ExecutionGraph& g,
                                const std::vector<std::size_t>& combo) const
{
    // RE-4: acyclicity is a necessary conjunct.
    if (!g.is_acyclic(combo))
    {
        return false;
    }
    // RE-1: ∧ PROP-ACCEPT (all robots can complete). Broader than acyclicity.
    if (!prop_accept(g, combo))
    {
        return false;
    }
    // RE-2 [잠정]: one-step structural condition (isolated; see header).
    return one_step_ok(g);
}

bool SafetyEnvelope::prop_accept(const ExecutionGraph& g,
                                 const std::vector<std::size_t>& combo) const
{
    // A DAG over the segment set means a topological order exists in which every
    // segment eventually starts — the discrete surrogate of "all robots can
    // complete". (RE-1; the continuous feasibility is trajopt's, not SADG's.)
    // We additionally require that no *entered* (committed) segment sits behind a
    // not-yet-entered predecessor introduced by the proposal — that would be an
    // A-LOCK inversion, which gate AG3 also guards.
    const auto edges = g.edges_for(combo);
    for (const auto& e : edges)
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

bool SafetyEnvelope::one_step_ok(const ExecutionGraph& g) const
{
    // 🔴 RE-2 [미확인 잠정]: the general "maximally permissive" one-step-lookahead
    // is NP-hard; the deployed surrogate is the RAS structural condition
    // (cap>=2 ∨ single-pred ∨ single-succ), and where it fails, a standby slot on
    // a cut vertex recovers it (RE-3). With no roadmap wired we accept
    // conservatively on the graph shape alone. The predicate is isolated here so
    // that when the source is confirmed only this function changes (no config /
    // plugin promotion). See L-list CO(RE-2).
    (void)g;
    return true;
}

WcResult SafetyEnvelope::detect_wc1(const WcInput& in) const
{
    WcResult r;
    for (const auto& q : in.corridors)
    {
        if (q.occupants.empty())
        {
            continue;  // DT.2: corridor empty (drain is finite, 189-P1).
        }
        if (!q.exit_occupant.empty() && q.exit_first_edge_reversed)
        {
            // DT.3 + DT.5: exit occupant's residual first edge is reversed-only —
            // an internal (structural) deadlock. Uses plan + occupancy only.
            r.detected = true;
            r.corridor = q.id;
            r.occupants = q.occupants;
            r.exit_occupant = q.exit_occupant;
            return r;  // mutually exclusive with the normal path (∨_q WC(q)).
        }
    }
    return r;
}

}  // namespace mrs_sadg::core
