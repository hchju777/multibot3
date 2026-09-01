// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/core/visit_order.hpp"

#include <algorithm>
#include <set>

#include "mrs_mapf/core/reservation.hpp"

namespace mrs_mapf::core
{

namespace
{

/// @brief Contract tie-break comparator (§286-0-1 #6): robot id UTF-8 codepoint
/// ascending, then that robot's steps[].index ascending.
bool tie_break_less(const TimedVisit& a, const TimedVisit& b)
{
    if (a.robot != b.robot)
    {
        return a.robot < b.robot;
    }
    return a.index < b.index;
}

bool timed_visit_less(const TimedVisit& a, const TimedVisit& b)
{
    if (a.nominal_entry_s != b.nominal_entry_s)
    {
        return a.nominal_entry_s < b.nominal_entry_s;
    }
    return tie_break_less(a, b);
}

}  // namespace

std::vector<VisitOrderEntry> build_visit_order(
    const std::vector<RobotPlan>& plans,
    const std::map<std::string, double>& avg_speed_mps,
    const Roadmap& roadmap,
    const std::vector<OrderingConstraint>& prefixes,
    const std::map<std::string, std::vector<double>>& known_schedules)
{
    std::map<std::string, std::vector<TimedVisit>> visitors;  // location -> visits.

    for (const auto& plan : plans)
    {
        std::vector<double> arrival;
        if (auto kit = known_schedules.find(plan.robot);
            kit != known_schedules.end() && kit->second.size() == plan.steps.size())
        {
            // The search's own actual (wait-respecting) schedule — preferred
            // whenever available (see this function's doc for why).
            arrival = kit->second;
        }
        else
        {
            double speed = 0.0;
            if (auto it = avg_speed_mps.find(plan.robot); it != avg_speed_mps.end())
            {
                speed = it->second;
            }
            if (speed > 0.0)
            {
                arrival = exact_arrival_schedule(plan.steps, roadmap, speed);
            }
        }
        for (std::size_t i = 0; i < plan.steps.size(); ++i)
        {
            TimedVisit tv;
            tv.robot = plan.robot;
            tv.index = plan.steps[i].index;
            // Fall back to the step's own index as an ordering proxy when a
            // nominal time could not be formed (missing arc / speed <= 0) —
            // still a valid deterministic total order (known limitation: not
            // time-derived in that case). See visit_order.hpp doc.
            tv.nominal_entry_s = (i < arrival.size()) ? arrival[i] : static_cast<double>(i);
            visitors[plan.steps[i].location].push_back(tv);
        }
    }

    std::map<std::string, std::vector<OrderingPrefixItem>> prefix_by_loc;
    for (const auto& oc : prefixes)
    {
        prefix_by_loc[oc.location] = oc.sequence;
    }

    std::vector<VisitOrderEntry> out;
    for (auto& [location, items] : visitors)
    {
        if (items.size() < 2)
        {
            continue;  // contract rule: only locations with >= 2 items are listed.
        }

        VisitOrderEntry entry;
        entry.location = location;

        std::set<std::pair<std::string, std::uint32_t>> already_placed;

        // Consumption point ② (§255-4-3): emit the received realized prefix first, verbatim.
        if (auto it = prefix_by_loc.find(location); it != prefix_by_loc.end())
        {
            for (const auto& p : it->second)
            {
                VisitItem vi;
                vi.robot = p.robot;
                vi.index = p.index;
                entry.sequence.push_back(vi);
                already_placed.insert({p.robot, p.index});
            }
        }

        std::sort(items.begin(), items.end(), timed_visit_less);
        for (const auto& tv : items)
        {
            const auto key = std::make_pair(tv.robot, tv.index);
            if (already_placed.count(key) != 0)
            {
                continue;  // already emitted via the received prefix.
            }
            VisitItem vi;
            vi.robot = tv.robot;
            vi.index = tv.index;
            entry.sequence.push_back(vi);
        }

        out.push_back(std::move(entry));
    }
    return out;
}

}  // namespace mrs_mapf::core
