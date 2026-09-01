// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_EXTENSION_TYPES_HPP
#define MRS_SADG_CORE_EXTENSION_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "mrs_sadg/core/types.hpp"

/// @file extension_types.hpp
/// @brief Small value types shared by the three extension points (337 §4).
///
/// None of these cross the boundary — they are the internal contract between the
/// tick service and the swappable plugins. Algorithm parameters (theta_gain,
/// node budget, D3, search period) arrive through the *Config structs, which are
/// populated from `config/sadg.yaml` (CN-6): no default is invented here.

namespace mrs_sadg::core
{

class ExecutionGraph;  // forward: views hold a const pointer only.

/// @brief Deterministic search deadline = node-expansion count (RS-2: no clock).
struct NodeBudget
{
    std::uint64_t max_expansions = 0;  ///< [값 부재] config: hard node-count deadline.
    std::uint64_t used = 0;            ///< expansions charged so far.

    /// @brief Remaining node budget.
    /// @return max_expansions - used (0 if exhausted).
    std::uint64_t remaining() const
    {
        return used >= max_expansions ? 0 : max_expansions - used;
    }
};

/// @brief Reachability view (upstream Rz + rz_seq, seam 8). Read-only in search.
struct ReachabilityView
{
    std::uint64_t rz_seq = 0;              ///< freshness sequence.
    std::vector<std::string> unreachable;  ///< segment ids currently unreachable.

    /// @brief Is a segment reachable under the current Rz view?
    /// @param id segment id.
    /// @return true iff not in the unreachable list.
    bool reachable(const SegmentId& id) const;
};

/// @brief A commit proposal: a chosen alternative per switch group + freshness.
///
/// Produced by the order-search layer, applied by the tick only (E1). Never
/// mutates the graph.
struct CommitProposal
{
    std::vector<std::size_t> combo;  ///< chosen alternative index per group.
    std::uint64_t rz_seq = 0;        ///< rz_seq the proposal was computed against.
    double predicted_delay = 0.0;    ///< J-hat = predicted cumulative delay (RS-1).
};

/// @brief Why an order search could not produce an improving proposal.
enum class SolveError
{
    kBudgetExhausted,  ///< node budget spent with no strict improvement (RS-5).
    kOutsideEnvelope,  ///< greedy initial not in Z_safe (RS-3).
    kCancelled         ///< a newer request cancelled this one (RS-5).
};

/// @brief Static parameters for an IExecutionLayer (from config/sadg.yaml).
struct LayerConfig
{
    double theta_gain = 0.0;  ///< [값 부재] config: strict Phi decrease threshold (RS-4).
};

/// @brief What an IExecutionLayer promises about its search.
struct LayerCapabilities
{
    bool anytime = false;         ///< returns z_prev on timeout (RS-5).
    bool deterministic = false;   ///< node-budget deadline, no wall clock (RS-2).
    bool global_optimal = false;  ///< claims global optimum (branch-bound: false).
};

/// @brief A visit-order view of one shared location (input to the generator).
struct VisitOrderView
{
    const VisitEntry* entry = nullptr;  ///< the shared-location occupancy sequence.
};

/// @brief One generated ordering slot (a single ordering edge).
struct AlternativeSlot
{
    Dependency dep;         ///< the ordering edge (predecessor exit -> successor entry).
    bool opposing = false;  ///< ［D5］ opposing pair? (excluded from switch alternatives).
};

/// @brief Static parameters for an IAlternativeGenerator (from config/sadg.yaml).
struct GeneratorConfig
{
    std::uint32_t max_alternatives_per_group = 0;  ///< [값 부재] config policy (RG-4).
    bool full_permutation = false;                 ///< [값 부재] config: perm vs adjacent.
};

/// @brief What an IAlternativeGenerator promises (12c capabilities).
struct GeneratorCapabilities
{
    bool heads_complete = false;                   ///< generates a complete head set.
    bool preserves_intra_robot_order = false;      ///< never reorders one robot's segments.
    std::uint32_t max_alternatives_per_group = 0;  ///< upper bound it will emit.
};

/// @brief Why alternative generation failed.
enum class CompileError
{
    kMalformedSequence,  ///< a visit sequence could not be mapped to segments.
    kBudgetTooSmall      ///< out span too small for the required alternatives.
};

/// @brief The escalation ladder rung (332 §5-9-5).
enum class Rung
{
    kNone = 0,           ///< 0: no action.
    kReorder = 1,        ///< 1: internal reorder (order search).
    kPartialReplan = 2,  ///< 2: partial replan (replan_request; EVAC binds here).
    kGlobalReplan = 3    ///< 3: global replan (replan_request).
};

/// @brief Read-only view of progress deficits (input to escalation policy).
struct DeficitView
{
    const ExecutionGraph* graph = nullptr;  ///< source graph (const).
    std::vector<std::string> robots;        ///< robots in scope.
    double stall_threshold = 0.0;           ///< [값 부재] config: D^stall_max.

    /// @brief Does any robot exceed the stall threshold?
    /// @return true iff some D_r > stall_threshold (> 0).
    bool any_over_threshold() const;
};

/// @brief Read-only view of the blocked edge set (input to escalation policy).
struct BlockedView
{
    std::size_t count = 0;  ///< |V_blk|.
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_EXTENSION_TYPES_HPP
