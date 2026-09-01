// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_SERVICE_EXECUTION_TICK_SERVICE_HPP
#define MRS_SADG_SERVICE_EXECUTION_TICK_SERVICE_HPP

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "mrs_sadg/core/commit_gate.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/extension_types.hpp"
#include "mrs_sadg/core/i_escalation_policy.hpp"
#include "mrs_sadg/core/instr_ring.hpp"
#include "mrs_sadg/core/safety_envelope.hpp"

/// @file execution_tick_service.hpp
/// @brief ET## — the single per-tick call order (337 §6, 340 §2). The one state
/// transition orchestrator. 🔴 Returns a value; throws no exception on the tick
/// path (CN-16). No allocation of segments, no lock, no clock. External output
/// is produced only here (release bits) but published by the node, not this
/// service. The order-search response is accepted only when all five accept
/// gates pass; any single failure keeps z_prev (RE-5).

namespace mrs_sadg::service
{

/// @brief One tick's inputs (drained channels + observations).
struct TickInput
{
    std::uint64_t tick_seq = 0;             ///< ET00.
    bool entry_present = true;              ///< ET01: entry channel had a message.
    std::vector<std::string> entry_events;  ///< entered:true segment ids.
    bool rz_present = true;                 ///< ET02: realizability channel had a message.
    core::ReachabilityView rz;              ///< reachability view (if present).
    bool t1_present = false;                ///< ET05: order-search response present.
    core::CommitProposal t1_response;       ///< the proposal (if present).
    std::vector<std::string> completions;   ///< ET16: completed segment ids.
    std::vector<std::pair<std::string, std::string>>
        blocked_decls;                       ///< ET17: upstream blocked edges.
    std::map<std::string, double> deficits;  ///< ET20: per-robot D_r (no clock in core).
    core::WcInput wc;                        ///< ET22: corridor snapshot for detect_wc1.
};

/// @brief One tick's outputs (gab-bu bits + escalation + instrumentation view).
struct TickOutput
{
    core::Rung rung = core::Rung::kNone;    ///< ET30 escalation decision.
    std::vector<core::SegmentId> released;  ///< ET33: arrival-vertex entry permits (boundary).
    bool committed = false;                 ///< a reorder committed this tick.
    core::GateId first_failed = core::GateId::kNone;  ///< which gate cut the reorder.
    bool wc1_detected = false;                        ///< ET22: STRUCT_WC1 present.
    bool republish_needed = false;                    ///< ET34: E2 flag.
};

/// @brief Runs the per-tick call order against a shared graph.
class ExecutionTickService
{
public:
    /// @brief Construct with the shared graph, envelope, policy and instrumentation.
    /// @param g the execution graph (written only here).
    /// @param env the safety envelope (core-fixed).
    /// @param policy the escalation policy (extension point).
    /// @param instr the instrumentation ring.
    ExecutionTickService(core::ExecutionGraph& g,
                         core::SafetyEnvelope& env,
                         core::IEscalationPolicy& policy,
                         core::InstrRing& instr)
        : g_(g), env_(env), policy_(policy), instr_(instr)
    {
    }

    /// @brief Run one tick (ET00-ET36). Never throws (CN-16).
    /// @param in the tick input.
    /// @return the tick output (gab-bu bits, escalation, instrumentation flags).
    TickOutput run_tick(const TickInput& in);

private:
    core::ExecutionGraph& g_;
    core::SafetyEnvelope& env_;
    core::IEscalationPolicy& policy_;
    core::InstrRing& instr_;
};

}  // namespace mrs_sadg::service

#endif  // MRS_SADG_SERVICE_EXECUTION_TICK_SERVICE_HPP
