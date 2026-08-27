// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_I_TRAJECTORY_SEARCH_HPP
#define MRS_TRAJOPT_CORE_I_TRAJECTORY_SEARCH_HPP

#include <vector>

#include "mrs_trajopt/core/layer_capabilities.hpp"
#include "mrs_trajopt/core/types.hpp"
#include "mrs_trajopt/core/velocity_profiler.hpp"

/// @file i_trajectory_search.hpp
/// @brief Extension point (i): the 2-stage dynamic search (321b §3-1, CN-10).
///
/// Replaces the old `ILowLevelLayer` (QP -> search, K22). `solve` produces a
/// **geometric chain** (poses + anchor flags); the VelocityProfiler fills speed
/// and time. The full state (t,x,y,theta,v,omega) is a profile output, not a
/// search output (U42-9). This header does NOT include pluginlib (CN-10).
/// Implementations: SearchHybridAstar / SearchFixedPathGating (AB-SWAP-CTRL).

namespace mrs_trajopt::core
{

/// @brief Inputs to one trajectory-tick search (TT01).
struct SearchContext
{
    Pose2 start;                       ///< current pose.
    double start_v = 0.0;              ///< current signed speed [m/s].
    std::vector<PassWindow> subgoals;  ///< the pass windows to thread (grain ①).
    Tube tube;                         ///< the published promise tube = HARD constraint.
    RobotLimits limits;                ///< per-robot limits.
};

/// @brief Why a search failed (TT11, 3-way decomposition).
enum class SearchFailCause
{
    kNone,
    kEmptyCandidates,  ///< candidates exhausted.
    kBrakeReject,      ///< braking filter rejected all.
    kBudgetExhausted   ///< node budget exhausted.
};

/// @brief Output of a search: the geometric chain and status.
struct SearchOutput
{
    bool found = false;                ///< F^srch != empty realized (RQ5-EX witness).
    std::vector<GeometricNode> chain;  ///< the geometric chain (poses + anchor + reverse).
    SearchFailCause cause = SearchFailCause::kNone;  ///< failure cause when !found.
    bool reverse_required = false;  ///< true iff only a reverse move could progress.
};

/// @brief The 2-stage dynamic search extension point.
class ITrajectorySearch
{
public:
    virtual ~ITrajectorySearch() = default;

    /// @brief Solve one trajectory-tick search inside the tube (TT02).
    ///
    /// The result must lie inside @p ctx.tube (hard constraint, TT01). On
    /// failure the previous remainder is held by the caller (INV-2 a1); a
    /// partial solution is never returned (BT-FAIL-SOUND).
    /// @param ctx the search context (start, subgoals, tube, limits).
    /// @param out filled with the geometric chain and status.
    /// @return true iff a solution was found (== out.found).
    virtual bool solve(const SearchContext& ctx, SearchOutput& out) = 0;

    /// @brief Report the cell's capabilities (321b §3-1).
    /// @return the capability declaration (warm start, reverse, lateral yield…).
    virtual LayerCapabilities capabilities() const = 0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_I_TRAJECTORY_SEARCH_HPP
