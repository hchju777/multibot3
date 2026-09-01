// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/layer_branch_bound.hpp"

#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"

namespace mrs_sadg::plugins
{

namespace
{

/// @brief Deterministic proxy for cumulative delay J (RS-1). Lower is better:
/// a robot with a large progress deficit that carries many incoming ordering
/// constraints (waits behind others) contributes more. 🔴 Proxy — the measured
/// delay model is deferred (V-2, L-list).
double predicted_cumulative_delay(const core::ExecutionGraph& g,
                                  const std::vector<std::size_t>& combo)
{
    double j = 0.0;
    for (const auto& e : g.edges_for(combo))
    {
        const core::Segment& v = g.segments()[e.second];
        j += g.deficit(v.robot);
    }
    return j;
}

/// @brief Advance an odometer over per-group alternative counts. Returns false
/// when it wraps back to all-zeros (enumeration complete).
bool next_combo(std::vector<std::size_t>& combo, const std::vector<std::size_t>& counts)
{
    for (std::size_t i = 0; i < combo.size(); ++i)
    {
        if (++combo[i] < counts[i])
        {
            return true;
        }
        combo[i] = 0;
    }
    return false;
}

}  // namespace

core::Result<core::CommitProposal, core::SolveError> LayerBranchBound::select(
    const core::ExecutionGraph& g,
    const core::ReachabilityView& rz,
    core::NodeBudget& budget)
{
    cancel_ = false;
    const std::size_t n_groups = g.switch_groups().size();
    std::vector<std::size_t> counts(n_groups, 1);
    for (std::size_t i = 0; i < n_groups; ++i)
    {
        counts[i] = g.switch_groups()[i].alternatives.size();
    }

    // OS03: greedy initial = original order (all zeros).
    std::vector<std::size_t> z0(n_groups, 0);
    // OS04 / RS-3: Z_safe gate — do not run a search from an unsafe initial.
    if (!g.is_acyclic(z0))
    {
        return core::Result<core::CommitProposal, core::SolveError>::err(
            core::SolveError::kOutsideEnvelope);
    }

    std::vector<std::size_t> best = z0;
    double best_j = predicted_cumulative_delay(g, z0);
    const double theta = cfg_.theta_gain;

    std::vector<std::size_t> combo = z0;
    while (next_combo(combo, counts))
    {
        if (budget.remaining() == 0)
        {
            break;  // OS06: deadline is node count.
        }
        if (cancel_)
        {
            return core::Result<core::CommitProposal, core::SolveError>::err(
                core::SolveError::kCancelled);  // OS07: cancel inside the loop.
        }
        ++budget.used;  // OS09: charge one node expansion.
        if (!g.is_acyclic(combo))
        {
            continue;  // OS11: acyclicity prunes inside the search too.
        }
        const double j = predicted_cumulative_delay(g, combo);
        // OS13 / RS-4: accept only on strict Phi decrease of at least theta_gain.
        if (best_j - j >= theta && best_j - j > 0.0)
        {
            best = combo;
            best_j = j;
        }
    }

    // OS15-16 / RS-5: no improvement + budget spent -> z_prev fallback (counted).
    if (best == z0 && budget.remaining() == 0)
    {
        return core::Result<core::CommitProposal, core::SolveError>::err(
            core::SolveError::kBudgetExhausted);
    }

    core::CommitProposal p;
    p.combo = best;
    p.rz_seq = rz.rz_seq;
    p.predicted_delay = best_j;
    return core::Result<core::CommitProposal, core::SolveError>::ok_value(p);
}

core::LayerCapabilities LayerBranchBound::capabilities() const
{
    core::LayerCapabilities c;
    c.anytime = true;
    c.deterministic = true;
    c.global_optimal = false;
    return c;
}

std::string_view LayerBranchBound::key() const
{
    return "mrs_sadg/LayerBranchBound";
}

}  // namespace mrs_sadg::plugins
