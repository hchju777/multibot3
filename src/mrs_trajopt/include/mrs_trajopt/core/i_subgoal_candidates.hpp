// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_I_SUBGOAL_CANDIDATES_HPP
#define MRS_TRAJOPT_CORE_I_SUBGOAL_CANDIDATES_HPP

#include <vector>

#include "mrs_trajopt/core/types.hpp"

/// @file i_subgoal_candidates.hpp
/// @brief Extension point (ii): the 1-stage subgoal candidate source (CN-10).
///
/// Unchanged by the redesign (321b §3-1). Implementations:
/// SubgoalFreeSpaceDisk / SubgoalRoadmapProjected (candidate-source axis).

namespace mrs_trajopt::core
{

/// @brief Inputs to subgoal candidate generation (1-stage, ST07).
struct SubgoalContext
{
    Pose2 start;                 ///< current pose.
    Pose2 goal;                  ///< the segment goal pose.
    double yield_quota_m = 0.0;  ///< the assigned lateral-yield quota [m].
    std::int64_t successor = 0;  ///< candidate-tree successor index (handle ③).
};

/// @brief The subgoal candidate source.
class ISubgoalCandidates
{
public:
    virtual ~ISubgoalCandidates() = default;

    /// @brief Generate the next pass-window sequence for @p ctx (ST07).
    /// @param ctx the subgoal context (start, goal, quota, successor).
    /// @param out filled with the pass-window sequence (grain ①).
    /// @return true iff a non-empty candidate sequence was produced.
    virtual bool generate(const SubgoalContext& ctx, std::vector<PassWindow>& out) = 0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_I_SUBGOAL_CANDIDATES_HPP
