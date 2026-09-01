// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_COMMIT_GATE_HPP
#define MRS_SADG_CORE_COMMIT_GATE_HPP

#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/extension_types.hpp"

/// @file commit_gate.hpp
/// @brief The four structural accept gates AG1-AG4 (ET06-ET09). AG5 (SAFE) is
/// SafetyEnvelope. 🔴 Each gate cuts on a *distinct* condition so a reorder that
/// fails any one is rejected and z_prev is kept (RE-5). Gate AG1 forbids caching
/// (332§5-9-3): acyclicity is recomputed every tick.

namespace mrs_sadg::core
{

/// @brief Which gate (if any) first rejected a proposal.
enum class GateId
{
    kNone = 0,           ///< all four passed.
    kAcyclic = 1,        ///< AG1 — the combined graph is cyclic.
    kReleaseOpened = 2,  ///< AG2 — a reorder touches an already-opened release.
    kAlockOrder = 3,  ///< AG3 — a committed (entered) segment placed behind an uncommitted one.
    kRecertStale = 4  ///< AG4 — stale rz_seq or an entered segment de-certified.
};

/// @brief The four structural gates (each independently falsifiable).
class CommitGate
{
public:
    /// @brief AG1 (ET06): recompute acyclicity — no cache (332§5-9-3, DF-B D1).
    /// @param g the graph.
    /// @param p the proposal.
    /// @return true iff the combined graph is acyclic.
    static bool gate_acyclic(const ExecutionGraph& g, const CommitProposal& p);

    /// @brief AG2 (ET07): a reorder must not add an ordering edge into a segment
    /// whose arrival-vertex entry permit is already open (freshness).
    /// @param g the graph.
    /// @param p the proposal.
    /// @return true iff no already-released segment is reordered.
    static bool gate_release_unopened(const ExecutionGraph& g, const CommitProposal& p);

    /// @brief AG3 (ET08): A-LOCK — no proposal edge places a committed (entered)
    /// segment behind an uncommitted predecessor (DF-B D4).
    /// @param g the graph.
    /// @param p the proposal.
    /// @return true iff the commit order is preserved.
    static bool gate_alock_order(const ExecutionGraph& g, const CommitProposal& p);

    /// @brief AG4 (ET09): RECERT-LOCK — the proposal's rz_seq is current and no
    /// in-progress (entered) segment is de-certified as unreachable.
    /// @param g the graph.
    /// @param p the proposal.
    /// @param rz the current reachability view.
    /// @return true iff fresh and no entered segment is de-certified.
    static bool gate_recert_inprogress(const ExecutionGraph& g,
                                       const CommitProposal& p,
                                       const ReachabilityView& rz);

    /// @brief Run AG1..AG4 in order; report the first failure.
    /// @param g the graph.
    /// @param p the proposal.
    /// @param rz the current reachability view.
    /// @return GateId::kNone if all pass, else the first failing gate.
    static GateId first_failure(const ExecutionGraph& g,
                                const CommitProposal& p,
                                const ReachabilityView& rz);
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_COMMIT_GATE_HPP
