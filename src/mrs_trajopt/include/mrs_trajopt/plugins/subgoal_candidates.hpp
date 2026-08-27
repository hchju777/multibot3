// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_PLUGINS_SUBGOAL_CANDIDATES_HPP
#define MRS_TRAJOPT_PLUGINS_SUBGOAL_CANDIDATES_HPP

#include "mrs_trajopt/core/i_subgoal_candidates.hpp"

/// @file subgoal_candidates.hpp
/// @brief Extension point (ii) implementations (candidate-source axis).
///
/// Two cells: SubgoalFreeSpaceDisk (disk sampling in free space) and
/// SubgoalRoadmapProjected (project onto roadmap edges). Both emit pass windows
/// (grain ①). 🔴 SIMPLIFIED: both place windows on the straight start->goal line
/// spaced by a fixed stride; the difference is the window radius policy. A real
/// free-space vs roadmap-projected difference needs the roadmap (see 20d limits).

namespace mrs_trajopt::plugins
{

/// @brief Free-space disk candidate source: wide pass windows on the segment.
class SubgoalFreeSpaceDisk : public core::ISubgoalCandidates
{
public:
    /// @brief Construct with a stride and window radius.
    /// @param stride_m spacing between pass windows [m] (> 0).
    /// @param window_radius_m position tolerance radius of each window [m].
    SubgoalFreeSpaceDisk(double stride_m, double window_radius_m)
        : stride_m_(stride_m), window_radius_m_(window_radius_m)
    {
    }

    /// @brief Generate pass windows on the start->goal line.
    /// @param ctx the subgoal context.
    /// @param out filled with the pass-window sequence.
    /// @return true iff at least one window was produced.
    bool generate(const core::SubgoalContext& ctx, std::vector<core::PassWindow>& out) override;

private:
    double stride_m_;
    double window_radius_m_;
};

/// @brief Roadmap-projected candidate source: narrower windows (edge-aligned).
class SubgoalRoadmapProjected : public core::ISubgoalCandidates
{
public:
    /// @brief Construct with a stride and window radius.
    /// @param stride_m spacing between pass windows [m] (> 0).
    /// @param window_radius_m position tolerance radius of each window [m].
    SubgoalRoadmapProjected(double stride_m, double window_radius_m)
        : stride_m_(stride_m), window_radius_m_(window_radius_m)
    {
    }

    /// @brief Generate edge-aligned pass windows on the start->goal line.
    /// @param ctx the subgoal context.
    /// @param out filled with the pass-window sequence.
    /// @return true iff at least one window was produced.
    bool generate(const core::SubgoalContext& ctx, std::vector<core::PassWindow>& out) override;

private:
    double stride_m_;
    double window_radius_m_;
};

}  // namespace mrs_trajopt::plugins

#endif  // MRS_TRAJOPT_PLUGINS_SUBGOAL_CANDIDATES_HPP
