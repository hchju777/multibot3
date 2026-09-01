// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_VISIT_ORDER_HPP
#define MRS_MAPF_CORE_VISIT_ORDER_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "mrs_mapf/core/types.hpp"

/// @file visit_order.hpp
/// @brief Builds `mrs.discrete_plan.visit_order` from a set of robot plans.
///
/// Contract rule (`discrete_plan` 4.0.0 `visit_order.description`, read in full
/// this round): pick each location's items in **first-entry order**; `steps[0]`
/// counts as a visit (4.0.0 확정 — §286-1 #6/#SV50); ties (same nominal entry
/// time) break by ① robot identifier UTF-8 codepoint ascending, ② that robot's
/// `steps[].index` ascending. `std::string::operator<` on well-formed UTF-8 is
/// byte-wise == codepoint-wise, so plain string comparison implements ①.
///
/// Consumption point ② of §255-4-3 (산출 제약): for a location with a received
/// `ordering_constraints` prefix, the produced sequence at that location MUST
/// begin with that received prefix, verbatim, with any newly computed visits
/// appended strictly after it — see `build_visit_order`'s `prefixes` parameter.

namespace mrs_mapf::core
{

/// @brief One (robot,index) with its nominal entry time, used only to derive
/// the order — the time value itself never leaves this function.
struct TimedVisit
{
    std::string robot;
    std::uint32_t index = 0;
    double nominal_entry_s = 0.0;
};

/// @brief Build the full `visit_order` array for @p plans.
///
/// For every location visited by >= 2 (robot,index) pairs total (including a
/// robot's own re-visits — 4.0.0's ">= 2 items" rule, not ">= 2 robots"), emits
/// one `VisitOrderEntry` whose `sequence` starts with @p prefixes[location] (if
/// present, verbatim) and is followed by the remaining visitors ordered by
/// nominal entry time, ties broken per the contract rule above.
///
/// @param plans every robot's plan (the full roster — in-scope newly-planned
///   plus out-of-scope byte-copied).
/// @param avg_speed_mps robot -> average speed, for nominal-entry-time ordering
///   of visitors this call has no @p known_schedules entry for (a missing
///   entry falls back further to arrival-order position, i.e. index order,
///   which is still a valid total order — see `20b_impl_mapf.md` known
///   limitations on time-free ordering fallback).
/// @param roadmap the roadmap (for segment length lookups feeding nominal time).
/// @param prefixes location -> already-realized prefix (from
///   `replan_request.ordering_constraints`), which must appear first, verbatim.
/// @param known_schedules robot -> the search's OWN actual (wait-respecting)
///   arrival-time schedule for that robot's `steps` (`TimedRobotPlan::arrival_s`),
///   when available. Takes precedence over recomputing a naive, waitless
///   nominal schedule from `avg_speed_mps` — recomputing independently can
///   disagree with the schedule the search actually used to avoid conflicts
///   (this package's own regression: a single-lane swap requiring one robot to
///   wait produced a spurious segment-graph cycle before this parameter
///   existed). Robots absent from this map (e.g. byte-copied out-of-scope
///   plans, whose original timing was never retained — time never crosses the
///   boundary) fall back to `avg_speed_mps`/index-order as documented above.
/// @return the visit_order array, ready for the adapter's `to_boundary`.
std::vector<VisitOrderEntry> build_visit_order(
    const std::vector<RobotPlan>& plans,
    const std::map<std::string, double>& avg_speed_mps,
    const Roadmap& roadmap,
    const std::vector<OrderingConstraint>& prefixes,
    const std::map<std::string, std::vector<double>>& known_schedules = {});

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_VISIT_ORDER_HPP
