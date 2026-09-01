// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_SAFETY_ENVELOPE_HPP
#define MRS_SADG_CORE_SAFETY_ENVELOPE_HPP

#include <string>
#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/standby_resolver.hpp"

/// @file safety_envelope.hpp
/// @brief SE## — the safety envelope (안2). 🔴 NOT an extension point: the
/// envelope is the one place safety lives under every method (T-DIV), so it is a
/// core-fixed class (337 §4-3). `safe_check` is RE-1 (accept iff SAFE ∧
/// PROP-ACCEPT), broader than acyclicity (RE-4). `detect_wc1` is RE-6/307
/// (STRUCT_WC1, internal deadlock). The one-step-lookahead predicate (RE-2) is
/// [미확인] provisional (Lawley/Reveliotis source unconfirmed) — isolated in an
/// internal function, never promoted to a config switch or a plugin.

namespace mrs_sadg::core
{

/// @brief One corridor's structural occupancy input for STRUCT_WC1 (DT.2/3/5).
struct Corridor
{
    std::string id;                         ///< corridor q.
    std::string entry_node;                 ///< 'a' — the drain entry.
    std::string exit_node;                  ///< exit vertex.
    std::vector<std::string> occupants;     ///< occ_cons(q), inside robots.
    std::string exit_occupant;              ///< r_ex at exit ("" if none).
    bool exit_first_edge_reversed = false;  ///< DT.5: residual first edge in B_omega(q).
};

/// @brief Input to detect_wc1 (a snapshot of corridor occupancy — server local).
struct WcInput
{
    std::vector<Corridor> corridors;  ///< corridors to scan.
};

/// @brief Result of STRUCT_WC1 detection.
struct WcResult
{
    bool detected = false;               ///< true iff an internal deadlock structure.
    std::string corridor;                ///< the corridor q.
    std::vector<std::string> occupants;  ///< occ_cons(q) to drain.
    std::string exit_occupant;           ///< r_ex.
};

/// @brief The safety envelope: SAFE accept gate + WC1 detection.
class SafetyEnvelope
{
public:
    /// @brief RE-1 accept gate: z accepted iff SAFE(result(s,z)) ∧ PROP-ACCEPT.
    /// Broader than acyclicity (RE-4): acyclicity is one necessary conjunct.
    /// @param g the execution graph.
    /// @param combo the proposal's per-group chosen alternative index.
    /// @return true iff the resulting graph is safe and all robots can complete.
    bool safe_check(const ExecutionGraph& g, const std::vector<std::size_t>& combo) const;

    /// @brief RE-6/307: detect STRUCT_WC1 (internal deadlock). Structural, no clock.
    /// @param in the corridor occupancy snapshot.
    /// @return the detection result (detected + which corridor / occupants).
    WcResult detect_wc1(const WcInput& in) const;

    /// @brief Optionally supply a roadmap adjacency so RE-3 standby recovery can
    /// widen the accept set beyond bare acyclicity.
    /// @param adj the roadmap adjacency (read-only, never mutated).
    /// @param d3 the required standby count (D3, [값 부재] config).
    void set_roadmap(const RoadmapAdjacency& adj, int d3)
    {
        roadmap_ = adj;
        d3_ = d3;
    }

private:
    /// @brief PROP-ACCEPT: acyclic ∧ every segment still reachable to completion.
    bool prop_accept(const ExecutionGraph& g, const std::vector<std::size_t>& combo) const;

    /// @brief RE-2 [미확인 잠정]: RAS structural one-step condition per resource.
    bool one_step_ok(const ExecutionGraph& g) const;

    RoadmapAdjacency roadmap_;
    int d3_ = 1;
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_SAFETY_ENVELOPE_HPP
