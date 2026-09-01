// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_PLUGINS_PRIORITY_SAFE_INTERVAL_SEARCH_HPP
#define MRS_MAPF_PLUGINS_PRIORITY_SAFE_INTERVAL_SEARCH_HPP

#include "mrs_mapf/core/i_planning_strategy.hpp"

/// @file priority_safe_interval_search.hpp
/// @brief The third family of `255`§255-2-2 — "우선순위 + 안전 구간"
/// (priority-ordered, safe-interval search). `255`§255-2-2 records this family's
/// literature guarantee as explicitly NONE ("저자가 '비최적·비완전'으로 명시") —
/// this plugin claims the same: **feasible-only when it succeeds, no completeness
/// guarantee**. It may report failure on an instance that has a solution a
/// different priority ordering would find.
///
/// Robots in `scope` are planned ONE AT A TIME, in ascending robot-identifier
/// order (a deterministic total order — not the literature's dynamic priority
/// scheme; see `20b_impl_mapf.md` known limitations), each treating every
/// already-committed robot (out-of-scope, via `RB-1` reconstruction of the
/// retained plan, plus every earlier-in-priority in-scope robot already solved
/// this call) as a moving obstacle in a continuous-time safe-interval search
/// (arbitrary non-integer waits allowed — no synchronous grid is used
/// internally by this plugin; the grid/tick decision is §255's "미대기" open
/// item and orthogonal to this search).
///
/// This is the ONLY concrete plugin implemented this round (`20b` "알려진
/// 한계" — the other three named families are not implemented).

namespace mrs_mapf::plugins
{

/// @brief Priority + safe-interval search plugin (CN-5 swap target).
class PrioritySafeIntervalSearch : public mrs_mapf::core::IPlanningStrategy
{
public:
    mrs_mapf::core::Result<std::map<std::string, mrs_mapf::core::TimedRobotPlan>,
                           mrs_mapf::core::PlanFailure>
    solve(const mrs_mapf::core::PlanningRequest& request,
          const std::vector<std::string>& scope) override;
};

}  // namespace mrs_mapf::plugins

#endif  // MRS_MAPF_PLUGINS_PRIORITY_SAFE_INTERVAL_SEARCH_HPP
